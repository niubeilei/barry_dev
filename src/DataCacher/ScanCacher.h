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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataxCacher_ScanCacher_h
#define Aos_DataxCacher_ScanCacher_h

#include "DataCacher/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "DataBlob/DataBlob.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/ReadCacherObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosScanCacher : virtual public AosReadCacherObj
{

protected:
	AosDataScannerObjPtr	mScanner;
	AosDataBlobObjPtr		mBlob;
	bool					mNoMoreData;	// Chen Ding, 10/30/2012	
	u32						mMaxTasks;

public:
	AosScanCacher(const bool regflag);
	AosScanCacher(const AosScanCacher &rhs);
	AosScanCacher(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosScanCacher(
			const AosDataScannerObjPtr &scanner,
			const AosDataBlobObjPtr &blob, 
			const AosRundataPtr &rdata);
	~AosScanCacher();

	virtual bool appendData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata);
	virtual bool appendRecord(const AosDataRecordObjPtr&, const AosRundataPtr&);
	virtual bool appendEntry(const AosValueRslt&, const AosRundataPtr&);
	virtual bool finish(
	 		const u64 &totalentries, 
	 		const AosXmlTagPtr &xml,
	 		const AosRundataPtr &rdata);
	virtual bool	clear();
	virtual int64_t	size();
	virtual int64_t getTotalFileLength();
	virtual char *	getData(int64_t &len) const;
	virtual AosDataBlobObjPtr toDataBlob() const;
	virtual AosDataCacherType::E getType() const;
	virtual bool split(vector<AosDataCacherObjPtr> &cachers, 
							const AosRundataPtr &rdata);

	// Reading interface
	virtual bool 	nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata);
	virtual bool	nextValue(AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool	nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool	firstValueInRange(
							const int64_t &idx, 
							AosValueRslt &value, 
							const AosRundataPtr &rdata);
	virtual bool	nextValueInRange(AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool	firstRecordInRange(
							const int64_t &idx, 
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata);
	virtual bool	nextRecordInRange(
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata);
	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosDataCacherObjPtr clone();
	virtual AosDataCacherObjPtr clone(
							const AosXmlTagPtr &config, 
							const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool initMemberData();
	bool copyMemberData(const AosScanCacher &obj);
};

#endif

