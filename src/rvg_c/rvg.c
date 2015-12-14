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
// This class defines the super class for all RVGs.
//
// An RVG may also be used as a domain. One can call the function 
// 'domain_check(...)' to determine whether a specific value is in 
// the domain or not.
//   
//
// Modification History:
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rvg.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "parser/parser.h"
#include "porting_c/mutex.h"
#include "rvg_c/rvg_xml.h"
#include "rvg_c/rsg.h"
#include "rvg_c/rvg_and.h"
#include "rvg_c/rvg_or.h"
#include "rvg_c/rig_basic.h"
#include "rvg_c/rig_unique.h"
#include "rvg_c/rsg_charset.h"
#include "rvg_c/rsg_enum.h"
#include "util2/global_data.h"
#include "util_c/memory.h"
#include "util_c/rc_obj.h"
#include "util_c/strutil.h"

static aos_lock_t *sg_rcobj_lock = 0;

int aos_rvg_integrity_check(struct aos_rvg *rvg)
{
	return 1;
}


// 
// Currently not doing anything
//
aos_xml_node_t * aos_rvg_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		<Node name="thename">
	// 			<Keyword>
	// 			<Description>
	// 			...
	// 		</Node>
	// 	</Parent>
	//
	aos_assert_r(rvg, 0);
	aos_assert_r(parent, 0);
	aos_xml_node_t *node = parent->mf->append_child_node(parent, "Node");
	aos_assert_r(node, 0);
	
	// Set the attrinute name="thename"
	aos_assert_g(!node->mf->add_attr((aos_field_t *)node, "name", rvg->name), cleanup);

	// Add the Keywords
	if (rvg->keywords)
	{
		aos_assert_g(!node->mf->append_child_str(node, "Keyword", rvg->keywords, 0, 0), cleanup);
	}

	if (rvg->description)
	{
		aos_assert_g(!node->mf->append_child_str(node, "Description", rvg->description, 0, 0), cleanup);
	}

	return node;

cleanup:
	node->mf->put((aos_field_t *)node);
	return 0;
}


// 
// Currently not doing anything.
//
int aos_rvg_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	// 
	// 	<Node name="thename">
	// 		<Keyword>
	// 		<Description>
	// 		...
	// 	</Node>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);

	// Retrieve the name
	char *name = node->mf->get_attr((aos_field_t *)node, "name");
	aos_assert_r(name, -1);
	if (rvg->name) aos_free(rvg->name);
	aos_assert_r(!aos_str_set(&rvg->name, name, strlen(name)), -1);

	// Retrieve the keywords
	int len;
	if (rvg->keywords) aos_free(rvg->keywords);
	aos_assert_r(!node->mf->first_child_str_dft(node, "Keyword", &rvg->keywords, &len, 0), -1);

	// Retrieve description
	if (rvg->description) aos_free(rvg->description);
	aos_assert_r(!node->mf->first_child_str_dft(node, "Description", &rvg->description, &len, 0), -1);

	return 0;
}


int aos_rvg_release_memory(aos_rvg_t *rvg) 
{
	aos_assert_r(rvg, -1);
	if (rvg->name) aos_free(rvg->name);
	rvg->name = 0;
	if (rvg->keywords) aos_free(rvg->keywords); 
	rvg->keywords = 0;
	if (rvg->description) aos_free(rvg->description);
	rvg->description = 0;
	return 0;
}


char * aos_rvg_get_keywords(aos_rvg_t *rvg)
{
	aos_assert_r(rvg, 0);
	return rvg->keywords;
}


char * aos_rvg_get_description(aos_rvg_t *rvg)
{
	aos_assert_r(rvg, 0);
	return rvg->description;
}


int aos_rvg_hold(struct aos_rvg *rvg)
{
	AOS_RC_OBJ_HOLD(rvg);
	return 0;
}


int aos_rvg_put(struct aos_rvg *rvg)
{
	AOS_RC_OBJ_PUT(rvg);
	return 0;
}


int aos_rvg_init(aos_rvg_t *rvg) 
{
	return 0;
}


aos_rvg_t *aos_rvg_factory_str(const char * const contents)
{
	aos_assert_r(contents, 0);
	int is_complete;
	int len = strlen(contents);
	aos_xml_node_t *node = aos_xml_node_create_from_str(0, contents, &len, &is_complete, 0);
	aos_assert_r(node, 0);
	aos_rvg_t *rvg = aos_rvg_factory_xml(node);
	aos_assert_r(rvg, 0);
	return rvg;
}


aos_rvg_t *aos_rvg_factory_xml(struct aos_xml_node *node)
{
	char buff[100];
	int buff_len = 100;
	aos_assert_r(node, 0);

	aos_assert_rm(!node->mf->first_child_str_b(
			node, "Type", buff, &buff_len, 0), 0,
			"Expecting the <Type> tag but not found: %s", 
			node->contents);

	if (strcmp(buff, "Xml") == 0)
	{
		aos_rvg_xml_t *t = aos_rvg_xml_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "Charset RSG") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rsg_charset_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "Enum RSG") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rsg_enum_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "RigBasic") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rig_basic_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "RigUnique") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rig_unique_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "RvgAnd") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rvg_and_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	if (strcmp(buff, "RvgOr") == 0)
	{
		aos_rvg_t *t = (aos_rvg_t *)aos_rvg_or_create_xml(node);
		aos_assert_r(t, 0);
		return (aos_rvg_t *)t;
	}

	aos_alarm("Unrecognized tag: %s. Node: %s", 
			buff, node->mf->dump((aos_field_t *)node));
	return 0;
}

