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
// mHasRecordsInDb:
// 	1. Whenever a record is added to db, it is set to true;
// 	2. When processing a record, if it failed retrieving a record, 
// 	   it checks whether the table is empty. If yes, it is set to true.
// 	3. When the class starts up, it calls checkDbRecordsLocked(), 
// 	   which checks the database. If there are records, it is set to true.
//
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupClt/RemoteBackupClt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Database/DbRecord.h"
#include "IILIDIdGen/U64IdGenForIIL.h"
#include "Porting/Sleep.h"
#include "RemoteBackupClt/Ptrs.h"
#include "RemoteBackupClt/RemoteBkReq.h"
#include "RemoteBackupUtil/RemoteBkType.h"
#include "Rundata/Rundata.h"
#include "SqlUtil/TableNames.h"
#include "SEUtil/SystemIdNames.h"
#include "SEModules/SiteMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/StorageDocids.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


/*OmnSingletonImpl(AosRemoteBackupCltSingleton,
                 AosRemoteBackupClt,
                 AosRemoteBackupCltSelf,
                "AosRemoteBackupClt");
*/
AosRemoteBackupClt::AosRemoteBackupClt()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mUseQueue(false),
mStartTransid(0),
mTransidPrefix(0),
mHasRecordsInDb(false),
mRemotePort(-1),
mNumPorts(-1),
mQueueFullSize(eDftQueueFullSize),
mSleepSecBeforeReconn(eDftSleepSecBeforeReconn),
mServerId(-1),
mSystemId(-1),
mIdGen(0),
mTableName("")
{
}

AosRemoteBackupClt::AosRemoteBackupClt(const AosXmlTagPtr &def)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mUseQueue(false),
mStartTransid(0),
mTransidPrefix(0),
mHasRecordsInDb(false),
mRemotePort(-1),
mNumPorts(-1),
mQueueFullSize(eDftQueueFullSize),
mSleepSecBeforeReconn(eDftSleepSecBeforeReconn),
mServerId(-1),
mSystemId(-1),
mIdGen(0),
mTableName("")
{
	bool rslt =	config(def);
	OmnString errormsg;
	if (!rslt)
	{
		errormsg = "RemoteBackClt config error!";
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;
	}
	rslt = start();
	if (!rslt)
	{
		errormsg = "RemoteBackClt start error!";
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;
	}
}


AosRemoteBackupClt::~AosRemoteBackupClt()
{
}


bool
AosRemoteBackupClt::start()
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "remotebackclient", 0, true, true, __FILE__, __LINE__);
	aos_assert_r(mThread, false);
	return mThread->start();
}


bool
AosRemoteBackupClt::config(const AosXmlTagPtr &config)
{
	mLock->lock();
	mServerId = OmnApp::getServerId();
	if (mServerId < 0)
	{
		OmnAlarm << "RemoteBackupClt runs on a front end server only. "
			<< "Either this is not a front end server or AosFrontEndSvr"
			<< " this class is initiated before AosFrontEndSvr."
			<< " Please fix the problem!" << enderr;
		return false;
		//exit(-1);
	}

	mSystemId = 1;
	if (mSystemId < 0)
	{
		OmnAlarm << "Missing System ID. Please use -systemid xxx to "
			<< "set the system ID!" << enderr;
		return false;
		//exit(-1);
	}

	u16 systemid = mSystemId;
	u16 serverid = mServerId;
	mTransidPrefix = (((u64)systemid) << 48) + (((u64)serverid) << 32);
					  	
	aos_assert_rl(config, mLock, false);
	AosXmlTagPtr tag = config->getFirstChild("remote_back_client");
	if (!tag)
	{
		OmnAlarm << "Missing remote backup client configuration!" << enderr;
		return false;
		//exit(-1);
	}

	int queuesize = tag->getAttrInt("queue_full_size", eDftQueueFullSize);
	if (queuesize > 0) mQueueFullSize = (u32)queuesize;

	mUseQueue = (tag->getAttrStr("use_queue") == "true");

	mSleepSecBeforeReconn = tag->getAttrInt("reconn_freq", eDftSleepSecBeforeReconn);
	if (mSleepSecBeforeReconn <= 0) mSleepSecBeforeReconn = eDftSleepSecBeforeReconn;

	mRemoteAddr.set(tag->getAttrStr("remote_addr", "0.0.0.0"));
	mRemotePort = tag->getAttrInt("remote_port", 5555);

	mTableName = tag->getAttrStr("db_table_name", AOSTABLENAME_REMOTE_BACKUP_CLT);
	if(mTableName != "")createDbTable();

	//mIdGen = OmnNew AosU64IdGenWithIIL(tag, "idgen",
	//		 	AOSSYSTEMID_REMOTEBACK_TRANSID, 
	//			AOSIILID_REMOTEBACK_TRANSID);
	
	//mRundata = OmnApp::getRundata();
	//aos_assert_r(mRundata, false);
	//mStartTransid = mIdGen->nextId(AosSiteMgr::getDftSiteid(), mRundata);
	//aos_assert_r(mStartTransid > 0, false);
	//mEndTransid = mStartTransid + eTransidBatchSize;
	
	//create new idgen
	AosXmlTagPtr cfg = tag->getFirstChild("idgen");
	if (!cfg)
	{
		OmnAlarm << "Missing idgen configuration!" << enderr;
		return false;
		//exit(-1);
	}

	u64 crtid = cfg->getAttrU64("init_value", 3000);
	u64 maxid = cfg->getAttrU64("maxid", 10000000);
	u32 blocksize = cfg->getAttrU32("block_size", 1);
	AosRundataPtr rdata = OmnApp::getRundata();
	aos_assert_rl(rdata, mLock, false);

	// Ketty 2012/07/30
	//mIdGen = AosU64IdGenForIIL::createNewIdGen(AOSSTORAGEDOCID_REMOTEBK_IDGEN,
	//		crtid, maxid, blocksize, rdata);
	mIdGen;// = AosU64IdGenForIIL::createNewIdGen(AOSSTORAGEDOCID_REMOTEBK_IDGEN,
			//crtid, maxid, blocksize, rdata);
	OmnNotImplementedYet;

	checkDbRecordsLocked();
	aos_assert_rl(getConn(), mLock, false);
	mLock->unlock();
	return true;
}

