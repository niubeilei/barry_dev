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
// 2014/05/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordJoin_h
#define Aos_DataRecord_RecordJoin_h

#include "DataRecord/DataRecord.h"


class AosRecordJoin : public AosDataRecord
{
public:
	enum JoinType
	{
		eInnerJoin,
		eOutJoin
	};	

private:
	JoinType							mJoinType;
	map<OmnString, AosDataRecordObjPtr>	mRecords;
	
public:
	AosRecordJoin(const bool flag AosMemoryCheckDecl);
	AosRecordJoin(const JoinType type AosMemoryCheckDecl);
	AosRecordJoin(
			const AosRecordJoin &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordJoin();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

	virtual int			getRecordLen() {return 0;}
	virtual int			getEstimateRecordLen() {return 0;} 

	virtual int			getMemoryLen() {return 0;}
	virtual char *		getData(AosRundata *rdata) {return 0;}
	virtual void		setMemory(char* data, const int64_t &len){}

	virtual bool 		setData(
							char *data, 
							const int len, 
							AosMetaData *metaData, 
							const int64_t offset) {return false;}
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const u64 task_docid,
							AosRundata *rdata AosMemoryCheckDecl) const;

	virtual bool		getFieldValue(
							const OmnString &field_name,
							AosValueRslt &value,
							const bool copy_flag,
							AosRundata* rdata);

	bool				setRecords(
							vector<AosDataRecordObjPtr> &records,
							AosRundata *rdata);
private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};

#endif

