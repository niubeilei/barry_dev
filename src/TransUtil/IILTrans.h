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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_IILTrans_h
#define AOS_TransUtil_IILTrans_h

#include "IILUtil/IILFuncType.h"
#include "JimoCall/JimoCall.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "JimoCall/JimoCall.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/Ptrs.h"
#include "TransUtil/CubicTrans.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosBuff;
#define AOSIILTRANSTEST_DEFINE_RECV_SEQNO u32 mIILTesterRecvSeqno
#define AOSIILTRANSTEST_DEFINE_MEMBERFUNCS \
    virtual u32 getRecvSeqno() const {return mIILTesterRecvSeqno;} \
	virtual bool setRecvSeqno(const u32 s) {mIILTesterRecvSeqno=s;return true;} \
	virtual u64 getIILTestDocid() const {return mDocid;} 

//class AosIILTrans : public AosTinyTrans
class AosIILTrans : public AosCubicTrans
{
public:
	enum
	{
		ePersisFlag = 0x01,
		eIILIDFlag  = 0x02,
		eStatusInitedFlag = 0x04,
		eValueUniqueFlag = 0x08,
		eDocidUniqueFlag = 0x10
	};

protected:
	u64								mIILID;
	OmnString						mIILName;
	char							mFlags;
	u64								mSnapshotId;

	// Gavin, 2015/08/24
	//STORAGE-31
	AosJimoCallPtr					mJimoCall;

	static AosTransProcThrdPtr 		smIILProcThrd;

public:
	AosIILTrans(
			const AosTransType::E opr, 
			const bool reg_flag
			AosMemoryCheckDecl);
	AosIILTrans(
			const AosTransType::E opr,
			const u64 iilid,
			const bool isPersis,
			const u64 snap_id,
			const bool need_save,
			const bool need_resp
			AosMemoryCheckDecl);
	AosIILTrans(
			const AosTransType::E opr,
			const OmnString &iilname,
			const bool isPersis,
			const u64 snap_id,
			const bool need_save,
			const bool need_resp
			AosMemoryCheckDecl);

	AosIILTrans(
			const AosTransType::E opr,
			const u32 vid,
			const bool is_cube,
			const u64 snap_id,
			const bool need_save,
			const bool need_resp
			AosMemoryCheckDecl);

	~AosIILTrans();

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff) = 0;
	virtual bool serializeFrom(const AosBuffPtr &buff) = 0;
	virtual AosTransPtr clone() = 0;
	//virtual AosConnMsgPtr clone() = 0;
	virtual bool proc();
	virtual bool directProc();

	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iil,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata) = 0;
	virtual AosIILType getIILType() const = 0;
	virtual int getSerializeSize() const;
	virtual bool needCreateIIL() const {return true;}
	virtual bool needProc() const {return false;}
	virtual u64 getSnapshotId() const {return mSnapshotId;}

	//virtual bool isAsyncReq() const {return false;}
	//virtual AosAsyncReqTransPtr getAsyncReqTransPtr() {return 0;}

	bool procTrans(const AosIILObjPtr &iil, const AosRundataPtr &rdata);
	
	// Ketty 2013/03/18
	u64 getIILID(const AosRundataPtr &rdata);
	OmnString getIILName() {return mIILName;}

	bool isPersis() const {return (mFlags & ePersisFlag);}
	bool isValueUnique() const {return (mFlags & eValueUniqueFlag);}
	bool isDocidUnique() const {return (mFlags & eDocidUniqueFlag);}
	void setPersis() {mFlags |= ePersisFlag;}
	void setValueUnique(const bool b) {if (b) mFlags |= eValueUniqueFlag;}
	void setDocidUnique(const bool b) {if (b) mFlags |= eDocidUniqueFlag;}

	bool sendErrResp();

	// Chen Ding, 01/28/2012
	// These three member functions are defined for testing purpose only.
	// If we want to test a type of IIL transactions, need to override
	// these member functions.
	virtual u32 getRecvSeqno() const {return 0;}
	virtual u64 getIILTestDocid() const {return 0;}
	virtual bool setRecvSeqno(const u32 s) {return false;}

	// Gavin, 2015/08/25
	// STORAGE-31
	virtual bool sendResp(const AosBuffPtr &buff);
	bool setJimoCall(const AosJimoCallPtr &jimo_call) {return mJimoCall = jimo_call;}
};

#endif
