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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_TestGen_h
#define Omn_TorturerGen_TestGen_h

#include "RVG/Ptrs.h"
#include "TorturerGen/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/DynArray.h"
#include "Util/String.h"
#include "XmlParser/Ptrs.h"



class AosTestGen : public OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnString						mFuncName;
	OmnDynArray<AosRVGPtr>			mParms;
	OmnDynArray<AosTestCheckPtr>	mChecks;

public:
	AosTestGen();
	AosTestGen(const OmnString &name);
	virtual ~AosTestGen();

	virtual OmnString	genCode(const OmnString &classname) = 0;

	bool	parseParms(const OmnXmlItemPtr &def);
	bool	parseChecks(const OmnXmlItemPtr &def);
	static AosTestGenPtr	createTest(const OmnXmlItemPtr &def);

	OmnString	getFuncDeclaration() const;
	OmnString	getFuncName() const;

private:
};

#endif

