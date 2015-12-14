////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActImportUnicomFile.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocidMgrObj.h"
#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/DocAsmVar.h"
#include "SEUtil/IILName.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Util/Ptrs.h"
#include "API/AosApi.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/BuffData.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SdocDocument.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"

const bool IS_PUBLIC = true;
AosDocProc::AosSeWordHash   AosActImportUnicomFile::mWords[eMaxThrdIds];

AosActImportUnicomFile::AosActImportUnicomFile(const bool flag)
:
AosTaskAction(AOSACTTYPE_IMPORTUNICOMFILE, AosActionType::eImportUnicomFile, flag),
mLock(OmnNew OmnMutex()),
mRunNum(0),
mFileIdx(0),
mThreadId(0)
{
}


AosActImportUnicomFile::~AosActImportUnicomFile()
{
}

AosActionObjPtr
AosActImportUnicomFile::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportUnicomFile(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActImportUnicomFile::initAction(
					const AosTaskObjPtr &task,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata)
{
	mTask = task;
	mSdoc = sdoc;
	
	mCrtDocNum = 0;
	mAttrMap.clear();

	mUuicomTitlePctrs = sdoc->getAttrStr("unicom_title_ctnr", "");
	aos_assert_r(mUuicomTitlePctrs != "", false)
	
	mUuicomFilePctrs = sdoc->getAttrStr("unicom_file_ctnr", "");
	aos_assert_r(mUuicomFilePctrs != "", false)
	
	bool rslt = createDataScanner(sdoc, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr attrs = sdoc->getFirstChild("attrs");
	if (!attrs) return true;

	AosXmlTagPtr attr = attrs->getFirstChild(true); 
	while (attr)
	{
		AosValueRslt value;
		AosXmlTagPtr key_doc = attr->getFirstChild("xpath");
		AosXmlTagPtr value_doc = attr->getFirstChild("value");

		rslt = AosValueSel::getValueStatic(value, key_doc, rdata);
		if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
		{
			rdata->setError() << "Failed to get the cash attribute name!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;                                                                   
		}
		OmnString key = value.getStr();
		if ( key == "")
		{
			rdata->setError() << "cash attribute name is null!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		rslt = AosValueSel::getValueStatic(value, value_doc, rdata);
		if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
		{
			rdata->setError() << "Failed to get the cash attribute name!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;                                                                   
		}
		OmnString strValue = value.getStr();
		if ( strValue == "")
		{
			rdata->setError() << "cash attribute name is null!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mAttrMap[key] = strValue;
		attr = attrs->getNextChild();
	}

	
	
	OmnString dftctnr_objid = "ctnr";
	dftctnr_objid << AOSZTG_DOCUMENT;
	
	mDocumentCtnr = sdoc->getAttrStr("zky_document_ctnr", dftctnr_objid);
	
	AosXmlTagPtr docasm_conf = sdoc->getFirstChild("zky_docassembler");
	aos_assert_r(docasm_conf, false);

	OmnString dclid = docasm_conf->getAttrStr(AOSTAG_NAME);
	aos_assert_r(dclid != "", false);
	
	AosXmlTagPtr datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);

	OmnString asm_key = datacol_tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(asm_key != "", false);
	
	mDocVarAssembler = AosDataAssembler::createAssemblerV1(asm_key, mTask, datacol_tag, rdata.getPtrNoLock());

	dclid = sdoc->getAttrStr(AOSTAG_NAME);
	aos_assert_r(dclid != "", false);
	
	datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);
	
	asm_key = datacol_tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(asm_key != "", false);

	AosXmlTagPtr iilasm_tag = datacol_tag->getFirstChild("asm");
	aos_assert_r(iilasm_tag, false);

	AosXmlTagPtr record_tag = iilasm_tag->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);

	mRcdLen = record_tag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(mRcdLen != -1, false);

	mHitIILAsm = AosDataAssembler::createAssemblerV1(asm_key, mTask, datacol_tag, rdata.getPtrNoLock());
	aos_assert_r(mHitIILAsm, false);
	
	mAttrPaths.clear();
	mAttrNames.clear();
	rslt = getIILAsms(sdoc, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(mAttrPaths.size() == mAttrNames.size(), false);

	 for (int i=0; i<eMaxThrdIds; i++)
	 {
	      mWordNorm[i] = OmnNew AosWordNorm("", "wordNorm");
	      mWordParser[i] = OmnNew AosWordParser();
	 }
	
	return true;
}


bool
AosActImportUnicomFile::runAction(const AosRundataPtr &rdata)
{
	AosBuffDataPtr metaData = OmnNew AosBuffData();
	bool rslt = mScanner->getNextBlock(metaData, rdata);
	aos_assert_r(rslt, false);


	if (!metaData || !metaData->getMetadata())
	{
		AosActionObjPtr thisptr(this, true);
  		mTask->actionRunFinished(rdata, thisptr);
		return true;
	}

	mLock->lock();
	mRunNum++;
	mLock->unlock();
	
	//
	//1. use the metaData's buff to create binarydoc
	//
	// Chen Ding, 2013/11/23
	// AosXmlTagPtr unicom_xml = metaData->getXmlDoc();
	AosXmlTagPtr unicom_xml = metaData->getMetadata();
	aos_assert_r(unicom_xml, false);

	OmnString fname = unicom_xml->getAttrStr("unicom_file_name", "");
	aos_assert_r(fname != "", false);

	OmnString isNoErr = unicom_xml->getAttrStr("zky_unicom_file_error", "000"); 

	mFileStart[fname] = isNoErr; 

	if (isNoErr != "000")
	{
		OmnAlarm << " Document is Error : " << isNoErr << enderr;
		mLock->lock();
		mRunNum--;
		aos_assert_rl(mRunNum >= 0, mLock, false);
		mLock->unlock();
		return true;
	}

	vector<AosXmlTagPtr> docs = metaData->getVectorDocs();
	vector<AosBuffPtr> buffs = metaData->getVectorBuffs();

	AosBuffPtr zip_buff = metaData->getBuff();
	unicom_xml->setAttr(AOSTAG_HPCONTAINER, mUuicomTitlePctrs);
	unicom_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	unicom_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
	unicom_xml->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	
	AosXmlTagPtr new_xml = AosCreateBinaryDoc(-1, unicom_xml->toString(), IS_PUBLIC, zip_buff, rdata);

	//aos_assert_r(new_xml, false);
	if (!new_xml)
	{
		OmnAlarm << " create binary doc error ! " << enderr;
		mLock->lock();
		mRunNum--;
		aos_assert_rl(mRunNum >= 0, mLock, false);
		mLock->unlock();
		return true;
	}

	//u64 unicomtitle_objid = new_xml->getAttrU64(AOSTAG_OBJID, 0);
	OmnString unicomtitle_objid = new_xml->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(unicomtitle_objid != "", false);
	OmnString unicomtitle_docid = new_xml->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(unicomtitle_docid != "", false);
	aos_assert_r(docs.size() == buffs.size(), false);

//OmnScreen << " ActImportUnicomFile docs size : " << docs.size() << endl;
	for(u32 i = 0; i < docs.size(); i++)
	{
		AosXmlTagPtr doc = docs[i];
		AosBuffPtr buff = buffs[i];
		map<OmnString, OmnString>::iterator it;
		for(it = mAttrMap.begin(); it != mAttrMap.end(); it++)
		{
			doc->setAttr(it->first, it->second);
		}
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		doc->setAttr("unicom_title_objid", unicomtitle_objid);
		doc->setAttr("unicom_title_docid", unicomtitle_docid);
		doc->setAttr(AOSTAG_HPCONTAINER, mUuicomFilePctrs);
		doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
		doc->setAttr(AOSTAG_JOB_DOCID, mTask->getJobDocid());
		
		//felicia, for batch upload file
		addAttrForUpload(doc, rdata);
		AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, doc->toString(), IS_PUBLIC, buff, rdata);
		//aos_assert_r(newdoc, false);
		
		if (!newdoc)
		{
			OmnAlarm << " create binary doc error ! " << enderr;
			mLock->lock();
			mRunNum--;
			aos_assert_rl(mRunNum >= 0, mLock, false);
			mLock->unlock();
			return true;
		}
		mCrtDocNum++;

		rslt = importDoc(newdoc, buff, rdata);
		if (!rslt)
		{
			OmnAlarm << " importDoc : " << newdoc->getAttrStr(AOSTAG_OBJID) << " error ! " << enderr;
		}
	}
	
	mLock->lock();
	mRunNum--;
	aos_assert_rl(mRunNum >= 0, mLock, false);
	mLock->unlock();
	return true;
}

bool
AosActImportUnicomFile::createDataScanner(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr dirpath_conf = sdoc->getFirstChild("dirpath");
	aos_assert_r(dirpath_conf, false);

	OmnString dirpath = dirpath_conf->getNodeText();
	aos_assert_r(dirpath != "", false);
	
	int physical_id = dirpath_conf->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(physical_id != -1, false);

	bool recursion = dirpath_conf->getAttrBool(AOSTAG_RECURSION, true);
	int rec_level = recursion ? -1 : 0;

	OmnString extension = dirpath_conf->getAttrStr(AOSTAG_EXTENSION);	

	OmnString row_delimiter = dirpath_conf->getAttrStr(AOSTAG_ROW_DELIMITER);

	OmnString chartype = dirpath_conf->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	aos_assert_r(CodeConvertion:.isNull(chartype), false);

	vector<AosFileInfo> file_list;
	bool rslt = AosNetFileCltObj::getFileListByAssignExtStatic(
		extension, dirpath, physical_id, rec_level, file_list, rdata.getPtr());
	aos_assert_r(rslt, false);

	mScanner = AosDataScannerObj::createUnicomDirScannerStatic(
		file_list, physical_id, chartype, row_delimiter, rdata);
	aos_assert_r(mScanner, false);
	return true;
}

bool
AosActImportUnicomFile::addAttrForUpload(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	OmnString file_type = doc->getAttrStr("zky_file_type");
	if (file_type == "上传")
	{
		//AosStrSplit split(info.mFileName, "/");            
		//vector<OmnString> strs= split.entriesV();
		//OmnString fname = strs[strs.size()-1];
		/*
		int startidx = file_name.find('/', true);
		OmnString fname = file_name.substr(startidx+1);  
		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");

		startidx = fname.find('.', true);
		OmnString document_type = fname.substr(startidx+1);
		*/
		OmnString fname = doc->getAttrStr(AOSTAG_SOURCE_FNAME);
		aos_assert_r(fname != "", false);

		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");
		
		OmnString document_type = doc->getAttrStr(AOSTAG_FILE_FORMAT);
		document_type = document_type.toLower();
		if (document_type == "doc" || document_type == "docx" || document_type == "wps" || document_type == "wpt")
		{
	//		doc->setAttr("zky_type_pic", "image_6/do89504623689759.png");
			doc->setAttr(AOSTAG_ZKY_TYPE,"DOC");
		}
		else if (document_type == "pdf")
		{
	//		doc->setAttr("zky_type_pic", "image_6/ao89504623689765.png");
			doc->setAttr(AOSTAG_ZKY_TYPE, "PDF");
		}
		else if (document_type == "xls" || document_type == "xlsx" || document_type == "et")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XLS");
		}
		else if (document_type == "ppt" || document_type == "pptx" || document_type == "dps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "PPT");
		}
		else if (document_type == "txt")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "TXT");
		}
		else if (document_type == "xml" || document_type == "xps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XML");
		}
		else if (document_type == "jpg" || document_type == "jpeg" || 
				 document_type == "bmp" || document_type == "gif" ||
				 document_type == "tif" || document_type == "png")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "IMAGE");
		}
		else if (document_type == "aip" || document_type == "rar" || document_type == "zip")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "RAR");
		}
		else
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "OTHER");
		}

		OmnString cid = rdata->getCid();
		aos_assert_r(cid != "", false);

		AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
		aos_assert_r(user_doc, false);

		OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
		aos_assert_r(user_name != "", false);

		doc->setAttr(AOSTAG_USERNAME, user_name);
	}

	return true;
}

