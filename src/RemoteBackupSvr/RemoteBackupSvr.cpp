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
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupSvr/RemoteBackupSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Database/DbRecord.h"
#include "SqlUtil/TableNames.h"
#include "Heartbeat/HbMsgNames.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Rundata/Rundata.h"
#include "RemoteBackupSvr/Ptrs.h"
#include "RemoteBackupSvr/BackupProc.h"
#include "RemoteBackupUtil/RemoteBkUtil.h"
#include "SEUtil/Ports.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


extern int gAosLogLevel;
static OmnString sgLocalAddr = "0.0.0.0";
static OmnString sgServerName = "RemoteBkSvr";
OmnMutexPtr AosRemoteBackupSvr::smLock = OmnNew OmnMutex(); 
OmnCondVarPtr AosRemoteBackupSvr::smCondVar = OmnNew OmnCondVar();
OmnThreadPtr AosRemoteBackupSvr::smThread;
bool	AosRemoteBackupSvr::smHasRecordsInDb = true;
AosU642U64_t AosRemoteBackupSvr::smLastTransid;

static int num = 0;

AosRemoteBackupSvr::AosRemoteBackupSvr()
:
mRundata(OmnApp::getRundata())
{
}


AosRemoteBackupSvr::~AosRemoteBackupSvr()
{
}


bool
AosRemoteBackupSvr::configStatic(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosRemoteBackupSvr::stopStatic()
{
    return true;
}


bool				
AosRemoteBackupSvr::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	if (gAosLogLevel >= 2)
	{
		cout << hex << "<0x" << pthread_self() << dec 
			<< ":" << __FILE__ << ":" << __LINE__
			<< "> Process request: (transid: " << req->getTransId()
			<< ")\n" << req->getData() << endl;
	}

	char *data = req->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;
	
	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << mRundata->getErrmsg() << ": " << data << enderr;
		sendResp(req, "false", "Internal Error");
		return false;
	}

	u32 siteid = child->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid,  false);
	mRundata->setSiteid(siteid);
	OmnString errmsg;
	if (!saveToDb(child, errmsg))
	{
		sendResp(req, "false", errmsg);
		return false;
	}
	sendResp(req, "true", "");
	return true;
}


AosNetReqProcPtr
AosRemoteBackupSvr::clone()
{
	return OmnNew AosRemoteBackupSvr();
}


bool
AosRemoteBackupSvr::startStatic(const AosNetReqProcPtr &server)
{
	static AosReqDistr lsReqDistr(server);
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	AosXmlTagPtr hbconf;
	if (conf)
	{
		hbconf = conf->getFirstChild("heartbeat");
	}

	if (hbconf)
	{
		if (!lsReqDistr.config(hbconf))
		{
			OmnAlarm << "Failed the configuration: " 
				<< OmnApp::getAppConfig()->toString() << enderr;
			exit(-1);
			return false;
		}
	}
	else
	{
		lsReqDistr.config(sgLocalAddr, AOSPORT_HEARTBEAT, sgServerName, eDftMaxConns);
	}
	lsReqDistr.start();

	//OmnThreadedObjPtr ss = (OmnThreadedObj *)server.getPtr();
	OmnThreadedObjPtr thisPtr((AosRemoteBackupSvr *)server.getPtr(), false);
	smThread = OmnNew OmnThread(thisPtr, "remotebackupsvr", 0, true, true, __FILE__, __LINE__);
	//smThread = OmnNew OmnThread(ss, "remotebackupsvr", 0, true, true, __FILE__, __LINE__);
	smThread->start();
	
	return true;
} 


void
AosRemoteBackupSvr::sendResp(
		const AosWebRequestPtr &req, 
		const OmnString &status,
		const OmnString &contents) 
{
	OmnString resp = "<response zky_status=\"";
	resp << status << "\">" << contents << "</response>";

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< ")\n" << resp << endl;
	}

	req->sendResponse(resp);
}


void
AosRemoteBackupSvr::sendResp(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
}


bool	
AosRemoteBackupSvr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	int lastChecked = eForceCheck;
	smHasRecordsInDb = 0;
    while (state == OmnThrdStatus::eActive)
	{
		smLock->lock();
		if (lastChecked-- && !smHasRecordsInDb)
		{
			smCondVar->wait(smLock);
			smLock->unlock();
			continue;
		}
		if (lastChecked == 0) lastChecked = eForceCheck;
		smLock->unlock();
		procOneRecord();
	}
	return true;
}


