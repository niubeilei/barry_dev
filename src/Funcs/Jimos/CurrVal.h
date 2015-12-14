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
// 2015/03/04 Created by Crystal Cao
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_CurrVal_h
#define Aos_Funcs_Jimos_CurrVal_h

#include "Funcs/GenFunc.h"

class AosCurrVal : public AosGenFunc
{
	OmnDefineRCObject;

private:

public:
	AosCurrVal(const int version);
	AosCurrVal();
	AosCurrVal(const AosCurrVal &rhs);
	~AosCurrVal();
	
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

