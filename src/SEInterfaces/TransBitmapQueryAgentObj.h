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
// 2013/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TransBitmapQueryAgentObj_h
#define Aos_SEInterfaces_TransBitmapQueryAgentObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "BitmapTrans/Ptrs.h"
#include "SEInterfaces/Ptrs.h"

class AosTransBitmapQueryAgentObj : public AosJimo
{
	OmnDefineRCObject;

private:

public:
	AosTransBitmapQueryAgentObj(const int version);
	
	virtual ~AosTransBitmapQueryAgentObj(); 
	static AosTransBitmapQueryAgentObjPtr getObject();

	virtual bool serializeFrom(const AosBuffPtr &buff) = 0;
	virtual bool serializeTo(const AosBuffPtr &buff) = 0;
	virtual bool proc() = 0;
	virtual void setRundata(const AosRundataPtr &rdata) = 0;

	static AosTransBitmapQueryAgentObjPtr	getAgent(
			const AosRundataPtr &rdata, 
			const u64 term_id,
			const vector<OmnString> &iilnames,
			const vector<AosQueryRsltObjPtr> &node_list,
			const vector<AosBitmapObjPtr> &partial_bitmaps, 
			const vector<u32> &expected_sections);
	virtual bool config(
			const AosRundataPtr &rdata, 
			const u64 term_id,
			const vector<OmnString> &iilnames,
			const vector<AosQueryRsltObjPtr> &node_list,
			const vector<AosBitmapObjPtr> &partial_bitmaps, 
			const vector<u32> &expected_sections) = 0;
	virtual void					setTrans(const AosTransBitmapQueryPtr &trans) = 0;
	virtual bool					procResp(
										const AosRundataPtr &rdata, 
										const u64 &and_term_id,
										OmnString &errmsg,
										const AosBitmapObjPtr &bitmap) = 0;

	static bool AosCreateTransBitmapQueryAgentJimoDoc(const AosRundataPtr &rdata);
};
#endif

