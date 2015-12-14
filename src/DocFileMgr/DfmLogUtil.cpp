////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/14/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLogUtil.h"

#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "DocFileMgr/DfmBody.h"
#include "Snapshot/SnapShotMgr.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/StrSplit.h"
#include <unistd.h>

OmnMutexPtr 	AosDfmLogUtil::smLock = OmnNew OmnMutex();
OmnFilePtr 		AosDfmLogUtil::smAllocFile = 0;
u64				AosDfmLogUtil::smFileSeqno = 0;
OmnString 		AosDfmLogUtil::smBaseDirName = "";
bool 			AosDfmLogUtil::smInit = false;


bool
AosDfmLogUtil::setDocInfoToBuff(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff, 
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	// u64		docid
	// u32		status, Opr
	// int64	trans_id_data_len
	// char *	trans_ids
	// int64	header_data_len
	// char *	header
	// int64	compress_len
	// int64	orig_len
	// int64 	body_data_len
	// char *	body

	// Ketty 2014/03/21
	// flush dfmDoc's mem data to mHeaderBuff. for IIL.
	doc->flushToHeaderBuff();

	buff->setU64(doc->getDocid());
aos_assert_r(doc->getDocid(), false);
	buff->setU32(doc->getOpr());
	transIdSerializeTo(buff, trans_ids);
	if (doc->getOpr() != AosDfmDoc::eSave) 
	{
		// delete opr
		buff->setI64(0);
		return true;
	}

	// modify or create opr 
	AosBuffPtr header_buff = doc->getHeaderBuff();
	aos_assert_r(header_buff && header_buff->dataLen(), false);
	buff->setI64(header_buff->dataLen());	
	buff->setBuff(header_buff); 

	AosBuffPtr body_buff = doc->getBodyBuff();
	aos_assert_r(body_buff && body_buff->dataLen(), false);

	// compress body
	int64_t compress_len = 0; 
	int64_t orig_len = body_buff->dataLen(); 
	if (doc->needCompress())
	{
		bool rslt = AosDfmBody::compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen();
	}
int64_t data_len = compress_len ? compress_len : orig_len;
aos_assert_r(body_buff->dataLen() == data_len, false);

	buff->setI64(compress_len);
	buff->setI64(orig_len);
	buff->setI64(body_buff->dataLen());	
	buff->setBuff(body_buff); 
	return true;
}


bool
AosDfmLogUtil::transIdSerializeTo(
		const AosBuffPtr &buff, 
		vector<AosTransId> &trans_ids)
{
	// Gavin, 2015/09/07
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion()) return true;

	int64_t data_len = trans_ids.size() * AosTransId::size(); 
	buff->setI64(data_len);

	int64_t crt_idx = buff->getCrtIdx();
	for (u32 i = 0; i < trans_ids.size(); i++)
	{
		trans_ids[i].serializeTo(buff);
	}
	aos_assert_r(buff->getCrtIdx() == crt_idx + data_len, false);
	return true;
}


bool
AosDfmLogUtil::serializeFromTransId(
		const AosBuffPtr &buff, 
		vector<AosTransId> &trans_ids,
		const bool read_trans_ids)
{
	int64_t data_len = buff->getI64(0);
	if (data_len <= 0) return true;
	int64_t crt_idx = buff->getCrtIdx();
	if (!read_trans_ids)
	{
		buff->setCrtIdx(crt_idx + data_len);	
		return true;
	}

	int64_t num_entry = data_len / AosTransId::size();
	aos_assert_r(num_entry, false);

	for (u32 i = 0; i < num_entry; i++)
	{
		trans_ids.push_back(AosTransId::serializeFrom(buff));
	}
	aos_assert_r(buff->getCrtIdx() - crt_idx  == data_len, false);
	return true;
}


