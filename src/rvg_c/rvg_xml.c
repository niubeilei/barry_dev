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
// Member Data
// struct aos_rsg * tag_name_rsg;       
// Used to randomly generate tag names.
//
// struct aos_rig * integer_value_rig;
// Used to randomly generate integral values.
//
// struct aos_rsg * str_value_rsg;
// Used to randomly generate string values. 
//
// struct aos_rsg * addr_value_rsg;
// Used to randomly generate IP address values.
//
// struct aos_rig * atomic_type_rig;
// Used to randomly determine atomic node types.
//
// struct aos_rig * num_child_rig;
// Used to randomly determine the number of child of a node.
//
// struct aos_rig * atomic_child_rig;
// Used to randomly determine whether to create an atomic child or
// a composite child.
//
// struct aos_rig * total_node_rig
// Used to randomly determine the total number of children to create. 
//
// Modification History:
// 02/14/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rvg_xml.h"

#include "alarm_c/alarm.h"
#include "parser/types.h"
#include "parser/xml_node.h"
#include "rvg_c/rsg.h"
#include "rvg_c/rig.h"
#include "util_c/dyn_array.h"
#include "util_c/memory.h"
#include "util2/value.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include <string.h>


int aos_rvg_xml_serialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<RvgXml>
	// 			<tag_name_rig>
	// 			<integer_value_rig>
	// 			<string_value_rig>
	// 			<ip_addr_value_rig>
	// 			<atomic_type_rig>
	// 			<num_child_rig>
	// 			<atomic_child_rig>
	// 			<total_node_rig>
	// 		</RvgXml>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_Xml, -1);
	aos_rvg_xml_t *self = (aos_rvg_xml_t *)rvg;

	aos_xml_node_t *node = parent->mf->append_child_node(parent, "RvgXml");
	aos_assert_r(node, -1);

	// Serialize "tag_name_rsg"
	aos_assert_r(!self->tag_name_rsg->mf->serialize(
				(aos_rvg_t *)self->tag_name_rsg, node), -1);

	// Serialize "integer_value_rig"
	if (self->integer_value_rig)
	{
		aos_assert_r(!self->integer_value_rig->mf->serialize(
				(aos_rvg_t *)self->integer_value_rig, node), -1);
	}

	// Serialize "string_value_rsg"
	if (self->str_value_rsg)
	{
		aos_assert_r(!self->str_value_rsg->mf->serialize(
				(aos_rvg_t *)self->str_value_rsg, node), -1);
	}

	// Serialize "ip_addr_rsg"
	if (self->addr_value_rsg)
	{
		aos_assert_r(!self->addr_value_rsg->mf->serialize(
				(aos_rvg_t *)self->addr_value_rsg, node), -1);
	}

	// Serialize "atomic_type_rig"
	aos_assert_r(!self->atomic_type_rig->mf->serialize(
				(aos_rvg_t *)self->atomic_type_rig, node), -1);

	// Serialize "num_child_rig"
	aos_assert_r(!self->num_child_rig->mf->serialize(
				(aos_rvg_t *)self->num_child_rig, node), -1);

	// Serialize "atomic_child_rig"
	aos_assert_r(!self->atomic_child_rig->mf->serialize(
				(aos_rvg_t *)self->atomic_child_rig, node), -1);

	aos_assert_r(!self->total_node_rig->mf->serialize(
				(aos_rvg_t *)self->total_node_rig, node), -1);
	return 0;
}


