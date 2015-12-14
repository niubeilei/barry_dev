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
// aos_scm_field_xml is a subclass of scm_field. 
//
// Member Data:
// 1. mandatory_first: 
//    All mandatory tags must appear before optional tags
// 2. mandatory_ordered:
//    All mandatory tags must appear in the same order as defined 
// 3. optional_ordered:
//    All optional tags must appear in the same order as defined
// 4. tag_name:
//    If it is defined, the message must match the tag name. 
//    Otherwise, it is an error. If it is null, it matches 
//    any tag. Default: null.
// 5. undefined_child_allowed:
//    If it is 1, it means the message to be parsed may contain
//    children that are not defined by this schema. If it is 0,
//    the message cannot contain any child that is not defined
//    by this schema.
//   
//
// Modification History:
// 02/08/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/scm_field_xml.h"

#include "alarm_c/alarm.h"
#include "parser/scm_field.h"
#include "parser/xml_node.h"
#include "parser/parser.h"
#include "parser/attr.h"
#include "util_c/memory.h"
#include "util2/value.h"
#include "util_c/buffer.h"

int aos_scm_field_xml_serialize(
		aos_scm_field_t *field, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<ScmField_Xml>			// Start here
	// 			<Name>
	// 			<Type>
	// 			<mandatory_ordered>
	// 			<optional_ordered>
	// 			<mandatory_first>
	//			<fields>
	//				<field>
	//					... (see scm_field.c definition)
	//					<TagName>
	//					<UndefinedAllowed>
	//				</field>
	//				...
	//			</fields>
	//			<UndefinedAllowed>	Default: 1
	//		</Scmfield_Xml>
	//		...
	//	</Parent>
	//
	aos_assert_r(field, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(field->type == eAosScmFieldType_Xml, -1);
	aos_scm_field_xml_t *self = (aos_scm_field_xml_t *)field;

	// Append the node
	aos_assert_r(field->name, -1);
	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "ScmField_Xml");
	aos_assert_r(node, -1);

	// Process the parent class. 
	aos_assert_r(!aos_scm_field_serialize(field, node), -1);

	// Add <TagName> tag
	if (self->tag_name)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"TagName", self->tag_name, 0, 0), -1);
	}

	// Add <UndefinedAllowed> tag
	if (!self->undefined_child_allowed)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"UndefinedAllowed", self->undefined_child_allowed), -1);
	}

	return 0;
}


int aos_scm_field_xml_deserialize(
		aos_scm_field_t *field, 
		aos_xml_node_t *node)
{
	// 
	// 	<Scmfield_Xml>
	// 		<name>
	// 		<mandatory_ordered>
	// 		<optional_ordered>
	// 		<mandatory_first>
	//		<fields>
	//			<field>
	//				... (see scm_field.c definition)
	//				<TagName>
	//				<UndefinedAllowed>
	//			</field>
	//			...
	//		</fields>
	//	</Scmfield_Xml>
	//	
	int len;
	aos_assert_r(field, -1);
	aos_assert_r(node, -1);
	aos_assert_r(field->type == eAosScmFieldType_Xml, -1);
	aos_scm_field_xml_t *self = (aos_scm_field_xml_t *)field;

	// Process the parent class
	aos_assert_r(!aos_scm_field_deserialize(field, node), -1);

	// Retrieve <TagName>
	aos_assert_r(!node->mf->first_child_str_dft(node, "TagName", 
			&self->tag_name, &len, 0), -1);

	// Retrieve <UndefinedAllowed>
	aos_assert_r(!node->mf->first_child_char_dft(
			node, "UndefinedAllowed",
			&self->undefined_child_allowed, 1), -1);

	aos_assert_r(!aos_scm_field_xml_integrity_check(field), -1);

	return 0;
}


const char * aos_scm_field_xml_dump(
		struct aos_scm_field *field)
{
	aos_not_implemented_yet;
	return 0;
}


int aos_scm_field_xml_dump_to(
		struct aos_scm_field *field,
		char *buff,
		int *buff_len)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_scm_field_xml_destroy(aos_scm_field_t *field)
{
	aos_scm_field_destroy(field);
	aos_free(field);
	return 0;
}


