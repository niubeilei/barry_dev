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
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/SearchEngTester.h"

#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "IILMgr/IIL.h"
#include "IILClient/IILClient.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"

extern AosXmlTagPtr gAosAppConfig;


AosSearchEngTester::AosSearchEngTester()
:
mLock(OmnNew OmnMutex()),
mTotalDocsToCheck(0),
mRemotePort(0),
mNumCreateThrds(0),
mNumModifyThrds(0),
mNumDeleteThrds(0),
mNumQueryThrds(0),
mNumDocThrds(0),
mTotalDocsCreated(0),
mStartTime(0),
mUpdateStopped(0)
{
	mName = "Tester";
}


bool 
AosSearchEngTester::start()
{
	cout << "    Start Tester ...";

	mAttrTname1   = "torturer_attrs";
	mAttrwdTname1 = "torturer_awords";
	mDocwdTname1  = "torturer_dwords";
	mTagTname1    = "torturer_tags";
	mDocTname1    = "torturer_docs";
	mDocFname	  = "torturer_docs";
	mDataDirname  = "/AOS/Data";

	AosTestXmlDoc::init(
			mAttrTname1, 
			mAttrwdTname1, 
			mDocwdTname1, 
			mTagTname1, 
			mDocTname1);

	mRemoteAddr.set(gAosAppConfig->getAttrStr("raddr", ""));
	mRemotePort = (int)gAosAppConfig->getAttrU64("rport", 0);

	// Read the number of docs created
	OmnString stmt = "select count(*) from ";
	stmt << mDocTname1;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
//	OmnDbRecordPtr record;
//	aos_assert_r(store->query(stmt, record), false);
//	mTotalDocsCreated = record->getU32(0, 0, rslt);

	AosSengAdmin::getSelf()->start(gAosAppConfig);
	AosSengAdmin::getSelf()->setDocFname(gAosAppConfig->getAttrStr("doc_file_name"));
	AosSengAdmin::getSelf()->setIdxFname(gAosAppConfig->getAttrStr("idx_file_name"));
	AosIIL::staticInit(gAosAppConfig);


	// Create the siteids
	for (u32 i=0; i<eMaxSiteids; i++)
	{
		mSiteids[i] = "site";
		mSiteids[i] << i;
	}

	// Create the unames 
	for (u32 i=0; i<eMaxUnames; i++)
	{
		mUnames[i] = "user";
		mUnames[i] << i;
	}

	// Create the appnames 
	for (u32 i=0; i<eMaxAppnames; i++)
	{
		mAppnames[i] = "app";
		mAppnames[i] << i;
	}

	// Create the ctnrnames 
	for (u32 i=0; i<eMaxCtnrnames; i++)
	{
		mCtnrnames[i] = "ctnr";
		mCtnrnames[i] << i;
	}

	OmnString restart = gAosAppConfig->getAttrStr("restart", "");
	if (restart == "true") prepareEnv();
	mStartTime = OmnGetSecond();

	// Create the DocCreation Threads
	mTotalDocsToCheck = mTestMgr->getTries();
	AosSearchEngTesterPtr thisptr(this, false);

	int createWeight = 100;
	int totalToCreate = gAosAppConfig->getAttrInt("totalcreate", 0);
	// mNumCreateThrds = gAosAppConfig->getAttrInt("createthrds", 0);
	mNumCreateThrds = 0;
	for (int i=0; i<mNumCreateThrds; i++)
	{
		mCreateThrds[i] = OmnNew AosCreateDocThrd(
			thisptr, totalToCreate, createWeight, 
			mRemoteAddr, mRemotePort);
	}

	// Create the Query Threads
	int queryWeight = 100;
	int totalToQuery = gAosAppConfig->getAttrInt("totalquery", 0);
	mNumQueryThrds = gAosAppConfig->getAttrInt("querythrds", 0);
//	mNumQueryThrds = 0;
	for (int i=0; i<mNumQueryThrds; i++)
	{
		mQueryThrds[i] = OmnNew AosQueryThrd(
			thisptr, totalToQuery, queryWeight, 
			mRemoteAddr, mRemotePort, 
			mAttrTname1, mDocwdTname1, mTagTname1);
		mQueryThrds[i]->setTestMgr(mTestMgr);
		mQueryThrds[i]->start();
	}

	// Create the Doc Threads
	int docWeight = 10000;
	mNumDocThrds = 1;
	for (int i=0; i<mNumDocThrds; i++)
	{
		mDocThrds[i] = OmnNew AosDocThrd(
			thisptr, mTotalDocsToCheck, docWeight, 
			mRemoteAddr, mRemotePort);
	}

	// Create the Modify Threads
	int modifyWeight = 0;
	int totalToMod = gAosAppConfig->getAttrInt("totalmod", 0);
	mNumModifyThrds = gAosAppConfig->getAttrInt("modthrds", 0);
	for (int i=0; i<mNumModifyThrds; i++)
	{
		mModifyThrds[i] = OmnNew AosModifyDocThrd(
			thisptr, totalToMod, modifyWeight, 
			mRemoteAddr, mRemotePort, mAttrTname1);
	}

	// Create the Delete Threads
	int deleteWeight = 10000;
	int totalToDelete = gAosAppConfig->getAttrInt("totaldel", 0);
	// mNumDeleteThrds = gAosAppConfig->getAttrInt("delthrds", 0);
	mNumDeleteThrds = 0; 
	for (int i=0; i<mNumDeleteThrds; i++)
	{
		mDeleteThrds[i] = OmnNew AosDeleteDocThrd(
			thisptr, totalToDelete, deleteWeight, 
			mRemoteAddr, mRemotePort, mAttrTname1);
	}

	return true;
}