bool
AosActImportUnicomFile::getIILAsms(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr attrs_tag = sdoc->getFirstChild("index_attrs");
	
	if (!attrs_tag)
		return true;

	AosXmlTagPtr attrtag = attrs_tag->getFirstChild();
	while(attrtag)
	{
		OmnString xpath = attrtag->getAttrStr("zky_attrpath");
		aos_assert_r(xpath != "", false);

		OmnString attrname = attrtag->getAttrStr("zky_attrname");
		aos_assert_r(attrname != "", false);
		
		OmnString opr = attrtag->getAttrStr("zky_opr");
		OmnString iiltype = attrtag->getAttrStr("zky_iiltype");
		if (iiltype == "hit")
		{
			mSplitAttrPaths.push_back(xpath);
			mSplitAttrNames.push_back(attrname);
			mSplitAttrWords.insert(make_pair(attrname, opr));
			attrtag = attrs_tag->getNextChild();
			continue;
		}
		if (opr != "" && iiltype == "")
		{
			mOprs.push_back(opr);
			mOprAttrPaths.push_back(xpath);
			mOprAttrNames.push_back(attrname);
		}
		else
		{
			mAttrPaths.push_back(xpath);
			mAttrNames.push_back(attrname);
		}


		map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAsms.find(attrname);
		if (itr == mIILAsms.end())
		{
			AosXmlTagPtr datacol_tag = mTask->getDataColTag(attrname, rdata);     
			aos_assert_r(datacol_tag, false);
			
			AosDataAssemblerObjPtr iilasm = AosDataAssembler::createAssemblerV1(attrname, mTask, datacol_tag, rdata.getPtrNoLock());
			aos_assert_r(iilasm, false);

			if (opr != "")
			{
				mOprIILAsms.insert(make_pair(attrname, iilasm));
			}
			else
			{
				mIILAsms.insert(make_pair(attrname, iilasm));
			}
		}

		attrtag = attrs_tag->getNextChild();
	}
	return true;
}