// 
// Description
// The field 'field' is under the way to construct an instance
// from its input. The xml_node being constructed is 'node'. 
// This function checks whether the node has been constructed
// correctly so far. 
//
// The items to be checked are:
// 1. mandatory_first:
//    All mandatory tags must appear before optional tags
// 2. mandatory_ordered:
//    All mandatory tags must appear in the same order as defined
// 3. optional_ordered:
//    All optional tags must appear in the same order as defined
//
// If 'final' is 1, it should also check the completeness of the
// node.
//
// Parameters
// field:		The schema field that controls the construction
// node:		The node that is under construction
// final:		Check completeness if it is not zero
//
// Returns
// 0 if good. 
// error codes if errors.
//
int aos_scm_field_xml_check_construction(
		aos_scm_field_t *schema, 
		aos_field_t *field,
		const int final)
{
	int optional_present, all_mandatory_present;
	aos_scm_field_t *fschema;
	aos_field_t * ffield;
	int i, j;
	aos_assert_r(schema, -1);
	aos_assert_r(field, -1);

	optional_present = 0;
	all_mandatory_present = 1;
	for (i=0; i<schema->fields_array.noe; i++)
	{
		fschema = schema->fields[i];
		if (!fschema->is_mandatory && fschema->is_present)
		{
			optional_present = 1;
		}

		if (fschema->is_mandatory && !fschema->is_present)
		{
			all_mandatory_present = 0;
		}
	}

	if (schema->mandatory_first)
	{
		// 
		// If any of mandatory children is not present yet, 
		// no optional nodes should be present now.
		//
		if (!all_mandatory_present && optional_present)
		{
			aos_alarm("Optional appears ahead of mandatory");
			return -1;
		}
	}

	if (schema->mandatory_ordered)
	{
		// 
		// All mandatory children should be in the same order
		// as defined by 'field'.
		//
		int mandatory_index = 0;
		for (i=0; i<field->fields_array.noe; i++)
		{
			ffield = field->fields[i];
			fschema = ffield->schema;
			aos_assert_r(fschema, -1);
			if (fschema->is_mandatory)
			{
				for (j=0; j<schema->fields_array.noe; j++)
				{
					if (schema->fields[j] == fschema)
					{
						if (j <= mandatory_index)
						{
							aos_alarm("Mandatory is not in order");
							return -1;
						}
						mandatory_index = j;
						break;
					}
				}
			}
		}
	}

	if (schema->optional_ordered)
	{
		// 
		// All optional children should be in the same order
		// as defined by 'field'.
		//
		int optional_index = 0;
		for (i=0; i<field->fields_array.noe; i++)
		{
			ffield = field->fields[i];
			fschema = ffield->schema;
			aos_assert_r(fschema, -1);
			if (!fschema->is_mandatory)
			{
				for (j=0; j<schema->fields_array.noe; j++)
				{
					if (schema->fields[j] == fschema)
					{
						if (j <= optional_index)
						{
							aos_alarm("Mandatory is not in order");
							return -1;
						}
						optional_index = j;
						break;
					}
				}
			}
		}
	}

	if (final)
	{
		// 
		// Check its completeness
		//
		for (i=0; i<schema->fields_array.noe; i++)
		{
			fschema = schema->fields[i];
			if (fschema->is_mandatory && !fschema->is_present)
			{
				return -1;
			}
		}
	}

	return 0;
}


// 
// Description
// It finds the child schema whose name is 'name'. If found, it returns
// the child. Otherwise, it returns null.
//
aos_scm_field_t *aos_scm_field_xml_find_child(
		struct aos_scm_field_xml *schema, 
		const char * const name)
{
	int i;
	aos_scm_field_t *child = 0;
	for (i=0; i<schema->fields_array.noe; i++)
	{
		child = schema->fields[i];
		if (strcmp(child->name, name) == 0) return child;
	}

	return 0;	
}