/*
bool
AosRemoteBackupClt::stop()
{
    return true;
}
*/

bool	
AosRemoteBackupClt::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// The thread maintains a queue. When the queue is full, additional transactions
	// are stored in the database. Once transactions are stored in the database, 
	// additional transactions are all saved in the database until the queue
	// becomes empty. 
	// 
	AosRemoteBkReqPtr request;
    while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mRequests.size() <= 0 && !mHasRecordsInDb)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		if (mRequests.size() > 0)
		{
			request = mRequests.front();
			mLock->unlock();
			OmnString ss;
			request->toString(ss);
			if (!sendRequest(ss))
			{
				// Failed sending. Will sleep for a while before trying again.
				OmnSleepSec(mSleepSecBeforeReconn);
				continue;
			}

			mLock->lock();
			mRequests.pop();
		}

		if (mRequests.size() == 0 && mHasRecordsInDb)
		{
			// It has something in the database. Need to process it.
			processOneRecordLocked();
		}
		mLock->unlock();
	}
	return true;
}


bool	
AosRemoteBackupClt::signal(const int threadLogicId)
{
	return true;
}


bool    
AosRemoteBackupClt::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


//bool
//AosRemoteBackupClt::retrieveAdditionalTransids()
//{
//	AosRundataPtr rdata = OmnApp::getRundata();
//	aos_assert_rl(mIdGen, mLock, false);
//	mStartTransid = mIdGen->nextId(AosSiteMgr::getDftSiteid(), rdata);
//	aos_assert_rl(mStartTransid > 0, mLock, false);
//	mEndTransid = mStartTransid + eTransidBatchSize;
//	return true;
//}


u64 
AosRemoteBackupClt::getTransidLocked()
{
	//AosRundataPtr rdata = OmnApp::getRundata();
	//aos_assert_r(mIdGen, 0);
	//mStartTransid = mIdGen->nextId(AosSiteMgr::getDftSiteid(), rdata);
	//aos_assert_r(mStartTransid > 0, 0);
	////return mTransidPrefix + mStartTransid;
	//u64 transid = mTransidPrefix + mStartTransid;
	AosRundataPtr rdata = OmnApp::getRundata();
	mStartTransid = mIdGen->getNextId(rdata);
	u64 transid = mTransidPrefix + mStartTransid ++;
	aos_assert_r(transid, 0);
	return transid;
}


bool 
AosRemoteBackupClt::docCreated(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 transid = getTransidLocked();
	AosRemoteBkReqPtr request = getRequest(AOSREMOTEBACK_CREATEDOC, transid, doc);
	aos_assert_rl(request, mLock, false);
	addRequestLocked(request, rdata);
	mLock->unlock();
	return true;
}


bool 
AosRemoteBackupClt::docDeleted(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 transid = getTransidLocked();
	AosRemoteBkReqPtr request = getRequest(AOSREMOTEBACK_DELETEDOC, transid, doc);
	aos_assert_rl(request, mLock, false);
	addRequestLocked(request, rdata);
	mLock->unlock();
	return true;
}


