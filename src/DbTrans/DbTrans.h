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
// 2014/10/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DbTrans_h
#define Aos_SEInterfaces_DbTrans_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DbTransObj.h"
#include "XmlUtil/DocTypes.h"


class AosDbTrans : public AosDbTransObj
{
	OmnDefineRCObject;

	struct SnapshotInfo {
		int mVirtualId;
		AosDocType::E mDocType;
	};

protected:
	u64 mSnapshotId;
	AosXmlTagPtr mDoc;
	vector <SnapshotInfo> mSnapshots;

public:
	AosDbTrans(const int version);
	~AosDbTrans();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool start(AosRundata *rdata);
	virtual bool commit(AosRundata *rdata);
	virtual bool rollback(AosRundata *rdata);


private:
	virtual bool commitPriv(AosRundata *rdata, SnapshotInfo &snapshot);
	virtual bool rollbackPriv(AosRundata *rdata, SnapshotInfo &snapshot);
};
#endif

