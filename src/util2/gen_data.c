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
// 02/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/gen_data.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/rc_obj.h"
#include "util_c/tracer.h"


extern aos_gen_data_entry_t *aos_gen_data_entry_create();

AOS_DECLARE_RCOBJ_LOCK;


int aos_gen_data_hold(aos_gen_data_t *data)
{
	AOS_RC_OBJ_HOLD(data);
}


int aos_gen_data_put(aos_gen_data_t *data)
{
	AOS_RC_OBJ_PUT(data);
}


int aos_gen_data_serialize(
		aos_gen_data_t *data, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Variables>
	// 			<Variable>
	// 				<Tag>
	// 				<Value>
	// 			</Variable>
	// 			...
	// 		</Variables>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(data, -1);
	aos_assert_r(parent, -1);

	if (aos_list_empty(&data->values)) return 0;

	aos_xml_node_t *node = parent->mf->append_child_node(parent, "Variables");
	aos_assert_r(node, -1);

	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &data->values, link)
	{
		aos_xml_node_t *nn = node->mf->append_child_node(node, "Variable");
		aos_assert_r(nn, -1);

		// Append <Tag>
		aos_assert_r(!nn->mf->append_child_str(nn, "Tag", 
				aos_gen_data_tag_2str(entry->tag), 0, 0), -1);

		// Append <Value>
		aos_assert_r(!entry->value.mf->serialize(&entry->value, nn), -1);
	}

	return 0;
}


int aos_gen_data_deserialize(
		aos_gen_data_t *data, 
		aos_xml_node_t *node)
{
	// 
	// 	<Variables>
	// 		<Variable>
	// 			<Tag>
	// 			<Value>
	// 		</Variable>
	// 		...
	// 	</Variables>
	//
	aos_assert_r(data, -1);
	aos_assert_r(node, -1);

	aos_xml_node_t *nn = node->mf->first_child(node);
	char buff[100];
	int buff_len = 100;
	while (nn)
	{
		// Retrieve <Tag>
		buff_len = 100;
		aos_assert_r(!nn->mf->first_child_str_b(nn, "Tag", buff, &buff_len, 0), -1);
		aos_gen_data_entry_t *entry = aos_gen_data_entry_create();
		aos_assert_r(entry, -1);
		entry->tag = aos_gen_data_tag_2enum(buff);
		aos_assert_rm(aos_data_type_check(entry->tag) == 1, -1, 
				"tag: %s", buff);

		aos_xml_node_t *vn = nn->mf->first_named_child(nn, "Value");
		aos_assert_r(vn, -1);
		aos_assert_r(!entry->value.mf->deserialize(&entry->value, vn), -1);

aos_trace("Found variable: %s", buff);	
		nn = node->mf->next_sibling(node);

		aos_list_add_tail(&entry->link, &data->values);
	}

	return 0;
}


int aos_gen_data_release_memory(aos_gen_data_t *data) 
{
	aos_assert_r(data, -1);
	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &data->values, link)
	{
		aos_list_del(&entry->link);
		entry->value.mf->release_memory(&entry->value);
		aos_free(entry);
	}

	return 0;
}


int aos_gen_data_destroy(aos_gen_data_t *data) 
{
	aos_assert_r(data, -1);
	data->mf->release_memory(data);
	aos_free(data);
	return 0;
}


int aos_gen_data_set_str(
		struct aos_gen_data *data, 
		const u16 tag, 
		char *value, 
		const int alloc_memory)
{
	aos_assert_r(data, -1);
	aos_gen_data_entry_t *entry = data->mf->get_entry(data, tag, 1);
	aos_assert_r(entry, -1);
	aos_assert_r(!entry->value.mf->set_str(
			&entry->value, value, alloc_memory), -1);
	return 0;
}


