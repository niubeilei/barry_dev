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
//
// Modification History:
// 	Created: 12/17/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataStore_DbTrans_h
#define Omn_DataStore_DbTrans_h

#include "Obj/ObjDb.h"


class AosDbTrans : public OmnDbObj
{
	OmnDefineRCObject;

private:
	u32			mId;
	char		mStatus;
	OmnString	mData;

public:
	AosDbTrans() {}
	~AosDbTrans() {}

	 //
	// OmnDbObj interface
	//
	virtual OmnRslt     serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString   retrieveStmt() const;
	virtual OmnString   insertStmt() const;
	virtual OmnString   updateStmt() const;
	virtual OmnString   removeStmt() const;
	virtual OmnString   removeAllStmt() const;
	virtual OmnString   existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	bool	storeTransData(const OmnString &data, u32 &transId);
	bool	releaseTrans(const u32 transId);
};
#endif
