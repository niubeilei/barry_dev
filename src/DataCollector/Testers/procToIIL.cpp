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
//   
//
// Modification History:
// 06/12/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCollector/Testers/procToIIL.h"
#include "DataCollector/Testers/Ptrs.h"

#include "API/AosApi.h"
#include "DataAssembler/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/VirtualFileObj.h" 
#include "SEInterfaces/VfsMgrObj.h"
#include "Sorter/Ptrs.h"
#include "Sorter/MultiFileSorter.h"
#include "Tester/Test.h"
#include "Thread/Sem.h"
#include "Tester/TestMgr.h"
#include "IILMgr/IILMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Porting/Sleep.h"
#include "VirtualFile/VirtualFile.h"

#include <dirent.h>
OmnMutex sgProcLock;

//extern bool needStop;
//extern bool AosExitSearchEngine();
int64_t						AosDataCheckTester::mProcTotal = 0;
int64_t						AosDataCheckTester::mAppendTotal = 0;
vector<AosActionObjPtr>		AosDataCheckTester::mActions;
OmnFilePtr					AosDataCheckTester::mCrtFile;
OmnString					AosDataCheckTester::mType = "";


AosDataCheckTester::AosDataCheckTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mCrtIdx(0),
mCrtSeqno(0),
mJobDocid(0),
mFileSize(0),
mPerNum(0)
{
	mRundata->setSiteid(100);
}

AosDataCheckTester::~AosDataCheckTester()
{
}

void updateAppendTotal(const int64_t &size)
{
	sgProcLock.lock();
	AosDataCheckTester::mAppendTotal += size;
	sgProcLock.unlock();
}

bool appendToFile(const AosBuffPtr &buff)
{
	sgProcLock.lock();
	aos_assert_r(AosDataCheckTester::mCrtFile, false);
	AosDataCheckTester::mCrtFile->append(buff->data(), buff->dataLen(), true);
	AosDataCheckTester::mProcTotal += buff->dataLen();
	sgProcLock.unlock();
	return true;
}

