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
// 03/05/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "thread_c/named_var.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "util_c/rc_obj.h"
#include "util_c/memory.h"
#include "util_c/magic.h"
#include "util2/value.h"


AOS_DECLARE_RCOBJ_LOCK;

int aos_named_var_hold(aos_named_var_t *var)
{
	AOS_RC_OBJ_HOLD(var);
}


int aos_named_var_put(aos_named_var_t *var)
{
	AOS_RC_OBJ_PUT(var);
}


int aos_named_var_release_memory(aos_named_var_t *var)
{
	aos_assert_r(var, -1);
	aos_assert_r(var->value.ref_count == 0, -1);
	aos_assert_r(!var->value.mf->release_memory(&var->value), -1);
	return 0;
}


int aos_named_var_destroy( struct aos_named_var *var)
{
	aos_assert_r(!var->mf->release_memory(var), -1);
	aos_free(var);
	return 0;
}


int aos_named_var_serialize(
		aos_named_var_t *var, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<NamedVar>
	// 			<Tag>
	// 			<Name>
	// 			<Value>
	// 		</NamedVar>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(var, -1);
	aos_assert_r(parent, -1);

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "NamedVar");
	aos_assert_r(node, -1);

	// Set <Tag>
	aos_assert_r(!node->mf->append_child_str(node, "Tag", 
			aos_gen_data_tag_2str(var->tag), 0, 0), -1);

	// Set <Name>
	aos_assert_r(!node->mf->append_child_str(node, "Name", 
			var->name, 0, 0), -1);

	// Set <Value>
	aos_assert_r(!var->value.mf->serialize(&var->value, node), -1);

	return 0;
}


int aos_named_var_deserialize(
		aos_named_var_t *var, 
		aos_xml_node_t *node)
{
	// 
	// 	<NamedVar>
	// 		<Tag>
	// 		<Name>
	// 		<Value>
	// 	</NamedVar>
	//
	aos_assert_r(var, -1);
	aos_assert_r(node, -1);

	// Retrieve <Tag>
	char buff[100];
	int buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(node, "Tag", 
			buff, &buff_len, 0), -1);
	var->tag = aos_gen_data_tag_2enum(buff);
	aos_assert_r(aos_gen_data_tag_check(var->tag) == 1, -1);

	// Retrieve <Name>
	int len = AOS_NAMED_VAR_MAX_NAME_LEN;
	aos_assert_r(!node->mf->first_child_str_b(node, "Name", 
			var->name, &len, 0), -1);

	// Retrieve <Value>
	aos_xml_node_t *child = node->mf->first_named_child(node, "Value");
	aos_assert_r(child, -1);
	aos_assert_r(!var->value.mf->deserialize(&var->value, child), -1);
	return 0;
}


static aos_named_var_mf_t sg_mf = 
{
	aos_named_var_hold,
	aos_named_var_put,
	aos_named_var_serialize,
	aos_named_var_deserialize,
	aos_named_var_release_memory,
	aos_named_var_destroy
};


// 
// !!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!
// This function only initialize tag and name
//
int aos_named_var_init_partial(
		aos_named_var_t *var, 
		const aos_data_type_e tag, 
		const char * const name)
{
	aos_assert_r(var, -1);
	var->magic = AOS_NAMED_VAR_MAGIC;
	aos_assert_r(name, -1);
	aos_assert_r(strlen(name) <= AOS_NAMED_VAR_MAX_NAME_LEN, -1);
	var->tag = tag;
	strcpy(var->name, name);
	return 0;
}


int aos_named_var_init(aos_named_var_t *var)
{
	aos_assert_r(var, -1);
	memset(var, 0, sizeof(aos_named_var_t));
	var->mf = &sg_mf;
	var->magic = AOS_NAMED_VAR_MAGIC;
	var->name[0] = 0;
	AOS_INIT_LIST_HEAD(&var->link);
	aos_assert_r(!aos_value_init(&var->value), -1);
	return 0;
}


aos_named_var_t *aos_named_var_create(
		const int tag, 
		const char * const name) 
{
	aos_named_var_t *var = aos_malloc(sizeof(aos_named_var_t));
	aos_assert_r(var, 0);
	aos_assert_g(!aos_named_var_init(var), cleanup);
	if (name) 
	{
		aos_assert_g(strlen(name) <= AOS_NAMED_VAR_MAX_NAME_LEN, cleanup);
		strcpy(var->name, name);
	}

	var->tag = tag;
	return var;

cleanup:
	aos_free(var);
	return 0;
}



aos_named_var_t *aos_named_var_create_str(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		char * value)
{
	aos_named_var_t *obj = aos_named_var_create(tag, name);
	aos_assert_g(!obj->value.mf->set_str(&obj->value, 
				value, strlen(value)), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_named_var_t *aos_named_var_create_ptr(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		void *ptr)
{
	aos_named_var_t *obj = aos_named_var_create(tag, name);
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->value.mf->set_ptr(&obj->value, ptr), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_named_var_t *aos_named_var_create_int(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		const int value)
{
	aos_named_var_t *obj = aos_named_var_create(tag, name);
	aos_assert_g(!obj->value.mf->set_int(&obj->value, value), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_named_var_t *aos_named_var_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_named_var_t *var = aos_malloc(sizeof(aos_named_var_t));
	aos_assert_r(var, 0);
	aos_assert_g(!aos_named_var_init(var), cleanup);
	aos_assert_g(!var->mf->deserialize(var, node), cleanup);
	return var;

cleanup:
	aos_free(var);
	return 0;
}

