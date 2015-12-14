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
// XML node is a subclass of aos_field. It represents an XML node. 
// There is a virtual node called 'root node', which is the parent
// node of first level XML nodes. The label for the root node is
// "__aos_root__". 
//
// An XML node can be either atomic or composite, which can contain 
// children. Childern can themselves be either atomic or 
// composite. 
//
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/xml_node.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/scm_field.h"
#include "parser/scm_field_xml.h"
#include "parser/field.h"
#include "util_c/rc.h"
#include "util_c/tracer.h"
#include "util_c/memory.h"
#include "util_c/buffer.h"
#include "util_c/strutil.h"
#include "util_c/rc_obj.h"
#include "util2/value.h"
#include "util2/global_data.h"


#define AOS_XML_ROOT_NODE_NAME "__root__"

// 
// Description:
// It parses a field from the position 'startpos'. 
//
// Returns:
// 0 if success
// 1 if incomplete
// -1 if errors
//
int aos_xml_node_parse(aos_field_t *field) 
{
	char name[AOS_XML_LABEL_MAX_LEN+1];
	int name_len = AOS_XML_LABEL_MAX_LEN;
	aos_attr1_t **attr = 0;
	int start, len, ret;
	aos_xml_node_t *child = 0;

	aos_assert_r(field, -1);

	if (field->with_schema)
	{
		aos_assert_r(field->schema, -1);
		return field->schema->mf->parse(field->schema, field);
	}

	// 
	// Will parse without using a schema.
	//
	aos_omparser_t parser;
	aos_assert_r(!aos_omparser_init(&parser, field->contents, field->content_len), -1);
	start = 0; 
	int num_attr = 0;
	ret = parser.mf->next_tag(&parser, name, name_len, &start, 
			&len, &attr, &num_attr);
	int child_found = 0;
	while (!ret)
	{
		child_found++;
		child = aos_xml_node_create2(name, start, len, 0, &parser);
		aos_assert_r(child, -1);
		child->mf->set_attr((aos_field_t *)child, attr, num_attr);
		num_attr = 0;
		attr = 0;
		aos_assert_g(!field->mf->add_child(field, (aos_field_t*)child), 
				cleanup);
		child->mf->put((aos_field_t *)child);
		child = 0;
		ret = parser.mf->next_sibling(&parser, name, name_len, 
				&start, &len, &attr, &num_attr);
	}

	aos_assert_g(ret > 0, cleanup);

	if (!child_found)
	{
		// 
		// This is an atomic field. 
		//
		field->value = aos_value_factory(field->data_type, 
				field->contents, 
				field->content_len);
		aos_assert_r(field->value, -1);
	}

	return 0;

cleanup:
	if (child) child->mf->put((aos_field_t *)child);
	return 0;
}

	
// 
// Description
// It retrieves the first named child of the current node.
//
// Returns
// The child if found. Otherwise, 0.
//
aos_xml_node_t * aos_xml_node_first_named_child(
		struct aos_xml_node *node, 
		const char * const name) 
{
	int i;
	aos_field_t *child;
	aos_assert_r(node, 0);
	aos_assert_r(name, 0);
	aos_assert_r(strlen(name) > 0, 0);

	if (!node->parsed)
	{
		aos_assert_r(!node->mf->parse(
				(aos_field_t *)node), 0);
		node->parsed = 1;
	}

	if (node->fields_array.noe <= 0) return 0;

	aos_assert_r(node->fields, 0);
	for (i=0; i<node->fields_array.noe; i++)
	{
		child = node->fields[i];
		if (child->mf->match_name(child, name) == 1)
		{
			node->crt_child = i;
			aos_assert_r(child->type == eAosFieldType_Xml ||
						 child->type == eAosFieldType_XmlMsg, 0);
			child->mf->hold((aos_field_t *)child);
			return (aos_xml_node_t *)child;
		}
	}

	// 
	// Did not find the child
	//
	node->crt_child = -1;
	return 0; 
}


