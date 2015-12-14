////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataMgr.cpp
// Description:
//	This class is designed for:
//	1. Synchronize data among redundant pair SPNRs
//  2. Populate networkwide data
//  3. Provide local data service to SPNR
//  4. Cache networkwide data   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/DataMgr.h"

#include "Database/MySQL.h"
#include "Database/Database.h"
#include "Database/RawTable.h"
#include "Database/DbRecord.h"
#include "MgcpEpMgr/MgcpEpMgr.h"
#include "MgcpEpMgr/MgcpEndpoint.h"
#include "NMS/Nms.h"
#include "RepliServer/MgcpEptRepli.h"
#include "RepliServer/MgcpConnRepli.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "SysObj/SoMgcpEndpoint.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlParser.h"

OmnSingletonImpl(OmnDataMgrSingleton, 
				 OmnDataMgr, 
				 OmnDataMgrSelf, 
				 "OmnDataMgr");


OmnDataMgr::OmnDataMgr()
:
//mEndpointReplicator(new OmnMgcpEptRepli()),
mQueueLock(OmnNew OmnMutex()),
mQueueCondVar(OmnNew OmnCondVar())
//mConnReplicator(new OmnMgcpConnRepli()),
{
	//
	// Create the Database
	//
	//mDatabase = new OmnMySQL(mUserName, mPassword, mHostName, mDbName,
	//					mDbPort, mConfigFileName);
	mDatabase = OmnNew OmnMySQL("spnr", "spnr", "localhost", "spnr", 3306, "my");
    if (!mDatabase->openDatabase())
    {
        return;
    }

    OmnTrace << "Database creation successful" << endl;
}


bool
OmnDataMgr::start()
{
	return true;
}


bool
OmnDataMgr::stop()
{
	return true;
}


OmnDataMgr::~OmnDataMgr()
{
}


OmnRslt
OmnDataMgr::config(const OmnSysObjPtr &def)
{
	return true;
}


OmnRslt
OmnDataMgr::config(const OmnXmlParserPtr &def)
{
	// 
	// Only central db and Stream Router need configuring 
	// DataMgr.
	//
	// Chen Ding, 09/12/2003
	// if (OmnNms::isStreamRouter() || OmnNms::isCentralDb())
	if (OmnNms::isMediaRouter() || OmnNms::isCentralDb())
	{
		OmnXmlItemPtr conf = def->tryItem(OmnXmlItemName::eDataMgrConfig);
		if (conf.isNull())
		{
			return OmnWarn << OmnErrId::eWarnConfigError
				<< "Missing DataMgrConfig" << enderr;
		}

		mDbId = conf->getInt(OmnXmlItemName::eDatabaseId, -1);
	}

	return true;
}


bool	
OmnDataMgr::getMgcpEndpoints(OmnValList<OmnSoMgcpEndpoint> &list)
{
	return false;
}


bool	
OmnDataMgr::getLocalActiveEndpoints(const int ipNetId,
				std::vector<OmnSoMgcpEndpointPtr> &list)
{
	//
	// Each SPNR has a local endpoint database, which contains all the
	// endpoints in the system. This database can be too big to load
	// it into memory. Instead, it calls this function to load endpoints
	// that are local to a specific IP network and are active. 
	// 

//	OmnString sqlStmt = OmnSqlStmt::getLocalActiveEndpoints(ipNetId);

//	if (!mEndpointTable->getLocalActiveEndpoints(ipNetId, list))
//	{
//		OmnError::log(OmnFileLine, 0, OmnErrId::eFailedToRetrieveLocalActiveEndpoint);
//		return false;
//	}

	return true;
}


bool	
OmnDataMgr::mgcpConnCreated(const OmnSoMgcpConn &conn)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. If it is a root to the connection, the connection should be
	//    sent to the Conn Center.
	//
	return false;
}


bool	
OmnDataMgr::mgcpConnDeleted(const OmnSoMgcpConn &conn)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. If it is a root to the connection, the connection should be
	//    sent to the Conn Center.
	//
	return false;
}


bool	
OmnDataMgr::mgcpConnModified(const OmnSoMgcpConn &conn)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. If it is a root to the connection, the connection should be
	//    sent to the Conn Center.
	//
	//return mConnReplicator->sendReplication(conn);

	OmnErr << "Not implemented yet" << enderr;
	return false;
}


bool	
OmnDataMgr::endpointCreated(const OmnSoMgcpEndpointPtr &endpoint)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. It saves the endpoint into the database
	//
	//return mEndpointTable->insertRecord(endpoint);
	OmnErr << "Not implemented yet" << enderr;
	return 0;
}


bool	
OmnDataMgr::endpointDeleted(const OmnString &spnrEID)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. It deletes the endpoint from the database
	//
	//return mEndpointTable->deleteRecord(spnrEID);
	OmnErr << "Not implemented yet" << enderr;
	return 0;
}


