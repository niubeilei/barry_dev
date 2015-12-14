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
// 03/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/types.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"
#include <string.h>


const char * aos_rvg_type_2str(const aos_rvg_type_e type)
{
	switch (type)
	{
	case eAosRvg_RsgCharset:
		 return "RsgCharset";

	case eAosRvg_Xml: 
		 return "Xml";

	case eAosRvg_RigBasic: 
		 return "RigBasic";

	case eAosRvg_RvgAnd:
		 return "RvgAnd";

	case eAosRvg_RvgOr:
		 return "RvgOr";

	case eAosRvg_RsgEnum: 
		 return "RsgEnum";

	case eAosRvg_RigUnique: 
		 return "RigUnique";

	default:
		 aos_alarm("Unrecognized rvg type: %d", type);
		 return "Invalid";
	}

	aos_should_never_come_here;
	return "Invalid";
}

aos_rvg_type_e aos_rvg_type_2enum(const char * const type)
{
	aos_assert_r(type, eAosRvg_Invalid);

	if (strcmp(type, "RsgCharset") == 0) return eAosRvg_RsgCharset;
	if (strcmp(type, "Xml") == 0) return eAosRvg_Xml;
	if (strcmp(type, "RigBasic") == 0) return eAosRvg_RigBasic;
	if (strcmp(type, "RvgAnd") == 0) return eAosRvg_RvgAnd;
	if (strcmp(type, "RvgOr") == 0) return eAosRvg_RvgOr;
	if (strcmp(type, "RsgEnum") == 0) return eAosRvg_RsgEnum;
	if (strcmp(type, "RigUnique") == 0) return eAosRvg_RigUnique;

	aos_alarm("Unrecognized rvg type: %s", type);
	return eAosRvg_Invalid;
}



const char * aos_order_2str(const aos_order_e order)
{
	switch (order)
	{
	case eAosOrder_NoOrder:
		 return "NoOrder";

	case eAosOrder_Ascending:
		 return "Ascending";

	case eAosOrder_Descending:
		 return "Descending";

	default:
		 aos_alarm("Unrecognized order: %d", order);
		 return "Invalid";
	}

	aos_should_never_come_here;
	return "Invalid";
}


aos_order_e aos_order_2enum(const char * const order)
{
	aos_assert_r(order, eAosOrder_Invalid);

	if (strcmp(order, "NoOrder") == 0) return eAosOrder_NoOrder;
	if (strcmp(order, "Ascending") == 0) return eAosOrder_Ascending;
	if (strcmp(order, "Descending") == 0) return eAosOrder_Descending;

	aos_alarm("Unrecognized order: %s", order);
	return eAosOrder_Invalid;
}

const char * aos_domain_check_2str(const aos_domain_check_e type)
{
	switch (type)
	{
	case eAosDomainCheck_ShortestMatch:
		 return "ShortestMatch";

	case eAosDomainCheck_LongestMatch:
		 return "LongestMatch";

	default:
		 aos_alarm("Unrecognized domain check type: %d", type);
		 return eAosDomainCheck_Invalid;
	}

	aos_should_never_come_here;
	return eAosDomainCheck_Invalid;
}


aos_domain_check_e aos_domain_check_2enum(const char * const type)
{
	aos_assert_r(type, eAosDomainCheck_Invalid);

	if (strcmp(type, "ShortestMatch") == 0) return eAosDomainCheck_ShortestMatch;
	if (strcmp(type, "LongestMatch") == 0) return eAosDomainCheck_LongestMatch;

	aos_alarm("Unrecognized domain check type: %s", type);
	return eAosDomainCheck_Invalid;
}

aos_rvg_value_entry_t *aos_rvg_value_entry_create()
{
	aos_rvg_value_entry_t *vv = aos_malloc(sizeof(aos_rvg_value_entry_t));
	aos_assert_r(vv, 0);
	memset(vv, 0, sizeof(aos_rvg_value_entry_t));
	return vv;
}