// 
// Description
// It retrieves the next named child of the current node.
//
// Returns
// The child if found. Otherwise, 0.
//
aos_xml_node_t * aos_xml_node_next_named_child(
		struct aos_xml_node *node, 
		const char * const name, 
		aos_omparser_t *parser)
{
	int i;
	aos_field_t *child = 0;

	aos_assert_r(node, 0);
	aos_assert_r(name, 0);
	aos_assert_r(strlen(name) > 0, 0);
	aos_assert_r(node->parsed, 0);
	aos_assert_r(node->fields_array.noe > 0, 0);
	aos_assert_r(node->crt_child >= 0, 0);
	aos_assert_r(node->crt_child < node->fields_array.noe, 0);
	aos_assert_r(node->fields, 0);

	for (i=node->crt_child+1; i<node->fields_array.noe; i++)
	{
		child = node->fields[i];
		if (child->mf->match_name(child, name) == 1)
		{
			node->crt_child = i;
			aos_assert_r(child->type == eAosFieldType_Xml ||
						 child->type == eAosFieldType_XmlMsg, 0);
			child->mf->hold((aos_field_t *)child);
			return (aos_xml_node_t *)child;
		}
	}

	// 
	// Did not find the child
	//
	node->crt_child = -1;
	return 0; 
}


aos_xml_node_t * aos_xml_node_next_sibling(
		struct aos_xml_node *node) 
{
	aos_assert_r(node, 0);
	aos_assert_r(node->crt_child >= 0, 0);
	aos_assert_r(node->crt_child < node->fields_array.noe, 0);
	aos_assert_r(node->parsed, 0);

	if (node->crt_child == node->fields_array.noe -1)
	{
		// No more sibling
		return 0;
	}

	node->crt_child++;
	aos_field_t *sibling = node->fields[node->crt_child];
	aos_assert_r(sibling->type == eAosFieldType_Xml ||
				 sibling->type == eAosFieldType_XmlMsg, 0);
	sibling->mf->hold((aos_field_t *)sibling);
	return (aos_xml_node_t *)sibling;
}


// 
// Description
// It deletes the first named child. If found, the child is deleted
// and 0 is returned. Otherwise, -eAosRc_NotFound is returned. 
// If errors, -eAosRc_Error is returned. 
//
int aos_xml_node_del_first_named_child(
		aos_xml_node_t *node, 
		const char * const label) 
{
	int i;
	aos_field_t *child = 0;

	aos_assert_r(node, -1);
	aos_assert_r(label, -1);
	aos_assert_r(strlen(label) > 0, -1);
	aos_assert_r(node->fields, -1);

	if (!node->parsed)
	{
		aos_assert_r(!node->mf->parse((aos_field_t *)node), -1);
	}

	for (i=0; i<node->fields_array.noe; i++)
	{
		child = node->fields[i];
		if (child->mf->match_name(child, label) == 1)
		{
			// 
			// Found
			//
			node->fields[i]->mf->put(node->fields[i]);
			aos_assert_r(!node->fields_array.mf->del_element(
					&node->fields_array, i, 1, 
					(char **)&node->fields), -1);
			node->crt_child = i;
			return 0;
		}
	}

	// 
	// Did not find the child
	//
	return -1;
}


// Description
// It dumpts the node into a string. The string is a temperary 
// string. The caller should use it immediately and should not
// modify the string. 
//
const char * aos_xml_node_dump(aos_field_t *node)
{
	int buff_len, len, index;
	char * buff = aos_get_global_str1(&buff_len);
	aos_assert_r(buff, 0);
	aos_assert_r(node, 0);

	// Dump the aos_field_t portion
	aos_field_t *field = (aos_field_t *)(node);
	aos_assert_r(field, 0);
	len = buff_len;
	aos_assert_r(!aos_field_dump_to(field, buff, &len), 0);
	if (len >= buff_len) return buff;
	
	// Dump itself
	index = len;
	aos_buff_append_line_int(buff, buff_len, 
			&index, "Noe: ", node->fields_array.noe);
	return buff;
}


// 
// Description
// It deletes an xml_node instance.
//
int aos_xml_node_destroy(aos_field_t *node)
{
	aos_assert_r(node->type == eAosFieldType_Xml, -1);
	aos_assert_r(!aos_field_release_memory(node), -1);

	aos_xml_node_t *self = (aos_xml_node_t *)node;
	if (self->parent) self->parent->mf->put((aos_field_t *)self->parent);
	aos_free(node);
	return 0;
}


int aos_xml_node_serialize(aos_field_t *msg, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<XmlMessage>
	// 			<aos_msg portion>
	// 			...
	// 		</XmlMessage>
	// 	</Parent>
	//
	// 	'parent' points to the parent. 
	//
	aos_assert_r(msg, -1);
	aos_assert_r(msg->type == eAosMsgType_Xml, -eAosRc_Error);
	aos_assert_r(parent, -1);

	aos_not_implemented_yet;
	return 0;
}