int aos_rvg_xml_deserialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *node)
{
	// 
	// 	<RvgXml>
	// 		<tag_name_rsg>			// Mandatory
	// 		<integer_value_rig>		// Optional
	// 		<string_value_rsg>		// Optional
	// 		<ip_addr_value_rsg>		// Optional
	// 		<atomic_type_rig>		// Mandatory
	// 		<num_child_rig>			// Mandatory
	// 		<atomic_child_rig>		// Mandatory
	// 		<total_node_rig>		// Mandatory
	// 	</RvgXml>
	//
	aos_xml_node_t *child;
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_rm(node->mf->match_name((aos_field_t *)node, "RvgXml") == 1, 
			-1, "Name: %s", node->name);
	aos_assert_r(rvg->type == eAosRvg_Xml, -1);
	aos_rvg_xml_t *self = (aos_rvg_xml_t *)rvg;

	// Create "tag_name_rig"
	child = node->mf->first_named_child(node, "tag_name_rsg");
	aos_assert_r(child, -1);
	self->tag_name_rsg = (aos_rsg_t *)aos_rvg_factory_xml(child);
	aos_assert_r(aos_is_rsg_type(self->tag_name_rsg->type) == 1, -1);

	// Create "integer_value_rig"
	child = node->mf->first_named_child(node, "integer_value_rig");
	if (child)
	{
		self->integer_value_rig = (aos_rig_t *)aos_rvg_factory_xml(child);
		aos_assert_r(aos_is_rig_type(self->integer_value_rig->type) == 1, -1);
	}

	// Create "string_value_rsg"
	child = node->mf->first_named_child(node, "string_value_rsg");
	if (child)
	{
		self->str_value_rsg = (aos_rsg_t *)aos_rvg_factory_xml(child);
		aos_assert_r(aos_is_rsg_type(self->str_value_rsg->type) == 1, -1);
	}

	// Create "ip_addr_value_rsg"
	child = node->mf->first_named_child(node, "ip_addr_value_rsg");
	if (child)
	{
		self->addr_value_rsg = (aos_rsg_t *)aos_rvg_factory_xml(child);
		aos_assert_r(aos_is_rsg_type(self->addr_value_rsg->type) == 1, -1); 
	}

	// Create "atomic_type_rig"
	child = node->mf->first_named_child(node, "atomic_type_rig");
	aos_assert_r(child, -1);
	self->atomic_type_rig = (aos_rig_t *)aos_rvg_factory_xml(child);
	aos_assert_r(aos_is_rig_type(self->atomic_type_rig->type) == 1, -1);

	// Create "num_child_rig"
	child = node->mf->first_named_child(node, "num_child_rig");
	aos_assert_r(child, -1);
	self->num_child_rig = (aos_rig_t *)aos_rvg_factory_xml(child);
	aos_assert_r(aos_is_rig_type(self->num_child_rig->type)==1, -1);

	// Create "atomic_child_rig"
	child = node->mf->first_named_child(node, "atomic_child_rig");
	aos_assert_r(child, -1);
	self->atomic_child_rig = (aos_rig_t *)aos_rvg_factory_xml(child);
	aos_assert_r(aos_is_rig_type(self->atomic_child_rig ->type)==1, -1);

	// Create "total_node_rig"
	child = node->mf->first_named_child(node, "total_node_rig");
	aos_assert_r(child, -1);
	self->total_node_rig = (aos_rig_t *)aos_rvg_factory_xml(child);
	aos_assert_r(aos_is_rig_type(self->total_node_rig->type)==1, -1);

	return 0;
}