bool 
AosSearchEngTester::prepareEnv()
{
	// . Clear the "testrawdocindex" table
	OmnString stmt = "delete from testrawdocindex";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store->runSQL(stmt), false);

	// . Delete records from 'testdocindex'
	stmt = "delete from testdocindex";
	aos_assert_r(store->runSQL(stmt), false);

	// . Delete records from 
	stmt = "delete from ";
	stmt << mAttrTname1;
	aos_assert_r(store->runSQL(stmt), false);

	stmt = "delete from ";
	stmt << mAttrwdTname1;
	aos_assert_r(store->runSQL(stmt), false);

	stmt = "delete from ";
	stmt << mDocwdTname1;
	aos_assert_r(store->runSQL(stmt), false);

	stmt = "delete from ";
	stmt << mTagTname1;
	aos_assert_r(store->runSQL(stmt), false);

	stmt = "delete from ";
	stmt << mDocTname1;
	aos_assert_r(store->runSQL(stmt), false);

	// 12. Delete all "XmlDocs_*"
	OmnString cmd = "rm ";
	cmd << mDataDirname << "/XmlDocs_*";
	system(cmd);

	cmd = "rm ";
	cmd << mDataDirname << "/" << mDocFname << "/*";
	system(cmd);

    return true;
}


bool 
AosSearchEngTester::torturer()
{
	while (1)
	{
		OmnSleep(1000);

		u64 tt = OmnTime::getCrtSecond();
		cout << "Total: " << mTotalDocsCreated 
			<< " tm: " << tt - mStartTime << endl;
	}
	return true;
}


bool
AosSearchEngTester::docCreated(
		const OmnString &siteid,
		const OmnString &docid, 
		const OmnString &objid,
		OmnString &contents)
{
	mLock->lock();
	OmnString fname = mDataDirname;
	fname << "/" << mDocFname << "/" << docid;
	OmnFile ff(fname, OmnFile::eCreate);
	aos_assert_rl(ff.isGood(), mLock, false);

	// OmnScreen << "Doc created: " << contents.length() << endl;
	// OmnScreen << contents << endl;
	ff.put(0, contents.data(), contents.length(), true);

	contents.escapeSymbal('\'', '\'');
	OmnString stmt = "insert into ";
	stmt << mDocTname1 << " (siteid, docid, objid, offset, docsize) values ('"
		<< siteid << "', '" << docid << "', '" << objid 
		<< "', " << 0 << ", "
		<< contents.length() << ")";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rl(store->runSQL(stmt), mLock, false);	
	mTotalDocsCreated++;
	mLock->unlock();
	return true;
}


bool
AosSearchEngTester::docModified(
		const OmnString &siteid,
		const OmnString &docid, 
		OmnString &contents)
{
	mLock->lock();
	OmnString fname = mDataDirname;
	fname << "/" << mDocFname << "/" << docid;
	OmnFile ff(fname, OmnFile::eCreate);
	aos_assert_rl(ff.isGood(), mLock, false);

	ff.put(0, contents.data(), contents.length(), true);

	contents.escapeSymbal('\'', '\'');
	OmnString stmt = "update ";
	stmt << mDocTname1 << " set docsize="
		<< contents.length() << " where siteid='"
		<< siteid << "' and docid='" << docid << "'"; 
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rl(store->runSQL(stmt), mLock, false);	
	mLock->unlock();
	return true;
}


bool
AosSearchEngTester::docDeleted(
		const OmnString &siteid,
		const OmnString &docid)
{
	mLock->lock();
	mTotalDocsCreated--;
	OmnString stmt = "delete from ";
	stmt << mDocTname1 << " where docid='"
		<< docid << "' and siteid='" << siteid << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rl(store->runSQL(stmt), mLock, false);	
	mLock->unlock();
	return true;
}


