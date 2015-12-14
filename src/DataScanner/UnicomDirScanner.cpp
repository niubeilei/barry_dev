////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/UnicomDirScanner.h"

#include "SEInterfaces/NetFileCltObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/BuffData.h"
#include "Porting/Sleep.h"
#include "API/AosApi.h"
#include "Util/UtUtil.h"
#include "Util/File.h"
#include "JobTrans/UnicomReadFileTrans.h"
#include <dirent.h>

const OmnString zipFormat = ".eippack";

AosUnicomDirScanner::AosUnicomDirScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_UNICOMDIR, AosDataScannerType::eUnicomDir, flag),
mStart(0),
mReadLength(0),
mPhysicalid(-1),
mCrtIdx(0),
mRecursion(false),
mCrtFileName(""),
mCrtFileLen(0),
mCrtScannerItr(mScanners.begin()),
mLock(OmnNew OmnMutex()),
mReadToSem(OmnNew OmnSem(0)),
mReadFromSem(OmnNew OmnSem(0)),
mNoMoreData(false),
mIsUnicomFile(false),
mIgnoreHead(false)
{
}


AosUnicomDirScanner::AosUnicomDirScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_UNICOMDIR, AosDataScannerType::eUnicomDir, false),
mStart(0),
mReadLength(0),
mPhysicalid(-1),
mCrtIdx(0),
mRecursion(false),
mCrtFileName(""),
mCrtFileLen(0),
mCrtScannerItr(mScanners.begin()),
mLock(OmnNew OmnMutex()),
mReadToSem(OmnNew OmnSem(0)),
mReadFromSem(OmnNew OmnSem(0)),
mNoMoreData(false),
mIsUnicomFile(false),
mIgnoreHead(false)
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosUnicomDirScanner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


AosUnicomDirScanner::~AosUnicomDirScanner()
{
	//OmnScreen << "dir scanner deleted" << endl;
}


AosJimoPtr
AosUnicomDirScanner::cloneJimo()  const
{
	try
	{
		return OmnNew AosUnicomDirScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosUnicomDirScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosUnicomDirScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosUnicomDirScanner::isFinished(const AosRundataPtr &rdata)
{
	return mNoMoreData;
}


AosDataScannerObjPtr
AosUnicomDirScanner::getScanner(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mScanners.size() == 0)
	{
		mLock->unlock();
		return 0;
	}

	AosDataScannerObjPtr scanner = *mCrtScannerItr;
	aos_assert_rl(scanner, mLock, 0);
	while(scanner->isFinished(rdata))
	{
		mScanners.erase(mCrtScannerItr);
		if (mScanners.size() == 0)
		{
			mLock->unlock();
			return 0;
		}
		mCrtScannerItr = mScanners.begin();
		scanner = *mCrtScannerItr;
	}

	mCrtScannerItr++;
	if (mCrtScannerItr == mScanners.end())
	{
		mCrtScannerItr = mScanners.begin();
	}
	mLock->unlock();
	return scanner;
}


bool
AosUnicomDirScanner::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (!info)
	{
		info = OmnNew AosBuffData();
	}
	aos_assert_r(info, false);
	AosDataScannerObjPtr scanner = getScanner(rdata);
	if (scanner)
	{
		scanner->readBuffFromQueue(info, rdata);
	}
	return true;
}

bool
AosUnicomDirScanner::getNextBlock(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	AosBuffDataPtr info = OmnNew AosBuffData();
	AosDataScannerObjPtr scanner = getScanner(rdata);
	if (scanner)
	{
		scanner->readBuffFromQueue(info, rdata);
	}
	if (info)
	{
		buff = info->getBuff();
	}
	else
	{
		buff = 0;
	}
	return true;
}

bool
AosUnicomDirScanner::readBuffFromQueue(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mNoMoreData)
	{
		if (mPrimaryBuff)
		{
			info = mPrimaryBuff;
			mPrimaryBuff = 0;
		}
		else
		{
			info = 0;
		}
		mLock->unlock();
		return true;
	}
	while(!mPrimaryBuff)
	{
		mReadToSem->post();
		if (mNoMoreData)
		{
			break;
		}
		mLock->unlock();
		//mReadFromSem->wait();
		bool timeout;
		mReadFromSem->timedWait(1000, timeout);
		mLock->lock();
	}
	if (mNoMoreData)
	{
		if (mPrimaryBuff)
		{
			info = mPrimaryBuff;
			mPrimaryBuff = 0;
		}
		else
		{
			info = 0;
		}
		mLock->unlock();
		return true;
	}
	info = mPrimaryBuff;
	mPrimaryBuff = 0;
	mReadToSem->post();
	mLock->unlock();
	return true;
}


