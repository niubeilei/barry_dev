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
// 08/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataxCacher_CacherVirtualFile_h
#define Aos_DataxCacher_CacherVirtualFile_h

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


class AosCacherVirtualFile : public AosWriteCacherObj
{

private:
	OmnMutexPtr					mLock;
	AosXmlTagPtr				mActions;
	AosDataCacherObjPtr			mFilteredTrashCan;
	AosDataCacherObjPtr			mUnrecogCacher;
	AosDataCacherObjPtr			mUnrecogTrashCan;
	AosValueSelObjPtr			mCacherPicker;
	u64							mNumErrors;
	AosNetFileObjPtr			mNetFile;

public:
	AosCacherVirtualFile(const bool flag);
	AosCacherVirtualFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosCacherVirtualFile();

	virtual bool appendData(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool appendRecord( const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool appendEntry( const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool finish(
	 		const u64 &totalentries, 
	 		const AosXmlTagPtr &xml,
	 		const AosRundataPtr &rdata);
	virtual bool finish(const AosRundataPtr &rdata);
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

	virtual char* nextValue(int&, const AosRundataPtr&);
	virtual bool getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	
private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
};

#endif

