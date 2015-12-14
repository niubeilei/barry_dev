//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 04/13/2009: Created by Sharon Shen
// 01/01/2013: Rewritten by Chen Ding
//////////////////////////////////////////////////////////////////////////
#include "LogClient/LogClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "LogUtil/LogEntry.h"
#include "LogUtil/LogEntryCtnr.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TransClient/TransModuleClt.h"
#include "TransClient/TransDistributor.h"
#include "TransUtil/SeLogReqRobin.h"
#include "Util/CompUint.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosSeLogClientSingleton,
				 AosSeLogClient,
				 AosSeLogClientSelf,
				 "AosSeLogClient");


AosSeLogClient::AosSeLogClient()
{
}



AosSeLogClient::~AosSeLogClient()
{
}


bool
AosSeLogClient::config(const AosXmlTagPtr &config)
{
	AosXmlTagPtr log_config = config->getFirstChild("SeLogClt");
	if (!log_config)
	{
		OmnAlarm << "No SeLogClient config!" << enderr;
		return false;
	}
	AosTransDistributorPtr idRobin = 
		OmnNew AosSeLogReqRobin(AosGetNumCubes(), AosGetNumPhysicals());
	mTransClient = OmnNew AosTransModuleClt(idRobin, AosTransModule::eLog, NULL);
	return true;
}


bool
AosSeLogClient::start()
{
	return true;
}


bool
AosSeLogClient::stop()
{
	return true;
}


bool
AosSeLogClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosLogEntryPtr entry;
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mPendingQueue.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		entry = mPendingQueue.front();
		mPendingQueue.pop();
		if (mPendingQueue.size() < mMaxQueueSize && mFullWaiters > 0)
		{
			mFullCondVar->signal();
		}
		mLock->unlock();
		procOneEntry(entry);
	}

	return true;
}


bool	
AosSeLogClient::signal(const int threadLogicId)
{
	return true;
}


void    
AosSeLogClient::heartbeat(const int tid)
{
}


bool
AosSeLogClient::procOneEntry(const AosLogEntryPtr &entry)
{
	AosRundataPtr rdata = entry->getRundata();
	AosXmlTagPtr container = entry->getLogContainer();
	if (!container)
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}
	u64 ctnr_docid = container->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(ctnr_docid, rdata, false);

	// 1. Assign a docid to the entry. 
	AosCompUint docid = getNextDocid(container, rdata);
	aos_assert_rr(docid.isValid(), rdata, false);
	entry->setDocid(docid);

	// 2. Append the entry to the cache.
	mapitr_t itr = mMap.find(ctnr_docid);
	AosLogEntryCtnrPtr ctnr;
	if (itr == mMap.end())
	{
		// No entry found. Add one.
		ctnr = OmnNew AosLogEntryCtnr();
		mMap[ctnr_docid] = ctnr;
	}
	else
	{
		ctnr = itr->second;
	}
	u64 entries = ctnr->appendEntry(entry);
	if (entries > mCrtMax)
	{
		if (mHeader)
		{
			mHeader->moveToFront(ctnr);
		}
		mHeader = ctnr;
	}

	// 3. If the cache is full, process the one that has
	//    the most entries. 
	mTotalEntries++;
	if (mTotalEntries > mMaxCached)
	{
		// It needs to process the header.
		AosLogEntryCtnrPtr header = mHeader;
		AosLogEntryCtnrPtr next = mHeader->nextEntry();
		header->removeFromList();
		if (header == next)
		{
			mHeader = 0;
		}
		else
		{
			mHeader = next;
		}

		u64 size = header->size();
		bool rslt = sendTrans(header);
		if (mTotalEntries < size)
		{
			AosSetErrorU(mRundata, "internal_error");
			mTotalEntries = 0;
		}
		else
		{
			mTotalEntries -= size;
		}
	}

	return true;
}