bool
AosUnicomDirScanner::readBuffToQueue(
		OmnThrdStatus::E &state, 
		const AosRundataPtr &rdata)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mNoMoreData)
		{
			state = OmnThrdStatus::eExit;
			mLock->unlock();
			return true;
		}
		if (mPrimaryBuff)
		{
			mLock->unlock();
			//mReadToSem->wait();
			bool timeout;
			mReadToSem->timedWait(1000, timeout);
			continue;
		}
		else
		{
			mLock->unlock();
		}

		AosBuffDataPtr info = OmnNew AosBuffData();
		bool rslt = false;
		if (mRowDelimiter == "")
		{
			rslt = readUnicomFileBuff(info, rdata);
		}

		mLock->lock();
		//if ((mIsUnicomFile && (!rslt || !info || (info->getVectorBuffs().size() <=0))))
		if ((mIsUnicomFile && (!rslt || !info )))
		{
			if (!mPrimaryBuff)
			{
				mNoMoreData = true;
				state = OmnThrdStatus::eExit;
			}
			mReadFromSem->post();
			mLock->unlock();
			return true;
		}

		if (!mPrimaryBuff)
		{
			mPrimaryBuff = info;
		}

		mReadFromSem->post();
		mLock->unlock();
	}
	return true;
}


bool
AosUnicomDirScanner::readUnicomFileBuff(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (mFileList.size() == 0 || mCrtIdx >= mFileList.size())
	{   
		/*
		if (mCrtIdx >= mFileList.size())
		{
			OmnAlarm << " mCrtIdx : " << mCrtIdx << " filesize : " << mFileList.size() << enderr;
		}
		*/
		return false;
	}

	mCrtFileName = mFileList[mCrtIdx].mFileName;
	mCrtFileLen = mFileList[mCrtIdx].mFileLen;

	AosTransPtr trans = OmnNew AosUnicomReadFileTrans(mCrtFileName, mPhysicalid, false, false);
	AosBuffPtr resp;

	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	//aos_assert_r(rslt, false);
	if (!rslt)
	{
		OmnAlarm << "send trans error!" << enderr;
		return false;
	}

	if (timeout)
	{
		AosSetErrorU(rdata, "Faild to add the trans, timeout");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (!resp|| resp->dataLen() < 0)
	{
		AosSetErrorU(rdata, "no response");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int numbuffs = resp->getInt(0);

	u32 len = resp->getU32(0);
	AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs); 
	aos_assert_r(buff, false);

	AosXmlTagPtr unicomtitle_doc = AosXmlParser::parse(buff->data(), len AosMemoryCheckerArgs);
	aos_assert_r(unicomtitle_doc, false);
	info->addMetadata(unicomtitle_doc);

	bool zip_error = unicomtitle_doc->getAttrBool("zip_error", false);
	if (zip_error)
	{
		mCrtIdx++;
		return true;
	}

	len = resp->getU32(0);
	AosBuffPtr zip_buff = resp->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_r(zip_buff, false);
	info->setBuff(zip_buff);

	vector<AosBuffPtr> new_buffs;
	vector<AosXmlTagPtr> new_xmls;
	for(int i=0; i<numbuffs; i++)
	{
		len = resp->getU32(0);
		buff = resp->getBuff(len, false AosMemoryCheckerArgs);
		
		AosXmlTagPtr tmp_doc = AosXmlParser::parse(buff->data(), len AosMemoryCheckerArgs);
		aos_assert_r(tmp_doc, false);

		len = resp->getU32(0);
		AosBuffPtr tmp_buff = resp->getBuff(len, true AosMemoryCheckerArgs);
		aos_assert_r(buff, false);

		new_xmls.push_back(tmp_doc);
		new_buffs.push_back(tmp_buff);
	}
	info->setVectorDocs(new_xmls);
	info->setVectorBuffs(new_buffs);
	
	mCrtIdx++;
	return true;
}


vector<AosXmlTagPtr>
AosUnicomDirScanner::getDomainItem(
		const AosXmlTagPtr &doc,
		int loopNum,
		const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> items;
	AosXmlTagPtr tmpDoc = doc->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr document_tag;
	AosXmlTagPtr item = tmpDoc;
	OmnString vv = tmpDoc->getAttrStr("name", "");
	bool tag = false;
	while(item && tmpDoc && (0 < loopNum))
	{
		OmnString tagname = tmpDoc->getTagname();
		if (tagname != "item" && !tag)
		{
			if (tagname == "document") 
			{
				tag = true;
				document_tag = tmpDoc;
				continue;
			}
			tmpDoc = tmpDoc->getFirstChild(true);
			loopNum--;
			continue;
		}
		if (tag && document_tag)	
		{
			item = document_tag->getNextChild();
			if (!item) continue;
			vv = item->getAttrStr("name", "");
			if (vv == AOSTAG_FLREVIEWEDJOBOS || 
				vv == AOSTAG_LOOKLEADER ||
				vv == AOSTAG_OTHERREADERS)
			{
				items.push_back(item);
			}
			continue;
		}
	}
	return items;
}


bool 
AosUnicomDirScanner::readXmlFindFile(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		AosBuffPtr &zip_buff,
		vector<AosBuffPtr> &new_buffs,
		vector<AosXmlTagPtr> &new_xmls)
{
	OmnScreen << " UnicomDirScanner starc :" << endl;
	//doc xml format
	//<SIP>
	//  <PackageDescription>
	//  .........
	//  </PackageDescription>
	//  <Package>
	//      <Description>
	//      ..........
	//      </Description>
	//      <Content>
	//          <Package>
	//              <Description>
	//              .............
	//              </Description>
	//          </Package>
	//      </Content>
	//      ............
	//  </Package>
	//</SIP>
	aos_assert_r(doc, false);
	new_buffs.clear();
	new_xmls.clear();
	
	doc->removeAttr("xmlns");
	doc->removeAttr("xmlns:xsi");
	doc->removeAttr("xsi:schemaLocation");   
	AosXmlTagPtr allDoc = doc->getFirstChild("AllDoc");
	aos_assert_r(allDoc, false); 
	int loopNum = 50;

	vector<AosXmlTagPtr> items = getDomainItem(allDoc, loopNum, rdata);
	aos_assert_r(items.size() > 0, false);

	OmnString vv;
	for (u32 i = 0; i < items.size(); i++)
	{
		AosXmlTagPtr textList = items[i]->getFirstChild(true);
		AosXmlTagPtr text = textList->getFirstChild(true);
		
		while(text)
		{
			if (vv != "") vv << ",";
			vv << (text->getNodeText());
			text = textList->getNextChild();
		}
	}

	doc->setAttr(AOSTAG_UN_READ_PERMISSION, vv);

	AosXmlTagPtr child_doc = doc->getFirstChild(true);
	OmnString zip_file_format = child_doc->getFirstChild("PackageFormat")->getNodeText();
	if(zip_file_format != "ZIP") return false;
	OmnString zip_file_path = child_doc->getFirstChild("PackageName")->getNodeText();
	OmnString newDir = zip_file_path;
OmnScreen << "DirScanner zip_file_path is : " << zip_file_path << endl;
	OmnString zip_file_name = zip_file_path;
	zip_file_name << ".zip";
	zip_file_path << zipFormat;
	OmnString path;
	unsigned found = mCrtFileName.indexOf(0, '/', true);
	path = mCrtFileName.substr(0, found);      
	path << zip_file_path;

OmnScreen << "@@@@ path : " << path << " zip_file_path : " << zip_file_path << endl;

	AosDiskStat disk_stat;
	int64_t filelen = 0;
	bool rslt = AosNetFileCltObj::getFileLengthStatic(
		path, mPhysicalid, filelen, disk_stat, rdata.getPtr());
	aos_assert_r(rslt, false);
	
	rslt = AosNetFileCltObj::readFileToBuffStatic(
		path, mPhysicalid, 0, filelen, zip_buff, disk_stat, rdata.getPtr());
	aos_assert_r(rslt, false); 
	OmnString new_zip_fname = writeLocalFile(zip_file_name, zip_buff, rdata);
	OmnFile zip_file(new_zip_fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
 	int zip_buff_size = zip_file.getLength();
	aos_assert_r(zip_buff_size > 0, false);

	// delete zip_file
	bool svr_death;
	rslt = AosNetFileCltObj::deleteFileStatic(
		path, mPhysicalid, svr_death, rdata.getPtr());
	aos_assert_r(rslt, false); 

	//felicia, 2013/08/27  for download
	doc->setAttr("zky_binarydoc", "blockfile");
	doc->setAttr(AOSTAG_SOURCE_LENGTH, zip_buff_size);
	doc->setAttr(AOSTAG_SOURCE_FNAME, zip_file_name);
	doc->setAttr(AOSTAG_RSC_FNAME, zip_file_name);		
	doc->setAttr("unicom_file_name", newDir);		
	doc->setAttr(AOSTAG_RSCDIR, "/tmp");
	
	OmnString unzip_file_path = unzip(new_zip_fname, newDir);
	//doc->removeNode("AllDoc", true, true);
	AosXmlTagPtr xml_template = doc->clone(AosMemoryCheckerArgsBegin);
	
	AosXmlTagPtr tag_package = xml_template->getChild(1);
	AosXmlTagPtr tag_content = tag_package->getChild(1);
	AosXmlTagPtr files_tag = tag_content->clone(AosMemoryCheckerArgsBegin);
	tag_content->removeAllChildren();

	AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
	while(file_tag)
	{
		//file_tag
		//<Package>
		//	<Descriptioni>
		//		<OriginalFileName>xxxx</OriginalFileName>
		//		<DocumentType></DocumentType>
		//		<FileSize></FileSize>
		//		<FileTimeStamp></FileTimeStamp>
		//		<FileChecksum></FileChecksum>
		//		<FileFormat></FileFormat>
		//	</Descriptioni>
		//</Package>
		
		//file_tag->unescapeCdataContents();
		AosXmlTagPtr file_tag_c = file_tag->clone(AosMemoryCheckerArgsBegin);
		AosXmlTagPtr fname_tag = (file_tag->getFirstChild(true))->getFirstChild(true);
		fname_tag->unescapeCdataContents();
OmnScreen << "unescapeCdataContents 1 :: " << fname_tag->getNodeText() << endl;
		fname_tag->unescapeCdataContents();
OmnScreen << "unescapeCdataContents 2 :: " << fname_tag->getNodeText() << endl;
		OmnString fname = fname_tag->getNodeText();

		OmnString fileChecksum = ((file_tag->getFirstChild(true))->getFirstChild("FileChecksum"))->getNodeText();

		OmnString fileFormat = ((file_tag->getFirstChild(true))->getFirstChild("FileFormat"))->getNodeText();

		if (fileFormat != "")
		{
			fname << "." << fileFormat;
		}
		OmnString file_path;
		file_path << unzip_file_path << "/" <<fname;	

		OmnFile file(file_path, OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!file.isGood())
		{
			OmnAlarm << " file is not good " << file_path << enderr;
			doc->setAttr("zky_unicom_file_error", 202);
			zip_file.deleteFile();
			return true;
		}

		int buff_size = file.getLength();
		
		AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t data_len = buff_size;
		int64_t read_len = file.readToBuff(0, data_len, data);

		aos_assert_r(read_len == buff_size, false);
		buff->setDataLen(data_len);

		OmnString filestr(buff->data(), buff->dataLen());

		
		OmnString mdstr = AosMD5Encrypt(filestr);	
		if (mdstr != fileChecksum)
		{
			OmnAlarm << " fileCheckSum error " << file_path << enderr;
			doc->setAttr("zky_unicom_file_error", 303);
		//	system("mkdir -p /tmp/MDFERROR/");
		//	OmnFile file("/tmp/MDFERROR/md5error.txt", OmnFile::eWriteCreate AosMemoryCheckerArgs);
		//	file.append(file_path, true);
		//	file.append("\n", 2, true);
			zip_file.deleteFile();
			return true;
		}

		AosXmlTagPtr binary_doc = xml_template->clone(AosMemoryCheckerArgsBegin);
		binary_doc->removeNode("AllDoc", true, true);

		((binary_doc->getChild(1))->getChild(1))->addNode(file_tag_c);
		
		binary_doc->setAttr(AOSTAG_UN_READ_PERMISSION, vv);
		binary_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		binary_doc->setAttr(AOSTAG_FILE_FORMAT, fileFormat);
		binary_doc->setAttr(AOSTAG_SOURCE_LENGTH, buff_size);
		binary_doc->setAttr(AOSTAG_SOURCE_FNAME, fname);

		mLoopTmp = 100;
		bool rstl = xmlParse(rdata, binary_doc);
		aos_assert_r(rstl, false);

		new_buffs.push_back(buff);
		new_xmls.push_back(binary_doc);
		file_tag = files_tag->getNextChild();
	}
	zip_file.deleteFile();

	return true;
}


OmnString 
AosUnicomDirScanner::unzip(
		OmnString &zipPath,
		const OmnString &fname)                                
{                                                                  
	aos_assert_r(zipPath != "", "");                            

	unsigned found = zipPath.indexOf(0, '/', true);  
	int len = zipPath.length(); 
	OmnString path = zipPath.substr(0, found);                 
	OmnString file = zipPath.substr(found+1, len);                  

	unsigned tmpf = file.indexOf(0, '.', true);                        
	OmnString dirname = file.substr(0, tmpf-1);                         
	
	OmnString newDir;
	newDir << path << fname;
	mRmDataDir = newDir;
OmnScreen << " UnicomDirScanner created Dir : " << newDir << endl;
	OmnString cmdCreateDir;
	cmdCreateDir << "mkdir " << newDir;
OmnScreen << " UnicomDirScanner created Dir : " << cmdCreateDir << endl;
	system(cmdCreateDir.data()); 

	OmnString dirPath; 
	dirPath << path << dirname; 
	OmnString cmdUnzip; 
	cmdUnzip << "unzip -oO CP936 " << zipPath << " -d " << newDir; 
OmnScreen << "UnicomDirScanner cmdUnzip is : " << cmdUnzip << endl;
	system(cmdUnzip.data()); 
	return newDir;                                             
}                                                                  


bool
AosUnicomDirScanner::xmlParse(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &xml)
{
	aos_assert_rr(xml, rdata, false);
	AosXmlTagPtr doc = xml->getFirstChild(true);
	while (doc && (mLoopTmp--))
	{
		if(doc->getFirstChild(true))
		{
			xmlParse(rdata, doc);
			doc = xml->getNextChild();
			continue;
		}
		OmnString name = doc->getTagname();
		aos_assert_rr(name != "", rdata, false);

		OmnString value = doc->getNodeText();
		
		if (value != "")
		{
			value.replace("\"", "&quot;", true);
			xml->setAttr(name, value);
		}

		doc = doc->getNextSibling();
		xml->removeNode(name, false, false);
	}
	return true;
}


OmnString 
AosUnicomDirScanner::writeLocalFile(
		const OmnString file_name,
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	OmnString new_file_path;
	new_file_path << "/tmp/" << file_name;
	OmnFile file(new_file_path, OmnFile::eWriteCreate AosMemoryCheckerArgs); 
	file.write(0, buff->data(), buff->dataLen());
	file.closeFile();
	return new_file_path;
}


bool
AosUnicomDirScanner::readWholeBuff(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (mFileList.size() == 0 || mCrtIdx >= mFileList.size())
	{
		return false;
	}
	mCrtFileName = mFileList[mCrtIdx].mFileName;
	mCrtFileLen = mFileList[mCrtIdx].mFileLen;

	AosBuffPtr buff;
	int64_t bytes_to_read = mCrtFileLen;
	AosDiskStat disk_stat;
	bool rslt = AosReadDataFile(rdata.getPtr(), buff, mPhysicalid, mCrtFileName, 
			mStart, bytes_to_read, disk_stat);
	aos_assert_rr(rslt, rdata, false);
	info->setBuff(buff);
	info->setFileInfo(mFileList[mCrtIdx]);
	info->setCrtBlockIdx(mFileList[mCrtIdx].getCrtBlockIdx());
	mCrtIdx++;
	return true;
}


bool 
AosUnicomDirScanner::createDataScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const vector<AosFileInfo> &filelist,
		const AosRundataPtr &rdata)
{
	try
	{
		AosUnicomDirScanner* scanner = OmnNew AosUnicomDirScanner(false);
		scanner->mDirNames = mDirNames;
		scanner->mFileList = filelist;
		scanner->mPhysicalid = mPhysicalid;
		scanner->mRowDelimiter = mRowDelimiter;
		scanner->mIsUnicomFile = mIsUnicomFile;
		scanner->mIgnoreHead = mIgnoreHead;
		scanner->mCharacterType = mCharacterType;
		scanners.push_back(scanner);
	}
	catch(...)
	{
		OmnAlarm << "Faild to create UnicomDirScanner" << enderr;
		return false;
	}
	return true;
}


bool 
AosUnicomDirScanner::AosReaderThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	bool rslt = mUnicomDirScanner->readBuffToQueue(state, mRundata);
	if (state == OmnThrdStatus::eExit)
	{
		mUnicomDirScanner = 0;
		mThread = 0;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosUnicomDirScanner::AosReaderThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosUnicomDirScanner::AosReaderThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosUnicomDirScanner::initUnicomDirScanner(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	mIsUnicomFile = true;
	bool rslt = initUnicomDirScanner(fileinfos, physical_id,
		false, character_type, row_delimiter, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosUnicomDirScanner::initUnicomDirScanner(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	aos_assert_r(fileinfos.size() > 0, false);

	mPhysicalid = physical_id;
	aos_assert_r(mPhysicalid != -1, false);

	mIgnoreHead = ignore_head;
	mCharacterType = character_type;
	mRowDelimiter = row_delimiter;

	map<int64_t, vector<AosFileInfo> > infosMap;
	map<int64_t, vector<AosFileInfo> >::iterator infosItr;
	int64_t devid = 0;
	for(u32 i=0; i<fileinfos.size(); i++)
	{
//		devid = fileinfos[i].mDevId;
		infosItr = infosMap.find(devid);
		if (infosItr != infosMap.end())
		{
			(infosItr->second).push_back(fileinfos[i]);
		}
		else
		{
			vector<AosFileInfo> list;
			list.push_back(fileinfos[i]);
			infosMap[devid] = list;
		}
	}

	bool rslt = false;
	for(infosItr = infosMap.begin(); infosItr != infosMap.end(); infosItr++)
	{
		rslt = createDataScanner(mScanners, infosItr->second, rdata);
		aos_assert_r(rslt, false);
	}

	mCrtScannerItr = mScanners.begin();
	for(u32 i=0; i<mScanners.size(); i++)
	{
		OmnSPtr<AosUnicomDirScanner::AosReaderThrd> reader = OmnNew AosReaderThrd(mScanners[i], rdata);
	}
	return true;
}


bool 
AosUnicomDirScanner::initUnicomDirScanner(
		vector<OmnString> &dirnames,
		const int physical_id,
		const bool recursion,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	mDirNames = dirnames;
	aos_assert_r(mDirNames.size() > 0, false);

	mPhysicalid = physical_id;
	aos_assert_r(mPhysicalid != -1, false);
	
	mRecursion = recursion;
	mIgnoreHead = ignore_head;
	mCharacterType = character_type;
	mRowDelimiter = row_delimiter;

	vector<AosFileInfo> filelist;
	for(u32 i=0; i<mDirNames.size(); i++)
	{
		filelist.clear();
		AosNetFileCltObj::getFileListStatic(
			mDirNames[i], mPhysicalid, recursion?-1:0, filelist, rdata.getPtr());
		if (filelist.size() != 0)
		{
			createDataScanner(mScanners, filelist, rdata);
		}
	}
	mCrtScannerItr = mScanners.begin();
	for(u32 i=0; i<mScanners.size(); i++)
	{
		OmnSPtr<AosUnicomDirScanner::AosReaderThrd> reader = OmnNew AosReaderThrd(mScanners[i], rdata);
	}
	return true;
}


int64_t
AosUnicomDirScanner::getTotalSize() const
{
	int64_t len = 0;
	for (u32 i=0; i<mScanners.size(); i++)
	{
		len += mScanners[i]->getTotalFileLengthByDir();
	}
	return len;
}


int
AosUnicomDirScanner::getPhysicalId() const
{
	OmnShouldNeverComeHere;
	return -1;
}
#endif
