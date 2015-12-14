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
// This class randomly generates character strings based on a charset.
// A charset is defined as:
// 		[(start, end, weight), 
// 		 (start, end, weight),
// 		 ...
// 		 (start, end, weight)]
// When generating a string, it first randomly determines the length
// of the string. It then randomly select characters from the charset
// based on the weights. 
//
// Additional Attributes:
// unique: Indicates whether characters can be repeated. 
// order:  Indicates whether characters in a generated string should
//          be kept the same order as they appear in the character set. 
//
// Modification History:
// 03/15/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rsg_charset.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "rvg_c/rsg.h"
#include "rvg_c/rig.h"
#include "rvg_c/charset.h"
#include "util_c/memory.h"
#include "util_c/buffer.h"
#include "util_c/rc_obj.h"
#include "util_c/util.h"


int aos_rsg_charset_next_value(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	int len;
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *rsg = (aos_rsg_charset_t *)rvg;
	
	if (rsg->length_rig)
	{
		aos_assert_r(!rsg->length_rig->mf->next_int(rsg->length_rig, &len), -1);
	}
	else
	{
		len = aos_next_int(rsg->min_len, rsg->max_len);
	}

	aos_assert_r(!value->mf->set_str(value, 0, len), -1);
	aos_assert_r(rsg->charset, -1);
	aos_assert_r(!rsg->charset->mf->next_str(rsg->charset, 
			value->value.string, len, rsg->repeat, rsg->ordered), -1);
	return 0;
}


int aos_rsg_charset_serialize(
		aos_rvg_t *rvg,
		aos_xml_node_t *parent)
{
	//
	//	<A_Name>
	// 		<Type>CharsetRSG</Type>
	//		<Charset>
	//			<Range>
	//				<Start>an_integer</Range>
	//				<End>an_integer</End>
	//				<Weight>100</Weight>
	//			</Range>
	//			...
	//		</Charset>
	//		<LengthRIG>
	//		<MinLen>
	//		<MaxLen>
	//		<Unique>		// Default: 0
	//		<Ordered>		// Default: 0
	//	</A_Name>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rvg;

	// aos_xml_node_t *node = parent->mf->append_child_node(
	// 		parent, self->name);
	aos_xml_node_t *node = aos_rsg_serialize(rvg, parent);
	aos_assert_r(node, -1);

	// aos_assert_r(!aos_rsg_serialize(rvg, node), -1);

	//
	// Add <Charset>
	//
	aos_assert_r(!self->charset->mf->serialize(
			self->charset, node), -1);

	// Add <LengthRIG>
	if (self->length_rig)
	{
		aos_assert_r(!self->length_rig->mf->serialize(
			(aos_rvg_t *)self->length_rig, node), -1);
	}

	// Add <Unique>
	if (self->repeat)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"Unique", self->repeat), -1);
	}

	// Add <Ordered>
	if (self->ordered)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"Ordered", aos_order_2str(self->ordered), 0, 0), -1);
	}

	if (self->min_len > 0)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"MinLen", self->min_len), -1);
	}

	if (self->max_len > 0)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"MaxLen", self->max_len), -1);
	}

	return 0;
}


int aos_rsg_charset_deserialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *node)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rvg;

	aos_assert_r(!aos_rsg_deserialize(rvg, node), -1);

	//
	// Retrieve <Charset>
	//
	aos_xml_node_t *csn = node->mf->first_named_child(
			node, "Charset");
	aos_assert_r(csn, -1);
	if (self->charset) self->charset->mf->put(self->charset);
	self->charset = aos_charset_create_xml(csn);
	aos_assert_r(self->charset, -1);
	csn->mf->put((aos_field_t *)csn); 

	// Retrieve <LengthRIG>
	if (self->length_rig) self->length_rig->mf->put(
			(aos_rvg_t *)self->length_rig);

	aos_xml_node_t *ln = node->mf->first_named_child(
			   	node, "LengthRIG");
	if (ln)
	{
		self->length_rig = (aos_rig_t *)aos_rvg_factory_xml(ln);
		aos_assert_r(self->length_rig, -1);
		ln->mf->put((aos_field_t *)ln);
	}

	// Retrieve <Repeat>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"Repeat", &self->repeat, 0), -1);

	// Retrieve <Ordered>
	char buff[100];
	int buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, 
			"Ordered", buff, &buff_len, "NoOrder"), -1);
	self->ordered = aos_order_2enum(buff);
	aos_assert_r(aos_order_check(self->ordered) == 1, -1);

	// Retrieve <MinLen>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"MinLength", &self->min_len, 0), -1);

	// Retrieve <MaxLen>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"MaxLength", &self->max_len, 0), -1);

	return 0;
}


int aos_rsg_charset_integrity_check(aos_rvg_t *rsg)
{
	aos_assert_r(rsg, -1);
	aos_assert_r(rsg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rsg;
	aos_assert_r(self->charset, -1);
	aos_assert_r(aos_is_binary(self->ordered) == 1, -1);
	return 1;
}


int aos_rsg_charset_release_memory(aos_rvg_t *rsg)
{
	aos_assert_r(rsg, -1);
	aos_assert_r(rsg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rsg;
	
	if (self->length_rig)
	{
		self->length_rig->mf->put((aos_rvg_t *)self->length_rig);
	}

	if (self->charset)
	{
		self->charset->mf->put(self->charset);
	}
	return 0;
}


int aos_rsg_charset_destroy(aos_rvg_t *rsg)
{
	AOS_DESTROY_OBJ(rsg);
}


// 
// Description:
// It checks whether the string in 'value' can be constructed
// through this rsg. 
static int aos_rsg_charset_domain_check(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rvg;

	if (value->data_size < self->min_len ||
		(self->max_len > 0 && value->data_size > self->max_len))
	{
		return 0;
	}

	aos_assert_r(self->charset->mf->domain_check(
			self->charset, value) == 1, -1);
	return 1;
}


static int aos_rsg_charset_domain_check_len(
			struct aos_rsg *rsg, 
			struct aos_value *value,
			int *idx, 
			const aos_domain_check_e type)
{
	aos_assert_r(rsg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rsg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_charset_t *self = (aos_rsg_charset_t *)rsg;

	aos_assert_r(self->charset->mf->domain_check_len(
			self->charset, value, idx, type, 
			self->min_len, self->max_len) == 1, -1);
	return 1;
}



static aos_rsg_charset_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rsg_charset_integrity_check,
	aos_rsg_charset_next_value,
	aos_rsg_charset_domain_check,
	aos_rsg_charset_serialize,
	aos_rsg_charset_deserialize,
	aos_rsg_charset_release_memory,
	aos_rsg_charset_destroy, 
	aos_rsg_charset_domain_check_len
};


int aos_rsg_charset_init(aos_rsg_charset_t *rsg)
{
	aos_assert_r(rsg, -1);
	memset(rsg, 0, sizeof(*rsg));
	rsg->mf = &sg_mf;
	rsg->type = eAosRvg_RsgCharset;
	rsg->min_len = AOS_RSG_CHARSET_DEFAULT_MINLEN;
	rsg->max_len = AOS_RSG_CHARSET_DEFAULT_MAXLEN;
	return 0;
}


aos_rsg_charset_t *aos_rsg_charset_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_rsg_charset_t *obj = aos_malloc(sizeof(*obj));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rsg_charset_init(obj), cleanup);
	obj->mf->hold((aos_rvg_t *)obj);
	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	obj->mf->release_memory((aos_rvg_t *)obj);
	aos_free(obj);
	return 0;
}