bool
AosActImportUnicomFile::finishedAction(const AosRundataPtr &rdata)
{
	while(1)
	{
		mLock->lock();
		aos_assert_rl(mRunNum >= 0, mLock, false);
		if (mRunNum == 0)
		{
			mLock->unlock();
			break;
		}
		mLock->unlock();
		OmnSleep(1);
	}
	
//	u64 procLen = 0;
//	u64 procTotal = 0;
	
	asmSendFinish(rdata);

	OmnString str;
	str << "<entries total_len=\"" << 1 << "\" "
		<< "noupdate_flag=\"" << false << "\">";
	std::map<OmnString, OmnString>::iterator it;	
	for (it = mFileStart.begin(); it != mFileStart.end(); it++)
	{
		str << "<entry zky_key=\"" << it->first << "\">" << it->second << "</entry>";
	}

	str << "</entries>";
	OmnScreen << str << endl;

	AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	
OmnScreen << " cccccccccccccc createDocNum : " << mCrtDocNum << endl;
	//Jozhi 2013-11-28
	aos_assert_r(mTask, false);
	mTask->updateTaskProcNum(xml, false, rdata);
	
	AosActionObjPtr thisptr(this, true);
	mTask->actionFinished(thisptr, rdata);

	return true;
}

bool
AosActImportUnicomFile::asmSendFinish(const AosRundataPtr &rdata)
{
	AosRundata *rdata_raw = rdata.getPtrNoLock();
	bool rslt = mDocVarAssembler->sendFinish(rdata_raw);
	aos_assert_r(rslt, false);

	rslt = mHitIILAsm->sendFinish(rdata_raw);
	aos_assert_r(rslt, false);

	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAsms.begin();
	for(; itr != mIILAsms.end(); ++itr)
	{
		rslt = (itr->second)->sendFinish(rdata_raw);
		aos_assert_r(rslt, false);
	}
	
	itr = mOprIILAsms.begin();
	for(; itr != mOprIILAsms.end(); ++itr)
	{
		rslt = (itr->second)->sendFinish(rdata_raw);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosActImportUnicomFile::importDoc(
		const AosXmlTagPtr &binary_doc,
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	int64_t fileidx = mFileIdx;
	mFileIdx++;
	
	OmnString dir = AosGetBaseDirname();
	OmnString source_fname = dir;
	source_fname << "source_" << mTask->getTaskDocid() << "_" << fileidx << ".txt";


	OmnString sfname = binary_doc->getAttrStr(AOSTAG_SOURCE_FNAME);
	aos_assert_r(sfname != "", false);

	AosStrSplit split(sfname, ".");            
	vector<OmnString> strs= split.entriesV();

	OmnString document_type = strs[strs.size()-1];
	aos_assert_r(document_type != "", false);

	OmnFilePtr file = OmnNew OmnFile(source_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->put(0, buff->data(), buff->dataLen(), true);

	OmnString fname = convertWordPdf(dir, source_fname, document_type, fileidx);
	if (fname == "")
	{
		file->deleteFile();
		return true;
	}

	vector<OmnString> has_attrs;
	vector<OmnString> has_values;
	map<OmnString, vector<OmnString> > split_values;
	AosXmlTagPtr predoc = getPreDoc(binary_doc, sfname, has_attrs, has_values, split_values, rdata);	
	aos_assert_r(predoc, false);

	proc(predoc, has_attrs, has_values, split_values, fname, rdata);	

	if (document_type != "txt" || document_type != "TXT")
	{
		file->deleteFile();
	}
	return true;
}

OmnString
AosActImportUnicomFile::convertWordPdf(
		const OmnString &dir,
		const OmnString &source_fname,
		const OmnString &document_type,
		const int64_t fileidx)
{
	OmnString target_fname = dir;
	target_fname << "target_" << mTask->getTaskDocid() << "_" << fileidx << ".txt";

	if (document_type == "doc" || document_type == "DOC")
	{
		OmnString cmd = "antiword ";
		cmd << "\"" << source_fname << "\" > \"" << target_fname << "\"";
		system(cmd.data());
	}
	else if (document_type == "pdf" || document_type == "PDF")
	{
		OmnString cmd = "pdftotext -layout -q ";
		cmd << "\"" << source_fname << "\" \"" << target_fname << "\"";
		system(cmd.data());
	}
	else if (document_type == "txt" || document_type == "TXT")
	{
		return source_fname;
	}
	else
	{
		OmnScreen << "~~~~~~~~ invalid document type " << document_type << endl;
		return "";
	}
	return target_fname;
}


AosXmlTagPtr
AosActImportUnicomFile::getPreDoc(
		const AosXmlTagPtr &doc,
		const OmnString &sfname,
		vector<OmnString> &has_attrs,
		vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	AosStrSplit split(sfname, "/");            
	vector<OmnString> strs= split.entriesV();
	OmnString fname = strs[strs.size()-1];

	OmnString file_objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(file_objid != "", 0);
	
	OmnString document_type = doc->getAttrStr(AOSTAG_ZKY_TYPE, "OTHER");

	OmnString title_objid = doc->getAttrStr("unicom_title_objid", "");
	OmnString title_docid = doc->getAttrStr("unicom_title_docid", "");

	OmnString ctime = OmnGetTime(AosLocale::eUSA);
	OmnString file_ctime = doc->getAttrStr(AOSTAG_CTIME, ctime);

	OmnString predocstr = "";
	u32 systemsec = OmnGetSecond();                   
	predocstr << "<doc "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
		<< AOSTAG_CT_EPOCH << "=\"" << systemsec << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSTAG_DOC_SECTION << "\" "
		<< AOSTAG_PARENTC << "=\"" << mDocumentCtnr << "\" "
		<< AOSTAG_CTIME << "=\"" << ctime << "\" "
		<< "zky_fileobjid=\"" << file_objid << "\" "
		<< "unicom_title_objid=\"" << title_objid << "\" "
		<< "unicom_title_docid=\"" << title_docid << "\" "
		<< AOSTAG_ZKY_TYPE << "=\"" << document_type << "\" "
		<< "zky_file_ctime=\"" << file_ctime << "\" "
		<< "zky_rscfn=\"" << fname << "\" ";

	OmnString cid = rdata->getCid();
	aos_assert_r(cid != "", 0);

	AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	aos_assert_r(user_doc, 0);

	OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
	aos_assert_r(user_name != "", 0);

	predocstr << AOSTAG_USERNAME << "=\"" << user_name << "\" "
			  << AOSTAG_CTNR_PUBLIC << "=\"true\" />";

//??????? attrname is the same, but the path is different, what can i do?

	AosXmlParser xmlparser;
	AosXmlTagPtr predoc = xmlparser.parse(predocstr, "" AosMemoryCheckerArgs);
	aos_assert_r(predoc, 0);

	for(u32 i=0; i<mAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mAttrPaths[i]);
	
		if(mAttrNames[i] == AOSTAG_OTYPE)
		{
			value = AOSTAG_DOC_SECTION;
		}
		else if(mAttrNames[i] == AOSTAG_CT_EPOCH)
		{
			value = "";
			value << systemsec;
		}
		else if(mAttrNames[i] == "zky_fileobjid")
		{
			value = file_objid;
		}
		else if(mAttrNames[i] == AOSTAG_PARENTC)
		{
			value = mDocumentCtnr;
		}
		
		if (value != "")
		{
			predoc->xpathSetAttr(mAttrPaths[i], value, true);
			has_attrs.push_back(mAttrNames[i]);
			has_values.push_back(value);
		}
	}
	
	for(u32 i=0; i<mOprAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mOprAttrPaths[i]);
		if (value != "")
		{
			AosStrSplit split(value, mOprs[i].data());            
			vector<OmnString> strs= split.entriesV();
			mOprValues.insert(make_pair(mOprAttrNames[i], strs));
			predoc->xpathSetAttr(mOprAttrPaths[i], value, true);
		}
	}
	
	for(u32 i=0; i<mSplitAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mSplitAttrPaths[i]);
		if (value != "")
		{
			map<OmnString, OmnString>::iterator split_itr = mSplitAttrWords.find(mSplitAttrNames[i]);
			aos_assert_r(split_itr != mSplitAttrWords.end(), 0);
			vector<OmnString> strs;
			if (split_itr->second != "")
			{
				AosStrSplit split(value, (split_itr->second).data());            
				strs= split.entriesV();
			}
			else
			{
				strs.push_back(value);
			}
			split_values.insert(make_pair(mSplitAttrNames[i], strs));
			predoc->xpathSetAttr(mSplitAttrPaths[i], value, true);
		}
	}

	return predoc;
}


