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
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/DataScanner.h"

#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"

#include "DataScanner/DocScanner.h"
#include "DataScanner/FileScanner.h"
#include "DataScanner/DataDclScanner.h"
#include "DataScanner/DirScanner.h"
#include "DataScanner/UnicomDirScanner.h"
#include "DataScanner/BinaryDocScanner.h"

static AosDataScannerObjPtr sgScanners[AosDataScannerType::eMax];
static OmnString			sgNames[AosDataScannerType::eMax];
static OmnMutex				sgLock;
static OmnMutex				sgInitLock;
static bool					sgInited = false;
static u64					sgInstanceId = 0;


AosDataScanner::AosDataScanner(
		const OmnString &name, 
		const AosDataScannerType::E type,
		const bool flag)
:
AosDataScannerObj(0),
mDiskError(false)
{
	sgLock.lock();
	mInstanceId = sgInstanceId++;
	sgLock.unlock();

	mType = type;
	if (flag)
	{
		 AosDataScannerObjPtr thisptr(this, false);   // bool false: true :  
		if (!registerScanner(name, thisptr))
		{
			OmnThrowException("Failed registering");
			return;
		}
	}
}


AosDataScanner::AosDataScanner(
		const OmnString &type,
		const u32 version)
:
AosDataScannerObj(version),
mScannerType(type),
mDiskError(false)
{
}


AosDataScanner::~AosDataScanner()
{
}


bool
AosDataScanner::registerScanner(const OmnString &name, const AosDataScannerObjPtr  &scanner)
{
	sgLock.lock();
	if (!AosDataScannerType::isValid(scanner->getType()))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data shuffler type: ";
		errmsg << scanner->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgScanners[scanner->getType()])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << scanner->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgScanners[scanner->getType()] = scanner;
	AosDataScannerType::addName(name, scanner->getType());
	sgLock.unlock();
	return true;
}


bool
AosDataScanner::init(const AosRundataPtr &rdata)
{
	// static AosDocScanner 			lsDocScanner(true);
	// static AosNewIILScanner 		lsNewIILScanner(true);
	//modify by crystal 2015/04/01
	//static AosFileScanner 			lsFileScanner(true);
	//static AosDirScanner 			lsDirScanner(true);
	static AosDocScanner 			lsDocScanner(true);
	static AosDclScanner 			lsDclScanner(true);
	//static AosUnicomDirScanner 		lsUnicomDirScanner(true);
	//static AosBinaryDocScanner 		lsBinaryDocScanner(true);

	sgInitLock.lock();
	if (sgInited) 
	{
		sgInitLock.unlock();
		return true;
	}

	AosDataScannerType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


bool
AosDataScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}


AosDataScannerObjPtr
AosDataScanner::createScannerFromBuff(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	AosDataScannerType::E type = (AosDataScannerType::E) buff->getU32(0);
	if (type == AosDataScannerType::eJimoScanner)
	{
		OmnString jimo_objid = buff->getOmnStr("");
		aos_assert_rr(jimo_objid != "", rdata, 0);
		AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), jimo_objid, 1);
		aos_assert_rr(jimo, rdata, 0);
		AosDataScannerObjPtr scanner = dynamic_cast<AosDataScannerObj*>(jimo.getPtr());
		aos_assert_rr(scanner, rdata, 0);
		return scanner;
	}

	if (AosDataScannerType::isValid(type))
	{
		sgLock.lock();
		AosDataScannerObjPtr scanner = sgScanners[type];
		sgLock.unlock();
		return scanner->clone(0, rdata);
	}

	AosSetErrorU(rdata, "failed_creating_jimo") << type << enderr;
	return 0;
}


