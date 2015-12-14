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
#ifndef Aos_SysDefinedValues_Jimos_QueryVarClipvar_h
#define Aos_SysDefinedValues_Jimos_QueryVarClipvar_h

#include "QueryVars/QueryVar.h"

class AosQueryVarClipvar : public AosQueryVar
{
	OmnDefineRCObject;

private:

public:
	AosQueryVarClipvar(const int version);
	~AosQueryVarClipvar();
	
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