bool
AosActImportUnicomFile::proc(
		const AosXmlTagPtr &predoc,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const OmnString &fname, 
		const AosRundataPtr &rdata)
{
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(ff && ff->isGood(), false);

	OmnString filestr, nextstr;
	int partnum = 1;
	bool finished = false;
	while(!(finished = AosSdocDocument::CreateFileDoc::getFileInfo(filestr, nextstr, ff)))
	{
		if (filestr == "") continue;
	
		bool rslt = asmAppend(filestr, partnum++, predoc, has_attrs, has_values, split_values, rdata);
		aos_assert_r(rslt, false);

		filestr = nextstr;
		nextstr = "";
	}
	
	if (filestr != "")
	{
		bool rslt = asmAppend(filestr, partnum++, predoc, has_attrs, has_values, split_values, rdata);
		aos_assert_r(rslt, false);
	}

	ff->deleteFile();
	return true;
}


bool 
AosActImportUnicomFile::asmAppend(
		const OmnString &filestr,
		const int &partnum,
		const AosXmlTagPtr &predoc,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	u64 docid = getNextDocid(rdata);
	aos_assert_r(docid, false);

	OmnString docstr = composeEntry(docid, filestr, partnum, predoc);
	aos_assert_r(docstr != "", false);

	AosValueRslt valueRslt;
	valueRslt.setDocid(docid);
	valueRslt.setCharStr1(docstr.data(), docstr.length(), false);
	mDocVarAssembler->appendEntry(valueRslt, rdata.getPtrNoLock());	

	iilAppendEntry(docid, has_attrs, has_values, rdata);
	OprIILAppendEntry(docid, predoc, split_values, rdata);

	AosWordParser wordParser;
	//wordParser.setSrc(docstr);
	wordParser.setSrc(filestr);
	OmnString word;

	u64 title_docid = predoc->getAttrU64("unicom_title_docid", 0);
	aos_assert_r(title_docid > 0, false);
	while (wordParser.nextWord(word))
	{
		if (word == "\n")
		{
			continue;
		}
		if (word.length() >= (mRcdLen - (int)sizeof(u64)))
		{
			word.setLength(mRcdLen - (int)sizeof(u64) - 2 );
		}

		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(word.data(), word.length(), true);
		valueRslt1.setDocid(docid);
		
		splitBytes(word, docid, title_docid, rdata);
		
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());

		valueRslt1.setDocid(title_docid);
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());
	}
	return true;
}

