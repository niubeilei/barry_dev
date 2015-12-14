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
#include "LogicExpr/LogicAndClause.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "LogicExpr/LogicTerm.h"
#include "XmlParser/XmlItem.h"

AosLogicAndClause::AosLogicAndClause()
{
}


AosLogicAndClause::~AosLogicAndClause()
{
}


bool 	
AosLogicAndClause::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<And>
	// 		<Term>
	// 		<Term>
	// 		...
	// 	</And>
	//
	aos_assert_r(def, false);

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		AosLogicTermPtr term = AosLogicTerm::createTerm(item);
		if (!term)
		{
			OmnAlarm << "Failed to create term: " 
				<< item->toString() << enderr;
			return false;
		}

		mTerms.append(term);
	}

	return true;
}


bool
AosLogicAndClause::evaluate(bool &rslt, 
							OmnString &errmsg,
							const AosLogicObjPtr &obj) const
{
	aos_assert_r(mTerms.entries() > 0, false);

	rslt = true;
	for (int i=0; i<mTerms.entries(); i++)
	{
		if (!mTerms[i]->evaluate(rslt, errmsg, obj)) return false;

		if (!rslt) return true;
	}

	return true;

}