int aos_rvg_xml_create_node(
		aos_rvg_xml_t *node, 
		aos_dyn_array_t *contents, 
		const char * const name,
		const char * const value, 
		const int value_len, 
		const int level)
{
	aos_assert_r(node, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(name, -1);
	aos_assert_r(value, -1);
	aos_assert_r(level >= 0, -1);

	// 
	// Add the opening tag
	//
	aos_assert_r(!contents->mf->add_char(contents, '\n', 0), -1);
	aos_assert_r(!contents->mf->add_chars(contents, ' ', level * 4, 0), -1);
	aos_assert_r(!contents->mf->add_char(contents, '<', 0), -1);
	aos_assert_r(!contents->mf->add_str(contents, name, strlen(name), 0), -1);
	aos_assert_r(!contents->mf->add_char(contents, '>', 0), -1);

	// Add the value
	aos_assert_r(!contents->mf->add_char(contents, '\n', 0), -1);
	aos_assert_r(!contents->mf->add_chars(contents, ' ', (level + 1)* 4, 0), -1);
	aos_assert_r(!contents->mf->add_str(contents, value, value_len, 0), -1);

	// Add the closing tag
	aos_assert_r(!contents->mf->add_char(contents, '\n', 0), -1);
	aos_assert_r(!contents->mf->add_chars(contents, ' ', level* 4, 0), -1);
	aos_assert_r(!contents->mf->add_char(contents, '<', 0), -1);
	aos_assert_r(!contents->mf->add_char(contents, '/', 0), -1);
	aos_assert_r(!contents->mf->add_str(contents, name, strlen(name), 0), -1);
	aos_assert_r(!contents->mf->add_char(contents, '>', 0), -1);

	return 0;
}


// 
// Description
// It creates an integer xml node
//
int aos_rvg_xml_create_int_node(
		aos_rvg_xml_t *node, 
		aos_dyn_array_t *contents, 
		const int level) 
{
	aos_value_t name_buff, value_buff;
	aos_value_init(&name_buff);
	aos_value_init(&value_buff);
	aos_assert_r(node, -1);
	aos_assert_r(contents, -1);

	aos_rsg_t *name_rsg = node->tag_name_rsg;
	aos_assert_r(name_rsg, -1);
	aos_assert_r(!name_rsg->mf->next_value((aos_rvg_t *)name_rsg, 
			&name_buff), -1);

	aos_rig_t *value_rig = node->integer_value_rig;
	aos_assert_g(value_rig, cleanup);
	aos_assert_g(!value_rig->mf->next_value((aos_rvg_t *)value_rig, 
			&value_buff), cleanup);

	aos_assert_r(!aos_rvg_xml_create_node(node, contents, 
			name_buff.value.string, 
			value_buff.value.string, value_buff.data_size, level), -1);
	return 0;

cleanup:
	name_buff.mf->release_memory(&name_buff);
	value_buff.mf->release_memory(&value_buff);
	return -1;
}


// 
// Description
// It creates a string xml node
//
int aos_rvg_xml_create_str_node(
		aos_rvg_xml_t *node, 
		aos_dyn_array_t *contents, 
		const int level) 
{
	aos_value_t name_buff, value_buff;
	aos_value_init(&name_buff);
	aos_value_init(&value_buff);
	aos_assert_r(node, -1);
	aos_assert_r(contents, -1);

	// Generate the name
	aos_rsg_t *name_rsg = node->tag_name_rsg;
	aos_assert_r(name_rsg, -1);
	aos_assert_r(!name_rsg->mf->next_value((aos_rvg_t *)name_rsg, 
			&name_buff), -1);

	// Generate the value
	aos_rsg_t *value_rsg = node->str_value_rsg;
	aos_assert_r(value_rsg, -1);
	aos_assert_g(!value_rsg->mf->next_value((aos_rvg_t *)value_rsg, 
			&value_buff), cleanup);

	aos_assert_g(!aos_rvg_xml_create_node(node, contents, 
			name_buff.value.string, 
			value_buff.value.string, value_buff.data_size, level), cleanup);

	name_buff.mf->release_memory(&name_buff);
	value_buff.mf->release_memory(&value_buff);
	return 0;

cleanup:
	name_buff.mf->release_memory(&name_buff);
	value_buff.mf->release_memory(&value_buff);
	return -1;
}


// 
// Description
// It creates an IP address xml node
//
int aos_rvg_xml_create_addr_node(
		aos_rvg_xml_t *node, 
		aos_dyn_array_t *contents, 
		const int level) 
{
	aos_value_t name_buff, value_buff;
	aos_value_init(&name_buff);
	aos_value_init(&value_buff);
	aos_assert_r(node, -1);
	aos_assert_r(contents, -1);

	// Generate the name
	aos_rsg_t *name_rsg = node->tag_name_rsg;
	aos_assert_r(name_rsg, -1);
	aos_assert_r(!name_rsg->mf->next_value((aos_rvg_t *)name_rsg, 
			&name_buff), -1);

	// Generate the value
	aos_rsg_t *value_rsg = node->addr_value_rsg;
	aos_assert_r(value_rsg, -1);
	aos_assert_g(!value_rsg->mf->next_value((aos_rvg_t *)value_rsg, 
			&value_buff), cleanup);

	aos_assert_r(!aos_rvg_xml_create_node(node, contents, 
			name_buff.value.string, 
			value_buff.value.string, value_buff.data_size, level), -1);

	name_buff.mf->release_memory(&name_buff);
	value_buff.mf->release_memory(&value_buff);
	return 0;

cleanup:
	name_buff.mf->release_memory(&name_buff);
	value_buff.mf->release_memory(&value_buff);
	return -1;
}


// 
// Description 
// This function creates an atomic xml node. 
//
int aos_rvg_xml_create_atomic(
		aos_rvg_xml_t *node, 
		aos_dyn_array_t *contents, 
		const int level)
{
	aos_assert_r(node, -1);
	aos_assert_r(contents, -1);
	aos_rig_t *type_rig = node->atomic_type_rig;
	aos_assert_r(type_rig, -1);

	int type;
	aos_value_t value;
	aos_assert_r(!aos_value_init(&value), -1);
	aos_assert_r(!type_rig->mf->next_value((aos_rvg_t *)type_rig, &value), -1);
	aos_assert_r(!value.mf->to_int(&value, &type), -1);

	aos_assert_r(aos_xml_node_type_check(type) == 1, -1);
	switch (type)
	{
	case eAosXmlNodeType_Int:
		 return aos_rvg_xml_create_int_node(node, contents, level);
		
	case eAosXmlNodeType_Str:
		 return aos_rvg_xml_create_str_node(node, contents, level);

	case eAosXmlNodeType_IpAddr:
		 return aos_rvg_xml_create_addr_node(node, contents, level);

	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


// 
// Description
// It creates a compound xml node. A compound node contains a 
// number of child nodes. 
//
int aos_rvg_xml_create_comp(
		aos_rvg_xml_t *def, 
		aos_dyn_array_t *contents, 
		int *crt_num, 
		const int total_nodes, 
		const int level)
{
	int i;

	aos_value_t name_buff;
	aos_value_init(&name_buff);
	aos_assert_r(def, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(def->type == eAosRvg_Xml, -1);
	aos_assert_r(crt_num, -1);
	aos_rvg_xml_t *self = (aos_rvg_xml_t *)def;

	// Generate the name
	aos_rsg_t *name_rsg = self->tag_name_rsg;
	aos_assert_r(name_rsg, -1);
	aos_assert_r(!name_rsg->mf->next_value((aos_rvg_t *)name_rsg, 
			&name_buff), -1);

	// Add the name tag
	aos_assert_g(!contents->mf->add_char(contents, '\n', 0), cleanup);
	aos_assert_g(!contents->mf->add_chars(contents, ' ', 
			level * 4, 0), cleanup);
	aos_assert_g(!contents->mf->add_element(contents, 
			name_buff.data_size + 2, 0), cleanup);
	contents->buffer[contents->noe++] = '<';
	strcpy(&contents->buffer[contents->noe], name_buff.value.string);
	contents->noe += name_buff.data_size;
	contents->buffer[contents->noe++] = '>';

	// Generate the children
	aos_rig_t *atomic_rig = self->atomic_child_rig;
	aos_assert_g(atomic_rig, cleanup);
	int num_child;
    aos_assert_g(!self->num_child_rig->mf->next_int(
			self->num_child_rig, &num_child), cleanup);
	aos_assert_g(num_child > 0, cleanup);
	*crt_num += num_child;

	for (i=0; i<num_child; i++)
	{
		int atomic;
	    aos_assert_g(!atomic_rig->mf->next_int(
				atomic_rig, &atomic), cleanup);
		aos_assert_g(atomic >= 0, cleanup);
		if (atomic == 0 || *crt_num >= total_nodes)
		{
			aos_assert_g(!aos_rvg_xml_create_atomic(
					self, contents, level+1), cleanup);
		}
		else
		{
			aos_assert_g(!aos_rvg_xml_create_comp( self, contents, 
					crt_num, total_nodes, level+1), cleanup);
		}
	}

	// Add the closing tag
	aos_assert_g(!contents->mf->add_char(contents, '\n', 0), cleanup);
	aos_assert_g(!contents->mf->add_chars(
			contents, ' ', level * 4, 0), cleanup);
	aos_assert_g(!contents->mf->add_element(contents, 
			name_buff.data_size + 3, 0), cleanup);
	contents->buffer[contents->noe++] = '<';
	contents->buffer[contents->noe++] = '/';
	strcpy(&contents->buffer[contents->noe], name_buff.value.string);
	contents->noe += name_buff.data_size;
	contents->buffer[contents->noe++] = '>';

	name_buff.mf->release_memory(&name_buff);
	return 0;

cleanup:
	name_buff.mf->release_memory(&name_buff);
	return -1;
}


static int aos_rvg_xml_next_value_bf(
		aos_rvg_t *rvg, 
		aos_buffer_t *buffer)
{
	int i;
	aos_dyn_array_t contents;
	aos_dyn_array_init(&contents, 0, 1, 10000, 100000000);

	aos_assert_r(rvg, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(rvg->type == eAosRvg_Xml, -1);
	aos_rvg_xml_t *self = (aos_rvg_xml_t *)rvg;

	aos_rig_t *total_node_rig = self->total_node_rig;
	aos_rig_t *child_rig = self->num_child_rig;
	aos_rig_t *atomic_rig = self->atomic_child_rig;
	aos_assert_r(total_node_rig, -1);
	aos_assert_r(child_rig, -1);
	aos_assert_r(atomic_rig, -1);

	int total_nodes;
    aos_assert_r(!total_node_rig->mf->next_int(
			total_node_rig, &total_nodes), -1);
	aos_assert_r(total_nodes > 0, -1);

	int crt_num = 0;
	int level = 0;
	while (crt_num < total_nodes)
	{
		int num_child;
	    aos_assert_r(!child_rig->mf->next_int(child_rig, &num_child), -1);
		crt_num += num_child;
		aos_assert_r(num_child > 0, -1);

		// 
		// Generate children
		//
		for (i=0; i<num_child; i++)
		{
			int atomic;
		    aos_assert_r(!atomic_rig->mf->next_int(
					atomic_rig, &atomic), -1);
			aos_assert_r(atomic >= 0, -1);
			if (atomic == 0)
			{
				aos_assert_g(!aos_rvg_xml_create_atomic(self, 
						&contents, level), cleanup);
			}
			else
			{
				aos_assert_g(!aos_rvg_xml_create_comp(self, 
						&contents, &crt_num, total_nodes, level), 
						cleanup);
			}
		}
	}

	aos_assert_r(!buffer->mf->append_str(buffer, contents.buffer), -1);
	aos_assert_r(!contents.mf->release_memory(&contents), -1);
	return 0;

cleanup:
	aos_assert_r(!contents.mf->release_memory(&contents), -1);
	return -1;
}


// 
// Description
// This function generates an XML document. 
//
static int aos_rvg_xml_next_value(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_buffer_t buffer; 
	aos_buffer_init(&buffer);
	aos_assert_r(!aos_rvg_xml_next_value_bf(rvg, &buffer), -1);

	aos_assert_g(!value->mf->set_str(value, buffer.buff, 
				buffer.data_len), cleanup);
	buffer.mf->release_memory(&buffer);
	return 0;

cleanup:
	buffer.mf->release_memory(&buffer);
	return -1;
}

int aos_rvg_xml_integrity_check(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


static int aos_rvg_xml_release_memory(aos_rvg_t *rvg)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(rvg->type == eAosRvg_Xml, -1);
	aos_rvg_xml_t *self = (aos_rvg_xml_t *)rvg;

	aos_assert_r(!aos_rvg_release_memory(rvg), -1);

	if (self->tag_name_rsg) 
	{
		self->tag_name_rsg->mf->put((aos_rvg_t *)self->tag_name_rsg);
		self->tag_name_rsg = 0;
	}

	if (self->integer_value_rig) 
	{
		self->integer_value_rig->mf->put((aos_rvg_t *)self->integer_value_rig);
		self->integer_value_rig = 0;
	}

	if (self->str_value_rsg) 
	{
		self->str_value_rsg->mf->put((aos_rvg_t *)self->str_value_rsg);
		self->str_value_rsg = 0;
	}

	if (self->addr_value_rsg) 
	{
		self->addr_value_rsg->mf->put((aos_rvg_t *)self->addr_value_rsg);
		self->addr_value_rsg = 0;
	}

	if (self->atomic_type_rig)
	{
		self->atomic_type_rig->mf->put((aos_rvg_t *)self->atomic_type_rig);
		self->atomic_type_rig = 0;
	}

	if (self->num_child_rig)
	{
		self->num_child_rig->mf->put((aos_rvg_t *)self->num_child_rig);
		self->num_child_rig = 0;
	}

	if (self->atomic_child_rig)
	{
		self->atomic_child_rig->mf->put((aos_rvg_t *)self->atomic_child_rig);
		self->atomic_child_rig = 0;
	}

	if (self->total_node_rig)
	{
		self->total_node_rig->mf->put((aos_rvg_t *)self->total_node_rig);
		self->total_node_rig = 0;
	}

	return 0;
}


static int aos_rvg_xml_destroy(aos_rvg_t *rvg)
{
	aos_assert_r(!aos_rvg_xml_release_memory(rvg), -1);
	aos_free(rvg);
	return 0;
}


static int aos_rvg_xml_domain_check(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_alarm("Should not call this function");
	return -1;
}


static aos_rvg_xml_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rvg_xml_integrity_check,
	aos_rvg_xml_next_value,
	aos_rvg_xml_domain_check,
	aos_rvg_xml_serialize,
	aos_rvg_xml_deserialize,
	aos_rvg_xml_release_memory,
	aos_rvg_xml_destroy,
};


int aos_rvg_xml_init(aos_rvg_xml_t *obj)
{
	aos_assert_r(obj, -1);
	memset(obj, 0, sizeof(aos_rvg_xml_t));
	obj->mf = &sg_mf;
	obj->type = eAosRvg_Xml;
	return 0;
}


aos_rvg_xml_t *aos_rvg_xml_create()
{
	aos_rvg_xml_t *obj = aos_malloc(sizeof(aos_rvg_xml_t));
	aos_assert_g(!aos_rvg_init((aos_rvg_t *)obj), cleanup);
	return obj;

cleanup:
	if (obj) aos_free(obj);
	return 0;
}


aos_rvg_xml_t *aos_rvg_xml_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_rvg_xml_t *obj = aos_rvg_xml_create();
	aos_assert_g(!aos_rvg_xml_init(obj), cleanup);
	
	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	if (obj) aos_free(obj);
	return 0;
}