bool mergeData(
		const int &record_len,
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
//return true;
	if (AosDataCheckTester::mType == "createFile")
	{
		appendToFile(buff);
		return true;
	}
	
	int size = buff->dataLen() / record_len;
	char * data = buff->data(); 
	for(int i=0; i<size; i++)
	{
		// entrie must not empty
		if(data[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
	
		data += record_len;
	}

	aos_assert_r(rdata, false);
	for (u32 i=0; i<AosDataCheckTester::mActions.size(); i++)
	{
		aos_assert_r(AosDataCheckTester::mActions[i]->run(buff, rdata), false);
	}
	sgProcLock.lock();
	AosDataCheckTester::mProcTotal += buff->dataLen();
	sgProcLock.unlock();
	OmnScreen << "mmmmmmmmm  mergeData  mProcTotal : " << AosDataCheckTester::mProcTotal << endl;
	return true;
}

bool
AosDataCheckTester::checkResults(const AosXmlTagPtr &conf)
{
	AosXmlTagPtr file_tag = conf->getFirstChild("file");
//	while(file)
//	{
		u64 fileid = file_tag->getAttrU64("file_id", 0);
		aos_assert_r(fileid, false);

		AosVirtualFileObjPtr file = OmnNew AosVirtualFile(0, fileid);
		aos_assert_r(file, false);

		vector<AosVirtualFileObjPtr> virFiles;
		virFiles.push_back(file);

		mPerCrt = 0;

		mScanner = AosDataScannerObj::createMultiFileScannerStatic(mRundata);
		aos_assert_r(mScanner, false);

		mScanner->setFiles(virFiles);
		AosActionCallerPtr thisptr(this, true);
		mScanner->setActionsCaller(thisptr);

		int64_t buffsize = eMaxBuffSize / mComp->size;
		buffsize *= mComp->size;

OmnScreen << "addreq buffsize 0 : " << buffsize << endl;
		
		mScanner->addRequest(0, buffsize);
		
//		file = conf->getNextChild("file");

/*		OmnString fname = file->getAttrStr("file_name");
		aos_assert_r(fname != "", false);
		bool rslt = checkResults(fname);
		aos_assert_r(rslt, false);
		file = conf->getNextChild("file");
*/
//	}
	return true;
}

void 
AosDataCheckTester::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
OmnScreen << "getnextblock : " << reqId << endl;
	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert(buff);
	int64_t bufflen = buff->dataLen();
OmnScreen << "@@@@ bufflen : " << bufflen << endl;
	if (bufflen > 0)
	{
		int size = bufflen / mComp->size;
		char * data = buff->data(); 
		if (mPerCrt && strncmp(mPerCrt, "", mComp->size)!=0)
		{
			aos_assert(mComp->cmp(mPerCrt, data) > 0);
		}

		for(int i=0; i<size; i++)
		{
			// entrie must not empty
			if(data[0] == 0)
			{
				OmnAlarm << "the entry is empty" << enderr;
			}
			if (i < size-2)
			{
				aos_assert(mComp->cmp(data, data+mComp->size)<=0);	
			}
			data += mComp->size;
		}
		mPerCrt = data-mComp->size;
	}


	if (finished)
	{
		aos_assert(mScanner);
		mScanner->destroyedMember();
		mScanner = 0;

OmnScreen << "file sort success ~~~~ " << endl;
		return;
	}

	int64_t buffsize = eMaxBuffSize / mComp->size;
	buffsize *= mComp->size;
OmnScreen << "addreq buffsize " << reqId << " : " << buffsize << endl;
	mScanner->addRequest(reqId, buffsize); 
}

bool
AosDataCheckTester::checkResults(const OmnString &fname)
{
//	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
//	aos_assert_r(file->isGood(), false);
//	OmnString fname = "/home/felicia/AOSCVS/src/DataCollector/Testers/CleanData/Data/vir_0_orig/site_100/mergeFile_bb__128";
//	aos_assert_r(fname != "", false);
	
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);

	int64_t filesize = file->getLength();                                   
	aos_assert_r(filesize % mComp->size == 0, false);

	int64_t entries = filesize/mComp->size;
	int64_t pernum = 10000000;
	int64_t readpos = 0;
	if (pernum > entries)
	{
		pernum = entries;
	}

	while(readpos < filesize)
	{
		int64_t readsize = pernum * mComp->size;
		if (readpos + readsize > filesize)
		{
			readsize = filesize - readpos;
		}
//OmnScreen << "@@@@ " << fname << " readsize:" << readsize << "  readpos:" << readpos << " filesize:" << filesize << endl;
		AosBuffPtr buff = OmnNew AosBuff(readsize AosMemoryCheckerArgs);
		file->readToBuff(readpos, readsize, buff->data());
		buff->setDataLen(readsize);

		int size = buff->dataLen() / mComp->size;
		char * data = buff->data(); 
		for(int i=0; i<size; i++)
		{
			// entrie must not empty
			if(data[0] == 0)
			{
				OmnAlarm << "the entry is empty" << enderr;
			}
			if (i < size-2)
			{
				aos_assert_r(mComp->cmp(data, data+mComp->size)<=0, false);	
			}
			data += mComp->size;
		}
		readpos += readsize;
	}
	OmnScreen << fname << " Sort success ~~~~~" << endl;
	return true;
}

