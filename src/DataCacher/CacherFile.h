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
#ifndef Aos_DataxCacher_CacherFile_h
#define Aos_DataxCacher_CacherFile_h

#include "DataCacher/Ptrs.h"
#include "DataSort/Ptrs.h"
#include "DataSort/DataSort.h"
#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/WriteCacherObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosCacherFile : public AosWriteCacherObj
{

private:
	AosDataSortObjPtr	mMerger;
	AosDataSort *		mMergerRaw;
	AosXmlTagPtr		mActions;

public:
	AosCacherFile(const bool flag);
	AosCacherFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosCacherFile();

	virtual bool appendData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata);
	virtual bool appendRecord(const AosDataRecordObjPtr&, const AosRundataPtr&);
	virtual bool appendEntry(const AosValueRslt&, const AosRundataPtr&);
	virtual bool finish(
	 		const u64 &totalentries, 
	 		const AosXmlTagPtr &xml,
	 		const AosRundataPtr &rdata);
	virtual bool finish(const AosRundataPtr &rdata);
	virtual bool clear();
	virtual int64_t size();
	virtual int64_t getTotalFileLength();
	virtual char* getData(int64_t&) const;
	virtual AosDataBlobObjPtr toDataBlob() const;
	virtual AosDataCacherObjPtr clone();
	virtual AosDataCacherObjPtr clone(const AosXmlTagPtr&, const AosRundataPtr&);
	virtual bool serializeTo(const AosBuffPtr&, const AosRundataPtr&);
	virtual bool serializeFrom(const AosBuffPtr&, const AosRundataPtr&);
	virtual bool split(vector<AosDataCacherObjPtr>&, const AosRundataPtr&);
	virtual AosReadCacherObjPtr convertToReadCacher();
	virtual bool nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata);
	virtual bool getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual void setMemory(const u64 maxmem);
	virtual int  getMaxBucket();
	
private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
};

#endif