bool	
OmnDataMgr::endpointModified(const OmnSoMgcpEndpointPtr &endpoint)
{
	//
	// It may do two things:
	// 1. If there are mirrors, it should synchronize the mirrors. 
	// 2. It modifies the endpoint in the database
	//
	//return mEndpointTable->modifyRecord(endpoint);
	OmnErr << "Not implemented yet" << enderr;
	return 0;
}


/*
bool 
OmnDataMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter OmnDataMgr::threadFunc." << endl;

	OmnSysObjPtr data;

	while (state == OmnThrdStatus::eActive)
	{
		//
		// Start the timer (one second)
		//
		mQueueLock->lock();
		if (mDataQueue.entries() <= 0)
		{
			//
			// No more data to process.
			//
			mQueueCondVar->wait(mQueueLock);
			mQueueLock->unlock();
			continue;
		}

		//
		// Found some data
		//
		data = mDataQueue.popFront();
		mQueueLock->unlock();

		switch (data->getType())
		{
		case OmnMsgId::eMgcpEndpoint:
			 mEndpointReplicator->recvReplication(*data.getPtr());
			 OmnErr << "Not implemented yet" << enderr;
			 break;

		case OmnMsgId::eMgcpConn:
			 mConnReplicator->recvReplication(*data.getPtr());
			 break;

		default:
			 OmnError::log(OmnFileLine, 0, OmnErrId::eUnclaimedDataMgrData);
			 break;
		}
	}

	OmnTrace << "Leave OmnDataMgr::threadFunc" << endl;
	return true;
}


bool 
OmnDataMgr::signal(const int threadLogicId)
{
	mQueueLock->lock();
	mQueueCondVar->signal();
	mQueueLock->unlock();
	return true;
}
*/


OmnRslt
OmnDataMgr::query(const OmnString &stmt, OmnDbRecordPtr &r)
{
	return mDatabase->query(stmt, r);
}


OmnRslt
OmnDataMgr::query(const OmnString &query, OmnRawTablePtr &raw)
{
	return mDatabase->query(query, raw);
}


OmnMgcpEndpointPtr  
OmnDataMgr::getEndpointByPid(const OmnString &pid)
{
	OmnErr << "Not implemented yet" << enderr;
	return 0;
}


OmnRslt 
OmnDataMgr::updateEndpointIpAddr(const OmnMgcpEndpointPtr &ep)
{
	return OmnErr << "Not implemented yet" << enderr;
}


OmnRslt 
OmnDataMgr::updateDb(const OmnString &stmt, OmnString &errMsg)
{
	return OmnErr << "Not implemented yet" << enderr;
}


OmnRslt
OmnDataMgr::runSQL(const OmnString &query)
{
	return mDatabase->runSQL(query);
}


// 
// Chen Ding, 05/07/2003
//
bool
OmnDataMgr::clearTable(const OmnString &tablename) const
{
	OmnString stmt = "delete from ";
	stmt << tablename;
	OmnTraceDb << "Clear table: " << tablename << endl;

	mDatabase->runSQL(stmt);
	return true;
}
	

int
OmnDataMgr::shrinkTable(const OmnString &tablename,
						const int p) const
{
	//
	// Call this function to remove percentage% of the records
	// from the table.
	//
	// If successful, it returns the number of remaining records. 
	// Otherwise, it returns -1. 
	//
	// This function assumes there is a field 'seqno' in the table. 
	// Otherwise, it is an error to call this function.
	//
	if (p >= 100)
	{
		if (!clearTable(tablename))
		{
			return -1;
		}

		return 0;
	}

	int percentage = p;
	if (percentage < 10)
	{
		// 
		// Will remove at least 10 percent.
		//
		percentage = 10;
	}

	// 
	// Retrieve the max number
	//
	OmnString stmt;
    stmt << "select max(seqno), min(seqno) from " << tablename;
	OmnDbRecordPtr record;
	OmnDataMgrSelf->query(stmt, record);
	int maxSeqno;
	int minSeqno;
	if (!record.isNull())
	{
		maxSeqno = record->getInt(0, 0);
		minSeqno = record->getInt(1, 0);
		OmnTraceDb << "Retrieved max seqno: " << maxSeqno
			<< ", min seqno: " << minSeqno	
			<< " from " << tablename << endl;
	}
	else
	{
		OmnAlarm << OmnErrId::eAlarmDatabaseError
			<< "Failed to retrieve max/min seqno from "
			<< tablename << enderr;
		return -1;
	}

	// 
	// Calculate the seqno below which all records shall be
	// removed.
	//
	int seqno = (maxSeqno - minSeqno) * percentage + minSeqno;
	stmt = "delete from ";
	stmt << tablename << " where seqno < " << seqno;

	OmnRslt rslt = mDatabase->runSQL(stmt);
	if (rslt.failed())
	{
		OmnAlarm << OmnErrId::eAlarmDatabaseError
			<< "Failed to remove records from: " << tablename << enderr;
		return -1;
	}

	return maxSeqno - seqno + 1;
}

