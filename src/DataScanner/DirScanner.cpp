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
#include "DataScanner/DirScanner.h"

#include "SEInterfaces/NetFileCltObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/BuffData.h"
#include "Porting/Sleep.h"
#include "API/AosApi.h"
#include "Util/UtUtil.h"
#include "Util/File.h"
#include <dirent.h>

const OmnString zipFormat = ".eippack";

AosDirScanner::AosDirScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_DIR, AosDataScannerType::eDir, flag),
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


AosDirScanner::AosDirScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_DIR, AosDataScannerType::eDir, false),
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
AosDirScanner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


AosDirScanner::~AosDirScanner()
{
	//OmnScreen << "dir scanner deleted" << endl;
}


AosJimoPtr
AosDirScanner::cloneJimo()  const
{
	try
	{
		return OmnNew AosDirScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosDirScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDirScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosDirScanner::isFinished(const AosRundataPtr &rdata)
{
	return mNoMoreData;
}


AosDataScannerObjPtr
AosDirScanner::getScanner(const AosRundataPtr &rdata)
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
AosDirScanner::getNextBlock(
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
AosDirScanner::getNextBlock(
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
AosDirScanner::readBuffFromQueue(
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
AosDirScanner::readBuffToQueue(
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
			if (!mIsUnicomFile)
			{
				rslt = readWholeBuff(info, rdata);
			}
			else
			{
				rslt = readUnicomFileBuff(info, rdata);
			}
		}
		else
		{
			rslt = readBuff(info, rdata);
		}

		mLock->lock();

		if (!mIsUnicomFile && (!rslt || !info || !info->getBuff() || (info->getBuff())->dataLen() <= 0))
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
AosDirScanner::smartReadBuff(
		const AosRundataPtr &rdata,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat)
{
	buff = 0;
	if (mFileList.size() == 0 || mCrtIdx >= mFileList.size())
	{
		return false;
	}
	aos_assert_r(mStart >= 0, false);
	int64_t bytes_to_read = eMaxBuffSize;
	int64_t buff_len = 0;
	while(true)
	{
		if (mCrtIdx >= mFileList.size())
		{
			break;
		}
		if (buff && buff_len >= eMinBuffSize)
		{
			break;
		}
		mCrtFileName = mFileList[mCrtIdx].mFileName;
		mCrtFileLen = mFileList[mCrtIdx].mFileLen;
		if (mReadLength >= mCrtFileLen)
		{
			mCrtIdx++;
			if (mCrtIdx >= mFileList.size())
			{
				break;
			}
			mReadLength = 0;
			mStart = 0;
			mCrtFileName = mFileList[mCrtIdx].mFileName;
			mCrtFileLen = mFileList[mCrtIdx].mFileLen;
		}

		if (buff_len < eMaxBuffSize)
		{
			bytes_to_read = eMaxBuffSize - buff_len;
		}

		if (bytes_to_read > mCrtFileLen - mReadLength)	
		{
			bytes_to_read = mCrtFileLen - mReadLength;
		}

		AosBuffPtr tmpbuff;
		bool rslt = AosReadDataFile(rdata.getPtr(), tmpbuff, mPhysicalid, mCrtFileName, 
				mStart, bytes_to_read, disk_stat);
		aos_assert_rr(rslt, rdata, false);
		char* data = tmpbuff->data();
		aos_assert_r(mRowDelimiter != "", false);
		char* last = strrstr(data, tmpbuff->dataLen(), mRowDelimiter.data());
		if(last)
		{
			char* end = last + mRowDelimiter.length();
			u32 real_len = end - data;
			if(real_len > bytes_to_read)
			{
				OmnAlarm << "read len error" <<enderr;
				real_len = bytes_to_read;
			}
			tmpbuff->setDataLen(real_len);
		}
		u32 bytes_read = tmpbuff->dataLen();
		mReadLength += bytes_read;
		mStart += bytes_read;
		if (tmpbuff->dataLen() >= eMinBuffSize)
		{
			buff = tmpbuff;
			break;
		}
		else
		{
			if (!buff)
			{
				buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			}
			buff->appendBuff(tmpbuff);
			buff_len = buff->dataLen();
		}
	}
	return true;
}


bool
AosDirScanner::readUnicomFileBuff(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (mFileList.size() == 0 || mCrtIdx >= mFileList.size())
	{   
		if (mCrtIdx >= mFileList.size())
		{
			OmnAlarm << " mCrtIdx : " << mCrtIdx << " filesize : " << mFileList.size() << enderr;
		}
		return false;
	}

	mCrtFileName = mFileList[mCrtIdx].mFileName;
	mCrtFileLen = mFileList[mCrtIdx].mFileLen;

	OmnFile file(mCrtFileName, OmnFile::eReadOnly AosMemoryCheckerArgs); 
	OmnString str;
	bool rslt = file.readToString(str); 
	aos_assert_r(rslt, false); 

	unsigned found = str.indexOf(0, '\n', false); 
	str = str.substr(found+1, str.length()-1); 
	AosXmlTagPtr file_xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(file_xml, false);

	vector<AosBuffPtr> new_buffs;
	vector<AosXmlTagPtr> new_xmls;
	AosBuffPtr zip_buff;
	rslt = readXmlFindFile(rdata, file_xml, zip_buff, new_buffs, new_xmls); 
	OmnString cmdRmDir;
	cmdRmDir << "rm -rf " << mRmDataDir;
	if (mRmDataDir != "" && mRmDataDir != "/" && mRmDataDir != "~/");
	{
		system(cmdRmDir.data());
	}

	if (rslt)
	{
		info->setVectorDocs(new_xmls);
		info->setVectorBuffs(new_buffs);
	}
	AosXmlTagPtr unicomtitle_doc = file_xml->clone(AosMemoryCheckerArgsBegin);
	
	mLoopTmp = 100;
	bool rstl = xmlParse(rdata, unicomtitle_doc);
	aos_assert_r(rstl, false);
	info->addMetadata(unicomtitle_doc);
	info->setBuff(zip_buff);
	file.deleteFile();
	mCrtIdx++;
	return true;
}


vector<AosXmlTagPtr>
AosDirScanner::getDomainItem(
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
AosDirScanner::readXmlFindFile(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		AosBuffPtr &zip_buff,
		vector<AosBuffPtr> &new_buffs,
		vector<AosXmlTagPtr> &new_xmls)
{
	OmnScreen << " DirScanner starc :" << endl;
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
	OmnFile zip_file(path, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(zip_file.getLength() > 0, false);
	int zip_buff_size = zip_file.getLength();
	zip_buff = OmnNew AosBuff(zip_buff_size AosMemoryCheckerArgs);
	zip_file.readToBuff(0, zip_buff_size, zip_buff->data());
	zip_buff->setDataLen(zip_buff_size);
	
	//felicia, 2013/08/27  for download
	doc->setAttr("zky_binarydoc", "blockfile");
	doc->setAttr(AOSTAG_SOURCE_LENGTH, zip_buff_size);
	doc->setAttr(AOSTAG_SOURCE_FNAME, zip_file_name);
	doc->setAttr(AOSTAG_RSC_FNAME, zip_file_name);		
	doc->setAttr("unicom_file_name", newDir);		
	doc->setAttr(AOSTAG_RSCDIR, "/tmp");
	
	OmnString unzip_file_path = unzip(path, newDir);
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
AosDirScanner::unzip(
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
OmnScreen << " DirScanner created Dir : " << newDir << endl;
	OmnString cmdCreateDir;
	cmdCreateDir << "mkdir " << newDir;
OmnScreen << " DirScanner created Dir : " << cmdCreateDir << endl;
	system(cmdCreateDir.data()); 

	OmnString dirPath; 
	dirPath << path << dirname; 
	OmnString cmdUnzip; 
	cmdUnzip << "unzip -oO CP936 " << zipPath << " -d " << newDir; 
OmnScreen << "DirScanner cmdUnzip is : " << cmdUnzip << endl;
	system(cmdUnzip.data()); 
	return newDir;                                             
}                                                                  


bool
AosDirScanner::xmlParse(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &xml)
{
	aos_assert_rr(xml, rdata, false);
	AosXmlTagPtr doc = xml->getFirstChild(true);
	while (doc && (mLoopTmp--))
	{
		if((doc->getTagname()) == "AllDoc") 
		{
			doc = xml->getNextChild();
			continue;
		}

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
		xml->setAttr(name, value);

		doc = doc->getNextSibling();
		xml->removeNode(name, false, false);
	}
	return true;
}


bool
AosDirScanner::readWholeBuff(
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
AosDirScanner::readBuff(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	aos_assert_r(info, false);
	if (mFileList.size() == 0 || mCrtIdx >= mFileList.size())
	{
		return false;
	}
	aos_assert_r(mStart >= 0, false);
	mCrtFileName = mFileList[mCrtIdx].mFileName;
	mCrtFileLen = mFileList[mCrtIdx].mFileLen;
	if (mReadLength >= mCrtFileLen)
	{
		mCrtIdx++;
		if (mCrtIdx >= mFileList.size())
		{
			return true;
		}
		mReadLength = 0;
		mStart = 0;
		mCrtFileName = mFileList[mCrtIdx].mFileName;
		mCrtFileLen = mFileList[mCrtIdx].mFileLen;
	}
	int64_t bytes_to_read = eMinBuffSize;
	if (mCrtFileLen - mReadLength < bytes_to_read)
	{
		bytes_to_read = mCrtFileLen - mReadLength;
	}

	AosBuffPtr buff;
	AosDiskStat disk_stat;
	bool rslt = AosReadDataFile(rdata.getPtr(), buff, mPhysicalid, mCrtFileName, 
			mStart, bytes_to_read, disk_stat);
	aos_assert_r(rslt, false);
	if (disk_stat.serverIsDown())
	{
		mDiskError = true;
		return true;
	}
	aos_assert_r(mRowDelimiter != "", false);
	char* data = buff->data();
	char* last = strrstr(data, buff->dataLen(), mRowDelimiter.data());
	if(last)
	{
		char* end = last + mRowDelimiter.length();
		u32 real_len = end - data;
		if(real_len > bytes_to_read)
		{
			OmnAlarm << "read len error" <<enderr;
			real_len = bytes_to_read;
		}
		buff->setDataLen(real_len);
	}
	u32 bytes_read = buff->dataLen();
	/*
	OmnScreen << "\n========================================" 
			  << "\n== file name: " << mCrtFileName 
			  << "\n== file length: " << AosNumber2String(mCrtFileLen)
		  	  //<< "\n== total read block: " << AosNumber2String(mLength)
		  	  << "\n== already read length: " << AosNumber2String(mReadLength)
			  << "\n== current read bytes: " << AosNumber2String(bytes_read)
			  << "\n========================================\n" << endl;
	*/
	if (mIgnoreHead && mStart == 0)
	{
		char* head = strstr(data, mRowDelimiter.data());
		if (head)
		{
			char* real_start = head + mRowDelimiter.length();
			char* real_end = buff->data() + bytes_read;
			u32 real_len = real_end - real_start;
			buff->reset();
			buff->setBuff(real_start, real_len);
			buff->setDataLen(real_len);
		}
	}
	mStart += bytes_read;
	mReadLength += bytes_read;
	if (mCharacterType != CodeConvertion::mDefaultType)
	{
		int64_t len = buff->dataLen() * 2;
		AosBuffPtr newbuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		OmnString to_type = CodeConvertion::mDefaultType;
		int64_t newlen = CodeConvertion::convert(mCharacterType.data(), to_type.data(),
				buff->data(), buff->dataLen(), newbuff->data(), len);
		aos_assert_r(newlen > 0, false);
		newbuff->setDataLen(newlen);
		buff = newbuff;
	}

	info->setBuff(buff);
	info->setFileInfo(mFileList[mCrtIdx]);
	info->setCrtBlockIdx(mFileList[mCrtIdx].getCrtBlockIdx());
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse("<metaData/>", "" AosMemoryCheckerArgs);
	xml->setAttr(AOSTAG_SOURCE_FNAME, mCrtFileName);
	xml->setAttr(AOSTAG_SOURCE_LENGTH, mCrtFileLen);
	int idx = mCrtFileName.indexOf(0, '/', true);
	aos_assert_r(idx >= 0, false);
	OmnString name = mCrtFileName.substr(idx + 1);
	xml->setAttr(AOSTAG_SOURCE_NAME, name);
	OmnString path = mCrtFileName.substr(0, idx - 1);
	xml->setAttr(AOSTAG_SOURCE_PATH, path);
	info->addMetadata(xml);
	return true;
}


char*
AosDirScanner::strrstr(const char* s1, const int len, const char* s2)
{
	int len2 = 0;
	if (!(len2 = strlen(s2)))
	{
		return (char*)s1;
	}

	char* pcRet = NULL;

	s1 = s1 + (len-1);

	for (int i=len; i>0; i--,--s1)
	{
		if (*s1 == *s2 && strncmp(s1, s2, len2) == 0 )
		{
			pcRet = (char *)s1;
			break;
		}
	}
	return pcRet;
}


bool 
AosDirScanner::createDataScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const vector<AosFileInfo> &filelist,
		const AosRundataPtr &rdata)
{
	try
	{
		AosDirScanner* scanner = OmnNew AosDirScanner(false);
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
		OmnAlarm << "Faild to create DirScanner" << enderr;
		return false;
	}
	return true;
}


bool 
AosDirScanner::AosReaderThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	bool rslt = mDirScanner->readBuffToQueue(state, mRundata);
	if (state == OmnThrdStatus::eExit)
	{
		mDirScanner = 0;
		mThread = 0;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDirScanner::AosReaderThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDirScanner::AosReaderThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosDirScanner::initUnicomDirScanner(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	mIsUnicomFile = true;
	bool rslt = initDirScanner(fileinfos, physical_id,
		false, character_type, row_delimiter, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDirScanner::initDirScanner(
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
		//devid = fileinfos[i].mDevId;
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
		OmnSPtr<AosDirScanner::AosReaderThrd> reader = OmnNew AosReaderThrd(mScanners[i], rdata);
	}
	return true;
}


bool 
AosDirScanner::initDirScanner(
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
		OmnSPtr<AosDirScanner::AosReaderThrd> reader = OmnNew AosReaderThrd(mScanners[i], rdata);
	}
	return true;
}


int64_t
AosDirScanner::getTotalSize() const
{
	int64_t len = 0;
	for (u32 i=0; i<mScanners.size(); i++)
	{
		len += mScanners[i]->getTotalFileLengthByDir();
	}
	return len;
}


int
AosDirScanner::getPhysicalId() const
{
	return -1;
}
#endif
