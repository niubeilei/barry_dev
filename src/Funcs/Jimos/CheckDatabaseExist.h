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
// 2015/01/30 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_CheckDatabaseExist_h
#define Aos_Funcs_Jimos_CheckDatabaseExist_h

#include "Funcs/GenFunc.h"


class AosCheckDatabaseExist : public AosGenFunc
{
	OmnDefineRCObject;

public:
	AosCheckDatabaseExist();
	AosCheckDatabaseExist(const int version);
	AosCheckDatabaseExist(const AosCheckDatabaseExist &rhs);
	~AosCheckDatabaseExist();
	

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