AosDfmDocPtr
AosDfmLogUtil::getDocInfoFromBuff(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &temp_doc,
		const AosBuffPtr &buff)
{
	vector<AosTransId> trans_ids;
	bool read_trans_ids = false;
	AosDfmDocPtr doc = getDocInfoFromBuff(temp_doc, buff, trans_ids, read_trans_ids);
	aos_assert_r(doc, 0);

	AosBuffPtr body_buff = doc->getBodyBuff();
	if (!body_buff) return doc;

	int64_t compress_len  = doc->getCompressLen();
	int64_t orig_len  = doc->getOrigLen();

int64_t data_len = compress_len ? compress_len : orig_len;
aos_assert_r(body_buff->dataLen() == data_len, 0);

	// uncompress body
	bool rslt = AosDfmBody::uncompressBody(body_buff, orig_len, compress_len, rdata AosMemoryCheckerArgs);
	aos_assert_rr(rslt && body_buff, rdata, 0);
	doc->setBodyBuff(body_buff);
aos_assert_r((doc->getBodyBuff())->dataLen() == orig_len, 0);
	return doc;
}


AosDfmDocPtr
AosDfmLogUtil::getDocInfoFromBuff(
		const AosDfmDocPtr &temp_doc,
		const AosBuffPtr &buff,
		vector<AosTransId> &trans_ids,
		const bool read_trans_ids)
{
	// u64		docid
	// u32		status
	// int64	trans_id_data_len
	// char *	trans_ids
	// int64	header_data_len
	// char *	header
	// int64	compress_len
	// int64	orig_len
	// int64 	body_data_len
	// char *	body
	u64 docid = buff->getU64(0);
	aos_assert_r(docid, 0);

	AosDfmDocPtr doc = temp_doc->clone(docid);
	aos_assert_r(doc->getDocid() == docid, 0);

	AosDfmDoc::Opr opr = (AosDfmDoc::Opr)buff->getU32(0);
	aos_assert_r(opr > AosDfmDoc::eInvalidOpr && opr < AosDfmDoc::eMaxOpr, 0);
	doc->setOpr(opr);

	serializeFromTransId(buff, trans_ids, read_trans_ids);
	if (opr != AosDfmDoc::eSave)
	{
		// delete opr
		int64_t header_data_len = buff->getI64(-1);
		aos_assert_r(opr == AosDfmDoc::eDelete, 0);
		aos_assert_r(header_data_len == 0, 0);
		doc->cleanBodyBuff();
		return doc;
	}

	// create or modify opr  
	int64_t header_data_len = buff->getI64(-1);
	aos_assert_r(header_data_len > 0, 0);
	AosBuffPtr header_buff = OmnNew AosBuff(header_data_len, 0 AosMemoryCheckerArgs);
	buff->getBuff(header_buff->data(), header_data_len); 
	header_buff->setDataLen(header_data_len);
	doc->setHeaderBuff(header_buff);

	int64_t compress_len = buff->getI64(-1); 
	aos_assert_r(compress_len >= 0, 0);
	doc->setCompressLen(compress_len);

	int64_t orig_len = buff->getI64(-1);
	aos_assert_r(orig_len > 0, 0);
	doc->setOrigLen(orig_len);

	int64_t body_data_len = buff->getI64(-1);
	aos_assert_r(body_data_len > 0, 0);
	AosBuffPtr body_buff = OmnNew AosBuff(body_data_len, 0 AosMemoryCheckerArgs);
	buff->getBuff(body_buff->data(), body_data_len); 
	body_buff->setDataLen(body_data_len);
	doc->setBodyBuff(body_buff);
	return doc;
}



OmnString
AosDfmLogUtil::composeFname(
		const u32 siteid,
		const u32 dfm_id,
		const u32 crt_file_seqno)
{
	// "dfmlog"+"_"+ "00000" + "_"+00000 + "_" + 00000
	OmnString fname_prefix =  "dfmlog_";
	char idaddzero[16];
	sprintf(idaddzero, "%05d", siteid);
	fname_prefix << idaddzero << "_";

	char ddaddzero[16];
	sprintf(ddaddzero, "%05d", dfm_id);
	fname_prefix << ddaddzero << "_";

	char seaddzero[16];
	sprintf(seaddzero, "%05d", crt_file_seqno);
	fname_prefix << seaddzero;
	return fname_prefix;
}