/*
bool
AosDataCheckTester::checkResults()
{
	OmnString fname = "/mnt/h2/CleanData/Data/vir_0_orig/site_100/proc_0_bb0__1645";
	aos_assert_r(fname != "", false);
	
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	
	u64 filesize = file->getLength();                                   
	AosBuffPtr buff = OmnNew AosBuff(filesize AosMemoryCheckerArgs);
	file->readToBuff(0, filesize, buff->data());
	buff->setDataLen(filesize);

	int size = buff->dataLen() / mComp->size;
	char * data = buff->data(); 
	for(int i=0; i<size; i++)
	{
		// entrie must not empty
		if(data[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
		if (i < size-2)
		{
			aos_assert_r(mComp->cmp(data, data+mComp->size)<=0, false);	
		}
		data += mComp->size;
	}
	return true;
}
*/
bool 
AosDataCheckTester::start()
{
	//<procToIIL>
	//<iilmergertest                                                 
	//  zky_filed = "xxx"
	///	zky_datacolid="system_type_start_datetime"
	//	zky_iilname="_zt44_unicom_rawdata_system_type_start_datetime">
	//	<CompareFun
	//	cmpfun_type="stru642"
	//	cmpfun_size="30"/>
	//	<actions>
	//	<action zky_type="iilbatchadd">
	//	<iil
	//	zky_iilname="_zt44_unicom_rawdata_system_type_start_datetime"
	//	zky_length="30"/>
	//	</action>
	//	</actions>
	//	</iilmergertest>
	//</procToIIL>
	
	AosXmlTagPtr app_conf1 = OmnApp::getAppConfig();
	AosXmlTagPtr app_conf = app_conf1->getFirstChild("procToIIL");
	aos_assert_r(app_conf, false);

	bool toproc = app_conf->getAttrBool("zky_proc", false);
	if (!toproc) return true;


/*	OmnString fname = app_conf->getAttrStr("zky_fileid");
	aos_assert_r(fname != "", false);
	mFile = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(mFile->isGood(), false);
	
	mFileSize = mFile->getLength();                                   
	mFileBuff = OmnNew AosBuff(mFileSize AosMemoryCheckerArgs);
	mFile->readToBuff(0, mFileSize, mFileBuff->data());
	mFileBuff->setDataLen(mFileSize);
	*/
//	mType = app_conf->getAttrStr("zky_type", "mergeData");
//	aos_assert_r(mType != "", false);

	//create Action
	AosXmlTagPtr act = app_conf->getFirstChild("action");
	aos_assert_r(act, false);
	
	//compare function
	AosXmlTagPtr conf = app_conf->getFirstChild("CompareFun");
	aos_assert_r(conf, false);
	
	mComp = AosCompareFun::getCompareFunc(conf);
	aos_assert_r(mComp, false);

/*
AosXmlTagPtr files = app_conf->getFirstChild("files");
aos_assert_r(files, false);
bool checkrslt = checkResults(files);
aos_assert_r(checkrslt, false);
return checkrslt;
*/
/*
	OmnString filestr = "<files>";
	u64 * fileids = (u64 *)mFileBuff->data();
	int serverid = AosGetSelfServerId(); 
	for(int i=0; i<mFileSize/sizeof(u64); i++)
	{
		filestr << "<file zky_storage_fileid=\"" << fileids[i] << "\""
				<< " zky_physicalid=\"" << serverid << "\"" 
				<< " zky_level=\"0\"/>";
	}
	filestr << "</files>";

	AosXmlParser parser;                                                         
	AosXmlTagPtr data = parser.parse(filestr, "" AosMemoryCheckerArgs);
	aos_assert_r(data, false);
	act->addNode(data);
*/
	AosActionObjPtr actobj = AosActionObj::getAction(act, mRundata);
	aos_assert_rr(actobj, mRundata, false);

	AosTaskObjPtr task; 
	AosTaskDataObjPtr task_data;
	bool rslt = actobj->run(task, act, mRundata);
	aos_assert_r(rslt, false);
/*
	mJobDocid = app_conf->getAttrU64("zky_jobdocid", 0);
	//aos_assert_r(mJobDocid > 0, false);

	mPerNum = app_conf->getAttrInt("zky_pernum", 100);
	aos_assert_r(mPerNum > 0, false);

	AosXmlTagPtr iil_conf = app_conf->getFirstChild("iil");
	while(iil_conf)
	{
		mFileIds.clear();
		mActions.clear();
		mAppendTotal = 0;
		mProcTotal = 0;
		mFileSize = 0;
		mFileBuff = 0;
		mProcFile = 0;
		mCrtFile = 0;
		mRundata->resetError();

		aos_assert_r(config(iil_conf), false);
//		return checkResults();



		aos_assert_r(getFields(iil_conf), false);
		aos_assert_r(mActions.size() > 0, false);
		if (mType == "createFile")
		{
			while(mFileIds.size()>0)
			{
				aos_assert_r(createIIL(), false);
				mFileIds.clear();
				getFields();
			}
			iil_conf = app_conf->getNextChild();
			//mFile->deleteFile();
			//mFile=0;
			continue;
		}

		if (mFileIds.size()== 0)
		{
			iil_conf = app_conf->getNextChild();
			//mFile->deleteFile();
			//mFile=0;
			continue;
		}
		
		aos_assert_r(createIIL(), false);

		iil_conf = app_conf->getNextChild();
		//mFile->deleteFile();
		//mFile=0;
	}
*/
	return true;
}