AosXmlTagPtr
AosSearchEngTester::readDoc()
{
	// It randomly reads one doc. 
	if (mTotalDocsCreated == 0) return 0;
	int vv = rand() % mTotalDocsCreated;

	OmnString stmt = "select siteid, docid from ";
	stmt << mDocTname1 << " limit " << vv << ", 1";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbRecordPtr record;
	aos_assert_r(store->query(stmt, record), false);	
	aos_assert_r(record, false);

	OmnRslt rslt;
	OmnString siteid = record->getStr(0, "", rslt);
	OmnString docid = record->getStr(1, "", rslt);
	return readDoc(siteid, docid);
}


AosXmlTagPtr
AosSearchEngTester::readDoc(
		const OmnString &siteid,
		const OmnString &docid)
{
	OmnString stmt = "select offset, docsize from ";
	stmt << mDocTname1 << " where siteid='" << siteid 
		<< "' and docid='" << docid << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbRecordPtr record;
	aos_assert_r(store->query(stmt, record), false);	
	aos_assert_r(record, false);

	OmnRslt rslt;
	u32 size = record->getU32(1, 0, rslt);
	
	OmnString fname = mDataDirname;
	fname << "/" << mDocFname << "/" << docid;
	OmnFile ff(fname, OmnFile::eReadOnly);
	aos_assert_r(ff.isGood(), false);

	OmnString contents;
	contents.setSize(size+1);
	char *data = (char *)contents.data();
	int rs = ff.readToBuff(0, size, data);
if (!(rs >= 0 && (u32)rs == size))
{
	OmnMark;
}
	aos_assert_r(rs >= 0 && (u32)rs == size, false);
	contents.setLength(size);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "");
	aos_assert_r(root, false);
	return root;
}


bool
AosSearchEngTester::stopUpdating()
{
	// It needs to inform all the doc creation threads to stop 
	// creating docs. 
	mLock->lock();
	if (mUpdateStopped > 0)
	{
		// Already stopped. 
		mUpdateStopped++;
		mLock->unlock();
		return true;
	}

	while (1)
	{
		bool allStopped = true;
		for (int i=0; i<mNumCreateThrds; i++)
		{
			if (mCreateThrds[i]->isCreating())
			{
				allStopped = false;
				break;
			}
		}

		if (allStopped) 
		{
			mUpdateStopped = 1;
			mLock->unlock();
			return true;
		}

		OmnSleep(100);
	}

	OmnShouldNeverComeHere;
	mLock->unlock();
	return false;
}


bool
AosSearchEngTester::releaseUpdating()
{
	mLock->lock();
	mUpdateStopped--;
	if (mUpdateStopped < 0)
	{
		OmnAlarm << "Something is wrong!" << mUpdateStopped << enderr;
		mUpdateStopped = 0;
	}

	mLock->unlock();
	return true;
}


bool
AosSearchEngTester::isUpdateStopped()
{
	mLock->lock();
	bool rslt = (mUpdateStopped != 0);
	mLock->unlock();
	return rslt;
}


void	
AosSearchEngTester::procFinished(const bool status, 
		const u64 &docId,
		const OmnString &errmsg, 
		const void *userdata, 
		const int numWords)
{
}


OmnString	
AosSearchEngTester::getSiteId()
{
	/*
	int id = rand() % eMaxSiteids;
	return mSiteids[id];
	*/
	return "1";
}


OmnString	
AosSearchEngTester::getUsername()
{
	int id = rand() % eMaxUnames;
	return mUnames[id];
}


OmnString	
AosSearchEngTester::getAppname()
{
	int id = rand() % eMaxAppnames;
	return mAppnames[id];
}


OmnString	
AosSearchEngTester::getCtnrname()
{
	int id = rand() % eMaxCtnrnames;
	return mCtnrnames[id];
}


bool
AosSearchEngTester::getDocidToDelete(
		OmnString &siteid, 
		OmnString &docid, 
		OmnString &objid)
{
	// It randomly pick a doc to delete.
	mLock->lock();
	if (mTotalDocsCreated <= 0) 
	{
		mLock->unlock();
		return "";
	}

	int idx = rand() % mTotalDocsCreated;

	OmnString stmt = "select siteid, docid, objid from ";
	stmt << mDocTname1 << " limit " << idx << "," << 1;
	OmnScreen << "To query: " << stmt << endl;
	OmnDbRecordPtr record;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rl(store->query(stmt, record), mLock, false);	
	aos_assert_rl(record, mLock, false);
	OmnRslt rslt;
	siteid = record->getStr(0, "", rslt);
	docid = record->getStr(1, "", rslt);
	objid = record->getStr(2, "", rslt);
	mLock->unlock();
	aos_assert_r(siteid != "", false);
	aos_assert_r(docid != "", false);
	aos_assert_r(objid != "", false);
	return true;
}

