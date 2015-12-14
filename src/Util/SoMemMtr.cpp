////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SoMemMtr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#if 0
#include "Util/SoMemMtr.h"

#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/GetTime.h"
#include "Util/SerialTo.h"
#include "Util/SerialFrom.h"


OmnSoMemMtr::OmnSoMemMtr()
:
OmnObject(OmnClassId::eOmnSoMemMtr),
OmnSysObj(OmnClassId::eOmnSoMemMtr),
mIsGood(false),
mLocId(0),
mLine(0),
mNewNum(0),
mDelNum(0),
mMemSize(0)
{
}


OmnSoMemMtr::~OmnSoMemMtr()
{
}


OmnString   
OmnSoMemMtr::toString() const
{
	//
	// Call this function to serialize the object to send through a connection.
	// The contents of each member data is put into a pair of <>. No spaces
	// are between fields. 
	//
	OmnString data;
   	data<< "<"
		<< mIsGood			<< "><"
		<< mSnapshotId		<< "><"
		<< mLocId			<< "><"
        << mFileName		<< "><"
		<< mLine 			<< "><"
		<< mNewNum 			<< "><"
		<< mDelNum			<< "><"
		<< mMemSize			<< ">";
	return data;
}


bool
OmnSoMemMtr::serializeTo(OmnSerialTo &s) const
{
	s.push(AosMsgId::eSoMemMtr);
//	s.push(eIsGood,mIsGood);
	s.push(eSnapshotId,mSnapshotId);
    s.push(eLocId,mLocId);
    s.push(eFileName,mFileName);
    s.push(eLine,mLine);
    s.push(eNewNum,mNewNum);
    s.push(eDelNum,mDelNum);
	s.push(eMemSize,mMemSize);
	return true;
}


bool
OmnSoMemMtr::serializeFrom(OmnSerialFrom &s) 
{
//	s.pop(eIsGood,mIsGood);
	mIsGood = true;
    s.pop(eLocId,mLocId);
	s.pop(eSnapshotId,mSnapshotId);
    s.pop(eFileName,mFileName);
    s.pop(eLine,mLine);
    s.pop(eNewNum,mNewNum);
    s.pop(eDelNum,mDelNum);
	s.pop(eMemSize,mMemSize);
    return true;
}


OmnString
OmnSoMemMtr::sqlFields() const
{
	return "snap_shot_id ,local_id ,filename ,line ,new_num,free_num,size";
}


OmnString
OmnSoMemMtr::sqlStmtInsert() const
{
	//
	// It creates a SQL statement to insert an endpoint into the SQL database
	//
	OmnString stmt = "insert into mem_mtr(";

	stmt << sqlFields() << ") values ( ";

    stmt << mSnapshotId		<< ",";
    stmt << mLocId		   	<< ",'";
    stmt << mFileName		<< "',";
    stmt << mLine			<< ",";
    stmt << mNewNum			<< ",";
    stmt << mDelNum			<< ",";
	stmt << mMemSize		<< ")";
	return stmt;
}


bool
OmnSoMemMtr::serializeToDb(const int snapshotId)
{
	mSnapshotId = snapshotId;
	OmnString stmt = sqlStmtInsert();
    bool rslt = OmnStoreMgrSelf->runSQL(stmt);

    if (!rslt)
    {
        OmnWarn << "Failed to insert table!" << enderr;
    }
    return rslt;
}


bool 
OmnSoMemMtr::retrieve(const int locId)
{
	// 1. Create the SQL statement
	OmnString stmt = "select ";
	stmt << "select "
		 << sqlFields() 
		 << " from mem_mtr where local_id=" << locId;

	// 2. Access the database
	OmnDbRecordPtr record;
	OmnStoreMgrSelf->query(stmt, record);
	
	if (record.isNull())
	{
		return false;
	}
	
	// 3. Construct SoDevice
	serializeFromDb(record);
	return true;
}


OmnRslt 
OmnSoMemMtr::serializeFromDb(const OmnDbRecordPtr &record)
{
	OmnRslt rslt;
	mSnapshotId =  		record->getInt(0,-1, rslt);
	mLocId	 	= 		record->getInt(1,0, rslt);
	mFileName 	=		record->getStr(2,"Error file", rslt);
	mLine		=		record->getInt(3,-1, rslt);
	mNewNum		=		record->getInt(4,0, rslt);
	mDelNum		=		record->getInt(5,0, rslt);

	return rslt;
}


int			
OmnSoMemMtr::objSize() const
{
	// XXX Not implemented yet
	return sizeof(*this);
}

#endif
