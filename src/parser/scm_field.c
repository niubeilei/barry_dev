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
#include "parser/scm_field.h"

#include "alarm_c/alarm.h"
#include "conditions_c/cond.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "parser/ms.h"
#include "parser/scm_field_xml.h"
#include "rvg_c/rvg.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/rc_obj.h"


int aos_scm_field_add_child(
	   aos_scm_field_t *field,
	   aos_scm_field_t *child)
{   
	int ret;
	aos_assert_r(field, -1);
	aos_assert_r(child, -1);
	aos_assert_r(field->fields_array.noe < AOS_SCM_FIELD_XML_MAX_CHILDREN, -1);
	
	ret = field->fields_array.mf->add_element(&field->fields_array, 
			1, (char **)&field->fields);  
	aos_assert_r(!ret, -1);
	
	field->fields[field->fields_array.noe++] = child;
	return 0;
}


int aos_scm_field_set_lws(
			struct aos_scm_field *field, 
			char *lws, 
			const u32 lws_len)
{
	aos_assert_r(field, -1);
	aos_assert_r(lws, -1);
	aos_assert_r(lws_len <= AOS_MAX_WHITE_SPACE_LEN, -1);

	if (field->lws)
	{
		aos_free(field->lws);
		field->lws = 0;
	}
	aos_assert_r(aos_str_set(&field->lws, lws, lws_len), -1);
	field->lws_len = lws_len;
	return 0;
}


int aos_scm_field_set_tws(
			struct aos_scm_field *field, 
			char *tws, 
			const u32 tws_len)
{
	aos_assert_r(field, -1);
	aos_assert_r(tws, -1);
	aos_assert_r(tws_len <= AOS_MAX_WHITE_SPACE_LEN, -1);

	if (field->tws)
	{
		aos_free(field->tws);
		field->tws = 0;
	}
	aos_assert_r(aos_str_set(&field->tws, tws, tws_len), -1);
	field->tws_len = tws_len;
	return 0;
}


int aos_scm_field_set_lws_gen(
		aos_scm_field_t *field, 
		aos_rvg_t *gen)
{
	aos_assert_r(field, -1);
	aos_assert_r(gen, -1);
	aos_rc_obj_assign(field->lws_gen, gen);
	return 0;
}


int aos_scm_field_set_tws_gen(aos_scm_field_t *field, aos_rvg_t *gen)
{
	aos_assert_r(field, -1);
	aos_assert_r(gen, -1);
	aos_rc_obj_assign(field->tws_gen, gen);
	return 0;
}


int aos_scm_field_set_map1(struct aos_scm_field *field, aos_value_map_t *map)
{
	aos_assert_r(field, -1);
	aos_assert_r(map, -1);
	aos_rc_obj_assign(field->value_map1, map);
	return 0;
}


int aos_scm_field_set_map2(struct aos_scm_field *field, aos_value_map_t *map)
{
	aos_assert_r(field, -1);
	aos_assert_r(map, -1);
	aos_rc_obj_assign(field->value_map2, map);
	return 0;
}


int aos_scm_field_set_cond(struct aos_scm_field *field, aos_cond_t *cond)
{
	aos_assert_r(field, -1);
	aos_assert_r(cond, -1);
	aos_rc_obj_assign(field->presence_cond, cond);
	return 0;
}