/*
bool
AosDfmLogUtil::getBaseDirName(vector<OmnString> &vv)
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);
	AosXmlTagPtr stm_cfg = config->getFirstChild(AOSCONFIG_STORAGEMGR);
	aos_assert_r(stm_cfg, false);

	AosXmlTagPtr dev_config = stm_cfg->getFirstChild(AOSCONFIG_DEVICE);
	aos_assert_r(dev_config, false);

	AosXmlTagPtr partition_config = dev_config->getFirstChild(AOSCONFIG_PARTITION);
	while(partition_config)
	{
		OmnString path;
		OmnString userdir = partition_config->getAttrStr("userdir", "");
		if(userdir == "" ) 
		{
			partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
			continue;
		}

		u32 len = userdir.length();
		if(userdir.data()[len-1] != '/')   userdir << "/";
		if(userdir.data()[0] == '/')   
		{
			path << userdir;
		}
		else
		{
			path << OmnApp::getAppBaseDir() << userdir; 
		}
	
		vv.push_back(path);
		partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
	}	
	return true;
}
*/


void
AosDfmLogUtil::getNormDirFileNames(
		const AosRundataPtr &rdata,
		vector<OmnString> &file_path,
		const u32 vid)
{
	OmnString path = getBaseDirname();
	path << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getNormDirName();

	DIR *pDir = opendir(path.data());
	if (!pDir) return;

	struct dirent *ent;
	while((ent = readdir(pDir)) != NULL)
	{
		OmnString dname = ent->d_name;
		if (strcmp(dname.data(), ".") == 0 || strcmp(dname.data(), "..") == 0)
		{
			continue;
		}

		OmnString log_dname(dname.data(), 13);
		OmnString fpef = "dfmlog_";

		char idaddzero[16];
		sprintf(idaddzero, "%05d", rdata->getSiteid());
		fpef << idaddzero << "_";
		if (log_dname != fpef)
		{
			continue;
		}

		OmnScreen << "DDDDDDDDDDDDDDDDDDD :" << vid << ";" << dname << endl;
		OmnString fname = path;
		fname << dname;

		// fname:dfmlog_siteid_dfmid_fileid (dfmlog_00100_00100_00001_208)
		aos_assert(dname.length() >= 18);
		file_path.push_back(fname);
	}
}

/*
 * Linda, 2014/03/04
void
AosDfmLogUtil::getSnapShotDirFileNames(
		vector<OmnString> &file_path,
		const u64 snap_id,
		const u32 vid, 
		const u32 siteid,
		const u32 dfm_id)
{
	OmnString path = getBaseDirname();
	path << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getSnapShotDirName(snap_id);

	DIR *pDir = opendir(path.data());
	if (!pDir)	return ;

	struct dirent *ent;
	while((ent = readdir(pDir)) != NULL)
	{
		OmnString dname = ent->d_name;
		if (strcmp(dname.data(), ".") == 0 || strcmp(dname.data(), "..") == 0)
		{
			continue;
		}

		OmnString log_dname(dname.data(), 13);
		OmnString fpef = "dfmlog_";
		char idaddzero[16];
		sprintf(idaddzero, "%05d", siteid);
		fpef << idaddzero << "_";
		if (log_dname != fpef)
		{
			continue;
		}

		OmnString fname_prefix = log_dname; 
		char addzero[16];
		sprintf(addzero, "%05d", dfm_id);
		fname_prefix << addzero << "_";

		OmnString fname_dname(dname.data(), fname_prefix.length()); 
		if (fname_dname != fname_prefix)
		{
			continue;
		}
		OmnScreen << "DDDDDDDDDDDDDDDDDDD :" << vid << ";" << dname << endl;
		OmnString fname = path;
		fname << dname;

		// fname:dfmlog_siteid_dfmid_fileid (dfmlog_00100_00100_00001_208)
		aos_assert(dname.length() >= 18);
		file_path.push_back(fname);
	}
}
*/


