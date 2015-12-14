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
//
// Modification History:
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GicCodeGen_GicCodeGen_h
#define Aos_GicCodeGen_GicCodeGen_h

#include "GicCodeGen/Ptrs.h"
#include "GicCodeGen/GicGenType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosGicCodeGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosGicGenType::E		mType;

public:
	AosGicCodeGen(const OmnString &name, const AosGicGenType::E type, const bool flag);
	~AosGicCodeGen();

	virtual bool generateCode(
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj, 
					const AosXmlTagPtr &gic_def,
					const AosXmlTagPtr &gic,
					AosHtmlCode &code, 
					const AosRundataPtr &rdata) = 0;

	static bool checkRegistrations();
	static bool generateCodeStatic(
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj, 
					const AosXmlTagPtr &gic_def,
					const AosXmlTagPtr &gic,
					AosHtmlCode &code, 
					const AosRundataPtr &rdata);

private:
	bool registerGicCodeGen(const AosGicCodeGenPtr &gic_gen, const OmnString &name);
};
#endif