AosDataScannerObjPtr
AosDataScanner::createScannerFromXml(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) init(rdata);

	if (worker_doc)
	{
		OmnString typestr = worker_doc->getAttrStr(AOSTAG_TYPE);
		AosDataScannerType::E type = AosDataScannerType::toEnum(typestr);
		if (AosDataScannerType::isValid(type))
		{
			sgLock.lock();
			AosDataScannerObjPtr scanner = sgScanners[type];
			sgLock.unlock();
			return scanner->clone(worker_doc, rdata);
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), worker_doc, jimo_doc);
	if (!jimo)
	{
		OmnString errmsg;
		if (worker_doc)
		{
			errmsg << "Worker Doc: " << worker_doc->toString();
		}

		if (jimo_doc)
		{
			if (errmsg != "") errmsg << ". ";
			errmsg << jimo_doc->toString();
		}
		AosSetErrorUser(rdata, "invalid_scanner_type") << errmsg << enderr;
		return 0;
	}

	AosDataScannerObjPtr scanner = dynamic_cast<AosDataScannerObj*>(jimo.getPtr());
	if (!scanner)
	{
		OmnString errmsg;
		if (worker_doc)
		{
			errmsg << "Worker Doc: " << worker_doc->toString();
		}

		if (jimo_doc)
		{
			if (errmsg != "") errmsg << ". ";
			errmsg << jimo_doc->toString();
		}
		AosSetErrorU(rdata, "invalid_scanner") << ": " << errmsg << enderr;
		return 0;
	}
	return scanner;
}


bool
AosDataScanner::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	mType = (AosDataScannerType::E)buff->getU32(0);
	return true;                                                                      
}


bool
AosDataScanner::serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	buff->setU32(mType);
	return true;                                                                      
}



int64_t
AosDataScanner::getTotalSize() const
{
	OmnNotImplementedYet;
	return 0;
}

int64_t
AosDataScanner::getTotalFileLengthByDir()
{
	OmnNotImplementedYet;
	return 0;
}

	
bool
AosDataScanner::getNextRecord(
		AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



bool 
AosDataScanner::readBuffToQueue(OmnThrdStatus::E &state, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosDataScanner::readBuffFromQueue(AosBuffDataPtr &info, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosDataScanner::isFinished(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



bool 
AosDataScanner::getNextBlock(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


void 
AosDataScanner::setFiles(const vector<AosNetFileObjPtr> &vv)
{
	OmnNotImplementedYet;
}


void 
AosDataScanner::setActionsCaller(const AosActionCallerPtr &caller)
{
	OmnNotImplementedYet;
}


AosBuffPtr 
AosDataScanner::getNextBlock(const u64 &reqId, const int64_t &expected_size)
{
	OmnNotImplementedYet;
	return 0;
}


void    
AosDataScanner::addRequest(const u64 &reqId, const int64_t &blocksize)
{
	OmnNotImplementedYet;
} 


void
AosDataScanner::destroyedMember()
{
	OmnNotImplementedYet;
}


bool
AosDataScanner::getNextBlock(
		AosBuffDataPtr &info, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataConnectorObjPtr
AosDataScanner::firstDataConnector()
{
	OmnShouldNeverComeHere;
	return 0;
}


AosDataConnectorObjPtr
AosDataScanner::nextDataConnector()
{
	OmnShouldNeverComeHere;
	return 0;
}


int 
AosDataScanner::getPhysicalId() const
{
	OmnShouldNeverComeHere;
	return -1;
}


u64
AosDataScanner::getInstanceId() const
{
	return mInstanceId;
}


bool 
AosDataScanner::split(
		const AosRundataPtr &rdata, 
		vector<AosXmlTagPtr> &datasets) const
{
	OmnShouldNeverComeHere;
	return false;
}
	
bool	
AosDataScanner::setRemainingBuff(
		const AosBuffDataPtr &buffdata,
		const bool incomplete,
		const int64_t proc_size,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool	
AosDataScanner::startReadData(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}
/*	
void
AosDataScanner::callBack(
		const u64 &reqId,
		const AosBuffDataPtr &buffdata,
		const bool isFinished)
{
	OmnShouldNeverComeHere;
	return;
}
*/
bool
AosDataScanner::addDataConnector(                  
		const AosRundataPtr &rdata,
		const AosDataConnectorObjPtr &cube)
{
	OmnShouldNeverComeHere;
	return false;
} 


bool 
AosDataScanner::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc)
{
	return true;
}
