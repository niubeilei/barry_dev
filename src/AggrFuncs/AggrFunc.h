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
#ifndef AOS_AggrFunc_AggrFunc_h
#define AOS_AggrFunc_AggrFunc_h

#include "SEInterfaces/AggrFuncObj.h"
#include "Thread/Ptrs.h"
#include "Util/DataTypes.h"

class AosAggrFunc : public AosAggrFuncObj
{
public:
	AosAggrFunc(const int version);
	AosAggrFunc();

	~AosAggrFunc();

	virtual bool config(AosRundata *rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	
	virtual AosAggrFuncObjPtr pickAggrFunc(
				const AosRundataPtr &rdata,
				const OmnString &func_name);
	
	virtual bool isFuncValid(
				const AosRundataPtr &rdata,
				const OmnString &func_name);
	
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
	
	virtual OmnString getAggrFuncName();

	bool isDataTypeValid(const AosDataType::E type);

	virtual bool updateOutputDataWithDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type,
			AosAggrFuncObj::E deltatype)
	{
		OmnNotImplementedYet;
		return false;
	}

};
#endif

