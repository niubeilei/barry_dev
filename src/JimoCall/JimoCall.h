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
// 2014/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCall_h
#define Aos_JimoCall_JimoCall_h

#include "BSON/BSON.h"
#include "CubeComm/EndPointInfo.h"
#include "JimoCall/Ptrs.h"
#include "JimoCall/JimoCallDialer.h"
#include "JimoCallServer/JimoCallServer.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilData/RepPolicy.h"
#include "UtilData/FN.h"
#include "Rundata/Rundata.h"

#include <string>

class OmnSem;
class AosClusterObj;
class AosJimoCallServer;

class AosJimoCall : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eCallIdle,
		eCallActive,
		eCallSuccess,
		eLogicalFail,
		ePhysicalFail,
		eCallTimeout
	};

	enum
	{
		eDftTimerMs = 10000,		// 10 seconds

	};

protected:
	OmnString			mPackage;		// Function to call
	int					mFunc;			// Function to call
	AosEndPointInfo		mFromEPInfo;
	AosEndPointInfo		mToEPInfo;
	u32                 mMsgID;
	u64                 mJimoCallID;
	Status				mStatus;
	AosBSON	 			mBSON;		// Used to compose call data and results
	OmnSem *			mSem;
	AosWebRequestPtr	mWebReq;
	AosJimoCallServer *	mCallServer; // Used by response only to send msg back
	OmnConnBuffPtr      mConnBuff;      // The buff that holds the response data
	u64					mStartTime;
	vector<bool>		mEndpointStatus;
	AosClusterObj *		mCluster;
	u32					mCrtEndpointIdx;
	u64					mDistId;
	u32					mReplicPolicy;
	u32					mTimerMs;
	int					mCubeID;
	u8					mDupid;

	vector<AosCubeMapObj::CubeInfo>	mEndpointIDs;

