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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataBlob_BlobRecord_h
#define AOS_DataBlob_BlobRecord_h

#include "DataBlob/DataBlob.h"
#include "Util/ValueRslt.h"
#include "Util/DataTypes.h"
#include "Util/BuffArray.h"
#include "Util/DataRecord.h"
#include "Util/UtUtil.h"
#include "SEInterfaces/BuffData.h"

class AosValueRslt;

class AosBlobRecord : public AosDataBlob
{
protected:
	AosBuffArrayPtr		mData;

	// These member data need to be configured
	int					mRecordLen;
	u64					mRewriteLoopIdx;
	AosXmlTagPtr		mBuffArrayTag;

public:
	AosBlobRecord();
	AosBlobRecord(
			const AosXmlTagPtr &conf, 
			const AosRundataPtr &rdata);
	AosBlobRecord(
			const AosDataRecordObjPtr &record,
			const AosRundataPtr &rdata);
	AosBlobRecord(const AosBlobRecord &rhs);
	~AosBlobRecord();

	// DataBlob interface
	virtual bool    sort();
	virtual int64_t	size() const;
	virtual u64 	getMemSize() const;
	virtual bool	clearData(); 
	virtual bool    setData(const char *record, const u64 &len);
	virtual bool setDataBlock(const AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual int 	getRecordLen() const;
	virtual int 	getRecordLen(const int idx) const; 	
	virtual u64     getRewriteLoopIdx() const;
	virtual AosDataBlobObjPtr clone();
	virtual AosDataBlobObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual bool start(const AosRundataPtr &rdata);

	virtual bool finish(const AosRundataPtr &rdata);

	// This interface for AosBlobRecordStr
	virtual void	resetRecordLoop();
	virtual void	resetRangeLoop();
	virtual bool 	resetRewriteLoop();
	virtual bool    setRewriteLoopIdx(const u64 idx);
	virtual bool 	rewriteRecordByIdx(
						const u64 idx,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	virtual bool 	rewriteNextRecord(
						const AosDataRecordObjPtr &record,
						const bool overflow_flag,
						const AosRundataPtr &rdata);
	
	virtual bool nextRecord(const AosDataRecordObjPtr &record);
	virtual bool nextValue(AosValueRslt &value, 
						const bool copy_flag, 
						const AosRundataPtr &rdata);
	virtual bool nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata);
	virtual bool nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool nextValueInRange(AosValueRslt &value, 
						const bool need_copy,
						const AosRundataPtr &rdata);
	virtual bool firstValueInRange(
						const int64_t &rcd_idx, 
						AosValueRslt &value,
						const AosRundataPtr &rdata);
	virtual const char *getMemory(
			            const u64 &start_idx,
						const u64 &end_idx,
						int64_t &mem_size,
						const AosRundataPtr &rdata) const;
	virtual bool firstRecordInRange(
						const int64_t &rcd_idx, 
						const AosDataRecordObjPtr &record, 
						const AosRundataPtr &rdata);
	virtual bool nextRecordInRange(const AosDataRecordObjPtr &record, const AosRundataPtr &);

	virtual bool getRecord(const int64_t &idx, const AosDataRecordObjPtr &record) const;
	virtual const char *getMemory(
	                    const int64_t &start_idx,
	                    const int64_t &end_idx,
	                    int64_t &length,
	                    const AosRundataPtr &rdata) const;

	virtual bool appendEntry(
						const AosValueRslt &value, 
						const AosRundataPtr &rdata);
	
	virtual bool appendRecord(
						const AosDataRecordObjPtr &value, 
						const AosRundataPtr &rdata);
	
	virtual bool serializeTo(
			        	const AosBuffPtr &buff, 
					    const AosRundataPtr &rdata); 

	virtual bool serializeFrom(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

	virtual int64_t findFirstEntry(
			            const int64_t &start_pos,
						const OmnString &key,
						const AosRundataPtr &rdata) const;

	virtual bool hasMoreData() const;

	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);

protected:
	bool init();
	virtual bool runFullActions(const AosRundataPtr &rdata);
};
#endif