int aos_xml_node_dump_to(
		struct aos_field *field, 
		char *buff, 
		int *len)
{
	aos_assert_r(field, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(len, -1);
	aos_assert_r(field->type == eAosFieldType_Xml, -1);
	aos_xml_node_t *self = (aos_xml_node_t *)field;

	int buff_len = *len;
	aos_assert_r(!aos_field_dump_to(field, buff, len), -1);
	if (*len >= buff_len) return 0;
	
	// Dump itself
	int index = *len;
	aos_buff_append_line_int(&buff[index], buff_len, &index, 
			"Crt Child: ", self->crt_child);

	*len = index;
	return 0;
}


int aos_xml_node_deserialize(
		aos_field_t *msg, 
		aos_xml_node_t *node)
{
	// 
	// 	<XmlMessage>
	// 		<aos_msg portion>
	// 		...
	// 	</XmlMessage>
	//
	aos_assert_r(msg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(msg->type == eAosMsgType_Xml, -eAosRc_Error);
	// aos_xml_node_t *self = (aos_xml_node_t *)msg;

	aos_not_implemented_yet;
	return 0;
}


// 
// Description
// This function creates an instance of aos_xml_node and
// append the node to this node. It then returns the newly
// created node. Note that the returned node is held. If the
// caller does not want to keep the object, it should put
// the object.
//
// If error, null is returned.
//
aos_xml_node_t * aos_xml_node_append_child_node(
		struct aos_xml_node *node, 
		const char * const label)
{
	aos_assert_r(node, 0);
	aos_assert_r(label, 0);

	aos_xml_node_t *nn = aos_xml_node_create3(label, 0, 0, 0);
	aos_assert_r(nn, 0);
	aos_assert_g(!node->mf->add_child((aos_field_t *)node, 
				(aos_field_t *)nn), cleanup);
	return nn;

cleanup:
	nn->mf->put((aos_field_t *)nn);
	return 0;
}


int aos_xml_node_append_child_int(
		struct aos_xml_node *node, 
		const char * const label,
		const int value)
{
	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *vv = aos_value_factory(eAosDataType_int32, 
			(void *)&value, sizeof(int));
	aos_assert_r(vv, -1);
	aos_rc_obj_assign(child->value, vv);
	return 0;
}


int aos_xml_node_append_child_addr(
		struct aos_xml_node *node, 
		const char * const label,
		const u32 value)
{
	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *vv = aos_value_factory(eAosDataType_u32, 
			(void *)&value, sizeof(u32));
	aos_assert_r(vv, -1);
	aos_rc_obj_assign(child->value, vv);
	child->mf->put((aos_field_t *)child);
	return 0;
}


int aos_xml_node_append_child_value(
		struct aos_xml_node *node, 
		const char * const label,
		aos_value_t *value)
{
	aos_assert_r(node, -1);
	aos_assert_r(label, -1);
	aos_assert_r(value, -1);

	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *vv = value->mf->clone(value);
	aos_assert_r(vv, -1);
	aos_rc_obj_assign(child->value, vv);
	child->mf->put((aos_field_t *)child);
	return 0;
}


int aos_xml_node_append_child_hex(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len)
{
	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *value = aos_value_factory(eAosDataType_string, 
			(void *)contents, len);
	aos_assert_r(value, -1);
	aos_rc_obj_assign(child->value, value);
	child->mf->put((aos_field_t *)child);
	return 0;
}


int aos_xml_node_append_child_bin(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len)
{
	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *value = aos_value_factory(eAosDataType_string, 
			(void *)contents, len);
	aos_assert_r(value, -1);
	aos_rc_obj_assign(child->value, value);
	child->mf->put((aos_field_t *)child);
	return 0;
}


int aos_xml_node_append_child_str(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents, 
		aos_attr1_t **attrs, 
		const int num_attrs)
{
	aos_xml_node_t *child = node->mf->append_child_node(node, label);
	aos_assert_r(child, -1);

	aos_value_t *value = aos_value_factory(eAosDataType_string, 
			(void *)contents, strlen(contents));
	aos_assert_g(value, cleanup);
	aos_rc_obj_assign(child->value, value);
	aos_assert_g(!child->mf->set_attr(
				(aos_field_t *)child, attrs, num_attrs), cleanup);
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_int(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value)
{
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	aos_assert_r(child, -1);

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(aos_data_type_is_int_type(child->type), cleanup);
	aos_assert_g(child->value, cleanup);
	aos_assert_g(!child->value->mf->to_int(child->value, value), cleanup);
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_u32(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value)
{
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) return -1;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(aos_data_type_is_int_type(child->type), cleanup);
	aos_assert_g(child->value, cleanup);
	aos_assert_g(!child->value->mf->to_u32(child->value, value), 
			cleanup);
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_int_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value, 
		const int dft_value)
{
	*value = dft_value;

	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) return 0;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(aos_data_type_is_int_type(child->type), cleanup);
	aos_assert_g(child->value, cleanup);
	if (!child->value->mf->to_int(child->value, value)) 
	{
		child->mf->put((aos_field_t *)child);
		return 0;
	}

	*value = dft_value;
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_addr_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value, 
		const u32 dft_value)
{
	*value = dft_value;

	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child)
	{
		*value = dft_value;
		return 0;
	}

	aos_assert_gm(child->data_type == eAosDataType_ip_addr, cleanup, 
			"Contents: %s", child->contents);

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(child->fields_array.noe == 0, cleanup);
	aos_assert_g(child->value, cleanup);

	if (!child->value->mf->to_u32(child->value, value)) 
	{
		child->mf->put((aos_field_t *)child);
		return 0;
	}

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_char(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value)
{
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	aos_assert_r(child, -1);

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(child->data_type == eAosDataType_char, cleanup);
	aos_assert_g(child->value, cleanup);
	aos_assert_g(!child->value->mf->to_char(child->value, value), cleanup);

	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_char_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value, 
		const char dft_value)
{
	*value = dft_value;

	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) return 0;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(child->data_type == eAosDataType_char, cleanup);
	aos_assert_g(child->value, cleanup);
	if (!child->value->mf->to_char(child->value, value)) 
	{
		child->mf->put((aos_field_t *)child);
		return 0;
	}

	child->mf->put((aos_field_t *)child);
	*value = dft_value;
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_hex(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len)
{
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	aos_assert_r(child, -1);

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(child->data_type == eAosDataType_binary, cleanup);
	aos_assert_g(child->value, cleanup);
	aos_assert_g(!child->value->mf->to_str(child->value, contents, 
			len), cleanup);

	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_hex_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value, 
		const int dft_len)
{
	aos_assert_g(node, dft_return);
	aos_assert_g(name, dft_return);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) goto dft_return;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), dft_return);
		child->parsed = 1;
	}

	aos_assert_g(child->data_type == eAosDataType_binary, dft_return);
	aos_assert_g(child->value, dft_return);
	if (!child->value->mf->to_str(child->value, contents, len)) 
	{
		child->mf->put((aos_field_t *)child);
		return 0;
	}