bool 
AosRemoteBackupClt::docModified(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 transid = getTransidLocked();
	AosRemoteBkReqPtr request = getRequest(AOSREMOTEBACK_MODIFYDOC, transid, doc);
	aos_assert_rl(request, mLock, false);
	addRequestLocked(request, rdata);
	mLock->unlock();
	return true;
}

bool
AosRemoteBackupClt::saveIILs(
		const u64 &iilid, 
		const AosBuffPtr &tmpfilebuff, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 transid = getTransidLocked();
	AosRemoteBkReqPtr request = getRequest(AOSREMOTEBACK_SAVEIILS, 
			transid, iilid, tmpfilebuff, rdata->getSiteid());
	aos_assert_rl(request, mLock, false);
	addRequestLocked(request, rdata);
	mLock->unlock();
	return true;
}

bool
AosRemoteBackupClt::addWord(
		const u64 &iilid,
		const OmnString &word,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(word != "", mLock, false);
	aos_assert_rl(iilid , mLock, false);
	u64 transid = getTransidLocked();
	AosRemoteBkReqPtr request = getRequest(AOSREMOTEBACK_ADDWORD,
			transid, iilid, word, rdata->getSiteid());
	aos_assert_rl(request, mLock, false);
	addRequestLocked(request, rdata);
	mLock->unlock();
	return true;
}


bool
AosRemoteBackupClt::addRequestLocked(
		const AosRemoteBkReqPtr &request, 
		const AosRundataPtr &rdata)
{
	// This function pushes the request to the queue. But if the queue
	// has too many entries, it should not push it to the queue. Instead,
	// it saves the request to the file.
	if (!mUseQueue || mHasRecordsInDb || mRequests.size() > mQueueFullSize)
	{
		// there are too many entries in the queue. Save it to db.
		return saveReqToDbLocked(request, rdata);
	}

	mRequests.push(request);
	mCondVar->signal();
	return true;
}


bool
AosRemoteBackupClt::saveReqToDbLocked(
		const AosRemoteBkReqPtr &request, 
		const AosRundataPtr &rdata)
{
	// This function appends the request to the file. 
	// If the file is too big, it should use the next
	// file. All files use the same prefix, with different
	// seqno.
	mHasRecordsInDb = true;
	OmnString str;
	request->toString(str);
	//str.escapeSymbal('\'', '\'');
	int length = str.length();
	OmnString stmt = "insert into ";
	//stmt << AOSTABLENAME_REMOTE_BACKUP_CLT << "(length, contents) values(?,?)";
	aos_assert_r(mTableName != "", false);
	stmt << mTableName << "(length, contents) values(?,?)";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	if (!store->insertBinary1(stmt, length, str))
	{
		OmnAlarm << "Failed the database operation!" << enderr;
		return false;
	}
		
	mCondVar->signal();
	return true;
}


bool
AosRemoteBackupClt::sendRequest(const OmnString &request)
{
	// This is a private member function. It attempts to send the
	// request to the remote site. If the remote server is not 
	// available, it does nothing. Otherwise, it sends the request
	// to the server. If the server acknowledges it, it pops the
	// request (note that the request was not popped yet) from 
	// the queue and returns.
	if (!mConn) getConn();
	if (!mConn)
	{
		// Looks like the remote side is not up. Do nothing.
		return false;
	}
	
	bool rslt = false;
	OmnString err;
	OmnConnBuffPtr buff;
	do
	{
		do
		{
			rslt = mConn->smartSend(request);
			if (!rslt)
			{
				if(!mConn->connect(err))
				{
					OmnSleep(3);
				}
			}
		}
		while(!rslt);

		rslt = mConn->smartRead(buff);
		if (!rslt)
		{
			if(!mConn->connect(err))
			{
				OmnSleep(3);
			}
		}
	}
	while(!rslt);

	//OmnString err;
	//bool rslt = mConn->smartSend(request);
	//if (!rslt)
	//{
	//	// Most likely, the server is not up. Do nothing.
	//	if(!mConn->connect(err) )
	//	{
	//		 OmnSleep(3);
	//	}
	//}

	//OmnConnBuffPtr buff;
	//rslt = mConn->smartRead(buff);
	//if (!rslt)
	//{
	//	if(!mConn->connect(err) )
	//	{
	//	    OmnSleep(3);
	//	}
	//	// The remote server is not working properly. 
	//	OmnAlarm << "Remote reading failed" << enderr;
	//	return false;
	//}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(buff, "" AosMemoryCheckerArgs);
	AosXmlTagPtr resp;
	if (!resproot || !(resp = resproot->getFirstChild()))
	{
		OmnAlarm << "Remote server respond negative" << enderr; 
		return false;
	}

	if (resp->getAttrBool(AOSTAG_STATUS, false))
	{
		// The request was sent and positively confirmed. 
		return true;
	}

	OmnAlarm << "Remote server respond negative: " << resp->toString() << enderr;
	return false;
}


