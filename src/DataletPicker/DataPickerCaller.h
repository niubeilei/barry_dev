////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILCache_DataPickerProc_h
#define AOS_IILCache_DataPickerProc_h

#include "DocClient/DocClientCaller.h"

class AosDataPickerProc : public AosDocClientCaller
{
	OmnDefineRCObject;

private:
	AosRundataPtr			mRunner;
	OmnString				mQuery;
	AosDataPickerProcPtr	mProc;

public:
	AosDataPickerProc(
			const AosRundataPtr &rdata, 
			const OmnString &query, 
			const AosDataPickerProcPtr &proc);
	~AosDataPickerProc();

	bool docRetrieved(const AosRundataPtr &rdata, const AosXmlTagPtr &doc);
	bool docRetrieved(const AosRundataPtr &rdata, const vector<AosXmlTagPtr> &docs);
	bool docRetrvFailed(const AosRundataPtr &rdata);
};
#endif

