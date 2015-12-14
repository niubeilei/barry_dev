////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/08/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoProg_AggrFuncCount_h
#define Aos_JimoProg_AggrFuncCount_h

#include "AggrFuncs/AggrFunc.h"
#include "Util/DataTypes.h"

class AosAggrFuncCount : public AosAggrFunc 
{
	OmnDefineRCObject;

private:
	AosDataType::E		mDataType;

public:
	AosAggrFuncCount(const int version);
	~AosAggrFuncCount();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(AosRundata *rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	
	virtual bool updateData(AosRundata *rdata, 
				char *field_data, 
				const int field_len, 
				const char *input_data, 
				const u32 input_data_len, 
				const AosDataType::E input_data_type);

	virtual bool updateOutputData(
				AosRundata *rdata, 
				char *input_value, 
				const AosDataType::E input_value_type,
				char *output_value, 
				const AosDataType::E output_value_type);

	virtual OmnString getAggrFuncName(){ return "count"; };



	virtual bool updateOutputDataWithDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type,
			AosAggrFuncObj::E deltatype);

private:
	//yang 2015.07.27
	//JIMODB-95
	bool updateOutputDataWithInsertDeltaData(
				AosRundata *rdata,
				char *input_value,
				const AosDataType::E input_value_type,
				char *output_value,
				const AosDataType::E output_value_type);
	bool updateOutputDataWithDeleteDeltaData(
				AosRundata *rdata,
				char *input_value,
				const AosDataType::E input_value_type,
				char *output_value,
				const AosDataType::E output_value_type);
	bool updateOutputDataWithUpdateDeltaData(
				AosRundata *rdata,
				char *input_value,
				const AosDataType::E input_value_type,
				char *output_value,
				const AosDataType::E output_value_type);
};

#endif

