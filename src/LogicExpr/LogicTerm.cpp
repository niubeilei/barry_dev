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
// 07/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "LogicExpr/LogicTerm.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "LogicExpr/LogicAndClause.h"
#include "LogicExpr/LogicOrClause.h"
#include "LogicExpr/LogicIfParm.h"
#include "LogicExpr/LogicExpr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"


AosLogicTerm::AosLogicTerm()
{
}


AosLogicTerm::~AosLogicTerm()
{
}


AosLogicTermPtr
AosLogicTerm::createTerm(const OmnXmlItemPtr &item)
{
	if (!item) return 0;

	OmnXmlItemPtr def = item;
	AosLogicTermPtr  term;
	int guard = 0;
	while (!term && guard < 2)
	{
		guard++;
		OmnString tagname = def->getTag();
		tagname.toLower();
		if (tagname == "and")
		{
			term = OmnNew AosLogicAndClause();
		}
		else if (tagname == "if-parm")
		{
			term = OmnNew AosLogicIfParm();
		}
		else if (tagname == "or")
		{
			term = OmnNew AosLogicOrClause();
		}
		else if (tagname == "expr")
		{
			term = OmnNew AosLogicExpr();
		}
		else if (tagname == "condition")
		{
			def->reset();
			OmnXmlItemPtr item = def->next();
			if (def->hasMore() || !item)
			{
				OmnAlarm << "Invalid condition definition: " 
					<< def->toString() << enderr;
				return  0;
			}

			def = item;
		}
		else
		{
			OmnAlarm << "Unrecognized tag: " << def->toString() << enderr;
			return 0;
		}
	}

	if (!term)
	{
		OmnAlarm << "Running out of memory" << enderr;
		return 0;
	}

	if (!term->config(def)) return 0;

	return term;
}

