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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_DataTable_h
#define Aos_Util_DataTable_h

#include "Database/DbRecord.h"
#include "DataFormat/Ptrs.h"
#include "DataFormat/TableType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


struct AosDataTableSorter : public binary_function<char *, char *, bool>
{
	int				mPos1;
	int				mLen1;
	AosDataType::E	mType1;
	int				mPos2;
	int				mLen2;
	AosDataType::E	mType2;

	AosDataTableSorter(
			const int pos, 
			const int len,
			const AosDataType::E type) 
	: 
	mPos1(pos), 
	mLen1(len),
	mType1(type),
	mPos2(-1),
	mLen2(-1),
	mType2(AosDataType::eInvalid)
	{
	}

	AosDataTableSorter(
			const int pos1, 
			const int len1,
			const AosDataType::E type1, 
			const int pos2, 
			const int len2, 
			const AosDataType::E type2) 
	: 
	mPos1(pos1), 
	mLen1(len1),
	mType1(type1),
	mPos2(pos2),
	mLen2(len2),
	mType2(type2)
	{
	}

	inline bool operator()(char *x, char *y)
	{
		switch (mType1)
		{
		case AosDataType::eU64:
			 return *(u64*)&x[mPos1] <= *(u64*)&y[mPos1];

		case AosDataType::eInt64:
		case AosDataType::eDouble:
		case AosDataType::eChar:

		case AosDataType::eString:
			 return strncmp(&x[mPos1], &y[mPos1], mLen1);

		default:
			 return false;
		}
	}
};


class AosDataTable : virtual public OmnRCObject
{
protected:
	AosTableType::E		mType;
	int					mStartIdx;
	int					mPageSize;

public:
	AosDataTable(const OmnString &name, const AosTableType::E type, const bool flag);
	~AosDataTable();

	// AosDataTable interface
	virtual int			getNumRecords() const = 0;
	virtual bool		clear() = 0;
	virtual bool		copyData(const char *data, const int len, AosRundata *) = 0;
	
	virtual char *		getRecord(const int record_idx, int &record_len) const = 0;
	virtual AosDataRecordObjPtr getRecord() const = 0;

	virtual int64_t		resize(const int64_t &num_records, AosRundata *rdata) = 0;
	virtual int64_t		setSize(const int64_t &data_size, AosRundata *rdata) = 0;
	virtual int			getFieldLen(const int idx, AosRundata *rdata) const = 0;
	virtual AosDataTablePtr clone(const AosXmlTagPtr &def, AosRundata *) const = 0;
	virtual bool 		appendRecord(const AosDataRecordObjPtr &, AosRundata *) = 0;
	virtual bool		sort() = 0;

	static AosDataTablePtr getTable(const AosXmlTagPtr &def, AosRundata *rdata);
	static AosDataTablePtr getTable(const int fieldlen1, 
								const int fieldlen2, 
								const int start_idx,
								const int pagesize, 
								AosRundata *rdata);
	static bool staticInit();

	int getStartIdx() const {return mStartIdx;}
	int getPageSize() const {return mPageSize;}
private:
	bool registerTable(const OmnString &name, AosDataTable *record);
};

#endif