// 
// Description
// This function parses the inputs from "parser" based on the 
// template defined by 'field'. This means that if 'field'
// is an atomic field, 'parser' should point to an atomic 
// field. If 'field' is not an atomic field, 'parser' should
// contain all the mandatory fields defined by 'field'. 
//
// This function assumes that parser current position is 
// at the opening tag of 'field'.
//
// Returns
// 0 if successful, it creates the node and returns it. 
// 1 if incomplete. 
// -1 Otherwise. 
//
int aos_scm_field_xml_parse(
		aos_scm_field_t *schema, 
		aos_field_t *field)
{
	/*
	int len, start_pos, ret;
	char tag[50];
	int tag_len = 50;
	aos_value_t *value;
	aos_xml_node_t *child_created = 0;
	aos_scm_field_t *child_schema;
	aos_scm_field_xml_t *self_schema;
	aos_attr1_t attr[AOS_ATTR_MAX];

	int num_attr = AOS_ATTR_MAX;
	aos_assert_r(schema, -1);
	aos_assert_r(field, -1);
	aos_assert_r(!schema->mf->integrity_check(schema), -1);
	aos_assert_r(schema->type == eAosScmFieldType_Xml, -1);
	self_schema = (aos_scm_field_xml_t *)schema;

	if (self_schema->tag_name)
	{
		aos_assert_r(field->mf->match_name(field, 
				self_schema->tag_name) == 1, -1);
	}

	if (field->parsed) return 0;
	field->parsed = 1;

	// Parse the open tag
	if (self_schema->tag_name)
	{
		ret = parser->mf->expect_open_tag(parser, 
			&self_schema->tag_name, attr, &num_attr);
		aos_assert_r(ret >= 0, ret);
	}
	else
	{
		if (field->name) 
		{
			aos_free(field->name);
			field->name = 0;
		}

		ret = parser->mf->expect_open_tag(parser, 
			&field->name, attr, &num_attr);
		aos_assert_r(ret >= 0, ret);
	}

	// If ret == 1, it means the parser does not contain
	// enough contents to construct the msg. 
	if (ret == 1) return ret;

	if (num_attr)
	{
		aos_assert_r(!field->mf->set_attr(
					field, attr, num_attr), -1);
	}

	if (schema->fields_array.noe == 0)
	{
		// This is an atomic field. 
		aos_assert_r(!parser->mf->expect_value(
				parser, schema->data_type, &value), -1);
		aos_assert_r(value, -1);
		aos_assert_r(!field->mf->set_value(field, value), -1);
		aos_assert_r(!field->mf->integrity_check(field), -1);
		return 0;
	}

	// 
	// This is a composite field. Need to parse its members.
	//
	num_attr = AOS_ATTR_MAX;
	ret = parser->mf->next_tag(parser, tag, tag_len, &start_pos, 
			&len, attr, &num_attr);

	while (!ret)
	{
		// 
		// Found a tag. Use the tag to find the schema field.
		//
		child_schema = self_schema->mf->find_child(self_schema, tag);
		if (!child_schema)
		{
			// 
			// The tag is not defined by this field. 
			//
			if (!self_schema->undefined_child_allowed)
			{
				aos_alarm("Parsed an unexpected child: %s", 
					parser->mf->dump(parser));
				return -1;
			}
		}
		else
		{
			// 
			// The schema field is found. Use the schema field
			// to parse the inputs. But note that we will delay
			// the parsing until the field is needed. 
			//
			child_created = aos_xml_node_create2(
					tag, start_pos, len, (aos_scm_field_t *)child_schema, 
					parser);
			aos_assert_r(child_created, -1);
			aos_assert_g(!field->mf->add_child(field, 
						(aos_field_t *)child_created), cleanup);
			aos_assert_r(!aos_scm_field_xml_check_construction(
						schema, field, 0), -1);
			child_schema->mf->set_present(
					(aos_scm_field_t *)child_schema, 1);
			child_created->mf->set_attr((aos_field_t *)
					child_created, attr, num_attr);
			num_attr = AOS_ATTR_MAX;
		}

		ret = parser->mf->next_tag(parser, tag, tag_len, 
				&start_pos, &len, attr, &num_attr);
	}

	// Check whether it is incomplete
	if (ret == 1) return ret;

	aos_assert_g(!aos_scm_field_xml_check_construction(
				schema, field, 1), cleanup);
	return 0;

cleanup:
	if (child_created) aos_free(child_created);
	return -1;

	*/
	aos_not_implemented_yet;
	return -1;
}


int aos_scm_field_xml_integrity_check(aos_scm_field_t *field)
{
	aos_scm_field_xml_t *self;
	aos_assert_r(field, -1);
	aos_assert_r(!aos_scm_field_integrity_check(field), -1);
	aos_assert_r(field->type == eAosScmFieldType_Xml, -1);

	self = (aos_scm_field_xml_t *)field;
	aos_assert_int_range_r(self->mandatory_ordered, 0, 1, -1);
	aos_assert_int_range_r(self->optional_ordered, 0, 1, -1);
	aos_assert_int_range_r(self->mandatory_first, 0, 1, -1);
	return 0;
}


