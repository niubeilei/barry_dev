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
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataScanner_VirtualFileScanner_h
#define AOS_DataScanner_VirtualFileScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "NetFile/NetFile.h"

class AosVirtualFileScanner : virtual public AosDataScanner
{
private:
	AosNetFileObjPtr 	mNetFile;
	vector<u64>			mCrtSegments;
	u32					mCrtIdx;
public:
	AosVirtualFileScanner(
			const OmnString &objid,
			const AosRundataPtr &rdata);
	AosVirtualFileScanner(const bool flag);
	AosVirtualFileScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosVirtualFileScanner();

	virtual bool  	split(
						vector<AosDataScannerObjPtr> &scanners, 
						const int64_t &record_len,
						const AosRundataPtr &rdata);
	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool 	appendBlock(
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	virtual bool 	modifyBlock(
						const int64_t &seekPos, 
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);

private:
	bool config(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	bool getAllCrtSegments(const AosRundataPtr &rdata);
};
#endif
#endif
