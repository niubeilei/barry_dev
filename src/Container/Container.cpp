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
// 2015/01/29 Created by <please add your name here>
////////////////////////////////////////////////////////////////////////////
#include "Container/Container.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JqlStatementObj.h"


AosContainer::AosContainer()
{
}


AosContainer::~AosContainer()
{
}


bool				 
AosContainer::dropTable(AosRundata *rdata)
{
	// It drops this table. This means to remove the doc
	// from the database. 
}


vector<OmnString>	 
AosContainer::getPrimaryKey() const
{
	// Not all tables define primary keys. If yes, it returns
	// the primary key. Note that a primary key may have 
	// multiple fields. 
}


bool				 
AosContainer::canModifyField(
		AosRundata *rdata, 
		const OmnString &name,
		const AosValueRslt &old_value, 
		const AosValueRslt &new_value)
{
	// This function checks whether the field can be modified
	// from the old value to the new value. A field cannot be
	// modified if:
	// 1. it is one of the primary key field, 
	// 2. there is a field validator and the validator 
	//    returns false.
}

	
bool				 
AosContainer::canModifyFields(
		AosRundata *rdata, 
		const OmnString &name,
		const vector<AosValueRslt> &old_values, 
		const vector<AosValueRslt> &new_values)
{
}

	
bool				 
AosContainer::isUnique(
		AosRundata *rdata, 
		const OmnString &field_name,
		const OmnString &field_value)
{
	// It checks whether the field value 'field_value' is
	// unique among all the values of the same field 'field_name'.
}


bool				 
AosContainer::isUnique(
		AosRundata *rdata, 
		const OmnString &field_name,
		const u64 field_value)
{
}


bool				 
AosContainer::isUnique(
		AosRundata *rdata, 
		const OmnString &field_name,
		const AosValueRslt &field_value)
{
}


bool				 
AosContainer::isUnique(
		AosRundata *rdata, 
		const vector<OmnString> &names, 
		const vector<AosValueRslt> &values)
{
}


AosContainerPtr createContainer(
		AosRundata *rdata, 
		const AosJqlStatementObjPtr &table_def)
{
	// This function creates a container based on the 
	// definition 'table_def'.
}


bool
AosContainer::createVirtualField(
		AosRundata *rdata, 
		AosDataFieldObj *field)
{
	// This function creates a virtual field. A virtual field
	// is defined by:
	// 	table
	// 	field being virtualized (only one)
	// 	joining fields: {[from, to], [from, to], ...}, optional
	// 	conditions
}


AosDataFieldObjPtr
AosContainer::getVirtualField(
		AosRundata *rdata, 
		const OmnString &mapped_field,
		const vector<OmnString> &key_fields,
		const vector<AosJQLCond> &conds)
{
	// A virtual field is defined by:
}


bool
AosContainer::doesMapExist(
		AosRundata *rdata, 
		const OmnString &mapped_field,
		const vector<OmnString> &key_fields,
		const vector<AosJQLCond> &conds)
{
	// A map is defined by:
	// 	1. One mapped field
	// 	2. One or more key fields,
	// 	3. Zero or more conditions
	// 
	// A container is configured as:
	// 	<container ...>
	// 		<maps>
	// 			<map map_id="ddd"
	// 				mapped_field="xxx", 
	// 				key_fields="xxx,xxx...">
	// 				<conditions>
	// 					<condition .../>
	// 					<condition .../>
	// 					...
	// 					<condition .../>
	// 				</conditions>
	// 			</map>
	// 		</maps>
	// 	</container>

}


bool
AosContainer::isJoinMapAvailable(
		AosRundata *rdata, 
		const vector<OmnString> &keys,
		const AosJoinType join_type,
		const vector<AosQueryCondPtr> &conds)
{
	// A join map is defined by:
	// 	1. One or more key fields:
	// 			[key1, key2, ...]
	//  2. Join type
	//  3. Zero or more conditions
	if (!mJoinMapsParsed)
	{
		if (!parseJoinMaps(rdata))
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
	}
	aos_assert_rr(mJoinMapsParsed, rdata, false);

	for (u32 i=0; i<mJoinMaps.size(); i++)
	{
		if (doesJoinMapMatch(mJoinMaps[i], keys, join_type, conds)) return true;
	}

	return false;
}


bool
AosContainer::parseJoinMaps(AosRundata *rdata)
{
	// Tables are defined as:
	// 	<table ...>
	// 		<join_maps>
	// 			<join_map .../>
	// 			<join_map .../>
	// 			...
	// 		</join_maps>
	// 	</table>
	aos_assert_rr(mDoc, rdata, false);

	mJoinMaps.clear();
	AosXmlTagPtr tags = mDoc->getFirstChild("join_maps");
	if (!tags) 
	{
		// There are no join maps
		return true;
	}

	AosXmlTagPtr tag = tags->getFirstChild();
	while (tag)
	{
		AosJoinMapPtr join_map = AosJoinMap::parseJoinMap(rdata, tag);
		if (!join_map)
		{
			AosLogError(rdata, true, "failed_parsing_join_map") << enderr;
			return false;
		}
		mJoinMaps.push_back(join_map);
		tag = tags->getNextChild();
	}

	return true;
}