// 
// Description
// This function uses the schema to parse a new field. If successful, 
// it will create the field. The caller should hold the instance
// returned by this function. 
//
// This function assumes someone has determined the schema. The 
// schema expects the current tag must match the name the 
// schema expects. The parser should stop right after the right
// bracket of the tag. 
//
// Returns:
// obj if success
aos_field_t * aos_scm_field_xml_parse_new(
		struct aos_scm_field *schema, 
		struct aos_omparser *parser, 
		int *is_eof) 
{
	aos_assert_r(schema, 0);
	aos_assert_r(schema->type == eAosScmFieldType_Xml, 0);
	aos_assert_r(parser, 0);
	
	aos_xml_node_t *msg = aos_xml_node_create2("", -1, 0, schema, parser);
	int ret = aos_scm_field_xml_parse(schema, (aos_field_t *)msg);
	if (ret == 1)
	{
		msg->mf->destroy((aos_field_t *)msg);
		*is_eof = 1;
		return 0;
	}

	return (aos_field_t *)msg;
}


// 
// Description
// This function uses the schema and the buffer to create a 
// new field. If successful, it will create the field. 
// The caller should hold the instance returned by this function. 
// If the buffer does not contain a complete message, it 
// holds the buffer. Additional data should be received
// and this function is called again. If it generated a message
// but the schema did not consume all the contents, the 
// remaining contents are held by this schema. One can 
// either call this function again with additional data 
// to create the next message, or retrieve the remaining
// data.
//
// Returns:
// 0 if success (msg created)
// 1 if incomplete
// -1 if errors
//
int aos_scm_field_xml_data_rcvd(
		aos_scm_field_t *schema, 
		aos_buffer_t *buff, 
		aos_field_t **msg)
{
	aos_assert_r(msg, -1);
	*msg = 0;
	aos_assert_r(schema, -1);
	aos_assert_r(schema->type == eAosScmFieldType_Xml, -1);
	aos_scm_field_xml_t *self = (aos_scm_field_xml_t *)schema;
	aos_assert_r(buff, -1);
	
	if (self->buffer)
	{
		aos_assert_r(!self->buffer->mf->append_data(
				self->buffer, buff), -1);
	}
	else
	{
		self->buffer = buff;
		buff->mf->hold(buff);
	}

	int ret = schema->mf->parse_from_buff(
			schema, self->buffer, msg);
	aos_assert_r(ret >= 0, ret);
	return ret;
}