/*
u32
AosDfmLogUtil::getCrtFileSeqno(
		const AosRundataPtr &rdata,
		const u32 vid, 
		const u32 dfm_id)
{
	u32 crt_file_seqno = 0;
	vector<OmnString> vv;
	bool rslt = getBaseDirName(vv);
	if (!rslt)
	{
		OmnAlarm << "read Base Dir faild!" << enderr;
		return 0;
	}
	for (u32 i = 0; i < vv.size(); i++)
	{
		OmnString path = vv[i];

		path << "vir_" << vid << "_orig"
			<< "/site_"<< rdata->getSiteid() << "/"; 
		
		DIR *pDir = opendir(path.data());
		if (!pDir)	return true;

		struct dirent *ent;
		while((ent = readdir(pDir)) != NULL)
		{
			OmnString dname = ent->d_name;
			if (strcmp(dname.data(), ".") == 0 || strcmp(dname.data(), "..") == 0)
			{
				continue;
			}
			
			OmnString log_dname(dname.data(), 7);
			if (log_dname != "dfmlog_")
			{
				continue;
			}

			OmnString fname_prefix = log_dname; 
			char addzero[16];
			sprintf(addzero, "%05d", dfm_id);
			fname_prefix << addzero << "_";

			OmnString fname_dname(dname.data(), fname_prefix.length()); 
			if (fname_dname != fname_prefix)
			{
				continue;
			}
			OmnScreen << "DDDDDDDDDDDDDDDDDDD :" << vid << ";" << dname << endl;
			OmnString fname = path;
			fname << dname;
			
			// fname:dfmlog_00100_00000_fileid (dfmlog_00100_00001_208)
			aos_assert_r(dname.length() >= 18, 0);
			const char *d = dname.data()+13;
			OmnString seqno(d, 5);
			int num = atoi(seqno.data());
			if (num >= 0 && (u32)num > crt_file_seqno)
			{
				crt_file_seqno = num;
			}
		}
		closedir(pDir);
	}
	return crt_file_seqno;
}
*/


void
AosDfmLogUtil::init()
{
	if (smInit) return; 

	smInit = true;
	createDfmLogBaseDir();

	OmnString alloc_fname = getBaseDirname();
	alloc_fname << getDfmLogBaseDirName();

	alloc_fname << "dfmlog_fname_alloc";
//OmnScreen << "init:" << alloc_fname << endl;
	smLock->lock();
	smAllocFile = OmnNew OmnFile(alloc_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!smAllocFile || !smAllocFile->isGood())
	{
		smAllocFile = OmnNew OmnFile(alloc_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!smAllocFile || !smAllocFile->isGood())
		{
			OmnAlarm << "Failed to open file: " << alloc_fname << enderr;
			smLock->unlock();
			return ;
		}
	}

	u64 crtFileSize = smAllocFile->getFileCrtSize();
	smFileSeqno = crtFileSize/eEntrySize;
	smLock->unlock();
}


OmnFilePtr
AosDfmLogUtil::createNewFile(
		const u32 vid, 
		const u32 siteid,
		const u32 dfm_id,
		//const u64 snap_id,
		u64 &file_id)
{
	init();
	file_id = ++smFileSeqno;

	//dirname: Data/DfmLog/vir_0_dfmlog/norm|snap_0
	OmnString fname_path = getDfmLogBaseDirName();
	fname_path << getVidDirName(vid);
	fname_path << getNormDirName();
	//Linda, 2014/03/04
	//if (snap_id == 0)
	//{
	//	fname_path << getNormDirName();
	//}
	//else
	//{
	//	fname_path << getSnapShotDirName(snap_id);
	//}

	OmnString filename = composeFname(siteid, dfm_id, file_id); 

	u64 offset = eEntryStart + file_id * eEntrySize;
	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eEntrySize);

	// file_id
	// fname_path
	// filename
	buff->setU64(file_id);
	buff->setOmnStr(fname_path);
	buff->setOmnStr(filename);
	smLock->lock();
	smAllocFile->put(offset, buff->data(), eEntrySize, true);
	smLock->unlock();

	OmnString full_fname = getBaseDirname(); 
	full_fname << fname_path  << filename;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		OmnAlarm << "failed_create_file" << ": " << full_fname << enderr;
		return 0;
	}
	return file;
}


