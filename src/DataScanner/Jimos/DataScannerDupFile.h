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
#ifndef AOS_DataScanner_Jimos_DataScannerFile_h
#define AOS_DataScanner_Jimos_DataScannerFile_h

#include "DataScanner/DataScanner.h"
#include "Thread/Ptrs.h"


class AosDataScannerFile : virtual public AosDataScanner
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	int64_t			mStartPos;
	int64_t			mFileLength;
	int64_t			mReadBlockSize;
	AosXmlTagPtr	mMetadata;
	OmnString		mFileName;
	int				mPhysicalid;
	OmnString		mCharset;

public:
	AosDataScannerFile();
	AosDataScannerFile(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
	AosDataScannerFile(const AosRundataPtr &rdata, 
				const OmnString &objid);
	~AosDataScannerFile();

	// Data Scanner Interface
	virtual int 	getPhysicalId() const {return mPhysicalid;}
	virtual bool 	getNextBlock(const AosRundataPtr &rdata, AosBuffDataPtr &info);
	virtual bool	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata);
	virtual bool 	getInstances(const AosRundataPtr &rdata, 
						vector<AosDataScannerObjPtr> &instances);

	// DataSource interface
	virtual void resetMemberLoop();
	virtual bool hasMore() const;
	virtual AosDataFileObjPtr nextFile(const AosRundataPtr &rdata);
	virtual AosDataSourceObjPtr cloneDataSource(const AosRundataPtr &rdata);
	virtual int64_t getTotalSize() const;
	virtual bool addDataFile(
					const AosRundataPtr &rdata, 
					const AosDataFileObjPtr &file);

private:
	bool getNextBlock( const AosRundataPtr &rdata,
					AosBuffPtr &buff, 
					AosDiskStat &disk_stat);
	bool initFile(const AosRundataPtr &rdata, const OmnString &objid);
	bool initFile(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
};
#endif
