////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Testcase.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/Testcase.h"

#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tester/Ptrs.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"

static OmnMutex sgLock;

OmnTestcase::OmnTestcase()
	:
OmnObject(OmnClassId::eOmnTestcase),
#ifdef AOS_DB_SUPPORT
OmnDbObj(OmnClassId::eOmnTestcase),
#endif
mStatus(eNotSetYet)
{
}
 

OmnTestcase::OmnTestcase(const OmnString &name, 
						 const char *file,
						 const int line)
	:
OmnObject(OmnClassId::eOmnTestcase),
#ifdef AOS_DB_SUPPORT
OmnDbObj(OmnClassId::eOmnTestcase),
#endif
mTcName(name),
mStatus(eNotSetYet),
mFile(file),
mLine(line)
{
}


OmnTestcase::~OmnTestcase()
{
}


OmnClassId::E	
OmnTestcase::getClassId() const 
{
	return OmnClassId::eOmnTestcase;
}


OmnTestcase & 
OmnTestcase::operator << (const OmnString &rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const std::string &rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const char *rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const OmnIpAddr &addr)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << addr.toString();
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const int64_t &rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const int rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const u64 &rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const float rhs)
{
	if (mStatus == eNotSetYet)
	{
		OmnAlarm << "Test case constructed wrong!" << enderr;
		return *this;
	}

	if (mStatus == eFailed)
	{
		sgLock.lock();
		mErrmsg << rhs;
		sgLock.unlock();
	}
	return *this;
}


OmnTestcase & 
OmnTestcase::operator << (const bool testCond)
{
	// 
	// This is the testcase testing condition. The testcase is passed
	// if the testCond is true. Otherwise, the testcase fails.
	//
	sgLock.lock();
	if (mStatus == eNotSetYet)
	{
		// 
		// This is the beginning of the test case (i.e., testing the test case).
		//
		mStatus = (testCond)?eSuccess:eFailed;
	}
	else
	{
		// 
		// The test case has already been evaluated. This must be a bool
		// value. Need to set it to the mString.
		//
		if (mStatus == eFailed)
		{
			if (testCond) mErrmsg << "true";
			else mErrmsg << "false";
		}
	}
	sgLock.unlock();
	return *this;
}


OmnRslt	
OmnTestcase::serializeTo(OmnSerialTo &s) const
{
	s.push(OmnMsgId::eTestCase);
	s.push(eTestId, mTestId);
	s.push(eSuiteName, mSuiteName);
	s.push(ePkgName, mPkgName);
	s.push(eTcName, mTcName);
	s.push(eDesc, mDesc);
	s.push(eExpected, mExpected);
	s.push(eActual, mActual);
	s.push(eErrmsg, mErrmsg);
	s.push(eFile, mFile);
	s.push(eLine, mLine);
	s.push(eStatus, (int)mStatus);
	return true;
}


OmnRslt    
OmnTestcase::serializeFrom(OmnSerialFrom &s)
{
	int status;
	s.pop(eTestId, mTestId);
	s.pop(eSuiteName, mSuiteName);
	s.pop(ePkgName, mPkgName);
	s.pop(eTcName, mTcName);
	s.pop(eDesc, mDesc);
	s.pop(eExpected, mExpected);
	s.pop(eActual, mActual);
	s.pop(eErrmsg, mErrmsg);
	s.pop(eFile, mFile);
	s.pop(eLine, mLine);
	s.pop(eStatus, status);

	mStatus = (Status)status;
	if (!isValidStatus(mStatus))
	{
		OmnAlarm << "Invalid status: " << mStatus << enderr;
		mStatus = eUnknown;
	}
	return true;
}


int
OmnTestcase::objSize() const
{
	return sizeof(*this) +
		mTestId.objSize() +
		mSuiteName.objSize() + 
		mPkgName.objSize() + 
		mTcName.objSize() +
		mTcName.objSize() + 
		mDesc.objSize() + 
		mExpected.objSize() + 
		mActual.objSize() + 
		mFile.objSize() + 
		mErrmsg.objSize();
}



