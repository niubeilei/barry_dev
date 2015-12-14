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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCacherObj_h
#define Aos_SEInterfaces_DataCacherObj_h

#include "DataCacher/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataCacherCreatorObj.h"
#include "SEInterfaces/DataCacherType.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataCacherObj : public OmnRCObject
{
	OmnDefineRCObject

protected:
	AosDataCacherType::E	mType;
	AosValueRslt			mUserData;

	static AosDataCacherCreatorObjPtr		smCreator;

public:
	AosDataCacherObj();
	AosDataCacherObj(
			const OmnString &name, 
			const AosDataCacherType::E type, 
			const bool flag);
	~AosDataCacherObj();

	virtual bool appendData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata) = 0;
	virtual bool appendRecord(const AosDataRecordObjPtr&, const AosRundataPtr&) = 0;
	virtual bool appendEntry(const AosValueRslt&, const AosRundataPtr&) = 0;
	virtual bool	clear() = 0;
	virtual int64_t size() = 0;
	virtual char *	getData(int64_t &len) const = 0;
	virtual AosDataBlobObjPtr toDataBlob() const = 0;

	virtual AosDataCacherObjPtr clone() = 0;
	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual AosDataCacherObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &) = 0;
	virtual bool split(vector<AosDataCacherObjPtr> &cachers, const AosRundataPtr &rdata) = 0;
	virtual int64_t getTotalFileLength() = 0;
	virtual bool 	nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata) = 0;
	virtual bool getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual void setMemory(const u64 maxmem){}
	virtual int  getMaxBucket(){return 0;}

	// Chen Ding, 08/23/2012
	virtual bool finish(
	 		const u64 &totalentries, 
	 		const AosXmlTagPtr &xml,
	 		const AosRundataPtr &rdata) = 0;

	void setUserData(const AosValueRslt &data) {mUserData = data;}
	AosValueRslt getUserData() const {return mUserData;}
	AosDataCacherType::E getType() const {return mType;}

	// These functions are removed. Chen Ding, 08/23/2012
	// virtual AosReadCacherObjPtr convertToReadCacher() = 0;

	static AosDataCacherObjPtr createDataCacher(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
	
	static AosDataCacherObjPtr createDataCacher(
	 						const AosDataScannerObjPtr &scanner,
	 						const AosDataBlobObjPtr &blob, 
	 						const AosRundataPtr &rdata);

	static bool registerDataCacher(const OmnString &name, const AosDataCacherObjPtr &cacher);

	static AosDataCacherObjPtr serializeFromStatic(
							const AosBuffPtr &buff, const AosRundataPtr &rdata);

	static void setCreator(const AosDataCacherCreatorObjPtr &creator) {smCreator = creator;}
	static AosDataCacherCreatorObjPtr getCreator() {return smCreator;}

protected:
};

#endif

