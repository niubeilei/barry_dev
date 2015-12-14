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
// 2015/4/01 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletProcs_GetDataletProc_h
#define Aos_DataletProcs_GetDataletProc_h

#include "DataletProcs/DataletProc.h"


class AosGetDataletProc : public AosDataletProc
{
	OmnDefineRCObject;
public:
	AosGetDataletProc(const int version);

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;
	virtual AosJimoPtr cloneJimo() const;

private:
	bool getDocByObjid(AosRundata *rdata, AosJimoCall &jimo_call);
	bool getDocByDocid(AosRundata *rdata, AosJimoCall &jimo_call);
	bool readLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool readUnLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool checkDocLock(AosRundata *rdata, AosJimoCall &jimo_call);
	bool procDocLock(AosRundata *rdata, AosJimoCall &jimo_call);
	bool batchGetDocs(AosRundata *rdata, AosJimoCall &jimo_call);
	bool getCSVDoc(AosRundata *rdata, AosJimoCall &jimo_call);
	bool getFixedDoc(AosRundata *rdata, AosJimoCall &jimo_call);
	bool isDataletDeleted(AosRundata *rdata, AosJimoCall &jimo_call);

	AosXmlTagPtr getDoc(const AosRundataPtr &rdata,
						const u64 &docid,
						const u64 snap_id,
						const bool &needbinarydata);
};

#endif

