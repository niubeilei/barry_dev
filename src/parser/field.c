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
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/field.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "parser/parser.h"
#include "parser/scm_field.h"
#include "rvg_c/rvg.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/rc.h"
#include "util_c/rc_obj.h"
#include "util_c/buffer.h"
#include "util2/value.h"

//
// Description
// If the node's name is the same as 'name', it returns 1. Otherwise,
// it returns 0. On error, it returns -eAosRc_Error.
//

int aos_field_match_name(
		 aos_field_t *field,
		 const char *const name)
{
	aos_assert_r(field, -eAosRc_Error);
	aos_assert_r(name, -eAosRc_Error);
	aos_assert_r(field->name, -eAosRc_Error);
	return (strcmp(field->name, name) == 0);
}


// 
// Description:
// It retrieves the node's contents into 'value'. The caller should have allocated
// the memory for 'value'. The memory size is 'value_len'. If the field contents
// are longer than 'value_len', it is an error. 
//
int aos_field_get_str(
		aos_field_t *field, 
		char *value, 
		int *value_len)
{
	aos_assert_r(field, -1);
	aos_assert_r(value, -1);
	aos_assert_r(value_len && *value_len > 0, -1);
	aos_assert_r(field->content_len >= 0, -1);
	aos_assert_rm(field->content_len < *value_len, -1, "len: %d, value_len: %d", 
			field->content_len, *value_len);

	*value_len = field->content_len;
	if (field->contents)
	{
		strncpy(value, field->contents, field->content_len);
	}

	value[field->content_len] = 0;
	return 0;
}


// 
// Description:
// It retrieves the node's contents into 'value'. The function will allocate
// memory for the value. The caller is responsible for deleting the memory.
//
int aos_field_get_str_b(
		aos_field_t *field, 
		char **value) 
{
	aos_assert_r(field, -1);
	aos_assert_r(value, -1);
	aos_assert_r(field->content_len >= 0, -1);

	*value = aos_malloc(field->content_len+1);
	if (field->contents)
	{
		strncpy(*value, field->contents, field->content_len);
	}

	(*value)[field->content_len] = 0;
	return 0;
}


// 
// Description:
// This function replaces the contents of the current field with the new
// contents: 'contents'. 
//
int aos_field_replace_contents(
		aos_field_t *field, 
		const char * const contents)
{
	aos_assert_r(field, -1);
	aos_assert_r(contents, -1);

	int len = strlen(contents);

	char * ptr = field->contents;
	if (field->content_len < len)
	{
		field->contents = aos_malloc(len+1);
		aos_assert_g(field->contents, cleanup);
		aos_free(ptr);
		ptr = 0;
	}

	memcpy(field->contents, contents, len);
	field->contents[len] = 0;
	return 0;

cleanup:
	field->contents = ptr;
	return -1;
}


// 
// Description
// The function checks whether the field has already been 
// parsed. If yes, it does nothing and returns 0. 
// Otherwise, it uses the field's schema to parse the field.
// If the field schema is null, it is an error. 
//
// Returns
// 0 upon success. 
// 1 if incomplete
// Otherwise, error codes.
//
int aos_field_parse(
		aos_field_t *field, 
		aos_omparser_t *parser) 
{
	int ret;
	aos_assert_r(field, -1);
	aos_assert_r(parser, -1);
	aos_assert_r(field->schema, -1);

	// Check whether the field was already parsed
	if (field->parsed) return 0;

	field->parsed = 1;

	ret = field->schema->mf->parse(
			field->schema, field);
	aos_assert_r(ret >= 0, ret);
	return ret;
}


int aos_msg_add_field(
		aos_field_t *field,
		aos_field_t *child)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_field_set_lws_gen(
		struct aos_field *field, 
		struct aos_rvg *gen)
{
	aos_assert_r(field, -1);
	aos_assert_r(gen, -1);
	aos_rc_obj_assign(field->lws_gen, gen);
	return 0;
}


int aos_field_set_tws_gen(struct aos_field *field, aos_rvg_t *gen)
{
	aos_assert_r(field, -1);
	aos_assert_r(gen, -1);
	aos_rc_obj_assign(field->tws_gen, gen);
	return 0;
}


int aos_field_add_child(
		struct aos_field *field, 
		struct aos_field *child)
{
	aos_assert_r(field, -1);
	aos_assert_r(child, -1);