dft_return:
	if (child) child->mf->put((aos_field_t *)child);
	aos_assert_r(!aos_str_set(contents, dft_value, dft_len), -1);
	return 0;
}


// 
// Description
// It retrieves the first child named 'name'. If found, it retrieves
// the child's value as string. If 'child_found' is not null, the
// child is returned through the variable. Note that the variable
// will hold the child. The caller should put the child when not
// needed.
//
// IMPORTANT: if contents are retrieved, the function will allocate
// memory for it. The caller should free the memory when it is not
// needed. Otherwise, it will cause memory leaks. It is guaranteed
// that either the function did not find the contents, in which case
// 'content' is set to null. Otherwise, 'content' is not null.
//
int aos_xml_node_first_child_str(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		aos_xml_node_t **child_found)
{
	char *buff;
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);
	aos_assert_r(contents, -1);

	*contents = 0;

	if (child_found) *child_found = 0;
	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	aos_assert_rm(child, -1, "name: %s", name);

	if (child_found) 
	{
		*child_found = child;
		child->mf->hold((aos_field_t *)child);
	}

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	if (child->value)
	{
		aos_assert_g(!child->value->mf->to_str(
				child->value, contents, len), cleanup);
		child->mf->put((aos_field_t *)child);
		return 0;
	}

	aos_assert_g(child->content_len >= 0, cleanup);
	buff = aos_malloc(child->content_len+1);
	strncpy(buff, child->contents, child->content_len);
	buff[child->content_len] = 0;
	*contents = buff;
	*len = child->content_len;
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return 0;
}


