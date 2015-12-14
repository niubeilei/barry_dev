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
// 2015/4/08 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletProcs_CreateDataletProc_h
#define Aos_DataletProcs_CreateDataletProc_h

#include "DataletProcs/DataletProc.h"

class AosCreateDataletProc : public AosDataletProc
{
	OmnDefineRCObject;
public:
	AosCreateDataletProc(const int version);

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;
	virtual AosJimoPtr cloneJimo() const;

private:
	bool createDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool createDataletSafe(AosRundata *rdata, AosJimoCall &jimo_call);
	bool saveToFile(AosRundata *rdata, AosJimoCall &jimo_call);
	bool retrieveBinaryDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool batchSaveStatDatalets(AosRundata *rdata, AosJimoCall &jimo_call);
/*	
	bool			mIsXml;
	u64				mDocid;
	AosXmlTagPtr 	mNewDoc;

	AosBuffPtr		mDocBuff;
	int 			mDocLen;
*/

};
#endif