OmnFilePtr
AosDfmLogUtil::openFile(const u64 file_id)
{
	u64 offset = eEntryStart + file_id * eEntrySize;
	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	smLock->lock();
	bool rslt = smAllocFile->readToBuff(offset, eEntrySize, buff->data());
	buff->setDataLen(eEntrySize);
	smLock->unlock();
	aos_assert_r(rslt, 0);

	u64 fid = buff->getU64(0);
	aos_assert_r(file_id == fid, 0);

	OmnString fname_path = buff->getOmnStr("");
	aos_assert_r(fname_path != "", 0);

	OmnString fname = buff->getOmnStr("");
	aos_assert_r(fname != "", 0);

	OmnString full_fname = getBaseDirname();
	full_fname << fname_path << fname;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		OmnAlarm << "failed_create_file" << ": " << full_fname << enderr;
		return 0;
	}
	return file;
}


bool
AosDfmLogUtil::removeFile(const u64 file_id)
{
	u64 offset = eEntryStart + file_id * eEntrySize;
	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	smLock->lock();
	bool rslt = smAllocFile->readToBuff(offset, eEntrySize, buff->data());
	smLock->unlock();
	aos_assert_r(rslt, false);
	buff->setDataLen(eEntrySize);
	buff->reset();

	u64 fid = buff->getU64(0);
	aos_assert_r(file_id == fid, false);

	OmnString fname_path = buff->getOmnStr("");
	aos_assert_r(fname_path != "", false);

	OmnString fname = buff->getOmnStr("");
	aos_assert_r(fname != "", false);

	OmnString full_fname = getBaseDirname();
	full_fname << fname_path << fname;
	unlink(full_fname.data());

	char data[eEntrySize];      
	memset(data, 0, eEntrySize);
	smLock->lock();
	smAllocFile->put(offset, data, eEntrySize, true);
	smLock->unlock();
	return true;
}


bool
AosDfmLogUtil::moveFile(
		const u64 file_id,
		const u32 virtual_id)
{
	u64 offset = eEntryStart + file_id * eEntrySize;
	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	smLock->lock();
	bool rslt = smAllocFile->readToBuff(offset, eEntrySize, buff->data());
	smLock->unlock();
	aos_assert_r(rslt, false);
	buff->setDataLen(eEntrySize);
	buff->reset();

	u64 fid = buff->getU64(0);
	aos_assert_r(file_id == fid, false);

	OmnString old_fname_path = buff->getOmnStr("");
	aos_assert_r(old_fname_path != "", false);

	OmnString old_fname = buff->getOmnStr("");
	aos_assert_r(old_fname != "", false);

	OmnString new_fname_path = getDfmLogBaseDirName();
	new_fname_path << getVidDirName(virtual_id)
				<< getNormDirName();

	memset(buff->data(), 0, eEntrySize);
	buff->reset();
	buff->setU64(file_id);
	buff->setOmnStr(new_fname_path);
	buff->setOmnStr(old_fname);

	smLock->lock();
	smAllocFile->put(offset, buff->data(), eEntrySize, true);
	smLock->unlock();

	OmnString old_full_fname = getBaseDirname();
	old_full_fname << old_fname_path << old_fname;

	OmnString new_full_fname  = getBaseDirname();
	new_full_fname << new_fname_path << old_fname;

	//OmnScreen << "move file " << old_full_fname << " to " << new_full_fname << endl;
	link(old_full_fname.data(), new_full_fname.data());

	unlink(old_full_fname.data());
	return true;
}


void
AosDfmLogUtil::print(
		const bool show_log,
		const OmnString &name,
		AosDfmCompareFunType::set_f &vv)
{
	if (!show_log) return;
	AosDfmCompareFunType::setitr_f itr;
	for (itr = vv.begin(); itr != vv.end(); ++itr)
	{
		AosDfmDocPtr doc = *itr;
		OmnString docstr;
		docstr	<< name << " docid:" << doc->getDocid() 
				<< "; opr:" << doc->getOpr() 
				<<"; seqno:" << doc->getBodySeqno() 
				<< "; offset:" << doc->getBodyOffset();
		if (doc->getOpr() != AosDfmDoc::eDelete)
			docstr <<"; size:" << doc->getBodyBuff()->dataLen();
		OmnScreen << docstr << endl;
	}
}