// 
// Description:
// This function is similar to the "_str" version except that it does 
// not allocate memory. The caller should ahve allocated memory and
// the allocated memory size is 'contents_len'. If the actual contents
// are bigger than 'contents_len', errors are reported and the 
// operation is aborted. 
//
int aos_xml_node_first_child_str_b(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *contents_len, 
		aos_xml_node_t **child_found)
{
	char *buff;
	int buff_len;

	aos_assert_r(node, -1);
	aos_assert_r(name, -1);
	aos_assert_r(contents_len, -1);

	if (child_found) *child_found = 0;

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	aos_assert_rm(child, -1, "Child: %s", name);

	if (child_found) 
	{
		*child_found = child;
		child->mf->hold((aos_field_t *)child);
	}

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), cleanup);
		child->parsed = 1;
	}

	aos_assert_g(child->value, cleanup);
	aos_assert_g(!child->value->mf->to_str(child->value, 
			&buff, &buff_len), cleanup);
	aos_assert_g(buff_len < *contents_len, cleanup);
	strcpy(contents, buff);
	*contents_len = buff_len;
	child->mf->put((aos_field_t *)child);
	return 0;

cleanup:
	child->mf->put((aos_field_t *)child);
	return -1;
}


int aos_xml_node_first_child_str_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value)
{
	aos_assert_r(node, -1);
	aos_assert_r(name, -1);
	aos_assert_r(contents, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) goto dft_return;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), dft_return);
		child->parsed = 1;
	}

	aos_assert_g(child->value, dft_return);
	if (!child->value->mf->to_str(child->value, contents, len)) 
	{
		child->mf->put((aos_field_t *)child);
		return 0;
	}

dft_return:
	if (dft_value)
	{
		aos_assert_r(!aos_str_set(contents, dft_value, 
				strlen(dft_value)), -1);
	}
	else
	{
		*contents = 0;
		if (len) *len = 0;
	}

	if (child) child->mf->put((aos_field_t *)child);
	return 0;
}


int aos_xml_node_first_child_str_b_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *len, 
		const char * const dft_value)
{
	char *buff;
	int buff_len;
	aos_assert_g(node, dft_return);
	aos_assert_g(name, dft_return);

	aos_assert_r(dft_value, -1);

	aos_xml_node_t *child = node->mf->first_named_child(node, name);
	if (!child) goto dft_return;

	if (!child->parsed)
	{
		aos_assert_g(!child->mf->parse((aos_field_t *)child), dft_return);
		child->parsed = 1;
	}

	aos_assert_g(child->value, dft_return);
	if (!child->value->mf->to_str(child->value, &buff, &buff_len)) 
	{
		aos_assert_r(buff_len < *len, -1);
		strcpy(contents, buff);
		*len = buff_len;
		child->mf->put((aos_field_t *)child);
		return 0;
	}

dft_return:
	if (child) child->mf->put((aos_field_t *)child);
	aos_assert_r(*len > strlen(dft_value), -1);
	strcpy(contents, dft_value);
	*len = strlen(dft_value);
	return 0;
}


int aos_xml_node_integrity_check(struct aos_field *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(!aos_field_integrity_check(field), -1);
	aos_assert_r(field->type == eAosFieldType_Xml, -1);
	aos_xml_node_t *self = (aos_xml_node_t *)field;
	aos_assert_r(self->crt_child == -1 || 
				 self->crt_child >= 0, -1);
	return 0;
}


// 
// Description
// It retrieves the node's first child. The node knows where it 
// starts (node->start_pos). It uses this to set the parser. 
// After that, it just uses the parser to retrieve the first
// tag. 
//
aos_xml_node_t * aos_xml_node_first_child(
		struct aos_xml_node *node)
{
	aos_assert_r(node, 0);
	if (!node->parsed)
	{
		aos_assert_r(!node->mf->parse((aos_field_t *)node), 0);
		node->parsed = 1;
	}

	if (node->fields_array.noe <= 0)
	{
		// There are no children for this node.
		return 0;
	}

	aos_assert_r(node->fields, 0);
	aos_assert_r(node->fields[0], 0);
	aos_assert_r(node->fields[0]->type == eAosFieldType_Xml, 0);
	node->crt_child = 0;
	node->fields[0]->mf->hold(node->fields[0]);
	return (aos_xml_node_t *)node->fields[0];
}


