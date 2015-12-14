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
#ifndef Aos_Funcs_Jimos_Trim_h
#define Aos_Funcs_Jimos_Trim_h

#include "Funcs/GenFunc.h"

class AosTrim : public AosGenFunc
{
	OmnDefineRCObject;

private:

public:
	AosTrim(const int version);
	AosTrim();
	AosTrim(const AosTrim &rhs);
	~AosTrim();
	
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