bool
AosLogClient::sendTrans(
		const u64 &ctnr_docid, 
		const AosLogEntryCtnrPtr &header)
{
	// This function sends all the entries contained in 'header'
	// to the current cube. 
	int cube_id = getCubeId(ctnr_docid);
	aos_assert_rr(cube_id >= 0 && cube_id < AosTotalVirtuals(), mRundata, false);

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = header->serializeTo(buff, mRundata);
	aos_assert_rr(rslt, mRundata, false);

	OmnNotImplementedYet;
	return false;
}


int
AosLogClient::getCubeId(const u64 &docid)
{
	cubemapitr_t itr = mCubeMap.find(docid);
	if (itr == mCubeMap.end())
	{
		// Did not find it. Need to create one.
		int cube_id = OmnRand::nextInt(0, AosTotalVirtuals()-1);
		int next_cube_id = cube_id+1;
		if (next_cubeId >= AosTotalVirtuals()) next_cube_id = 0;
		mCubeMap[docid] = next_cube_id;
		return cube_id;
	}

	return itr->second++;
}	


AosCompUint 
AosSeLogClient::getNextDocid(
		const AosXmlTagPtr &container, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosCompUint(0);
}


bool
AosSeLogClient::addLog(
		const AosLogEntryPtr &log_entry,
		const AosRundataPtr &rdata)
{
	// This is the main interface function to add a log entry into 
	// the system:
	// 1. Obtains a transaction ID and assigns the transaction ID to the entry.
	// 2. Save the transaction. 
	// 3. Check whether it is full. If yes, it needs to send the transactions.
	aos_assert_rr(log_entry, rdata, false);
	mLock->lock();

	// 1. Retrieve the log container
	AosLogEntryCtnrPtr container = getLogContainer(log_entry, rdata);
	if (!container)
	{
		// The caller does not have the right to create log.
		mLock->unlock();
		return false;
	}

	// 2. Get the transid
	u64 transid = getNextTransidLocked(rdata);
	log_entry->setTransid(transid);

	// 3. Save the transaction
	bool rslt = saveTrans(log_entry, rdata);
	aos_assert_rl(rslt, mLock, false);

	// 4. Add the entry to the container
	bool need_send = false;
	rslt = container->appendEntry(log_entry, need_send, rdata);
	aos_assert_rl(rslt, mLock, false);
	mTotalEntries++;
	if (need_send)
	{
		rslt = sendLogsLocked(container, rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	if (needSend())
	{
		rslt = sendLogsLocked(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}


u64
AosSeLogClient::getNextTransidLocked()
{
	u32 transid = mTransid++;
	if (mTransid == eMaxTransId) mTransid = 0;
	return mModuleId + transid;
}


bool
AosSeLogClient::sendLogsLocked(const AosRundataPtr &rdata)
{
	// This function send logs for the container that contains the most entries. 
	u64 size = 0;
	AosLogEntryCtnrPtr container;
	for (mapitr_t itr = mMap.begin(); itr != mMap.end(); itr++)
	{
		AosLogEntryCtnrPtr cc = itr->second;
		aos_assert_rr(cc, rdata, false);

		u64 ss = cc->size();
		if (ss > size)
		{
			container = cc;
			size = ss;
		}
	}

	aos_assert_rr(container, rdata, false);
	return sendLogsLocked(container, rdata);
}


bool
AosSeLogClient::sendLogsLocked(
		const AosLogEntryCtnrPtr &container, 
		const AosRundataPtr &rdata)
{
	// This function sends the logs for 'container'. This is done 
	// by a separate thread. 
	u64 size = container->size();
	removeContainer(container, rdata);

	if (mTotalEntries < size)
	{
		OmnAlarm << "Internal error: " << mTotalEntries << ":" << size << enderr;
		mTotalEntries = 0;
	}
	else
	{
		mTotalEntries -= size;
	}

	AosSendLogThrdPtr runner = OmnNew AosSendLogThrd(container, rdata);
	//OmnThreadShellMgr::getSelf()->proc(runner);
	return true;
}


/*
AosXmlTagPtr
AosSeLogClient::createTrans(
		const OmnString &operate, 
		const OmnString &pctr_objid,
		const OmnString &logname,
		const OmnString &log_contents,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(pctr_objid != "", rdata, 0);
	
	u64 docid;
	if (pctr_objid == AOSOBJIDPRE_ROOTCTNR || pctr_objid == AOSSTYPE_SYSACCT)
	{
		docid = AOSDOCID_ROOTCTNR;
	}
	else
	{
		docid = AosDocClientObj::getDocClient()->getDocidByObjid(pctr_objid, rdata);
		aos_assert_r(docid > 0, 0);

	}
	// Set Container objid into contents
	AosXmlParser parser;
	AosXmlTagPtr contents = parser.parse(log_contents, "" AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);
	contents->setAttr(AOSTAG_LOG_CTNROBJID, pctr_objid);

	// Set the time created to contents
	u64 crtLogTime = OmnGetTimestamp();
	contents->setAttr(AOSTAG_CT_EPOCH, crtLogTime);
	OmnString lname = "";
	if(logname == "")
	{
		lname = AOSTAG_DFT_LOGNAME;
	}
	else
	{
		lname = logname;
	}

	// Linda, 2012/12/26
	AosXmlTagPtr workdoc;
	if (contents)
	{
		workdoc = rdata->getWorkingDoc();
		rdata->setWorkingDoc(contents, true);
	}

	if (!AosEvent::checkEvent1(0, contents, AOSEVENTTRIGGER_LOG_CREATED, rdata))
	{
		rdata->setError() << "Failed !!!!!!!!!";
		AOSMONITORLOG_FINISH(rdata);
	}
	if (workdoc) rdata->setWorkingDoc(workdoc, false);

	OmnString ssid = rdata->getSsid();
	OmnString trans = "<request ";
	trans <<  "ssid=\"" << ssid << "\" "
		  << AOSTAG_LOG_PCTROBJID << "=\"" << pctr_objid << "\" "
		  << "logname=\""<< lname <<"\" >"
		  << "<loginfo>" << contents->toString() << "</loginfo>"
		  << "</request>";
			  
	AosXmlTagPtr root = parser.parse(trans, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, 0);
	root->setAttr(AOSTAG_CTNRDOCID, docid);
	return root;
}


bool
AosSeLogClient::addLogTrans(
		const AosXmlTagPtr &trans,
		const AosRundataPtr &rdata)
{
	if (!mStartAddLog) return true;
	
	aos_assert_rr(mTransClient, rdata, false);
	
	u64 docid = trans->getAttrU64(AOSTAG_CTNRDOCID, 0);
	aos_assert_r(docid>0, false);
	if(!mTransClient->addTrans(rdata, trans, true, docid))
	{
		rdata->setError() << "fail to addLogTrans: " << trans->toString() ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}
*/


AosXmlTagPtr 
AosSeLogClient::retrieveLog(
		const AosCompUint &logid,
		const AosRundataPtr &rdata)
{
	/*
	// 'logid' is in the following format:
	// 		Reserved Byte 	(1 byte)
	// 		Seqno			(20 bits)
	// 		Offset			(24 bits)
	// 		ModuleID		(10 bits)
	OmnString docstr = "";
	docstr << "<request " << AOSTAG_LOG_OPR
		<< "=\"" << AOSLOGOPR_RETRIEVE << "\" >"
		<< "<![CDATA[" << logid << "]]>"
		<< "</request>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, 0);
	
	AosXmlTagPtr resp = addRetrieveTransInSameCtnr(doc, rdata, logid);
	aos_assert_rr(resp, rdata, 0);
	AosXmlTagPtr content = resp->getFirstChild("Contents");
	aos_assert_r(content, 0);
	AosXmlTagPtr log = content->getFirstChild();
	aos_assert_r(log, 0);
	if (log->isRootTag())
	{
		log = log->getFirstChild();
	}
	
	AosXmlTagPtr thelog = parser.parse(log->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(thelog, 0);
	return thelog;
	*/
	OmnNotImplementedYet;
	return 0;
}