bool	
AosRemoteBackupSvr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosRemoteBackupSvr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosRemoteBackupSvr::saveToDb(
		const AosXmlTagPtr &req, 
		OmnString &errmsg)
{
	// 'req' is in the form:
	// 	<req AOSTAG_TRANSID="xxx" 
	// 		AOSTAG_TYPE="xxx" .../>
	u64 transid = req->getAttrU64(AOSTAG_TRANSID, 0);
	if (transid == 0)
	{
		errmsg = "Missing transaction id";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	u32 clientid = 0;
	u32 trans_seqno = 0;
	AosRemoteBkUtil::decomposeTransid(transid, clientid, trans_seqno);
	if (clientid == 0)
	{
		errmsg = "Missing client id";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosU642U64Itr_t itr = smLastTransid.find(clientid);
	if (itr != smLastTransid.end())
	{
		u64 dd = itr->second;
		if (trans_seqno <= dd)
		{
			// Duplicated transid
			return true;
		}
	}

	AosRemoteBkType::E tt = AosRemoteBkType::toEnum(req->getAttrStr(AOSTAG_TYPE));
	if (!AosRemoteBkType::isValid(tt))
	{
		errmsg = "Unrecognized transaction type ";
		OmnAlarm << errmsg << req->getAttrStr(AOSTAG_TYPE) << enderr;
		return false;
	}

	AosBackupProcPtr proc = AosBackupProc::getProc(tt);
	if (!proc)
	{
		errmsg = "Unrecognized request";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString ss = req->toString();
	//ss.escapeSymbal('\'', '\'');
	int length = ss.length();
if (num++ >= 50)
{
OmnScreen << "transid :" << transid << " length:" << length << endl;
num = 0;
}
	// save record to database; 
	OmnString stmt = "insert into ";
	stmt << AOSTABLENAME_REMOTE_BACKUP_SVR<< "(transid, type, length, xml)"
		<< " values(?,?,?,?)";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);

	int type = tt;
	if (!store->insertBinary(stmt, transid, type, length, ss))
	{
		errmsg = "Failed the database operation";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Update the last transid for the system.
	stmt = "select clientid, last_transid from ";
	stmt << AOSTABLENAME_REMOTE_BKSVR_MGMT << " limit 1";
	OmnDbRecordPtr record;
	OmnRslt err;
	bool rslt = OmnDbRecord::retrieveRecord(stmt, record);
	if (rslt && record)
	{
		stmt = "update ";
		stmt << AOSTABLENAME_REMOTE_BKSVR_MGMT << " set last_transid="
			<< trans_seqno << " where clientid=" << clientid;
	}
	else
	{
		stmt = "insert into ";
		stmt << AOSTABLENAME_REMOTE_BKSVR_MGMT << "(clientid, last_transid)"
			<< " values(" << clientid << ", " << trans_seqno << ")"; 
	}
	if (!store->runSQL(stmt))
	{
		errmsg = "Failed update the database";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	smLock->lock();
	smLastTransid[clientid] = trans_seqno;
	smHasRecordsInDb = true;
	smCondVar->signal();
	smLock->unlock();
	return true;
}


bool
AosRemoteBackupSvr::procOneRecord()
{
	OmnString stmt = "select transid, type, length, xml from ";
	stmt << AOSTABLENAME_REMOTE_BACKUP_SVR << " limit 1";
	OmnDbRecordPtr record;
	OmnRslt err;
	bool rslt = OmnDbRecord::retrieveRecord(stmt, record);
	if (!rslt || !record)
	{
		smHasRecordsInDb = false;
		return true;
	}

	u64 transid = record->getU64(0, 0, err);
	AosRemoteBkType::E type = (AosRemoteBkType::E)record->getInt(1, -1, err);

	int xml_length = record->getInt(2, -1, err);
	aos_assert_r(xml_length, false);

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(xml_length);
	aos_assert_r(buff, false);
	record->getBinary(3, buff->getData(), xml_length, err);
	buff->setDataLength(xml_length);

	aos_assert_r(transid, false);
	aos_assert_r(AosRemoteBkType::isValid(type), false);

	AosXmlParser parser;
	AosXmlTagPtr req = parser.parse(buff, "" AosMemoryCheckerArgs);
	aos_assert_r(req , false);

	AosXmlTagPtr child = req->getFirstChild();
	aos_assert_r(child, false);

	// start Proc 
	AosBackupProc::checkRegistrations();

	AosBackupProcPtr proc = AosBackupProc::getProc(type);
	aos_assert_r(proc, false);
	proc->proc(transid, child, mRundata);

	// Need to remove the record.
	stmt = "delete from ";
	stmt << AOSTABLENAME_REMOTE_BACKUP_SVR << " where transid=" << transid;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);
	store->runSQL(stmt);
	return true;
}