/*
int aos_xml_node_get_str_b(
		struct aos_xml_node *node, 
		char *buff, 
		int *buff_len) 
{
	aos_assert_r(node, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(buff_len && *buff_len >= node->len+1, -1);
	aos_assert_r(node->fields_array.noe == 0, -1);
	aos_assert_r(node->parser, -1);
	aos_assert_r(node->content_len > 0, -1);

	strncpy(buff, &node->parser->buffer[node->start_pos], 
			node->len);
	*buff_len = node->len;
	buff[node->len] = 0;
	return 0;
}
*/

	
int aos_xml_node_get_value(
		aos_field_t *node, 
		aos_value_t *value)
{
	aos_assert_r(node, -1);
	aos_assert_r(value, -1);
	aos_assert_r(node->fields_array.noe == 0, -1);
	aos_assert_r(node->content_len > 0, -1);
	aos_assert_r(aos_data_type_check(node->data_type) == 1, -1);

	aos_assert_r(!value->mf->set_value(value, node->data_type, 
			node->contents, node->content_len), -1);
	return 0;
}
	

static aos_xml_node_mf_t sg_mf = 
{
	AOS_FIELD_MEMFUNC_INIT,

	aos_xml_node_dump_to,
	aos_xml_node_dump, 
	aos_xml_node_destroy, 
	aos_xml_node_serialize, 
	aos_xml_node_deserialize,
	aos_xml_node_integrity_check, 
	aos_xml_node_parse,
	aos_xml_node_get_value,

	aos_xml_node_first_child,
	aos_xml_node_first_named_child,
	aos_xml_node_next_named_child,
	aos_xml_node_next_sibling,
	aos_xml_node_del_first_named_child,
	aos_xml_node_first_child_int,
	aos_xml_node_first_child_u32,
	aos_xml_node_first_child_int_dft,
	aos_xml_node_first_child_char,
	aos_xml_node_first_child_char_dft,
	aos_xml_node_first_child_str,
	aos_xml_node_first_child_str_b,
	aos_xml_node_first_child_str_dft,
	aos_xml_node_first_child_str_b_dft,
	aos_xml_node_first_child_hex,
	aos_xml_node_first_child_hex_dft,
	aos_xml_node_first_child_addr_dft,
	aos_xml_node_append_child_int,
	aos_xml_node_append_child_hex,
	aos_xml_node_append_child_str,
	aos_xml_node_append_child_bin,
	aos_xml_node_append_child_node,
	aos_xml_node_append_child_addr,
	aos_xml_node_append_child_value
};


int aos_xml_node_reset(aos_field_t *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(!aos_field_reset(field), -1);
	aos_assert_r(field->type == eAosFieldType_Xml ||
				 field->type == eAosFieldType_XmlMsg, -1);
	aos_xml_node_t *self = (aos_xml_node_t *)field;
	if (self->parent)
	{
		self->parent->mf->put((aos_field_t *)self->parent);
		self->parent = 0;
	}

	self->crt_child = -1;
	return 0;
}


int aos_xml_node_init(
		aos_xml_node_t *node,
		const aos_field_type_e type,
		const char * const name, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(node, -1);
	memset(node, 0, sizeof(aos_xml_node_t));
	node->mf = &sg_mf;
	node->type = type;
	aos_assert_r(type == eAosFieldType_Xml ||
				 type == eAosFieldType_XmlMsg, -1);

	// Set aos_field member data
	aos_assert_r(!aos_field_init((aos_field_t *)node, 
			type, name, eAosDataType_NoDataType, contents, len), -1);

	// Set aos_xml_node_t member data
	if (node->parent)
	{
		node->parent->mf->put((aos_field_t *)node->parent);
		node->parent = 0;
	}

	node->crt_child = -1;	
	return 0;
}