	aos_assert_r(!field->fields_array.mf->add_element(
			&field->fields_array, 1, (char **)&field->fields), -1);
	field->fields[field->fields_array.noe++] = child;
	child->mf->hold(child);
	return 0;
}


int aos_field_set_value(
		struct aos_field *field, 
		struct aos_value *value)
{
	aos_assert_r(field, -1);
	aos_assert_r(value, -1);

	aos_rc_obj_assign(field->value, value);
	return 0;
}


int aos_field_integrity_check(aos_field_t *field)
{
	int i;
	aos_assert_r(field, -1);
	aos_assert_r(field->mf, -1);
	aos_assert_r(aos_field_type_check(field->type) == 1, -1);
	aos_assert_r(field->name, -1);
	aos_assert_r(strlen(field->name) > 0, -1);
	aos_assert_t(field->with_schema, field->schema, -1);
	aos_assert_int_range_r(field->parsed, 0, 1, -1);
	aos_assert_t(!field->contents, field->content_len == 0, -1);
	aos_assert_r(field->content_len >= 0, -1);
	aos_assert_t(field->lws_gen, !field->lws_gen->mf->
			integrity_check(field->lws_gen), -1);
	aos_assert_t(field->tws_gen, !field->tws_gen->mf->
			integrity_check(field->tws_gen), -1);
	aos_assert_r(aos_len_type_check(field->len_type) == 1, -1);
	aos_assert_r(aos_data_type_check(field->data_type) == 1, -1);
	aos_assert_r(field->fields_array.mf->integrity_check(
				&field->fields_array, (char **)&field->fields) == 1,
		   		-1);
	aos_assert_t(field->value, field->value->mf->
			integrity_check(field->value) == 1, -1);
	aos_assert_r(field->ref_count >= 0, -1);

	for (i=0; i<field->fields_array.noe; i++)
	{
		aos_assert_r(field->fields[i], -1);
	}
	return 0;
}