int aos_gen_data_set_ptr(
		struct aos_gen_data *data, 
		const u16 tag, 
		void *value) 
{
	aos_assert_r(data, -1);
	aos_gen_data_entry_t *entry = data->mf->get_entry(data, tag, 1);
	aos_assert_r(entry, -1);
	aos_assert_r(!entry->value.mf->set_ptr(&entry->value, value), -1);
	return 0;
}

int aos_gen_data_set_int(
		struct aos_gen_data *data, 
		const u16 tag, 
		const int value) 
{
	aos_assert_r(data, -1);
	aos_gen_data_entry_t *entry = data->mf->get_entry(data, tag, 1);
	aos_assert_r(entry, -1);
	aos_assert_r(!entry->value.mf->set_int(&entry->value, value), -1);
	return 0;
}


int aos_gen_data_exist(
		struct aos_gen_data *data, 
		const u16 tag) 
{
	aos_assert_r(data, -1);

	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &data->values, link)
	{
		if (entry->tag == tag)
		{
			return 1;
		}
	}

	return 0;
}


// 
// Description:
// It retrieves the aos_value_t that matches the tag. If found, 
// 0 is returned and 'value' holds the value (but the hold function
// is not called). If not found, 1 is returned and 'value' is 0.
//
int aos_gen_data_get_value(
		struct aos_gen_data *data, 
		const u16 tag, 
		aos_value_t **value) 
{
	aos_assert_r(data, -1);
	aos_assert_r(value, -1);

	*value = 0;
	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &data->values, link)
	{
		if (entry->tag == tag)
		{
			*value = &entry->value;
			return 0;
		}
	}

	return 1;
}


int aos_gen_data_get_ptr(
		struct aos_gen_data *data, 
		const u16 tag, 
		void **ptr)
{
	aos_value_t *value;
	aos_assert_r(!aos_gen_data_get_value(data, tag, &value), -1);
	aos_assert_r(!value->mf->to_ptr(value, ptr), -1);
	return 0;
}


int aos_gen_data_get_str(
		struct aos_gen_data *data, 
		const u16 tag, 
		char **str,
		int *len)
{
	aos_value_t *vv;
	aos_assert_rm(!aos_gen_data_get_value(data, tag, &vv), -1, 
			"Tag: %s", aos_gen_data_tag_2str(tag));
	aos_assert_r(!vv->mf->to_str(vv, str, len), -1);
	return 0;
}


int aos_gen_data_get_int(
		struct aos_gen_data *data, 
		const u16 tag, 
		int *value)
{
	aos_value_t *vv;
	aos_assert_r(!aos_gen_data_get_value(data, tag, &vv), -1);
	aos_assert_r(!vv->mf->to_int(vv, value), -1);
	return 0;
}


int aos_gen_data_get_int_dft(
		struct aos_gen_data *data, 
		const u16 tag, 
		int *value, 
		const int dft)
{
	aos_value_t *vv = 0;
	aos_assert_r(aos_gen_data_get_value(data, tag, &vv) >= 0, -1);
	if (vv)
	{
		aos_assert_r(!vv->mf->to_int(vv, value), -1);
	}
	else
	{
		*value = dft;
	}

	return 0;
}


int aos_gen_data_get_u32(
		struct aos_gen_data *data, 
		const u16 tag, 
		u32 *value)
{
	aos_value_t *vv;
	aos_assert_r(!aos_gen_data_get_value(data, tag, &vv), -1);
	aos_assert_r(!vv->mf->to_u32(vv, value), -1);
	return 0;
}


// 
// Description:
// It retrieves the data with the tag. If not found, and if 'flag'
// is non-zero, it will create one and insert it.
//
aos_gen_data_entry_t *aos_gen_data_get_entry(
		aos_gen_data_t *data, 
		const aos_gen_data_tag_e tag, 
		const int flag)
{
	aos_assert_r(data, 0);

	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &data->values, link)
	{
		if (entry->tag == tag)
		{
			return entry;
		}
	}

	if (flag)
	{
		// Need to add the tag
		entry = aos_gen_data_entry_create();
		aos_assert_r(entry, 0);
		entry->tag = tag;
		aos_list_add_tail(&entry->link, &data->values);
		return entry;
	}

	return 0;
}


