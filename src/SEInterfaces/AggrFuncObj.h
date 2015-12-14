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
#ifndef AOS_SEInterfaces_AggrFuncObj_h
#define AOS_SEInterfaces_AggrFuncObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "SEInterfaces/AggregationType.h"
#include "Util/Ptrs.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosAggrFuncObj : public AosJimo
{
protected:
	static AosAggrFuncObjPtr	smObject;

public:

	enum E
	{
		INSERT,
		DELETE,
		UPDATE
	};


	AosAggrFuncObj(const int version);

	~AosAggrFuncObj();
	
	static void setAggrFuncObj(const AosAggrFuncObjPtr &obj) {smObject = obj;}
	
	static AosAggrFuncObjPtr pickAggrFuncStatic(
				const AosRundataPtr &rdata,
				const OmnString &func_name);
	
	static bool	isFuncValidStatic(
				const AosRundataPtr &rdata,
				const OmnString &func_name);
	
	virtual bool isFuncValid(
				const AosRundataPtr &rdata,
				const OmnString &func_name) = 0;
	
	virtual AosAggrFuncObjPtr pickAggrFunc(
				const AosRundataPtr &rdata,
				const OmnString &func_name) = 0;

	virtual bool updateData(AosRundata *rdata, 
				char *field_data, 
				const int field_len, 
				const char *input_data, 
				const u32 input_data_len, 
				const AosDataType::E input_data_type) = 0;
	
	virtual bool updateOutputData(
				AosRundata *rdata, 
				char *input_value, 
				const AosDataType::E input_value_type,
				char *output_value, 
				const AosDataType::E output_value_type) = 0;

	
	virtual OmnString getAggrFuncName() = 0;


	virtual bool updateOutputDataWithDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type,
			AosAggrFuncObj::E deltatype) = 0;

};
#endif