/*
// 
// Description
// It creates an xml node from a file. This function does not
// parse the message. Message parsing is done as needed. 
//
// !!!!!!!!!!!!IMPORTANT!!!!!!!!!!!
// The caller should call aos_msg_hold(msg) to hold the message created. 
// Otherwise, it can lead to memory corruption.
//
// Parameters:
// fname(IN):	the filename. 
//
// Returns
// If successful, it returns the message created. Otherwise, it 
// will raise alarms and return 0.
//
aos_xml_node_t * 
aos_xml_node_create_from_file(const char * const fname, const int with_root)
{
	char name[AOS_XML_LABEL_MAX_LEN+1];
	int name_len = AOS_XML_LABEL_MAX_LEN;
	aos_attr1_t *attr = 0;
	int num_attr = 0;
	int start, len, ret;
	aos_field_t *child = 0;

	aos_omparser_t *parser = 0;
	aos_assert_r(fname, 0);
	parser = aos_omparser_create_from_file(fname);
	aos_assert_r(parser, 0);

	aos_xml_node_t *node = aos_xml_node_create2(
			AOS_XML_ROOT_NODE_NAME, 
			0, parser->data_len, 0, parser);
	aos_assert_g(node, cleanup);
	node->is_root = 1;
	node->parsed = 1;

	// 
	// Parse all the first level xml tags
	//
	ret = parser->mf->next_tag(parser, name, name_len, &start, 
			&len, &attr, &num_attr);
	while (!ret)
	{
		child = (aos_field_t *)aos_xml_node_create2(name, start, len, 0, parser);
		aos_assert_g(child, cleanup);
		aos_assert_g(!node->mf->add_child((aos_field_t *)node, 
					child), cleanup);
		child->mf->set_attr(child, attr, num_attr);
		child->mf->put((aos_field_t *)child);
		child = 0;
		num_attr = 0;
		ret = parser->mf->next_sibling(parser, name, name_len, 
				&start, &len, &attr, &num_attr);
	}

	parser->mf->put(parser);
	aos_assert_g(ret > 0, cleanup);
	return node;

cleanup:
	parser->mf->put(parser);
	node->mf->put((aos_field_t *)node);
	return 0;
}
*/


aos_xml_node_t * aos_xml_node_create3(
		const char * const label,
		aos_scm_field_t *schema, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(label, 0);
	aos_assert_r(schema, 0);
	aos_assert_r(len >= 0, 0);

	aos_xml_node_t *node = aos_malloc(sizeof(aos_xml_node_t));
	aos_assert_r(node, 0);
	memset(node, 0, sizeof(aos_xml_node_t));

	aos_assert_g(!aos_xml_node_init(node, 
			eAosFieldType_Xml, label, contents, len), cleanup);

	if (contents)
	{
		node->contents = aos_malloc(len+1);
		aos_assert_g(node->contents, cleanup);
		memcpy(node->contents, contents, len);
		node->content_len = len;
	}

	if (schema) schema->mf->hold(schema);
	node->schema = schema;

	node->len_type = eAosLenType_Contents;
	if (schema) node->with_schema = 1;

	node->mf->hold((aos_field_t *)node);
	return node;

cleanup:
	aos_free(node);
	return 0;
}


aos_xml_node_t * aos_xml_node_create2(
		const char * const label, 
		const int start, 
		const int len, 
		aos_scm_field_t *schema, 
		aos_omparser_t *parser)
{
	aos_assert_r(label, 0);
	aos_assert_r(parser, 0);
	aos_assert_rm(start >= 0, 0, "start = %d", start);
	aos_assert_r(len >= 0, 0);

	aos_xml_node_t *node = aos_malloc(sizeof(aos_xml_node_t));
	aos_assert_r(node, 0);

	aos_assert_g(!aos_xml_node_init(node, 
			eAosFieldType_Xml, label, &parser->buffer[start], len), cleanup);

	if (schema) schema->mf->hold(schema);
	node->schema = schema;

	node->len_type = eAosLenType_Contents;
	if (schema) node->with_schema = 1;

	node->mf->hold((aos_field_t *)node);
	return node;

cleanup:
	if (node) aos_free(node);
	return 0;
}


