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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_bit_count_h
#define Aos_Funcs_Jimos_bit_count_h

#include "Funcs/GenFunc.h"
#include <bitset>
#include <string>
class AosBit_count : public AosGenFunc
{
	OmnDefineRCObject;

private:

public:
	AosBit_count(const int version);
	AosBit_count();
	AosBit_count(const AosBit_count &rhs);
	~AosBit_count();
	
	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata *rdata, 
					AosValueRslt &value, 
					AosDataRecordObj *record);

	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

	virtual bool syntaxCheck(
					const AosRundataPtr &rdata, 
					OmnString &errmsg);
};
#endif

