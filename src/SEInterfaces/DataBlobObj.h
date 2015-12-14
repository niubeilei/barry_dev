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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataBlobObj_h
#define Aos_SEInterfaces_DataBlobObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosValueRslt;

class AosDataBlobObj : virtual public OmnRCObject
{
private:
	static AosDataBlobObjPtr		smDataBlobObj;

public:
	virtual bool isSorted() const = 0;
	virtual bool sort() = 0;
	virtual int64_t size() const = 0;
	virtual int 	getRecordLen() const = 0;
	virtual bool hasMoreData() const = 0;
	virtual bool setDataBlock(const AosBuffDataPtr &info, const AosRundataPtr &rdata) = 0;
	virtual int64_t findFirstEntry(
				const int64_t &start_pos, 
				const OmnString &key, 
				const AosRundataPtr &rdata) const = 0;
	virtual const char *getMemory(
				const u64 &start_idx, 
				const u64 &end_idx, 
				int64_t &mem_size, 
				const AosRundataPtr &rdata) const = 0;

	// Read cacher interface
	virtual bool nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata) = 0;
	virtual bool	nextValue(AosValueRslt &value, 
							const bool need_copy,
							const AosRundataPtr &rdata) = 0;
	virtual bool	nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata) = 0;
	virtual bool	firstValueInRange(
							const int64_t &idx, 
							AosValueRslt &value, 
							const AosRundataPtr &rdata) = 0;
	virtual bool	nextValueInRange(AosValueRslt &value, 
							const bool need_copy,
							const AosRundataPtr &rdata) = 0;
	virtual bool	firstRecordInRange(
							const int64_t &idx, 
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata) = 0;
	virtual bool	nextRecordInRange(
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata) = 0;
	virtual bool 	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual bool 	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual AosDataBlobObjPtr clone() = 0;
	virtual AosDataBlobObjPtr createDataBlob(
							const AosDataRecordObjPtr &record,
							const AosRundataPtr &rdata) = 0;
	virtual AosDataBlobObjPtr createDataBlob(
							const OmnString &sep,
							const AosRundataPtr &rdata) = 0;
	virtual AosDataBlobObjPtr createDataBlob(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	static AosDataBlobObjPtr createDataBlobStatic(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	static AosDataBlobObjPtr createDataBlobStatic(
						const OmnString &sep,
						const AosRundataPtr &rdata);
	static AosDataBlobObjPtr createDataBlobStatic(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	static AosDataBlobObjPtr getDataBlobObj() {return smDataBlobObj;}
	static void setDataBlobObj(const AosDataBlobObjPtr &d) {smDataBlobObj = d;}
	static AosDataBlobObjPtr serializeFromStatic(
							const AosBuffPtr &buff,
							const AosRundataPtr &rdata);
};
#endif
