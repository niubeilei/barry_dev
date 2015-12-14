////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApiCodeGen.h
// Description:
//   
//
// Modification History:
// 06/30/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_ApiCodeGen_h
#define Omn_Torturer_ApiCodeGen_h

#include "TorturerGen/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosApiCodeGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxApiCodeGens = 1000
	};

private:
    OmnDynArray<AosTesterGenPtr>  	mApiCodeGens;

public:
	AosApiCodeGen();
	virtual ~AosApiCodeGen();

	bool 	addApiCodeGen(const AosTesterGenPtr &gen);
	bool	genCode(const OmnString &libDir, 
					const OmnString &libName, 
					const OmnString &torturerDir,
					const OmnString &torturerName,
					const OmnString &additionalLibs,
					const bool force) const;
	bool	parseApi(const OmnXmlItemPtr &item,
					const OmnString &srcDir,
					const OmnString &testDir);

private:
};

#endif