// 
// Description
// It serializes its contents into an xml configuration node. 
// Note that this function shall be called by its subclasses. 
// The node 'node' shall have been created for this class. 
//
int aos_scm_field_serialize(
		aos_scm_field_t *field, 
		aos_xml_node_t *node)
{
	// 
	//  <name_based_on_field_type>
	//  	<Name>				// Default: null
	//  	<Type>	
	//  	<StartPos>			// Default: -1
	//  	<Length>			// Default: -1
	//  	<LenUnit>			// Default: eAosLenUnit_Byte
	//  	<LengthType>		// Default: eAosLenType_Fixed
	//  	<DataType>			// Default: eAosDataType_NoDataType
	//  	<MinLen>			// Default: -1
	//  	<MaxLen>			// Default: -1
	//  	<NeedTrimLWS>		// Default: 0
	//  	<LWS>				// Default: null
	//  	<NeedTrimTWS>		// Default: 0
	//  	<TWS>				// Default: null
	//  	<CaseSensitive>		// Default: 1
	//  	<LWS_Gen>			// Default: null
	//  	<TWS_Gen>			// Default: null
	//  	<ValueMap1>			// Default: null
	//  	<ValueMap2>			// Default: null
	//  	<Presence>			// Default: eAosPresence_Mandatory
	//  	<PresenceCond>		// Default: null
	//  	<IsMandatory>		// Default: 0
	//  	<mandatory_ordered>	// Default: 0
	//  	<optional_ordered>	// Default: 0
	//  	<mandatory_first>	// Default: 0
	//      <fields>			// Default: no fields
	//          <field>
	//          ...
	//          </field>
	//          ...
	//      </fields>
	//  </name_based_on_field_type>
	//
	int i;	
	aos_scm_field_t *ff;
	aos_assert_r(field, -1);
	aos_assert_r(node, -1);

	// Set <Name>
	if (field->name)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"Name", field->name, 0, 0), -1);
	}

	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, 
			"Type", aos_scm_field_type_2str(field->type), 0, 0), -1);

	// Set <StartPos>
	aos_assert_r(!node->mf->append_child_int(node, 
			"StartPos", field->start_pos), -1);
	
	// Set <Length>
	aos_assert_r(!node->mf->append_child_int(node, 
			"Length", field->len), -1);
	
	// Set <LenUnit>
	aos_assert_r(!node->mf->append_child_int(node, 
			"LenUnit", field->len_unit), -1);
	
	// Set <LengthType>
	aos_assert_r(!node->mf->append_child_str(node, 
			"LengthType", aos_len_type_2str(field->len), 0, 0), -1);

	// Set <DataType>
	if (field->data_type != eAosDataType_NoDataType)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"DataType", aos_data_type_2str(field->data_type), 0, 0), -1);
	}

	// Set <MinLen>
	aos_assert_r(!node->mf->append_child_int(node, 
			"MinLen", field->min_len), -1);

	// Set <MaxLen>
	aos_assert_r(!node->mf->append_child_int(node, 
			"MaxLen", field->max_len), -1);

	// Set <NeedTrimLWS>
	aos_assert_r(!node->mf->append_child_int(node, 
			"NeedTrimLWS", field->need_trim_lws), -1);

	// Set <LWS>
	aos_assert_r(!node->mf->append_child_hex(node, 
			"LWS", field->lws, field->lws_len), -1);

	// Set <NeedTrimTWS>
	aos_assert_r(!node->mf->append_child_int(node, 
			"NeedTrimTWS", field->need_trim_tws), -1);

	// Set <TWS>
	aos_assert_r(!node->mf->append_child_hex(node, 
			"TWS", field->tws, field->tws_len), -1);

	// Set <CaseSensitive>
	aos_assert_r(!node->mf->append_child_int(node, 
			"CaseSensitivity", field->case_sensitivity), -1);

	// Set <LWS_Gen>
	if (field->lws_gen)
	{
		aos_assert_r(!field->lws_gen->mf->serialize(
			field->lws_gen, node), -1);
	}

	// Set <TWS_Gen>
	if (field->tws_gen)
	{
		aos_assert_r(!field->tws_gen->mf->serialize(
			field->tws_gen, node), -1);
	}

	//  <ValueMap1>
	if (field->value_map1)
	{
		aos_assert_r(!field->value_map1->mf->serialize(
			field->value_map1, node), -1);
	}

	// Set <ValueMap2>
	if (field->value_map2)
	{
		aos_assert_r(!field->value_map2->mf->serialize(
			field->value_map2, node), -1);
	}

	// Set <Presence>
	aos_assert_r(!node->mf->append_child_int(node, 
			"Presence", field->presence_type), -1);

	// Set <PresenceCond>
	if (field->presence_cond)
	{
		aos_assert_r(!field->presence_cond->mf->serialize(
			field->presence_cond, node), -1);
	}

	// Add <mandatory_ordered>
	if (field->mandatory_ordered)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"mandatory_ordered", field->mandatory_ordered), -1);
	}

	// Add <mandatory_first>
	if (field->mandatory_first)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"mandatory_first", field->mandatory_first), -1);
	}

	// Add <optional_ordered>
	if (field->optional_ordered)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"optional_ordered", field->optional_ordered), -1);
	}

	// Add <IsMandatory>
	if (field->is_mandatory)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"IsMandatory", field->is_mandatory), -1);
	}

	// Add <Fields>
	aos_xml_node_t *fields = node->mf->append_child_node(
			node, "Fields");
	aos_assert_r(fields, -1);
	for (i=0; i<field->fields_array.noe; i++)
	{
		ff = field->fields[i];
		aos_assert_r(!ff->mf->serialize(ff, fields), -1);
	}

	return 0;
}


int aos_scm_field_hold(aos_scm_field_t *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(field->ref_count >= 0, -1);
	field->ref_count++;
	return 0;
}


int aos_scm_field_put(aos_scm_field_t *field)
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