bool
AosRemoteBackupClt::getConn()
{
	try
	{
		mNumPorts = 1;
		mConn = OmnNew OmnTcpClient("nn", mRemoteAddr, 
				mRemotePort, mNumPorts, eAosTLT_FirstFourHigh);	
		OmnString str;
		aos_assert_r(mConn->connect(str), false);
		return true;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating connection: " << mRemoteAddr.toString()
			<< ":" << mRemotePort << ":" << mNumPorts << enderr;
		return false;
	}
}


bool
AosRemoteBackupClt::processOneRecordLocked()
{
	// This function reads in all the records in the table, sends
	// them to the remote site. 
	//OmnString stmt = "select seqno, length, contents from ";
	//stmt << AOSTABLENAME_REMOTE_BACKUP_CLT << " limit 1";
	aos_assert_r(mTableName != "", false);
	OmnString stmt = "select seqno, length, contents from ";
	stmt << mTableName << " limit 1";

	OmnDbRecordPtr record;
	OmnRslt err;
	bool rslt = OmnDbRecord::retrieveRecord(stmt, record);
	if (!rslt)
	{
		checkDbRecordsLocked();
		if (!mHasRecordsInDb)
		{
			// This means there are no more records in db. 
			return true;
		}

		OmnAlarm << "Failed retrieving record!" << enderr;
		return false;
	}
		
	if (!record)
	{
		checkDbRecordsLocked();
		aos_assert_r(!mHasRecordsInDb, false);
		return true;
	}

	int64_t seqno = record->getInt64(0, -1, err);
	int  xml_length = record->getInt(1, -1, err);
	aos_assert_r(xml_length, false);

	AosBuffPtr buff = OmnNew AosBuff(xml_length, 0 AosMemoryCheckerArgs);
	aos_assert_r(buff, false);
	record->getBinary(2, buff->data(),  xml_length, err);
	buff->setDataLen(xml_length);

	OmnString ss(buff->data(), buff->dataLen());
	if (seqno < 0 || ss == "")
	{
		OmnAlarm << "Record incorrect: " << seqno << ":" << ss << enderr;
		return false;
	}
			
	if (!sendRequest(ss))
	{
		// Possibly the remote server is not up yet. 
		return false;
	}

	// Need to delete the record
	stmt = "delete from ";
	//stmt << AOSTABLENAME_REMOTE_BACKUP_CLT << " where seqno = " << seqno;
	aos_assert_r(mTableName != "", false);
	stmt << mTableName << " where seqno = " << seqno;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);
	store->runSQL(stmt);
	return true;
}


AosRemoteBkReqPtr 
AosRemoteBackupClt::getRequest(
		const OmnString &type, 
		const u64 &transid,
		const AosXmlTagPtr &doc)
{
	return OmnNew AosRemoteBkReq(type, transid, doc);
}

AosRemoteBkReqPtr
AosRemoteBackupClt::getRequest(
		const OmnString &type,
		const u64 &transid,
		const u64 &iilid,
		const AosBuffPtr &buff,
		const u32 &siteid)
{
	return OmnNew AosRemoteBkReq(type, transid, iilid, buff, siteid); 
}


AosRemoteBkReqPtr 
AosRemoteBackupClt::getRequest(
		const OmnString &type, 
		const u64 &transid,
		const u64 &iilid,
		const OmnString &word,
		const u32 &siteid)
{
	return OmnNew AosRemoteBkReq(type, transid, iilid, word, siteid);
}

void
AosRemoteBackupClt::checkDbRecordsLocked()
{
	// This function checks whether the table is empty.
	OmnString stmt = "select seqno, contents from ";
	//stmt << AOSTABLENAME_REMOTE_BACKUP_CLT << " limit 1";
	aos_assert(mTableName != "");
	stmt << mTableName << " limit 1";

	OmnDbRecordPtr record;
	OmnRslt err;
	bool rslt = OmnDbRecord::retrieveRecord(stmt, record);
	if (!rslt || !record)
	{
		mHasRecordsInDb = false;
	}
	else
	{
		mHasRecordsInDb = true;
	}
}

bool
AosRemoteBackupClt::createDbTable()
{
	aos_assert_r(mTableName != "", false);
	OmnString stmt = "create table if not exists ";
	stmt << mTableName << " ( " 
		<< "seqno INT NOT NULL AUTO_INCREMENT, "
		<< "contents text, "
		<< "primary key (seqno))";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	store->runSQL(stmt);
	return true;
}
