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
// Content Selector is a class that selects some contents based 
// on various kinds of conditions. This serves as the super class
// of all content selectors.
//
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/cs.h"

#include "parser/cs_fixed.h"
#include "parser/cs_enum.h"
#include "parser/xml_node.h"


int aos_cont_selector_init(
		aos_cont_selector_t *cs, 
		const aos_cs_type_e type)
{
	aos_assert_r(cs, -1);
	cs->type = type;
	return 0;
}


// 
// Description
// This is a factory function that creates a new content
// selector based on the xml configuration: 'conf'. If successful, 
// it creates the content selector. Otherwise, an alarm is raised
// and null is returned.
//
// Different types of content selectors are identified by their
// xml tag names.
//
aos_cont_selector_t *aos_cont_selector_factory(
		struct aos_xml_node *conf)
{
	aos_assert_r(conf, 0);

	if (conf->mf->match_name((aos_field_t *)conf, "CS_Fixed"))
	{
		aos_cs_fixed_t *cs = aos_cs_fixed_create_xml(conf);
		aos_assert_r(cs, 0);
		return (aos_cont_selector_t *)cs;
	}

	if (conf->mf->match_name((aos_field_t *)conf, "CS_Enum"))
	{
		aos_cs_enum_t *cs = aos_cs_enum_create_xml(conf);
		aos_assert_r(cs, 0);
		return (aos_cont_selector_t *)cs;
	}

	aos_alarm("Unrecognized contents: %s", conf->mf->dump(
				(aos_field_t *)conf));
	return 0;
}