int aos_scm_field_deserialize(
		aos_scm_field_t *field, 
		aos_xml_node_t *node)
{
	// 
	// For the XML format, please see the function 'serialize(...)'.
	//
	int len;
	int value;
	char buff[100]; 
	int buff_len = 100;
	aos_xml_node_t *tn;
	aos_scm_field_t *ff;
	aos_assert_r(field, -1);
	aos_assert_r(node, -1);

	// Retrieve <Name>
	aos_assert_r(!node->mf->first_child_str_dft(node, 
			"Name", &field->name, &len, 0), -1);

	// Retrieve <Type>
	aos_assert_r(!node->mf->first_child_str_b(node, 
			"Type", buff, &buff_len, 0), -1);
	field->type = aos_scm_field_type_2enum(buff);
	aos_assert_r(aos_scm_field_type_check(field->type) == 1, -1);

	// Retrieve <StartPos>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"StartPos", &field->start_pos, -1), -1);
	
	// Retrieve <Length>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"Length", &field->len, -1), -1);
	aos_assert_t(field->start_pos > 0, field->len >= 0, -1);
	
	// Retrieve <LenUnit>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"LenUnit", &value, eAosLenUnit_Byte), -1);
	field->len_unit = value;	
	aos_assert_r(aos_len_unit_check(field->len_unit) == 1, -1);

	// Retrieve <LengthType>
	buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, 
			"LengthType", buff, &buff_len, "fixed"), -1);
	field->len_type = aos_len_type_2enum(buff);
	aos_assert_r(aos_len_type_check(field->len_type) == 1, -1);

	// Retrieve <DataType>
	buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, 
			"DataType", buff, &buff_len, "no_data_type"), -1);
	field->data_type = aos_data_type_2enum(buff);
	aos_assert_r(aos_data_type_check(field->data_type) == 1, -1);

	// Retrieve <MinLen>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"MinLen", &field->min_len, -1), -1);

	// Retrieve <MaxLen>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"MaxLen", &field->max_len, -1), -1);

	// Retrieve <NeedTrimLWS>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"NeedTrimLWS", &field->need_trim_lws, 0), -1);

	// Retrieve <LWS>
	aos_assert_r(!node->mf->first_child_hex_dft(node, 
			"LWS", &field->lws, &field->lws_len, "", 0), -1);

	// Retrieve <NeedTrimTWS>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"NeedTrimTWS", &field->need_trim_tws, 0), -1);

	// Retrieve <TWS>
	aos_assert_r(!node->mf->first_child_hex_dft(node, 
			"TWS", &field->tws, &field->tws_len, "", 0), -1);

	// Retrieve <CaseSensitive>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"CaseSensitivity", &field->case_sensitivity, 1), -1);

	// Retrieve <LWS_Gen>
	tn = node->mf->first_named_child(node, "LWS_Gen");
	if (tn)
	{
		field->lws_gen = aos_rvg_factory_xml(tn);
		aos_assert_r(field->lws_gen, -1);
	}

	// Retrieve <TWS_Gen>
	tn = node->mf->first_named_child(node, "TWS_Gen");
	if (tn)
	{
		field->tws_gen = aos_rvg_factory_xml(tn);
		aos_assert_r(field->tws_gen, -1);
	}

	// Retrieve <ValueMap1>
	tn = node->mf->first_named_child(node, "ValueMap1");
	if (tn)
	{
		field->value_map1 = aos_value_map_create(tn);
		aos_assert_r(field->value_map1, -1);
	}

	// Retrieve <ValueMap2>
	tn = node->mf->first_named_child(node, "ValueMap2");
	if (tn)
	{
		field->value_map2 = aos_value_map_create(tn);
		aos_assert_r(field->value_map2, -1);
	}

	// Retrieve <Presence>
	buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, 
			"Presence", buff, &buff_len, "Mandatory"), -1);
	field->presence_type = aos_presence_2enum(buff);
	aos_assert_r(aos_presence_check(field->presence_type) == 1, -1);

	// Retrieve <PresenceCond>
	tn = node->mf->first_named_child(node, "PresenceCond");
	if (tn)
	{
		field->presence_cond = aos_cond_factory(tn);
		aos_assert_r(field->presence_cond, -1);
	}

	// Retrieve <mandatory_ordered>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"mandatory_ordered", &field->mandatory_ordered, 0), -1);

	// Retrieve <optional_ordered>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"optional_ordered", &field->optional_ordered, 0), -1);

	// Retrieve <mandatory_first>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"mandatory_first", &field->mandatory_first, 0), -1);

	// Retrieve <IsMandatory>
	aos_assert_r(!node->mf->first_child_char_dft(node, 
			"IsMandatory", &field->is_mandatory, 0), -1);

	// Retrieve <Fields>
	aos_xml_node_t *fields = node->mf->first_named_child(
			node, "Fields");
	if (fields)
	{
		tn = fields->mf->first_child(fields);
		for (; tn; tn = fields->mf->next_sibling(fields))
		{
			ff = aos_scm_field_factory(tn);
			aos_assert_r(ff, -1);
			aos_assert_r(!field->mf->add_child(field, ff), -1);
		}
	}

	return 0;
}


