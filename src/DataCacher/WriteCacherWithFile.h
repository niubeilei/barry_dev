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
#if 0
Renamed to AosCacherFile.h. Chen Ding, 08/29/2012
#ifndef Aos_DataxCacher_WriteCacherWithFile_h
#define Aos_DataxCacher_WriteCacherWithFile_h

#include "DataCacher/Ptrs.h"
#include "DataSort/Ptrs.h"
#include "DataSort/DataSort.h"
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


class AosWriteCacherWithFile : public AosWriteCacherObj
{

private:
	AosDataSortPtr      mMerger;
	AosXmlTagPtr		mActions;

public:
	AosWriteCacherWithFile(const bool flag);
	AosWriteCacherWithFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosWriteCacherWithFile();

	virtual bool appendData(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool finish(
			const u64 totalentries, 
			const AosXmlTagPtr &xml,
			const AosRundataPtr &rdata);
	virtual bool clear();
	virtual int64_t size();
	virtual char* getData(int64_t&) const;
	virtual AosDataBlobObjPtr toDataBlob() const;
	virtual AosDataCacherObjPtr clone();
	virtual AosDataCacherObjPtr clone(const AosXmlTagPtr&, const AosRundataPtr&);
	virtual bool serializeTo(const AosBuffPtr&, const AosRundataPtr&);
	virtual bool serializeFrom(const AosBuffPtr&, const AosRundataPtr&);
	virtual bool split(vector<AosDataCacherObjPtr>&, const AosRundataPtr&);
	virtual AosReadCacherObjPtr convertToReadCacher();
	virtual bool setFileName(int, const OmnString&);
	virtual char* nextValue(int&, const AosRundataPtr&);
	virtual bool appendRecord(const AosDataRecord&, const AosRundataPtr&);
	virtual bool appendEntry(const AosValueRslt&, const AosRundataPtr&);

	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	
private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
};

#endif
#endif

