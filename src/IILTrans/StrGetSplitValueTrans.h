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
// 04/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_StrGetSplitValue_h
#define AOS_IILTrans_StrGetSplitValue_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "TransUtil/Ptrs.h"
#include "Util/Opr.h"


class AosIILTransStrGetSplitValue : public AosIILTrans
{

	int 				mNumBlocks;
	//OmnString 			mIILName;
	AosQueryRsltObjPtr 	mQuery_rslt;

public:
	AosIILTransStrGetSplitValue(const bool regflag);
	AosIILTransStrGetSplitValue(
		const OmnString &iilname,
		const int num_blocks,
		const AosQueryRsltObjPtr &query_rslt,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrGetSplitValue(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	// IILTrans Interface	
	//virtual bool proc(const AosIILPtr &iil, const AosRundataPtr &rdata);
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;
};

#endif
