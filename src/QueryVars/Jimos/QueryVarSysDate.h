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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryVars_Jimos_QueryVarSysDate_h
#define Aos_QueryVars_Jimos_QueryVarSysDate_h

#include "QueryVars/QueryVar.h"

class AosQueryVarSysDate : public AosQueryVar
{
	OmnDefineRCObject;

private:
	i64		mSysTime;

public:
	AosQueryVarSysDate(const int version);
	~AosQueryVarSysDate();
	
	virtual bool eval(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &def, 
					AosValueRslt &value);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

};
#endif