bool
AosDataCheckTester::getFields(const AosXmlTagPtr &app_conf)
{
	//get fileids
	OmnString fname = app_conf->getAttrStr("zky_fileid");
	aos_assert_r(fname != "", false);
	
	mFile = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(mFile->isGood(), false);
	
	mFileSize = mFile->getLength();                                   
	mFileBuff = OmnNew AosBuff(mFileSize AosMemoryCheckerArgs);
	mFile->readToBuff(0, mFileSize, mFileBuff->data());
	mFileBuff->setDataLen(mFileSize);
	getFields();
	//////
	return true;
}

bool
AosDataCheckTester::getFields()
{
	createFile();
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	u64 * fileids = (u64 *)mFileBuff->data();
	//for(u64 i=0; i<filesize/sizeof(u64); i++)
	int crtidx = mCrtIdx;
	for(u64 i=mCrtIdx; i< crtidx+mPerNum; i++)
	{
/*		if (i >= mFileSize/sizeof(u64))
		{
			break;
		}
		mCrtIdx++;
		mFileIds.push_back(fileids[i]);
		OmnFilePtr ff = vfsMgr->openFile(fileids[i], mRundata AosMemoryCheckerArgs);
		u64 fsize = ff->getLength();
		aos_assert_r(fsize % mComp->size == 0, false);
		OmnScreen << "@@@@@@ filename : " << ff->getFileName() << "  filesize : " << fsize << endl;
		sgProcLock.lock();
		mAppendTotal += fsize;
		sgProcLock.unlock();
*/
	}
	return true;
}

bool
AosDataCheckTester::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);

	//minSize
	mMinSize = app_conf->getAttrInt("min_size", 50000000); //50M
	 
	//maxSize 
	mMaxSize = app_conf->getAttrInt("max_size", 800000000); // 800M

    //buffSize                                                              
	mBuffSize = app_conf->getAttrInt("buff_size", 1500000); //1.5M

	//operator
	OmnString opr = app_conf->getAttrStr(AOSTAG_OPR);
	aos_assert_r(opr != "", false);
	mOperator = AosDataColOpr::toEnum(opr);
	bool isvalid = AosDataColOpr::isValid(mOperator);
	aos_assert_r(isvalid, false);

	//compare function
	AosXmlTagPtr conf = app_conf->getFirstChild("CompareFun");
	aos_assert_r(conf, false);
	
	mComp = AosCompareFun::getCompareFunc(conf);
	aos_assert_r(mComp, false);
	
	mStable = conf->getAttrBool("stable", false);

	mDataColid = app_conf->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	aos_assert_r(mDataColid != "", false);

	mIILName = app_conf->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);
	
	
	//Actions
	AosXmlTagPtr actions = app_conf->getFirstChild(AOSTAG_ACTIONS);
	aos_assert_r(actions, false);
	AosXmlTagPtr act = actions->getFirstChild();
	while(act)
	{
		try
		{
			AosActionObjPtr actobj = AosActionObj::getAction(act, mRundata);
			aos_assert_rr(actobj, mRundata, false);
			mActions.push_back(actobj);
		}
		catch(...)
		{
			OmnAlarm << "failed to create action:" << act->toString() << enderr;
			return false;
		}
		act = actions->getNextChild();
	}
	aos_assert_r(mActions.size() > 0, false);

	mRundata->setSiteid(100);