// 
// Description
// One should call this function before using a schema to parse 
// the contents. This is needed in case the schema is reused to 
// parse multiple messages. 
//
int aos_scm_field_reset_parsing(
		struct aos_scm_field *field, 
		const int start, 
		const int len)
{
	int i;
	aos_assert_r(field, -1);
	field->start_pos = start;
	field->len = len;
	field->is_present = 0;

	for (i=0; i<field->fields_array.noe; i++)
	{
		field->fields[i]->mf->reset_parsing(field->fields[i], -1, 0);
	}

	return 0;
}

int aos_scm_field_set_msg_selector(
		struct aos_scm_field *schema, 
		struct aos_msg_selector *selector)
{
	aos_assert_r(schema, -1);
	aos_assert_r(selector, -1);

	aos_rc_obj_assign(schema->msg_selector, selector);
	return 0;
}


int aos_scm_field_set_present(
		aos_scm_field_t *field, 
		const char value)
{
	aos_assert_r(field, -1);
	aos_assert_int_range_r(value, 0, 1, -1);
	field->is_present = value;
	return 0;
}


int aos_scm_field_parse(
		aos_scm_field_t *field, 
		aos_omparser_t *parser)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_scm_field_dump(
		struct aos_scm_field *field, 
		char *buff, 
		int *buff_len)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_scm_field_integrity_check(aos_scm_field_t *field)
{
	// aos_not_implemented_yet;
	return 0;
}


// 
// Description
// It creates an instance of aos_scm_field_t based on an 
// xml configuration. 
//
aos_scm_field_t *aos_scm_field_factory(aos_xml_node_t *node)
{
	char type[100];
	int type_len = 100;
	aos_scm_field_t *field = 0;
	aos_assert_r(node, 0);
	
	aos_assert_r(!node->mf->first_child_str_b(
			node, "Type", type, &type_len, 0), 0);

	if (strcmp(type, "XML") == 0)
	{
		field = (aos_scm_field_t *)aos_scm_field_xml_create(node);
		aos_assert_r(field, 0);
		return field;
	}

	aos_alarm("Unrecognized node: %s", node->mf->dump((aos_field_t *)node));
	return 0;
}


int aos_scm_field_destroy(aos_scm_field_t *field)
{
	aos_assert_r(field, -1);

	if (field->name) aos_free(field->name);
	if (field->lws) aos_free(field->lws);
	if (field->tws) aos_free(field->tws);
	if (field->lws_gen) aos_assert_r(
			!field->lws_gen->mf->put(field->lws_gen), -1);
	if (field->tws_gen) aos_assert_r(
			!field->tws_gen->mf->put(field->tws_gen), -1);
	if (field->value_map1) aos_assert_r(
			!field->value_map1->mf->put(field->value_map1), -1);
	if (field->value_map2) aos_assert_r(
			!field->value_map2->mf->put(field->value_map2), -1);
	if (field->presence_cond) aos_assert_r(
			!field->presence_cond->mf->put(field->presence_cond), -1);

	if (field->fields)
	{
		int i;
		aos_scm_field_t *ff;
		for (i=0; i<field->fields_array.noe; i++)
		{
			ff = field->fields[i];
			aos_assert_r(ff, -1);
			aos_assert_r(!ff->mf->put(ff), -1);
		}
	}

	aos_assert_r(!field->fields_array.mf->
			release_memory(&field->fields_array), -1);
	field->fields = 0;

	return 0;
}


int aos_scm_field_init(
		aos_scm_field_t *field, 
		const aos_scm_field_type_e type, 
		const char * const name,
		const aos_len_unit_e len_unit,
		const aos_len_type_e len_type, 
		const aos_data_type_e data_type,
		const aos_presence_e presence_type)
{
	aos_assert_r(field, -1);
	aos_assert_r(aos_scm_field_type_check(type) == 1, -1);
	aos_assert_r(aos_len_unit_check(len_unit) == 1, -1);
	aos_assert_r(aos_len_type_check(len_type) == 1, -1);
	aos_assert_r(aos_data_type_check(data_type) == 1, -1);
	aos_assert_r(aos_presence_check(presence_type) == 1, -1);

	field->type = type;
	if (field->name) aos_free(field->name);
	field->name = 0;
	if (name)
	{
		aos_assert_r(!aos_str_set(&field->name, name, strlen(name)), -1);
	}
	field->len_unit = len_unit;
	field->len_type = len_type;
	field->data_type = data_type;
	field->max_len = -1;
	field->presence_type = presence_type;
	aos_assert_r(!aos_dyn_array_init(&field->fields_array, (char **)&field->fields, 
			sizeof(aos_field_t *), 3, AOS_SCM_FIELD_XML_MAX_CHILDREN), -1);
	return 0;
}