// 
// Description
// This function uses the schema and the buffer to create a 
// new field. If successful, 
// it will create the field. The caller should hold the instance
// returned by this function. 
//
// Returns:
// 0 if success
// 1 if incomplete
// -1 if errors
//
int aos_scm_field_xml_parse_from_buff(
		aos_scm_field_t *schema, 
		aos_buffer_t *buff, 
		aos_field_t **field)
{
	aos_assert_r(schema, -1);
	aos_assert_r(schema->type == eAosScmFieldType_Xml, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(field, -1);
	
	int len = buff->data_len;
	int is_incomplete;
	*field = (aos_field_t *)aos_xml_node_create_from_str(
			schema, buff->buff, &len, &is_incomplete, 0);

	if (*field) return 0;

	if (is_incomplete) return 1;

	return -1;
}


aos_scm_field_t * aos_scm_field_xml_get_generic_schema()
{
	static aos_scm_field_t *ls_schema = 0;

	if (ls_schema)
	{
		ls_schema = (aos_scm_field_t *)aos_scm_field_xml_create_default();
	}

	return ls_schema;
}


// 
// Description
// This function selects a schema based on the current contents. 
// 1. If it is an atomic schema, it assumes the parser points 
//    at the beginning of its opening tag. It returns itself.
// 2. If it is a composite schema, it assumes the parser points
//    to the beginning of one of its child tag. It retrieves
//    the tag and uses the tag to determine which schema to return.
//
aos_scm_field_t * aos_scm_field_xml_select_msg(
			aos_scm_field_t *schema, 
			aos_omparser_t *parser, 
			int *is_eof)
{
	char tag[AOS_XML_LABEL_MAX_LEN+1];
	int tag_len = AOS_XML_LABEL_MAX_LEN;
	aos_attr1_t **attr = 0;
	int num_attr = 0;
	int start, len;

	*is_eof = 0;
	aos_assert_r(schema, 0);
	aos_assert_r(parser, 0);
	aos_assert_r(schema->type == eAosScmFieldType_Xml, 0);
	aos_scm_field_xml_t *self = (aos_scm_field_xml_t *)schema;

	if (self->is_atomic)
	{
		return schema;
	}

	// !!!!!!!!!!!!!!!! Improvements Needed !!!!!!!!!!!!
	// The function 'next_tag' will allocate memory for attributes, but this function
	// does not need it. We should consider adding a flag to the function to control
	// whether attributes need to be retrieved or not. For the time being, after 
	// calling the function, we free the memory
	aos_assert_r(schema->fields_array.noe > 0, 0);
	int ret = parser->mf->next_tag(parser, tag, tag_len, 
				&start, &len, &attr, &num_attr);
    aos_assert_r(ret >= 0, 0);

	// Free the memory for the attributes
	if (num_attr > 0) aos_attr_free_all(attr, num_attr);

	// Check whether it is end of the buffer 
	if (ret == 1) 
	{
		*is_eof = 1;
		return 0;
	}

	aos_scm_field_t *child = self->mf->find_child(self, tag);
	if (child)
	{
		// 
		// Found the schema for the current msg. 
		//
		aos_assert_r(!child->mf->reset_parsing(child, start, len), 0);
		return child;
	}

	if (self->undefined_child_allowed)
	{
		return aos_scm_field_xml_get_generic_schema();
	}

	aos_alarm("Encounted a tag that is not allowed: %s. \nParser: %s", 
			schema->mf->dump(schema), parser->mf->dump(parser));
	return 0;
}


int aos_scm_field_xml_reset_parsing(
		aos_scm_field_t *schema, 
		const int start, 
		const int len)
{
	aos_assert_r(schema, -1);

	aos_assert_r(!aos_scm_field_reset_parsing(schema, start, len), -1);
	return 0;
}


// 
// Description
// This function determines whether the field 'field' can be
// added to the msg 'msg'. This is determined by the schema. 
// If it is yes, it returns 1. Otherwise, it returns 0. If 
// errors, it returns -1.
//
int aos_scm_field_xml_field_allowed(
		aos_scm_field_xml_t *schema, 
		aos_field_t *msg, 
		aos_field_t *field)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_scm_field_xml_mf_t sg_mf = 
{
	aos_scm_field_hold,
	aos_scm_field_put, 
	aos_scm_field_set_present,
	aos_scm_field_set_msg_selector,
	aos_scm_field_add_child,
	aos_scm_field_set_lws_gen,
	aos_scm_field_set_tws_gen,
	aos_scm_field_set_lws,
	aos_scm_field_set_tws,
	aos_scm_field_set_map1,
	aos_scm_field_set_map2,
	aos_scm_field_set_cond,
	aos_scm_field_xml_select_msg,
	aos_scm_field_xml_serialize, 
	aos_scm_field_xml_deserialize, 
	aos_scm_field_xml_parse_new,
	aos_scm_field_xml_parse_from_buff, 
	aos_scm_field_xml_data_rcvd, 
	aos_scm_field_xml_parse, 
	aos_scm_field_xml_dump, 
	aos_scm_field_xml_dump_to, 
	aos_scm_field_xml_integrity_check,
	aos_scm_field_xml_destroy, 
	aos_scm_field_xml_reset_parsing,

	aos_scm_field_xml_find_child,
	aos_scm_field_xml_field_allowed
};


// 
// Description
// It creates an instance of aos_scm_field_xml_t based on the 
// xml configuration 'node'. 
//
aos_scm_field_xml_t *aos_scm_field_xml_create(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);

	aos_scm_field_t *field = aos_malloc(sizeof(aos_scm_field_xml_t));
	aos_assert_r(field, 0);
	memset(field, 0, sizeof(aos_scm_field_xml_t));
	field->mf = (aos_scm_field_mf_t *)&sg_mf;
	field->type = eAosScmFieldType_Xml;

	aos_assert_g(!aos_scm_field_xml_deserialize(field, node), cleanup);
	return (aos_scm_field_xml_t *)field;

cleanup:
	aos_free(field);
	return 0;
}


aos_scm_field_xml_t *aos_scm_field_xml_create_default()
{
	aos_scm_field_xml_t *s = aos_malloc(sizeof(aos_scm_field_xml_t));
	aos_assert_r(s, 0);
	memset(s, 0, sizeof(aos_scm_field_xml_t));
	s->mf = &sg_mf;
	aos_assert_g(aos_scm_field_init((aos_scm_field_t *)s, 
			eAosScmFieldType_Xml, 0,
			eAosLenUnit_Unknown,
			eAosLenType_Unknown,
			eAosDataType_Unknown,
			eAosPresence_Unknown), cleanup);
	s->start_pos = -1;
	return s;

cleanup:
	aos_free(s);
	return 0;
}