void
OmnTestcase::reset()
{
	// It resets all the member to its initial values
	sgLock.lock();
	mTestId = "";
	mSuiteName = "";
	mPkgName = "";
	mTcName = "";
	mDesc = "";
	mExpected = "";
	mActual = "";
	mFile = "";
	mLine = 0;
	mStatus = eNotSetYet;
	mErrmsg = "";
	sgLock.unlock();
}


OmnString
OmnTestcase::toString() const
{
	OmnString str;
	str << "Class OmnTestCase:"
		<< "\n    mTestId:    " << mTestId
		<< "\n    mSuiteName: " << mSuiteName
		<< "\n    mPkgName:   " << mPkgName
		<< "\n    mTcName:    " << mTcName
		<< "\n    mDesc:      " << mDesc
		<< "\n    mExpected:  " << mExpected
		<< "\n    mActual:    " << mActual
		<< "\n    mFile:      " << mFile
		<< "\n    mLine:      " << mLine
		<< "\n    mStatus:    " << mStatus 
		<< "\n    mErrmsg:    " << mErrmsg;
	return str;
}


#ifdef AOS_DB_SUPPORT
void
OmnTestcase::removeRslts(const OmnString &testId)
{
	// 
	// This function removes all test results for test ID: "testId". 
	// This function shall be called before begin a testing for
	// the test ID.
	//
	OmnString stmt = "delete from tc_rslt where test_id='";
	stmt << testId<< "'";

	// 
	// This function saves this suite entry into the database. 
	//
    OmnDataStorePtr store = getStore();

	if (!store)
	{
		return;
	}

	// OmnCheck(!store.isNull(), 
	// 	OmnString("Specified storage not available available: ") << mStoreId);

    OmnTraceDb << "Run SQL statement: \n" << stmt << endl;
    OmnRslt rslt = store->runSQL(stmt);
	if (rslt)
    {
		OmnTraceDb << "SQL statement Successful! " << endl;
	}
}


OmnDbObjPtr	
OmnTestcase::clone() const
{
	sgLock.lock();
	OmnTestcasePtr tc = OmnNew OmnTestcase(mTcName, __FILE__, __LINE__);
	tc.setDelFlag(false);
	tc->mTestId = mTestId;
	tc->mSuiteName = mSuiteName;
	tc->mPkgName = mPkgName;
	tc->mDesc = mDesc;
	tc->mExpected = mExpected;
	tc->mActual = mActual;
	tc->mFile = mFile;
	tc->mLine = mLine;
	tc->mStatus = mStatus;
	tc->mErrmsg = mErrmsg;
	sgLock.unlock();

	return tc.getPtr();
}


OmnDbObjPtr	
OmnTestcase::getNotInDbObj() const
{
	OmnTestcasePtr tc = OmnNew OmnTestcase(mTcName, __FILE__, __LINE__);
	tc.setDelFlag(false);

	tc->mTestId = mTestId;
	tc->mSuiteName = mSuiteName;
	tc->mPkgName = mPkgName;

	return tc.getPtr();
}


bool		
OmnTestcase::hasSameObjId(const OmnDbObjPtr &rhs) const
{
	if (rhs->getClassId() != OmnClassId::eOmnTestcase)
	{
		return false;
	}

	OmnTestcase *r = reinterpret_cast<OmnTestcase*>(rhs.getPtr());

	return r->mTestId == mTestId &&
		   r->mSuiteName == mSuiteName &&
		   r->mTcName == mTcName &&
		   r->mPkgName == mPkgName;
}


OmnRslt
OmnTestcase::serializeFromDb()
{
	// 
	// It retrieves a test case from the database. In the current
	// implementation, test cases are not stored in database, only
	// its execution results. 
	// 
	OmnString stmt = "select * from tc_rslt where ";
	stmt << "test_id='" << mTestId
		 << "' and suite_name='" << mSuiteName
		 << "' and package_name='" << mPkgName
		 << "' and testcase_name='" << mTcName
		 << "'";
	
	OmnTraceDb << "To retrieve test case: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << OmnErrId::eDatabaseError 
			<< "Failed to retrieve test case: " 
			<< toString() << " from db!" << enderr;
	}

	if (!record)
	{
		// 
		// Didn't find the record
		//
		rslt.setErrmsg("Didn't find the test case");
		OmnTraceDb << "Didn't find the test case: " << toString() << endl;
		return OmnRslt(OmnErrId::eObjectNotFound, "");
	}

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
OmnTestcase::updateStmt() const
{
	OmnString stmt = "update tc_rslt set ";
	stmt << "test_id='" << mTestId 
		 << "', suite_name='" << mSuiteName
		 << "', package_name='" << mPkgName
		 << "', testcase_name='" << mTcName
		 << "', description='" << mDesc
		 << "', expected='" << mExpected
		 << "', actual='" << mActual
		 << "', file='" << mFile
		 << "', line=" << mLine
		 << ", status='" << mStatus
		 << "', message='" << mErrmsg 
		 << "'";
	return stmt;
}


