////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Testcase.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_Testcase_h
#define OMN_Tester_Testcase_h

#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Tester/TestDef.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include <string>

class OmnSerialFrom;
class OmnSerialTo;

class OmnEndTc
{
public:
	OmnEndTc() {}
	~OmnEndTc() {}
};


#ifndef starttc
#define starttc OmnTestcase(OmnFileLine)
#endif


#ifndef endtc
#define endtc OmnEndTc();
#endif


#ifdef AOS_DB_SUPPORT
class OmnTestcase : public OmnDbObj
#else 
class OmnTestcase : public OmnObject
#endif
{
	OmnDefineRCObject;

private:
	enum Status
	{
		eNotSetYet,
		eUnknown,
		eSuccess,
		eFailed
	};

	enum
	{
		eTestId,
		eSuiteName,
		ePkgName,
		eTcName,
		eDesc,
		eExpected, 
		eActual,
		eStatus,
		eErrmsg,
		eFile,
		eLine
	};

	OmnMutexPtr		mLock;
	OmnString		mTestId;
	OmnString		mSuiteName;
	OmnString		mPkgName;
	OmnString		mTcName;
	OmnString		mDesc;
	Status			mStatus;
	OmnString		mErrmsg;
	OmnString		mExpected;
	OmnString		mActual;
	OmnString		mFile;
	int				mLine;
	OmnString		mTitle;
	OmnString		mInfo;

public:
	OmnTestcase();
	OmnTestcase(const OmnString &name, 
				const char *file, 
				const int line);
	~OmnTestcase();

	OmnClassId::E	getClassId() const;

	// 
	// OmnObject interface
	//
	virtual int objSize() const;

	// 
	// OmnDbObj interface
	//
#ifdef AOS_DB_SUPPORT
    virtual OmnRslt		serializeFromDb();
	virtual OmnString	insertStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnDbObjPtr	clone() const;
	virtual OmnDbObjPtr	getNotInDbObj() const;
	virtual bool		hasSameObjId(const OmnDbObjPtr &rhs) const;
	OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
#endif

	OmnRslt		serializeTo(OmnSerialTo &s) const;
	OmnRslt		serializeFrom(OmnSerialFrom &s);
	void		removeRslts(const OmnString &testId);

	bool		isValid() const {
					return (mStatus == eSuccess || 
							mStatus == eFailed ||
							(mStatus == eNotSetYet && 
							  (mExpected.length() > 0 || mActual.length() > 0)));}
	OmnString	toString() const;
	void		reset();
	bool		isSuccess() const {
					return mStatus == eSuccess ||
						   (mStatus == eNotSetYet && mExpected.length() > 0 
							&& mExpected == mActual);}
	void		setTestId(const OmnString &tid) {mTestId = tid;}
	void		setTitle(const OmnTcTitle &title) {mTitle = title.mTitle;}
	void		setSuiteName(const OmnString &sn) {mSuiteName = sn;}
	void		setPkgName(const OmnString &pn) {mPkgName = pn;}
	void		setExpected(const OmnString &e) {mExpected = e;}
	void		setActual(const OmnString &a) {mActual = a;}
	void		setSuccess() {mStatus = eSuccess;}
	void		setFailed() {mStatus = eFailed;}

	OmnTestcase & operator = (const OmnTestcase &rhs);
	OmnTestcase & operator << (const OmnString &rhs);
	OmnTestcase & operator << (const std::string &rhs);
	OmnTestcase & operator << (const char *rhs);
	OmnTestcase & operator << (const float rhs);
	OmnTestcase & operator << (const bool rhs);
	OmnTestcase & operator << (const OmnIpAddr &addr);
	OmnTestcase & operator << (const u64 &rhs);
	OmnTestcase & operator << (const int64_t &rhs);
	OmnTestcase & operator << (const int rhs);

	OmnTestcase & setInfo(const OmnString &str)
	{
		mInfo << str;
		return *this;
	}

	OmnTestcase & setInfo(const char *str)
	{
		mInfo << str;
		return *this;
	}

	OmnTestcase & setInfo(const int v)
	{
		mInfo << v;
		return *this;
	}

	OmnTestcase & setInfo(const bool v)
	{
		if (v)
		{
			mInfo << "true";
		}
		else
		{
			mInfo << "false";
		}
		return *this;
	}

	OmnTestcase & setInfo(const OmnIpAddr &addr)
	{
		mInfo << addr.toString();
		return *this;
	}

	static bool isValidStatus(const Status s)
	{
		return (s == eNotSetYet || s == eSuccess || s == eFailed);
	}

private:
	OmnString	sqlStmtInsert() const;
};



#endif

