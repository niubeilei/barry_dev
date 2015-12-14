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
#ifndef Aos_SEInterfaces_QueryFuncObj_h
#define Aos_SEInterfaces_QueryFuncObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/ValueRslt.h"


class AosQueryFuncObj : public AosJimo
{
protected:
	static AosQueryFuncObjPtr	smObject;

public:
	AosQueryFuncObj(const int version);
	~AosQueryFuncObj();

	static void setQueryFuncObj(const AosQueryFuncObjPtr &obj) {smObject = obj;}

	static bool evalStatic(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);

	virtual bool pickJimo(  const AosRundataPtr &rdata, 
						const AosXmlTagPtr &func_name, 
						AosValueRslt &value) = 0;

	virtual bool eval(  const AosRundataPtr &rdata, 
						const AosXmlTagPtr &func_name, 
						AosValueRslt &value) = 0;

};
#endif

