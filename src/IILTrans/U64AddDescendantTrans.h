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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_U64AddDescendant_h
#define AOS_IILTrans_U64AddDescendant_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"

#include <vector>

class AosIILTransU64AddDescendant : public AosIILTrans
{

private:
	//OmnString	mIILName;
	u64			mDocid;
	u64			mParentDocid;
	vector<u64> mAncestors;

public:
	AosIILTransU64AddDescendant(const bool regflag);
	AosIILTransU64AddDescendant(
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		vector<u64> &ancestors,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransU64AddDescendant(){mAncestors.clear();}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();

	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;

	u64		getDocid() const {return mDocid;}
	u64		getParentDocid() const {return mParentDocid;}
};

#endif