void
AosActImportUnicomFile::splitBytes(
		const OmnString &word,
		const u64 &docid,
		const u64 &title_docid,
		const AosRundataPtr &rdata)
{
	const char * a  = word.data();
	//static unsigned char arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 brr[] = {0b00000000, 0b11000000, 0b11100000, 0b11110000};
	int total = 0;
	//OmnScreen << " ###########  hitword : " << word << endl;
	for(int i=0; i<word.length(); )
	{
		int size = 0;
		if ((*a & arr[0]) == brr[0])
		{
			size = 1;
		}
		else if ((*a & arr[1]) == brr[1])
		{
			size = 2;
		}
		else if ((*a & arr[2]) == brr[2])
		{
			size = 3;
		}
		else if((*a & arr[3]) == brr[3])
		{
			size = 4;
		}
		else
		{
			OmnScreen << " Alarm : invalid byte  " << endl;
			return;
		}
	
		total += size;
		if (total > word.length())
		{
			OmnScreen << " Alarm : out of the boundary " << endl;
			return;
		}

		OmnString ss(a, size);
		//OmnScreen << " ###########  hit : " << ss << "  docid : " << docid << endl;

		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(a, size, true);
		valueRslt1.setDocid(docid);
		
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());

		valueRslt1.setDocid(title_docid);
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());
		a += size;
		i += size;
	}
}

