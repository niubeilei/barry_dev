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
// 09/11/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryFormat_QueryFormatCSV_h
#define Aos_QueryFormat_QueryFormatCSV_h

#include "SEInterfaces/QueryFormatObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Jimo/Jimo.h"
#include "QueryFormat/Ptrs.h"
#include "QueryInto/Ptrs.h"
#include "SEInterfaces/QueryIntoObj.h"

#include "Util/RCObjImp.h"


class AosQueryFormatCSV : public AosQueryFormatObj,  public AosJimo
{
	OmnDefineRCObject;

private:
	vector<AosExprObjPtr>			mFields;
	AosDataRecordObjPtr				mOutputRecord;
	AosQueryIntoObjPtr				mQueryIntoFile;

public:
	AosQueryFormatCSV();
	AosQueryFormatCSV(const int ver);
	AosQueryFormatCSV(const AosQueryFormatCSV &proc);
	~AosQueryFormatCSV();

	AosJimoPtr cloneJimo() const;
	AosQueryFormatCSVPtr  clone() const;

	bool	 config(
				const AosXmlTagPtr &format,
				const AosXmlTagPtr &output_record,
				const AosRundataPtr &rdata);

	virtual void 	setQueryInto(const AosQueryIntoObjPtr &query_into);

	virtual bool proc(
				const AosDatasetObjPtr &dataset,
				AosRundata* rdata);

};
#endif