int aos_field_dump_to(
		struct aos_field *field, 
		char *buff, 
		int *len)
{
	int offset, i;

	aos_assert_r(field, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(len, -1);

	int buff_len = *len;
	int index = 0;

	aos_buff_append_line_str(buff, buff_len, &index, 
			"Field Type: ", aos_field_type_2str(field->type));
	aos_buff_append_line_str(buff, buff_len, &index, 
			"Name: ", field->name);
	aos_buff_append_line_int(buff, buff_len, &index, 
			"Length: ", field->content_len);
	aos_buff_append_line_int(buff, buff_len, &index, 
			"Parsed: ", field->parsed);


	offset = index;
	for (i=0; i<field->fields_array.noe; i++)
	{
		buff_len = *len - offset;
		index = buff_len;
		field->fields[i]->mf->dump_to(field->fields[i], &buff[offset], &index);
		offset += index;
	}

	*len = offset;
	return 0;
}


int aos_field_hold(aos_field_t *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(field->ref_count >= 0, -1);
	field->ref_count++;
	return 0;
}


int aos_field_put(aos_field_t *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(field->ref_count >= 0, -1);
	field->ref_count--;
	if (field->ref_count == 0)
	{
		aos_assert_r(!field->mf->destroy(field), -1);
	}

	return 0;
}


int aos_field_release_memory(struct aos_field *field)
{
	aos_assert_r(field, -1);

	if (field->name) 
	{
		aos_free(field->name);
		field->name = 0;
	}

	if (field->schema) field->schema->mf->put(field->schema);
	if (field->lws_gen) field->lws_gen->mf->put(field->lws_gen);
	if (field->tws_gen) field->tws_gen->mf->put(field->tws_gen);
	field->fields_array.mf->release_memory(&field->fields_array); 
	field->fields = 0;
	field->attrs_array.mf->release_memory(&field->attrs_array);
	field->attrs = 0;
	return 0;
}


// This function sets the attributes 'attr' to this field. It simply takes the 
// memory. The caller should make sure after calling this function, it cannot 
// release the memory.
int aos_field_set_attr(
		aos_field_t *field,
		aos_attr1_t **attr, 
		const int num_attr)
{
	if (num_attr == 0) return 0;

	aos_assert_r(field, -1);
	aos_assert_r(attr, -1);

	aos_assert_r(num_attr >= 0, -1);

	/* Changed the attr to a dynamc array. Chen Ding, 09/22/2008
	// Release the memory for the current attributes
	if (field->num_attr > 0) aos_attr_free_all(field->attr, field->num_attr);

	// Make sure the field has enough attributes
	if (field->attr_size < num_attr)
	{
		if (field->attr_size > 0)
		{
			aos_assert_r(field->attr, -1);
			aos_free(field->attr);
		}
		int size = num_attr + 30;
		field->attr = aos_malloc(size* 4);
		aos_assert_r(field->attr, -1);
		field->attr_size = size;
	}
	memset(field->attr, 0, field->attr_size * 4);

	for (i=0; i<num_attr; i++)
	{
		// aos_attr_copy(field->attr[i], attr[i]);
		field->attr[i] = attr[i];

		// !!!!!!!!!!!!!!!!! Improvements !!!!!!!!!!!!!!!!
		// This is not a good idea to check the attribute type here. 
		// We can do it whenever we actually need to know the field data type. 
		// Chen Ding, 09/06/2008
		if (strcmp(attr[i]->name, "data_type") == 0)
		{
			field->data_type = aos_data_type_2enum(attr[i]->value);
			aos_assert_r(aos_data_type_check(field->data_type) == 1, -1);
		}
	}

	field->num_attr = num_attr;
	*/

	// Make sure the array is big enough
	int diff = num_attr - field->attrs_array.noe;
	if (diff > 0)
	{
		int ret = field->attrs_array.mf->add_element(&field->attrs_array, 
				diff, (char **)&field->attrs);
		aos_assert_r(!ret, -1);
	}

	memcpy(field->attrs, attr, num_attr * 4);
	field->attrs_array.noe = num_attr;
	return 0;
}


// This function adds an attribute to the field. 
// If 'attr' does not have an empty slot, it will add
// additional slots. 
int aos_field_add_attr(
		aos_field_t *field, 
		const char * const name,
		const char * const value)
{
	aos_assert_r(field, -1);
	aos_assert_r(name, -1);
	aos_assert_r(value, -1);
	aos_assert_r(!field->mf->get_attr(field, name), -1);
	aos_assert_r(strlen(name) < AOS_ATTR_NAME_MAX_LEN, -1);

	int ret = field->attrs_array.mf->add_element(&field->attrs_array, 
			1, (char **)&field->attrs);
	aos_assert_r(!ret, -1);

	aos_attr1_t *attr = aos_malloc(sizeof(aos_attr1_t));
	strcpy(attr->name, name);
	attr->value = aos_malloc(strlen(value)+1);
	aos_assert_r(attr->value, -1);
	strcpy(attr->value, value);
	field->attrs[field->attrs_array.noe++] = attr;
	return 0;
}


// 
// Description:
// It retrieves the attribute whose name is 'name'. If not found, it returns
// null. Otherwise, it returns the pointer pointing to the value of the
// attribute. Note that the caller should not delete the memory. Normally
// the caller should use the memory immediately. If it needs to hold the
// value for long, it is highly recommended to make a copy of it. 
//
char *aos_field_get_attr(
		aos_field_t *field,
		const char * const name) 
{
	aos_assert_r(field, 0);
	aos_assert_r(name, 0);

	int i;
	for (i=0; i<field->attrs_array.noe; i++)
	{
		if (strcmp(field->attrs[i]->name, name) == 0)
		{
			return field->attrs[i]->value;
		}
	}

	return 0;
}


int aos_field_get_attr_int64(
		struct aos_field *field, 
		const char * const name, 
		int64_t *value)
{
	aos_assert_r(field, -1);
	char *vv = field->mf->get_attr(field, name);
	aos_assert_r(vv, -1);
	aos_assert_r(!aos_value_atoll(vv, value), -1);
	return 0;
}


int aos_field_get_attr_u64(
		struct aos_field *field, 
		const char * const name, 
		u64 *value)
{
	aos_assert_r(field, -1);
	char *vv = field->mf->get_attr(field, name);
	aos_assert_r(vv, -1);
	aos_assert_r(!aos_value_atoull(vv, value), -1);
	return 0;
}


// 
// Description
// This function is used when creating a new object. The caller 
// should have called memset(...) to reset the memroy.
//
int aos_field_reset(aos_field_t *field)
{
	aos_assert_r(field, -1);
	if (field->contents) field->contents[0] = 0;
	field->content_len = 0;
	return 0;
}


int aos_field_set_contents(
		struct aos_field *field, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(field, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(len >= 0, -1);

	if (field->contents) aos_free(field->contents);
	field->contents = aos_malloc(len+1);
	aos_assert_r(field->contents, -1);
	memcpy(field->contents, contents, len);
	field->contents[len] = 0;
	return 0;
}


// 
// Description:
// It converts the node's contents into an int64_t value.
//
int aos_field_get_int64(
		struct aos_field *field, 
		int64_t *value)
{
	aos_assert_r(field, -1);
	aos_assert_r(value, -1);
	aos_assert_r(field->contents, -1);
	aos_assert_r(!aos_value_atoll(field->contents, value), -1);
	return 0;
}


int aos_field_get_u64(
		struct aos_field *field, 
		u64 *value)
{
	aos_assert_r(field, -1);
	aos_assert_r(value, -1);
	aos_assert_r(field->contents, -1);
	aos_assert_r(!aos_value_atoull(field->contents, value), -1);
	return 0;
}


// 
// Description: 
// It retrieves the value of the first node that has the attribute 'attr_name'
// and the attribute value equals to 'attr_value'. If found, 'value' points
// to the memory. Note that the caller should not free the memory, not modify
// it (normally). It is also important to know that the memory may be modified, 
// or even deleted by others. If the caller wants to keep the memory, it 
// should allocate memory for it.
//
// Return:
// If found, it returns 1. If not found, it returns 0. On errors, it returns -1.
//
int aos_field_get_str_by_attr(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		char **value)
{
	*value = 0;
	aos_assert_r(field, -1);
	aos_assert_r(attr_name, -1);
	aos_assert_r(attr_value, -1);
	aos_assert_r(value, -1);

	int i, j;
	aos_field_t *child;
	for (i=0; i<field->fields_array.noe; i++)
	{
		child = field->fields[i];
		for (j=0; j<child->attrs_array.noe; j++)
		{
			if (strcmp(child->attrs[j]->name, attr_name) == 0 &&
				strcmp(child->attrs[j]->value, attr_value) == 0)
			{
				*value = child->contents;
				return 1;
			}
		}
	}

	// Did not find
	return 0;
}


int aos_field_get_int_by_attr(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		int *value)
{
	aos_assert_r(field, -1);

	char *buff;
	int ret = field->mf->get_str_by_attr(field, attr_name, attr_value, &buff);
	aos_assert_r(ret >= 0, -1);
	if (ret == 0) return 0;

	// 
	// It is still possible that the field does not have any contents, which 
	// is considered an error.
	//
	aos_assert_r(buff, -1);

	aos_assert_r(!aos_atoi(buff, strlen(buff), value), -1);
	return 1;
}


int aos_field_init(aos_field_t *field, 
				   const aos_field_type_e type, 
				   const char * const name,
				   const aos_data_type_e data_type, 
				   const char * const contents, 
				   const int len)
{
	aos_assert_r(field, -1);
	aos_assert_r(aos_data_type_check(data_type) == 1, -1);
	aos_assert_r(len >= 0, -1);

	field->type = type;
	if (field->name) 
	{
		aos_free(field->name);
		field->name = 0;
	}

	if (name)
	{
		aos_assert_r(!aos_str_set(&field->name, name, strlen(name)), -1);
	}

	if (len > 0)
	{
		aos_assert_r(contents, -1);
		if (field->contents) aos_free(field->contents);
		field->contents = aos_malloc(len+1);
		memcpy(field->contents, contents, len);
		field->contents[len] = 0;
	}
	field->content_len = len;
	field->data_type = data_type;
	aos_assert_r(!aos_dyn_array_init(&field->fields_array, (char **)&field->fields, 
			sizeof(aos_field_t *), 5, AOS_FIELD_MAX_MEMBERS), -1);
	aos_assert_r(!aos_dyn_array_init(&field->attrs_array, (char **)&field->attrs, 
			sizeof(aos_attr1_t *), 5, AOS_FIELD_MAX_ATTRS), -1);

	return 0;
}


aos_field_t *aos_field_factory(aos_xml_node_t *conf)
{   
	aos_assert_r(conf, 0);
	// if (conf->mf->match_label(conf, "XmlMessage") == 1)
	// {   
	// 	aos_msg_xml_t *msg = aos_msg_xml_create_xml(conf);
	// 	return (aos_msg_t*)msg;
	// }
	
	aos_alarm("Unrecognized type: %s", conf->mf->dump((aos_field_t *)conf));
	return 0;
}