int aos_gen_data_set_u32(
		struct aos_gen_data *data, 
		const u16 tag, 
		const u32 value) 
{
	aos_assert_r(data, -1);
	aos_gen_data_entry_t *entry = data->mf->get_entry(data, tag, 1);
	aos_assert_r(entry, -1);
	aos_assert_r(!entry->value.mf->set_u32(&entry->value, value), -1);
	return 0;
}


int aos_gen_data_is_empty(aos_gen_data_t *data)
{
	aos_assert_r(data, -1);
	return aos_list_empty(&data->values);
}


static aos_gen_data_mf_t sg_mf = 
{
	aos_gen_data_hold,
	aos_gen_data_put,
	aos_gen_data_serialize,
	aos_gen_data_deserialize,
	aos_gen_data_release_memory,
	aos_gen_data_destroy,
	aos_gen_data_set_str,
	aos_gen_data_set_int,
	aos_gen_data_set_u32,
	aos_gen_data_set_ptr,
	aos_gen_data_get_value,
	aos_gen_data_get_entry,
	aos_gen_data_get_ptr,
	aos_gen_data_get_str,
	aos_gen_data_get_int,
	aos_gen_data_get_int_dft,
	aos_gen_data_get_u32,
	aos_gen_data_exist
};


int aos_gen_data_init(aos_gen_data_t *data)
{
	aos_assert_r(data, -1);
	memset(data, 0, sizeof(aos_gen_data_t));
	data->mf = &sg_mf;
	AOS_INIT_LIST_HEAD(&data->values);
	return 0;
}


aos_gen_data_entry_t *aos_gen_data_entry_create()
{
	aos_gen_data_entry_t *entry = aos_malloc(sizeof(aos_gen_data_entry_t));
	aos_assert_r(entry, 0);
	memset(entry, 0, sizeof(aos_gen_data_entry_t));
	AOS_INIT_LIST_HEAD(&entry->link);
	aos_value_init(&entry->value);
	return entry;
}


aos_gen_data_t * aos_gen_data_create()
{
	aos_gen_data_t *dd = aos_malloc(sizeof(aos_gen_data_t));
	aos_assert_r(dd, 0);
	aos_assert_g(!aos_gen_data_init(dd), cleanup);
	return dd;

cleanup:
	aos_free(dd);
	return 0;
}

int aos_gen_data_clone(
		aos_gen_data_t **to, 
		aos_gen_data_t *from)
{
	aos_assert_r(to, -1);
	aos_assert_r(from, -1);

	*to = aos_gen_data_create();
	aos_assert_r(*to, -1);
	return aos_gen_data_copy(*to, from);
}


aos_gen_data_entry_t *aos_gen_data_entry_clone(
		aos_gen_data_entry_t *from)
{
	aos_assert_r(from, 0);
	aos_gen_data_entry_t *entry = aos_malloc(sizeof(aos_gen_data_entry_t));
	aos_assert_r(entry, 0);
	AOS_INIT_LIST_HEAD(&entry->link);
	aos_assert_g(!from->value.mf->copy(&from->value, &entry->value), cleanup);
	entry->tag = from->tag;
	return entry;

cleanup:
	aos_free(entry);
	return 0;
}


int aos_gen_data_copy(aos_gen_data_t *to, aos_gen_data_t *from)
{
	aos_assert_r(to, -1);
	aos_assert_r(from, -1);

	aos_gen_data_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &from->values, link)
	{
		aos_gen_data_entry_t *ee = aos_gen_data_entry_clone(entry);
		aos_assert_r(ee, -1);
		aos_list_add_tail(&ee->link, &to->values);
	}

	return 0;
}