void
AosActImportUnicomFile::OprIILAppendEntry(
		const u64 docid,
		const AosXmlTagPtr &predoc,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr;
	map<OmnString, vector<OmnString> >::iterator vitr = mOprValues.begin();
	for(; vitr != mOprValues.end(); ++vitr)
	{
		itr = mOprIILAsms.find(vitr->first);
		aos_assert(itr != mOprIILAsms.end());
	
		vector<OmnString> values = vitr->second;
		for(u32 i=0; i<values.size(); i++)
		{
			AosValueRslt valueRslt1;
			valueRslt1.setCharStr1(values[i].data(), values[i].length(), true);
			valueRslt1.setDocid(docid);
			(itr->second)->appendEntry(valueRslt1, rdata.getPtrNoLock());
		}
	}
	
	u64 title_docid = predoc->getAttrU64("unicom_title_docid", 0);
	aos_assert(title_docid > 0);
	
	map<OmnString, vector<OmnString> >::iterator split_vitr = split_values.begin();
	for(; split_vitr != split_values.end(); ++split_vitr)
	{
		OmnString attrname = split_vitr->first;
		vector<OmnString> strs = split_vitr->second;
		addAttrWordIIL(attrname, strs, docid, title_docid, rdata);
	}
}


void AosActImportUnicomFile::onThreadInit(const AosRundataPtr &, void **data)
{
	mLock->lock();
	long tid = mThreadId;
	mThreadId++;
	mLock->unlock();
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)tid);
}

