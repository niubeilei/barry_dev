////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_GenericScanner_h
#define AOS_DataScanner_GenericScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ReadFile.h"

class AosGenericScanner : virtual public AosDataScanner
{
	
private:
	AosDataReaderObjPtr		mReader;

public:
	AosGenericScanner(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &conf);
	~AosGenericScanner();

	virtual bool getNextBlock(const AosRundataPtr &rdata, AosBuffDataPtr &data);
	virtual bool reset();
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	virtual bool serializeTo(const AosRundataPtr &rdata, const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosRundataPtr &rdata, const AosBuffPtr &buff);

private:
	bool config(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
};
#endif