void
AosDfmLogUtil::printSetFs(
		const bool show_log,
		const OmnString &name,
		AosDfmCompareFunType::set_fs &docs)
{
	if (!show_log) return; 
	AosDfmCompareFunType::setitr_fs itr;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		AosDfmDocPtr doc = *itr;
		OmnString docstr;
		docstr << name << " opr:" << doc->getOpr() 
				<< "; docid:" << doc->getDocid()
				<<"; seqno:" << doc->getBodySeqno() 
				<< "; offset:" << doc->getBodyOffset();
		if (doc->getOpr() != AosDfmDoc::eDelete)
			docstr <<"; size:" << doc->getBodyBuff()->dataLen();
		OmnScreen << docstr << endl;
	}
}


OmnString
AosDfmLogUtil::getNormDirName()
{
	OmnString dirname;
	dirname << AOS_NORMAl_DIRNAME_PRE << "/";
	return dirname;
}
	
bool
AosDfmLogUtil::createNormDir(const u32 vid)
{
	init();
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getNormDirName();

	DIR *dir = opendir(fulldir.data());
	if (dir == NULL)
	{
//OmnScreen << "createNormir:" << fulldir << endl;
		mkdir(fulldir.data(), 0755);
	}
	return true;
}

/*
 * Linda, 2014/03/04
OmnString
AosDfmLogUtil::getSnapShotDirName(const u64 snap_id)
{
	OmnString dirname;
	dirname << AosSnapShotMgr::getSnapShotDirName(snap_id);
	return dirname;
}


OmnString
AosDfmLogUtil::getSnapShotFullPath(
		const u32 vid,
		const u64 snap_id)
{
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getSnapShotDirName(snap_id);
	return fulldir;
}


bool
AosDfmLogUtil::createSnapShotDir(
		const u32 vid,
		const u64 snap_id)
{
	init();
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getSnapShotDirName(snap_id);

	DIR *dir = opendir(fulldir.data());
	if (dir == NULL)
	{
//OmnScreen << "createSnapShotDir:" << fulldir << endl;
		mkdir(fulldir.data(), 0755);
	}
	return true;
}


bool
AosDfmLogUtil::removeSnapShotDir(
		const u32 vid,
		const u64 snap_id)
{
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName()
			<< getVidDirName(vid)
			<< getSnapShotDirName(snap_id);

	//unlink(fulldir.data());
	OmnString cmd = "rm -r ";
	cmd << fulldir;
	system(cmd.data());
	return true;
}
*/


OmnString
AosDfmLogUtil::getVidDirName(const u32 vid)
{
	//dirname: Data/DfmLog/vir_0_dfmlog
	OmnString dirname;
	dirname << "vir_" << vid <<"_log/";
	return dirname;
}


bool
AosDfmLogUtil::createVidDir(const u32 vid)
{
	init();
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName()
			<< getVidDirName(vid);

	DIR *dir = opendir(fulldir.data());
	if (dir == NULL)
	{
//OmnScreen << "createVidDir:" << fulldir << endl;
		mkdir(fulldir.data(), 0755);
	}
	return true;
}


OmnString
AosDfmLogUtil::getDfmLogBaseDirName()
{
	init();
	OmnString name = AOS_DFMLOG_DIRNAME;
	name << "/";
	return name;
}

bool
AosDfmLogUtil::createDfmLogBaseDir()
{
	OmnString fulldir = getBaseDirname();
	fulldir << getDfmLogBaseDirName();

	DIR *dir = opendir(fulldir.data());
	if (dir == NULL) 
	{
		mkdir(fulldir.data(), 0755);
	}
	return true;
}


OmnString
AosDfmLogUtil::getBaseDirname()
{
	if (smBaseDirName != "") return smBaseDirName;

	OmnString userdir = AosGetBaseDirname();
	u32 len = userdir.length();
	if(userdir.data()[len-1] != '/')   userdir << "/";
	smBaseDirName = userdir;
	return smBaseDirName;
}


bool
AosDfmLogUtil::removeEmptyDir(const OmnString &path)
{
	//OmnScreen << "removeNullDir path: " << path << endl;
	rmdir(path.data());
	return true;

}


