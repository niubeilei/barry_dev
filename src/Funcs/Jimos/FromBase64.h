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
// 2014-11-4 Created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_FromBase64_h
#define Aos_Funcs_Jimos_FromBase64_h

#include "Funcs/GenFunc.h"

#define Aos_Funcs_Jimos_FromBase64_Max_Encoded_Len 10240

class AosFromBase64 : public AosGenFunc
{
	OmnDefineRCObject;

private:
public:
	AosFromBase64(const int version);
	AosFromBase64();
	AosFromBase64(const AosFromBase64 &rhs);
	~AosFromBase64();
	
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

