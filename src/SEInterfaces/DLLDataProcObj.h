////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DLLDataProcObj_h
#define Aos_SEInterfaces_DLLDataProcObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

class AosDLLDataProcObj : public AosDLLObj, public AosDataProcObj
{
protected:
	OmnString		mProcName;

public:
	AosDLLDataProcObj(
					const OmnString &proc_name, 
					const OmnString &libname, 
					const OmnString &method, 
					const OmnString &version);
	virtual ~AosDLLDataProcObj();

	virtual AosDataProcObjPtr clone();
	virtual AosDataProcObjPtr createDataProc(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	virtual AosDataProcObjPtr create(
			            const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};
#endif

