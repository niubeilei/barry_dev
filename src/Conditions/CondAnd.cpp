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
// This is a condition that AND two or more conditions. The condition 
// is true if and only if all its member conditions are true.
//   
//
// Modification History:
// 01/28/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondAnd.h"

#include "alarm/Alarm.h"
#include "Conditions/CondData.h"



AosCondAnd::AosCondAnd()
{
}


AosCondAnd::~AosCondAnd()
{
}


bool 
AosCondAnd::serialize(TiXmlNode& node)
{
	//
	// 	<CondAnd>
	// 		<Condition>
	// 			...
	// 		</Condition>
	// 		<Condition>
	// 			...
	// 		</Condition>
	// 		...
	// 	</CondAnd>
	//
	node.SetValue("CondAnd");
	int size = mConditions.size();
	for (int i=0; i<size; i++)
	{
		TiXmlElement cnode("Condition");
		mConditions[i]->serialize(cnode);
		node.InsertEndChild(cnode);
	}
	return true;
}


bool 
AosCondAnd::deserialize(TiXmlNode* node)
{
	aos_assert_r(node, false);
	aos_assert_r(node->ValueStr() == "CondAnd", false);

	TiXmlNode *child = node->FirstChild();
	for (; child!=NULL; child = child->NextSibling())
	{
		AosConditionPtr cond = AosCondition::factory(child);
		if (!cond)
		{
			OmnAlarm << "failed to create condition" << enderr;
			return false;
		}
		mConditions.push_back(cond);
	}
	return true;
}


bool 
AosCondAnd::check(const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	int size = mConditions.size();
	for (int i=0; i<size; i++)
	{
		if (!mConditions[i]->check(ed)) return false;
	}

	return true;
}