/*
	
	u64 iilid = AosIILMgr::getSelf()->getIILID(mIILName, false, mRundata);
	AosIILType iiltype;
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
		iilid, mRundata->getSiteid(), iiltype, false, false, mRundata);

	bool need_delete = app_conf->getAttrBool("zky_deleteiil", true);
	bool rslt = true;
	if (iil)
	{
	 	if(need_delete)
		{
			rslt = AosIILMgr::getSelf()->deleteIILSafe((AosIILObj*)(iil.getPtr()), mRundata);
	 		aos_assert_r(rslt, false);
		}
		rslt = AosIILMgr::getSelf()->returnIILPublic(iil, mRundata);
		aos_assert_r(rslt, false);
	}
	else
	{
		if(iilid == 0)
		{
			iilid = AosIILMgr::getSelf()->getIILID(mIILName, true, mRundata);
		}
		rslt = AosIILMgr::getSelf()->createIILPublic1(
					iilid, 100, eAosIILType_Str, false, false, mRundata);
		aos_assert_r(rslt, false);
		aos_assert_r(iilid, false);
	}
*/	
	return true;
}

u32 
AosDataCheckTester::getVirtualId()
{
		// Ketty 2013/02/02
		//mTotalLock->lock();
		//aos_assert_rl(mTableid != "", mTotalLock, 0);
		//u32 hash_key = AosIILClient::getSelf()->getHashKey(mTableid);
		//mTotalLock->unlock();
		//u32 virtual_id = hash_key % AosGetNumCubes();
		//return virtual_id;
	
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	aos_assert_r(total_vids.size(), 0);

	u32 idx = rand() % (total_vids.size());
	return total_vids[idx];
}


bool 
AosDataCheckTester::createFile()
{
	if (mType != "createFile")
	{
		return true;
	}
	
	OmnString fname = "proc_";
	fname << mJobDocid << "_" << mDataColid << "_";
	aos_assert_r(fname != "", false);
	
	u32 virtual_id = getVirtualId();
	u64 file_id;
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	mCrtFile = vfsMgr->createNewFile(mRundata, virtual_id,
			file_id, fname, 0, false);
	aos_assert_r(mCrtFile, false);
	if(!mProcFile)
	{
		fname = AosGetBaseDirname();
		fname << "/proc_fileids_" << mJobDocid << "_" << mDataColid << "_" << mCrtSeqno << "_" << (u64)(unsigned long)this;
		mProcFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(mProcFile->isGood(), false);
		mCrtSeqno++;
	}
	mProcFile->append((char *)&file_id, sizeof(u64));
	return true;
}

bool
AosDataCheckTester::createIIL()
{
/*	aos_assert_r(mFileIds.size() > 0, false);		

	AosMultiFileSorterPtr sorter = OmnNew AosMultiFileSorter(mMinSize, mMaxSize, mBuffSize, 
			        mOperator, mComp, mFileIds, mRundata);
	bool crslt = sorter->sort(mRundata);
	aos_assert_r(crslt, false);
	int sleep_sec = 0;
	while(1)
	{
		if (mAppendTotal == mProcTotal) break;
		if (sleep_sec >= 300)
		{
			OmnScreen << "************ proc error  " << mDataColid << "  mAppendTOtal : " << mAppendTotal << " mProcTotal : " << mProcTotal << endl;
		}

		sleep_sec += 1;
		OmnSleep(1);
	}

	sorter->clear();
	sorter = 0;
	//deleteFiles(mRundata); 
	OmnScreen << mDataColid << " Sort  success !!! " << endl;
	*/
	return true;
}

void
AosDataCheckTester::deleteFiles(const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);
	for(u64 i=0; i<mFileIds.size(); i++)
	{
		vfsMgr->removeFile(mFileIds[i], rdata);
	}
}
#endif