static aos_xml_node_t *
aos_xml_node_parse_contents(
		aos_omparser_t *parser, 
		aos_scm_field_t *schema,
		int *is_incomplete, 
		const int with_root)
{
	char name[AOS_XML_LABEL_MAX_LEN+1];
	int name_len = AOS_XML_LABEL_MAX_LEN;
	aos_attr1_t **attr = 0;
	int num_attr = 0;
	int start, len, ret;
	aos_xml_node_t *child = 0;

	// Create the message
	aos_xml_node_t *root = 0;
	if (with_root)
	{
		root = aos_xml_node_create2(
			AOS_XML_ROOT_NODE_NAME, 
			0, parser->data_len, 0, parser);
		aos_assert_r(root, 0);
		root->is_root = 1;
	}

	// 
	// !!!!!!!!!!! Node: 'with_root' is not processed for 'schema' case. 
	// This needs to be handled !!!!!!!!!!!!!
	//
	if (schema)
	{
		ret = schema->mf->parse(schema, (aos_field_t *)root);
		aos_assert_g(ret >= 0, cleanup);
		parser->mf->put(parser);
		parser = 0;

		if (ret == 1)
		{
			// Data are not complete
			*is_incomplete = 1;
			root->mf->destroy((aos_field_t *)root);
			return 0;
		}

		root->parsed = 1;
		return root;
	}

	// 
	// Parse all the first level xml tags
	//
	int num_children = 0;
	while (num_children++ < 1000000)
	{
		name_len = AOS_XML_LABEL_MAX_LEN;

		ret = parser->mf->next_tag(parser, name, name_len, &start, 
					&len, &attr, &num_attr);
		if (ret) break;

		child = aos_xml_node_create2(
				name, start, len, 0, parser);
		aos_assert_g(child, cleanup);
		child->mf->set_attr((aos_field_t *)child, attr, num_attr);
		attr = 0;
		num_attr = 0;

		if (num_children == 1)
		{
			// The first child just created. Check whether it is with
			// root. 
			if (with_root)
			{
				aos_assert_g(!root->mf->add_child((aos_field_t *)root, 
					(aos_field_t *)child), cleanup);
				child->mf->put((aos_field_t *)child);
				child = 0;
			}
			else
			{
				root = child;
				child = 0;
			}
		}
		else
		{
			aos_assert_g(with_root, cleanup);
			aos_assert_g(!root->mf->add_child((aos_field_t *)root, 
				(aos_field_t *)child), cleanup);
			child->mf->put((aos_field_t *)child);
			child = 0;
		}
	} 

	aos_assert_g(ret > 0, cleanup);
	aos_assert_g(root, cleanup);
	parser->mf->put(parser);
	return root;

cleanup:
	if (root) root->mf->put((aos_field_t *)root);
	return 0;
}


// 
// Description
// It creates an xml node from a string. The string can be read 
// from files or received from the network. If 'schema' is not 
// null, it uses the schema to parse the message. If the string 
// does not contain the complete message, "*is_incomplete" is 
// set to 1.  'str_len' serves as both IN and OUT parameter. 
// As an IN parameter, 
// it indicates the length of the string. As an OUT parameter, 
// it indicates the number of characters consumed for the message 
// (only when it successfully created the message).
//
// if "with_root" is true, it will add a root node. Otherwise, 
// the inputs must contain only one node (serves as the root). 
//
// !!!!!!!!!!!!IMPORTANT!!!!!!!!!!!
// The caller should call aos_msg_hold(msg) to hold the message 
// created.  Otherwise, it can lead to memory corruption.
//
// Returns
// If successful, it returns the message created. Otherwise, it 
// will raise alarms and return 0.
//
aos_xml_node_t * 
aos_xml_node_create_from_str(
		aos_scm_field_t *schema,
		const char * const str, 
		int *str_len, 
		int *is_incomplete, 
		const int with_root)
{
	aos_assert_r(str, 0);
	aos_assert_r(str_len, 0);
	aos_assert_r((*str_len) >= 0, 0);
	aos_assert_r(is_incomplete, 0);

	*is_incomplete = 0;

	// Create the parser
	aos_omparser_t *parser = aos_omparser_create_from_str(str, *str_len);
	aos_assert_r(parser, 0);
	
	// Parse the contents
	aos_xml_node_t *root = aos_xml_node_parse_contents(parser, schema, 
			is_incomplete, with_root);
	aos_assert_g(root, cleanup);
	*str_len = parser->crt_pos;
	return root;

cleanup:
	parser->mf->put(parser);
	return 0;
}


// 
// Description
// It creates an xml node from a file. This function does not
// parse the message. Message parsing is done as needed. 
//
// !!!!!!!!!!!!IMPORTANT!!!!!!!!!!!
// The caller should call aos_msg_hold(msg) to hold the message created. 
// Otherwise, it can lead to memory corruption.
//
// Parameters:
// fname(IN):	the filename. 
//
// Returns
// If successful, it returns the message created. Otherwise, it 
// will raise alarms and return 0.
//
aos_xml_node_t * 
aos_xml_node_create_from_file(const char * const fname, const int with_root)
{
	aos_omparser_t *parser = 0;
	parser = aos_omparser_create_from_file(fname);
	aos_assert_r(parser, 0);

	// Parse the contents
	int is_incomplete = 0;
	aos_xml_node_t *root = aos_xml_node_parse_contents(parser, 0, 
			&is_incomplete, with_root);
	aos_assert_g(root, cleanup);
	aos_assert_g(!is_incomplete, cleanup);
	return root;

cleanup:
	parser->mf->put(parser);
	return 0;
}

