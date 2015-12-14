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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_Jimos_DataScannerCompound_h
#define AOS_DataScanner_Jimos_DataScannerCompound_h

#include "DataScanner/DataScanner.h"
#include "Thread/Ptrs.h"


class AosDataScannerCompound : virtual public AosDataScanner
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxMemberScanners = 1000000
	};

	OmnMutexPtr						mLock;
	AosXmlTagPtr					mMetadata;
	vector<AosDataScannerObjPtr>	mScanners;
	int								mCrtScannerIdx;

public:
	AosDataScannerCompound();
	AosDataScannerCompound(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
	AosDataScannerCompound(const AosRundataPtr &rdata, 
				const OmnString &objid);
	~AosDataScannerCompound();

	// Data Scanner Interface
	virtual int 	getPhysicalId() const;
	virtual bool 	getNextBlock(AosBuffDataPtr &data, const AosRundataPtr &rdata);
	virtual bool	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual int64_t	getTotalSize() const;
	virtual AosJimoPtr cloneJimo() const;
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata);

private:
	bool initFile(const AosRundataPtr &rdata, const OmnString &objid);
	bool initFile(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	AosXmlTagPtr createDefaultMetadata(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &def);
};
#endif
