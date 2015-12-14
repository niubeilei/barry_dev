////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FuncModule.h
// Description:
//   
//
// Modification History:
// 05/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_FuncModule_h
#define Omn_Torturer_FuncModule_h

#include "TorturerGen/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosFuncModule : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxFuncs = 1000
	};

private:
	OmnString									mModuleId;
    OmnDynArray<AosFuncTesterGenPtr>  	mFuncs;

public:
	AosFuncModule(const OmnString &moduleName);
	virtual ~AosFuncModule();

	bool 	addFunc(const AosFuncTesterGenPtr &func);
	bool	parseApi(const OmnXmlItemPtr &item,
					const OmnString &srcDir);

	bool	genCode(const OmnString &srcDir, 
					const OmnString &torturerName,
					const OmnString &additionalLibs,
					const bool force) const;

private:
	bool 	createMain(const OmnString &torturerDir, 
					const OmnString &torturerName, 
					const bool force) const;
	bool 	createMakefile(const OmnString &torturerDir, 
					const OmnString &torturerName, 
					const OmnString &libname, 
					const bool force) const;
};

#endif

