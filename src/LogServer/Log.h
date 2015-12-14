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
// 03/20/2009: Created by Sally Sheng
// 12/31/2012: Turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_Log_h
#define AOS_LogServer_Log_h

#include "LogServer/LogDataTypes.h"
#include "TinyXml/TinyXml.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "LogServer/Ptrs.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"


class AosLog : public virtual OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Reset
    {
		eResetInvalid,

        eNever,
		eApplicationRestart,
	    eFirstFile,
	    eNextFile
	};

    enum Archive
    {
		eArchiveInvalid,

        eNone,
        eNormal,
        eManual,
        eSystem
    };

	enum Level
	{
		eLevelInvalid,

		eMinimumLevel,
		eNormalLevel,
		eMaximumLevel
	};

public:
    OmnString      mLogId;
	char           mLogIdType;
	OmnString      mAppName;
    OmnString      mAppInstance;
    Level          mLevel;
    bool           mStatus;
    Reset          mReset;
//  uint64         mMaxSize;
    int            mSecurityLevel;
//  TruncateMethod mTruncateMethod;
    int            mReliability;
//  ???            mListingVpd;
//  ???            mViewVpd;
    Archive        mArchive;
//	uint64         mTotalEntries;
//  OmnString      mContainer; //subdir for roundrobin and muitple
//  int            mGroupSize; //only for roundrobin

public:
    AosLog(){}
	AosLog(const OmnDbRecordPtr &record);
    ~AosLog(){}

	// AosLogInterface
	virtual bool		procReq(const AosNetRequestPtr &req) = 0;
	virtual AosLogType	getType() const = 0;
	virtual bool        createLog(TiXmlNode *config) = 0;
	virtual AosLogPtr   createInstanceLog(const OmnString &appInstance) = 0; 

	bool           configLog(TiXmlNode *config);
	OmnString      getLogId() const {return mLogId;}
	char           getLogIdType() const {return mLogIdType;}
	OmnString      getAppName() { return mAppName;}
    OmnString      getAppInstance() {return mAppInstance;}
    Level          getLevel() {return mLevel;}
    Reset          getReset() {return mReset;}
    bool           getStatus() {return mStatus;}
    int            getReliability() {return mReliability;}
    int            getSecurityLevel() {return mSecurityLevel;}
    Archive        getArchive() {return mArchive;}

	static bool        checkLogExist(const OmnString &logid);
    static AosLogPtr   createLogFromDb(const OmnString &logid);
	static AosLogPtr   createLogInDb(const AosNetRequestPtr &request);


protected:
	void           setLogId(const OmnString &logid) {mLogId = logid;}
	void           setLogIdType(const char logType) {mLogIdType = logType;}
    void           setAppName(const OmnString &appName) {mAppName = appName;}
    void           setAppInstance(const OmnString &appInstance) {mAppInstance = appInstance;}
    void           setLevel(const Level &level) {mLevel = level;}
    void           setReset(const Reset &reset) {mReset = reset;}
    void           setStatus(const bool &status) {mStatus = status;}
    void           setReliability(const int &reliability) {mReliability = reliability;}
    void           setSecurityLevel(const int &level) {mSecurityLevel = level;}
    void           setArchive(const Archive &archive) {mArchive = archive;}

};
#endif
#endif
