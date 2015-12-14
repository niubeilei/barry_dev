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
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Message_BuffMsg_h
#define Omn_Message_BuffMsg_h

#include "Message/MsgId.h"
#include "Message/MsgCategory.h"
#include "Message/MsgFieldIds.h"
#include "Message/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/CubeMgrObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosBuffMsg : virtual public OmnRCObject
{
protected:
	enum
	{
		eBuffMsgPoison1 = 0x03624364,
		eBuffMsgPoison2 = 0x36467013
	};

	OmnMsgId::E			mMsgId;
	AosMsgCategory::E 	mMsgCategory;
	bool				mIsRequest;
	AosBuffPtr			mBuff;
	vector<int>			mFieldIdx;
	vector<u8>			mFieldType;

	static vector<AosBuffMsgPtr> smMsgs;
	static int					 smVersion;
	static int					 smInvalidVersion;
	static OmnMutexPtr			 smLock;

public:
	AosBuffMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const int send_phyid, 
		const int recv_phyid, 
		const bool is_request, 
		const int version = smVersion);
	AosBuffMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const u32 send_cubeid, 
		const u32 recv_cubeid, 
		const bool is_request, 
		const int version = smVersion);
	AosBuffMsg(const AosBuffPtr &buff);
	virtual ~AosBuffMsg();

	static AosBuffMsgPtr createMsg(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual AosBuffMsgPtr clone(const AosRundataPtr &rdata, 
							const AosBuffPtr &buff) = 0;

	bool registerMsg(const AosBuffMsgPtr &msg);
	AosBuffPtr getBuff() const;
	AosRundataPtr getRundata();
	int getSendPhysicalId() {return getFieldInt(AosMsgFieldId::eSendPhysicalId, -1);}
	int getRecvPhysicalId() {return getFieldInt(AosMsgFieldId::eRecvPhysicalId, -1);}
	u32 getSendCubeId() {return getFieldU32(AosMsgFieldId::eSendCubeId, AOS_INV_CUBEID);}
	u32 getRecvCubeId() {return getFieldU32(AosMsgFieldId::eRecvCubeId, AOS_INV_CUBEID);}
	int getVersion() {return getFieldInt(AosMsgFieldId::eVersion, smInvalidVersion);}
	OmnMsgId::E getMsgId() {return mMsgId;}
	AosMsgCategory::E getMsgCategory() {return mMsgCategory;}

	bool setFieldU64(const AosMsgFieldId::E field, const u64 &value);
	bool setFieldU32(const AosMsgFieldId::E field, const u32 value);
	bool setFieldU16(const AosMsgFieldId::E field, const u16 value);
	bool setFieldU8(const AosMsgFieldId::E field, const u8 value);
	bool setFieldInt64(const AosMsgFieldId::E field, const int64_t &value);
	bool setFieldInt(const AosMsgFieldId::E field, const int value);
	bool setFieldInt16(const AosMsgFieldId::E field, const int16_t value);
	bool setFieldChar(const AosMsgFieldId::E field, const char value);
	bool setFieldFloat(const AosMsgFieldId::E field, const float &value);
	bool setFieldDouble(const AosMsgFieldId::E field, const double &value);
	bool setFieldStr(const AosMsgFieldId::E field, const OmnString &value);
	bool setFieldBuff(const AosMsgFieldId::E field, const AosBuffPtr &buff);

	bool setFieldU64(const OmnString &name, const u64 &value);
	bool setFieldU32(const OmnString &name, const u32 value);
	bool setFieldU16(const OmnString &name, const u16 value);
	bool setFieldU8(const OmnString &name, const u8 value);
	bool setFieldInt64(const OmnString &name, const int64_t &value);
	bool setFieldInt(const OmnString &name, const int value);
	bool setFieldInt16(const OmnString &name, const int16_t value);
	bool setFieldChar(const OmnString &name, const char value);
	bool setFieldFloat(const OmnString &name, const float &value);
	bool setFieldDouble(const OmnString &name, const double &value);
	bool setFieldStr(const OmnString &name, const OmnString &value);
	bool setFieldBuff(const OmnString &name, const AosBuffPtr &buff);

	u64 	getFieldU64(const AosMsgFieldId::E field, const u64 &dft);
	u32 	getFieldU32(const AosMsgFieldId::E field, const u32 dft);
	u16 	getFieldU16(const AosMsgFieldId::E field, const u16 dft);
	u8  	getFieldU8(const AosMsgFieldId::E field, const u8 dft);
	int64_t getFieldInt64(const AosMsgFieldId::E field, const int64_t &dft);
	int 	getFieldInt(const AosMsgFieldId::E field, const int dft);
	int16_t getFieldInt16(const AosMsgFieldId::E field, const int16_t dft);
	char 	getFieldChar(const AosMsgFieldId::E field, const char dft);
	float 	getFieldFloat(const AosMsgFieldId::E field, const float dft);
	double	getFieldDouble(const AosMsgFieldId::E field, const double &dft);
	OmnString getFieldStr(const AosMsgFieldId::E field, const OmnString &dft);
	AosBuffPtr getFieldBuff(const AosMsgFieldId::E field, const bool copy);

	u64 	getFieldU64(const OmnString &name, const u64 &dft);
	u32 	getFieldU32(const OmnString &name, const u32 dft);
	u16 	getFieldU16(const OmnString &name, const u16 dft);
	u8  	getFieldU8(const OmnString &name, const u8 dft);
	int64_t getFieldInt64(const OmnString &name, const int64_t &dft);
	int 	getFieldInt(const OmnString &name, const int dft);
	int16_t getFieldInt16(const OmnString &name, const int16_t dft);
	char 	getFieldChar(const OmnString &name, const char dft);
	float 	getFieldFloat(const OmnString &name, const float dft);
	double	getFieldDouble(const OmnString &name, const double &dft);
	OmnString getFieldStr(const OmnString &name, const OmnString &dft);
	AosBuffPtr getFieldBuff(const OmnString &name, const bool copy);

	// Below are the functions to set user defined fields.
	inline bool setFieldU64(const u32 field_idx, const u64 &value)
	{
		return setFieldU64(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldU32(const u32 field_idx, const u32 value)
	{
		return setFieldU32(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldU16(const u32 field_idx, const u16 value)
	{
		return setFieldU16(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldU8(const u32 field_idx, const u8 value)
	{
		return setFieldU8(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldInt64(const u32 field_idx, const int64_t &value)
	{
		return setFieldInt64(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldInt(const u32 field_idx, const int value)
	{
		return setFieldInt(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldInt16(const u32 field_idx, const int16_t value)
	{
		return setFieldInt16(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldChar(const u32 field_idx, const char value)
	{
		return setFieldChar(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldFloat(const u32 field_idx, const float &value)
	{
		return setFieldFloat(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldDouble(const u32 field_idx, const double &value)
	{
		return setFieldDouble(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldStr(const u32 field_idx, const OmnString &value)
	{
		return setFieldStr(AosMsgFieldId::getUDF() + field_idx, value);
	}
	inline bool setFieldBuff(const u32 field_idx, const AosBuffPtr &buff)
	{
		return setFieldBuff(AosMsgFieldId::getUDF() + field_idx, buff);
	}

	// Below are the functions to retrieve user defined fields.
	inline u64 getFieldU64(const int field_idx, const u64 &dft)
	{
		return getFieldU64(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline u32 getFieldU32(const int field_idx, const u32 dft)
	{
		return getFieldU32(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline u16 getFieldU16(const int field_idx, const u16 dft)
	{
		return getFieldU16(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline u8 getFieldU8(const int field_idx, const u8 dft)
	{
		return getFieldU8(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline int64_t getFieldInt64(const int field_idx, const int64_t &dft)
	{
		return getFieldInt64(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline int getFieldInt(const int field_idx, const int dft)
	{
		return getFieldInt(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline int16_t getFieldInt16(const int field_idx, const int16_t dft)
	{
		return getFieldInt16(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline char getFieldChar(const int field_idx, const char dft)
	{
		return getFieldChar(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline float getFieldFloat(const int field_idx, const float dft)
	{
		return getFieldFloat(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline double getFieldDouble(const int field_idx, const double &dft)
	{
		return getFieldDouble(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline OmnString getFieldStr(const int field_idx, const OmnString &dft)
	{
		return getFieldStr(AosMsgFieldId::getUDF() + field_idx, dft);
	}

	inline AosBuffPtr getFieldBuff(const int field_idx, const bool copy)
	{
		return getFieldBuff(AosMsgFieldId::getUDF() + field_idx, copy);
	}

private:
	bool setMsgHead(const OmnMsgId::E msgid, const bool is_request);
	bool parseMsg();
	static bool verifyMsgHead(
				const AosBuffPtr &buff,
				OmnMsgId::E &msgid, 
				bool &is_request);
};
#endif