public:
	AosJimoCall();
	AosJimoCall(
		AosRundata *rdata,
		const OmnString &package,
		const int func,
		AosClusterObj *cluster);		// For normal RPC
	AosJimoCall(
		const OmnConnBuffPtr &buff,
		AosJimoCallServer *server);

	~AosJimoCall();

	virtual bool makeCall(AosRundata *rdata);
	virtual bool callResponded(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual void checkTimer(AosRundata *rdata);

	void dump() const;	//by white, 2015-08-14 17:30

	OmnString	getPackageID() const {return mPackage;}
	void	setPackageID(const OmnString package) {mPackage = package;}

    int		getFuncID() const {return mFunc;}
    void	setFuncID(const int func_id) {mFunc = func_id;}

	AosBSON &getBSON() {return mBSON;}
	OmnConnBuffPtr getConnBuff() const {return mConnBuff;}

	AosEndPointInfo getFromEPInfo() const {return mFromEPInfo;}
	void	setFromEPInfo(const OmnIpAddr ipAddr, const int port) {mFromEPInfo.mIpAddr = ipAddr; mFromEPInfo.mPort = port;}

	AosEndPointInfo getToEPInfo() const {return mToEPInfo;}
	void	setToEPInfo(const OmnIpAddr ipAddr, const int port) {mToEPInfo.mIpAddr = ipAddr; mToEPInfo.mPort = port;}

	u32		getMsgID() const {return mMsgID;}
	void	setMsgID(const u32 messageid){mMsgID = messageid;}

	u64		getJimoCallID() const {return mJimoCallID;}

	Status	getStatus() const {return mStatus;}
	void	setIdle() {mStatus = eCallIdle;}
	void	setSuccess() {mStatus = eCallSuccess;}
	void	setHardwareFail() {mStatus = ePhysicalFail;}
	void	setLogicalFail() {mStatus = eLogicalFail;}
	bool	isCallSuccess() const {return mStatus == eCallSuccess;}
	bool	isLogicalFail() const {return mStatus == eLogicalFail;}
	bool	isCallFinished() const {return (mStatus == eCallSuccess ||
											mStatus == eLogicalFail ||
											mStatus == ePhysicalFail);}

	int		getCubeID() const {return mCubeID;}
	u8		getDupid() const {return mDupid;}

	void	setBSON(const AosBuffPtr &buff);
	void	setConnBuff(const OmnConnBuffPtr &conn_buff);
	void	setSem(OmnSem * sem);
	OmnSem*	getSem() {return mSem;}

	void	setJimoCallServer(AosJimoCallServer *server) {mCallServer = server;}
	void	reset();
	bool	msgRead(AosRundata *rdata, AosBuff *buff);
	bool	sendResp(AosRundata *rdata);
	bool	getFieldValue(AosRundata*, AosFN::E name, AosValueRslt &value);

	inline OmnString getOmnStr(AosRundata *rdata, AosFN::E name, const OmnString &dft = "")
	{
		return mBSON.getOmnStr(name, dft);
	}

	inline u32 getU32(AosRundata *rdata, AosFN::E name, const u32 dft = 0)
	{
		return mBSON.getU32(name, dft);
	}

	inline u64 getU64(AosRundata *rdata, AosFN::E name, const u64 dft = 0)
	{
		return mBSON.getU64(name, dft);
	}

	inline int getInt(AosRundata *rdata, AosFN::E name, const int dft = 0)
	{
		return mBSON.getInt( name, dft);
	}

	inline int getI64(AosRundata *rdata, AosFN::E name, const i64 dft = 0)
	{
		return mBSON.getI64( name, dft);
	}

	inline bool getBool(AosRundata *rdata, AosFN::E name, const bool dft = false)
	{
		return mBSON.getBool(name, dft);
	}


//	AosBuffPtr  getBuff(AosRundata* rdata, AosFN::E name)
//	{
//		return mBSON.getBuff(name, 0);
//	}

	AosBuffPtr  getBuff(AosRundata* rdata, AosFN::E name, const AosBuffPtr &dft = 0)
	{
		AosBuffPtr ptr = mBSON.getBuff(name, dft);
		if( ptr.getPtr() == 0 )
			return dft;
		return ptr;
	}


	AosBuff * getBuffRaw(AosRundata* rdata, AosFN::E name, AosBuff* dft)
	{
		AosBuffPtr ptr = mBSON.getBuff(name, dft);
		if( ptr.getPtr() == 0 )
			return dft;

		return ptr.getPtr();
	}

	inline AosJimoCall & arg(const u32 field_id, const u64 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const u32 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const u8 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const i64 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const i32 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const i16 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const i8 value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const std::string &value)
	{
	 	mBSON.setValue(field_id, value);
	 	return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const OmnString &value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const double value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const char * const value)
	{
		mBSON.setValue(field_id, OmnString(value));
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const bool &value)
	{
		mBSON.setValue(field_id, value);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const AosBuffPtr &value)
	{
		mBSON.setValue(field_id, value.getPtr());
		return *this;
	}

	inline AosJimoCall &arg(const u32 field_id, const char * const data, const int len)
	{
		mBSON.setValue(field_id, data, len);
		return *this;
	}

	inline AosJimoCall & arg(const u32 field_id, const AosRepPolicy &value)
	{
		OmnAlarm << enderr;
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const u64 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const u32 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const u16  value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const u8 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const i64 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const i32 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const i16 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const i8 value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const double value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const std::string &value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const OmnString &value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const bool &value)
	{
		mBSON.setValue(field_name, value);
		return *this;
	}

	inline AosJimoCall & arg(const OmnString &field_name, const AosBuffPtr &value)
	{
		mBSON.setValue(field_name, value.getPtr());
		return *this;
	}

	inline AosJimoCall & SiteId(const u32 siteid)
	{
		mBSON.setValue(AosFN::eSiteid, siteid);
		return *this;
	}

	virtual bool waitForFinish() {return true;}

protected:
	void logCall(AosRundata *rdata);
	void logFailedTry(AosRundata *rdata);
	void logFailedCall(AosRundata *rdata);
	void logTimeoutCall(AosRundata *rdata);

	AosBuff *composeBuff(AosRundata *rdata, i64 &dupid_pos)
	{
		AosBuff *buff = mBSON.getBuffRaw();
		aos_assert_r(buff, 0);

		rdata->serializeToBuffForJimoCall(buff);

		buff->appendU64((u64)OmnMsgId::eJimoCallReq);
		buff->appendU64(mJimoCallID);
		buff->appendOmnStr(mPackage);
		buff->appendInt(mFunc);
		buff->appendInt(1);		// For cube id
		dupid_pos = buff->getCrtIdx();
		buff->appendU8(0);		// For dupid
		return buff;
	}

	AosBuffPtr composeBuffNew(AosRundata *rdata, i64 &dupid_pos)
	{
		AosBuffPtr buff = mBSON.getBuff();
		aos_assert_r(buff, 0);

		rdata->serializeToBuffForJimoCall(buff.getPtr());

		buff->appendU64((u64)OmnMsgId::eJimoCallReq);
		buff->appendU64(mJimoCallID);
		buff->appendOmnStr(mPackage);
		buff->appendInt(mFunc);
		buff->appendInt(1);		// For cube id
		dupid_pos = buff->getCrtIdx();
		buff->appendU8(0);		// For dupid
		return buff;
	}

};
#endif