void AosActImportUnicomFile::onThreadExit(const AosRundataPtr &, void **data)
{
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)0);
}

bool
AosActImportUnicomFile::addAttrWordIIL(
		const OmnString &attrname,
		const vector<OmnString> &values,
		const u64 &docid,
		const u64 &title_docid,
		const AosRundataPtr &rdata)
{
	vector<OmnString> attrwords;
    
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	void* udata = thread->getUdata();
	int logicId = (long)udata;
	aos_assert_r(logicId < eMaxThrdIds, false);

	for(u32 i=0; i<values.size(); i++)
	{
		if (values[i] == "") continue;
		mWords[logicId].reset();
		bool rslt = AosDocProc::getSelf()->collectWords(mWords[logicId], mWordParser[logicId], mWordNorm[logicId], values[i]);
		aos_assert_r(rslt, false);

		OmnString word;
		AosEntryMark::E mark;
		while(mWords[logicId].nextEntry(word, mark))
		{
//			OmnString iilname = AosDocProc::getSelf()->getAttrWordIILName(mark, "", attrname, word);
			OmnString iilname = AosIILName::composeAttrWordIILName(attrname, word);
			if (iilname == "") continue;

			if (iilname.length() > eMaxStrLen)
			{
				OmnAlarm << "iilname is too long " << iilname << enderr;
				continue;
			}

			AosValueRslt valueRslt1;
			valueRslt1.setCharStr1(iilname.data(), iilname.length(), true);
			valueRslt1.setDocid(docid);
			//OmnScreen << "@@@@@@  hitiilasm key : " << iilname << " docid : " << docid << endl;
			mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());

			valueRslt1.setDocid(title_docid);
			mHitIILAsm->appendEntry(valueRslt1, rdata.getPtrNoLock());
		}
	}
	return true;
}

void
AosActImportUnicomFile::iilAppendEntry(
		const u64 docid,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr;
	for(u32 i=0; i<has_attrs.size(); i++)
	{
		itr = mIILAsms.find(has_attrs[i]);
		aos_assert(itr != mIILAsms.end());
		
		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(has_values[i].data(), has_values[i].length(), true);
		valueRslt1.setDocid(docid);
		(itr->second)->appendEntry(valueRslt1, rdata.getPtrNoLock());
	}
}

OmnString
AosActImportUnicomFile::composeEntry(
		const u64 &docid,
		const OmnString &filestr, 
		const int partnum,
		const AosXmlTagPtr &predoc)
{
	AosXmlTagPtr doc = predoc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(doc, "");

	doc->setAttr("zky_partnum", partnum);
	doc->setAttr(AOSTAG_DOCID, docid);

	doc->setNodeText("zky_context", filestr, true);

	OmnString docstr = doc->toString();
	return docstr;
}


u64
AosActImportUnicomFile::getNextDocid(const AosRundataPtr &rdata)
{
	OmnString objid = "";
	u64 docid = AosDocidMgrObj::getDocidMgr()->nextDocid(objid, rdata);
	return docid;
}

#endif
