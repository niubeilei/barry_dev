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
#if 0
#ifndef AOS_IILTrans_DistQueryDocByName_h
#define AOS_IILTrans_DistQueryDocByName_h

#include "TransUtil/IILTrans.h"
#include "Util/Opr.h"


class AosIILTransDistQueryDocByName : public AosIILTrans
{
private:
	//OmnString 				mIILName;
	AosBitmapObjPtr			mQuery_bitmap;
	AosQueryContextObjPtr	mQuery_context;
	OmnString				mDocScannerId;
	u64						mBlockSize;

public:
	AosIILTransDistQueryDocByName(const bool regflag);
	AosIILTransDistQueryDocByName(
			const OmnString &iilname,
			const OmnString &docscanner_id,
			const u64 blocksize,
			const AosQueryContextObjPtr &query_context,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransDistQueryDocByName(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();

	// IILTrans Interface	
	virtual bool proc(
			const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual bool needCreateIIL() const {return false;}
	virtual int getSerializeSize() const;
	virtual bool needProc() const {return true;}
};

#endif
#endif