OmnString
OmnTestcase::removeStmt() const
{
    //
    // Test case results are identified by:
	//
	//	(Test ID, Suite Name, Package Name, Test case Name).
	//
	// This function assumes the caller has set these values in the 
	// corresponding member data. 
    //
	OmnString stmt = "delete from tc_rslt where test_id='";
	stmt << mTestId			<< "' and suite_name='"
		 << mSuiteName		<< "' and package_name='"
		 << mPkgName		<< "' and testcase_name='"
		 << mTcName			<< "'";

    return stmt;
}


OmnString
OmnTestcase::existStmt() const
{
    //
    // Test case results are identified by:
	//
	//	(Test ID, Suite Name, Package Name, Test case Name).
	//
	// This function assumes the caller has set these values in the 
	// corresponding member data. 
    //
	OmnString stmt = "select * from tc_rslt where test_id='";
	stmt << mTestId			<< "' and suite_name='"
		 << mSuiteName		<< "' and package_name='"
		 << mPkgName		<< "' and testcase_name='"
		 << mTcName			<< "'";

    return stmt;
}


OmnString
OmnTestcase::insertStmt() const
{
    //
    // It creates a SQL statement to insert an endpoint into the SQL database
    //
    OmnString stmt = "insert into tc_rslt ("
		"test_id,"
		"suite_name,"
		"package_name,"
		"testcase_name,"
		"description,"
		"expected,"
		"actual,"
		"file,"
		"line,"
		"status,"
        "message"
        ") values ( '";

    stmt << mTestId		<< "','"
		 << mSuiteName	<< "','"
		 << mPkgName	<< "','"
		 << mTcName		<< "','"
		 << mDesc		<< "','"
		 << mExpected   << "','"
		 << mActual     << "','"
		 << mFile		<< "',"
		 << mLine		<< ","
		 << mStatus		<< ",'"
		 << mErrmsg	 	<< "')";

    return stmt;
}


OmnRslt
OmnTestcase::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
	mTestId    = record->getStr(0, "NoTestId", rslt);
	mSuiteName = record->getStr(1, "NoName", rslt);
	mPkgName   = record->getStr(2, "NoName", rslt);
	mTcName    = record->getStr(3, "NoName", rslt);
	mDesc      = record->getStr(4, "NoDesc", rslt);
	mExpected  = record->getStr(5, "", rslt);
	mActual    = record->getStr(6, "", rslt);
	mFile      = record->getStr(7, "", rslt);
	mLine      = record->getInt(8, 0, rslt);
	mStatus    = (Status)record->getInt(9, eUnknown, rslt);
	mErrmsg    = record->getStr(10, "NoErrMsg", rslt);

	if (!isValidStatus(mStatus))
	{
		OmnAlarm << "Invalid test case result status: " << mStatus << enderr;
		mStatus = eUnknown;
	}

	return rslt;
}

#endif


OmnTestcase &
OmnTestcase::operator = (const OmnTestcase &rhs)
{

	if (this == &rhs) return *this;

	sgLock.lock();
	mTestId = rhs.mTestId;
	mLock = rhs.mLock;
	mSuiteName = rhs.mSuiteName;
	mPkgName = rhs.mPkgName;
	mTcName = rhs.mTcName;
	mDesc = rhs.mDesc;
	mStatus = rhs.mStatus;
	mErrmsg = rhs.mErrmsg;
	mExpected = rhs.mExpected;
	mActual = rhs.mActual;
	mFile = rhs.mFile;
	mLine = rhs.mLine;
	mTitle = rhs.mTitle;
	mInfo = rhs.mInfo;
	sgLock.unlock();

	return *this;
}
