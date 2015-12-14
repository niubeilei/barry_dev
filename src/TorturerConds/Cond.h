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
// 05/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_Cond_h
#define Omn_TorturerCond_Cond_h

#include "CliTorturer/Ptrs.h"
#include "TorturerConds/Util.h"
#include "TorturerConds/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"

class AosCondData;

class AosTortCond : virtual public OmnRCObject
{
public:
	enum CheckTime
	{
		eCheckBeforeExecution, 
		eCheckAfterExecution, 
		eAlwaysCheck
	};

protected:
	AosTortCondOpr		mOpr;

public:
	AosTortCond();
	AosTortCond(const AosTortCondOpr opr);
	~AosTortCond();

	virtual bool	check(const AosCondData &cmd, 
						  bool &rslt, 
						  OmnString &errmsg) const = 0;
	virtual CheckTime	getCheckTime() const = 0;
	bool 		checkValues(const int v1, const int v2, 
						const OmnString &str1, 
						const OmnString &str2, 
						bool &rslt, 
						OmnString &errmsg) const;
	bool 		checkValues(const u32 v1, const u32 v2, 
						const OmnString &str1, 
						const OmnString &str2, 
						bool &rslt, 
						OmnString &errmsg) const;
	static bool parseConditions(const OmnXmlItemPtr &def, 
						OmnDynArray<AosTortCondPtr> &conds);

	static bool	parseArithNameValue(const OmnXmlItemPtr &item,
						OmnDynArray<AosTortCondPtr> &conds);
};

#endif

