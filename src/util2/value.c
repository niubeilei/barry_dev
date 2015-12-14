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
// 02/02/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/value.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "porting_c/addr.h"
#include "porting_c/get_errno.h"
#include "util_c/memory.h"
#include "util_c/rc_obj.h"
#include "util_c/file.h"
#include "util_c/util.h"
#include "util_c/strutil.h"
#include "util2/global_data.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <errno.h>


AOS_DECLARE_RCOBJ_LOCK;

int aos_value_integrity_check(aos_value_t *value)
{
	aos_assert_r(value, -1);
	aos_assert_r(aos_data_type_check(value->type) == 1, -1);
	aos_assert_r(value->data_size <= value->mem_size, -1);
	aos_assert_r(value->mem_size >= 0, -1);
	return 1;
}


// 
// Description
// It sets the memory no less than 'len'. If memory needs to be
// re-allocated, it will copy the old values into the new memory.
//
// Returns
// 0 if success. 
// Negative if failed. 
//
int aos_value_set_memory(
		aos_value_t *value, 
		const int len, 
		const int keep_contents)
{
	aos_assert_r(value, -1);
	aos_assert_r(len > 0, -1);

	char *tmp = 0;
	if (len < value->mem_size) 
	{
		aos_assert_r(!keep_contents, -1);
		if (value->mem_size - len >= AOS_VALUE_TOO_BIG)
		{
			// The memory shrunk to small enough that we need to 
			// reallocate memory for it.
			tmp = aos_malloc(len+1);
			aos_assert_r(tmp, -1);
			if (value->data_size > 0)
			{
				aos_assert_g(value->value.string, cleanup);
				aos_free(value->value.string);
			}
			value->value.string= tmp;
		}
		value->mem_size = len;
		value->data_size = 0;
		value->value.string[0] = 0;
		return 0;
	}
	
	// Need to allocate new memory
	aos_assert_t(value->data_size > 0, value->value.string, -1);
	tmp = aos_malloc(len+1);
	aos_assert_r(tmp, -1);
	if (value->data_size > 0)
	{
		if (keep_contents)
		{
			memcpy(tmp, value->value.string, value->data_size);
		}
		else
		{
			value->data_size = 0;
		}
		value->value.string[value->data_size] = 0;

		aos_free(value->value.string);
	}
	value->value.string = tmp;
	value->mem_size = len;
	return 0;

cleanup:
	if (tmp) aos_free(tmp);
	return -1;
}


int aos_value_serialize(
		aos_value_t *value, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		<Value type="the_type" [value="thevalue"] [filename="filename"] [length="len"]>
	// 			...
	// 		</Value>
	// 	</Parent>
	//
	aos_assert_r(value, -1);
	aos_assert_r(parent, -1);

	aos_xml_node_t *node = parent->mf->append_child_node(parent, "Value");
	aos_assert_r(node, -1);

	// Set <Type>
	aos_assert_r(!node->mf->add_attr((aos_field_t *)node, "type", 
			aos_data_type_2str(value->type)), -1);

	// Set filename
	if (value->filename)
	{
		aos_assert_r(!node->mf->add_attr((aos_field_t *)node, "filename", value->filename), -1);
	}

	// Set length
	if (value->bin_len)
	{
		aos_assert_r(!node->mf->add_attr((aos_field_t *)node, "length", 
			aos_str_int_to_str(value->bin_len)), -1);
	}

	// Set <Value>
	char *buff;
	int len;
	aos_assert_r(!value->mf->to_str(value, &buff, &len), -1);
	aos_assert_r(!node->mf->set_contents((aos_field_t *)node, buff, len), -1);
	aos_free(buff);

	return 0;
}


int aos_value_deserialize(
		aos_value_t *value, 
		aos_xml_node_t *node)
{
	// 
	// 	<Value type="the_type" [value="thevalue"] [filename="filename"] [length="len"]>
	// 		...
	// 	</Value>
	//
	int ret;
	aos_assert_r(value, -1);
	aos_assert_r(node, -1);

	// Retrieve type
	char *type = node->mf->get_attr((aos_field_t *)node, "type");
	aos_assert_r(type, -1);
	value->type = aos_data_type_2enum(type);
	aos_assert_rm(aos_data_type_check(value->type) == 1, -1, "Type: %s", type);

 	u64 uv;
	switch (value->type)
	{
	case eAosDataType_char:
		 {
		 char c;
		 char *vv = node->mf->get_attr((aos_field_t *)node, "value");
		 if (vv)
		 {
			 c = vv[0];
		 }
		 else
		 {
		 	aos_assert_r(!node->mf->first_child_char(
				node, "Value", &c), -1);
		 }
		 aos_assert_r(!value->mf->set_char(value, c), -1);
		 }
		 break;

	case eAosDataType_int16:
		 {
		 	int64_t v;
		 	ret = node->mf->get_attr_int64((aos_field_t *)node, "value", &v);
		 	aos_assert_r(ret == 0 || ret == 1, -1);
		 	if (ret == 0)
		 	{
		 		aos_assert_r(!node->mf->get_int64((aos_field_t *)node, &v), -1);
		 	}

		 	aos_assert_r(aos_check_int16(v) == 1, -1);
		 	aos_assert_r(!value->mf->set_int16(value, (int64_t)v), -1);
		 }
		 break;

	case eAosDataType_int32:
		 {
		 	int64_t v;
			ret = node->mf->get_attr_int64((aos_field_t *)node, "value", &v);
			aos_assert_r(ret == 0 || ret == 1,-1);
			if (ret == 0)
			{
				aos_assert_r(!node->mf->get_int64((aos_field_t *)node, &v), -1);
			}
		 	aos_assert_r(aos_check_int32(v) == 1, -1);

		 	aos_assert_r(!value->mf->set_int(value, (int64_t)v), -1);
		 }
		 break;

	case eAosDataType_int64:
	case eAosDataType_u64:
	case eAosDataType_float:
	case eAosDataType_double:
	case eAosDataType_NoDataType:
	case eAosDataType_binary:
		 aos_not_implemented_yet;
		 return -1;

	case eAosDataType_u8:
		 ret = node->mf->get_attr_u64((aos_field_t *)node, "value", &uv);
		 aos_assert_r(ret == 0 || ret == 1, -1);
		 if (ret == 0)
		 {
			aos_assert_r(!node->mf->get_u64((aos_field_t *)node, &uv), -1);
		 }
		 aos_assert_r(aos_check_u8(uv) == 1, -1);
		 aos_assert_r(!value->mf->set_u8(value, (u8)uv), -1);
		 break;

	case eAosDataType_u16:
		 ret = node->mf->get_attr_u64((aos_field_t *)node, "value", &uv);
		 aos_assert_r(ret == 0 || ret == 1, -1);
		 if (ret == 0)
		 {
			aos_assert_r(!node->mf->get_u64((aos_field_t *)node, &uv), -1);
		 }
		 aos_assert_r(aos_check_u8(uv) == 1, -1);
		 aos_assert_r(!value->mf->set_u16(value, (u16)uv), -1);
		 break;

	case eAosDataType_u32:
		 ret = node->mf->get_attr_u64((aos_field_t *)node, "value", &uv);
		 aos_assert_r(ret == 0 || ret == 1, -1);
		 if (ret == 0)
		 {
			aos_assert_r(!node->mf->get_u64((aos_field_t *)node, &uv), -1);
		 }
		 aos_assert_r(aos_check_u8(uv) == 1, -1);
		 aos_assert_r(!value->mf->set_u32(value, (u32)uv), -1);
		 break;

	case eAosDataType_ip_addr: 
		 ret = node->mf->get_attr_u64((aos_field_t *)node, "value", &uv);
		 aos_assert_r(ret == 0 || ret == 1, -1);
		 if (ret == 0)
		 {
			aos_assert_r(!node->mf->get_u64((aos_field_t *)node, &uv), -1);
		 }
		 aos_assert_r(aos_check_u8(uv) == 1, -1);
		 aos_assert_r(!value->mf->set_u64(value, uv), -1);
		 break;

	case eAosDataType_string:
		 {
		 	char *contents;
			char *filename;

			// Check whether to retrieve from file (src="filename")
			filename = node->mf->get_attr((aos_field_t *)node, "src");

			// Handler the case value="the contents"
		 	contents = node->mf->get_attr((aos_field_t *)node, "value");

			int len;
		 	if (filename)
		 	{
				// This means that the value should be read from file.
				// The file name is stored in 'buff'
				aos_assert_r(!contents, -1);
				aos_assert_r(!aos_file_read_file(filename, &contents, &len), -1);
				aos_assert_r(!aos_str_set(&value->filename, filename, strlen(filename)), -1);
		 	}
			else
			{
				// Not from file. 
				if (value->filename) aos_free(value->filename);
				value->filename = 0;

				if (!contents)
				{
					// Read the contents from the tag.
					aos_assert_r(!node->mf->get_str_b((aos_field_t *)node, &contents), -1);
				}
				len = strlen(contents);
			}

		 	aos_assert_r(!value->mf->set_str(value, contents, len), -1);
		 }
		 break;

	default:
		 aos_alarm("Unrecognized type: %d", value->type);
		 return -1;
	}

	return 0;
}


int aos_value_to_char(struct aos_value *value, char *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (char)value->value.u8_value;;
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_to_int16(struct aos_value *value, int16_t *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);
	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = (int16_t)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (int16_t)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 *vv = value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = value->value.u16_value;
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_to_int(struct aos_value *value, int *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = (int)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (int)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 *vv = value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = (int)value->value.u16_value;
		 return 0;

	case eAosDataType_int32:
		 *vv = value->value.int16_value;
		 return 0;

	case eAosDataType_u32:
		 aos_assert_r(value->value.u32_value <= SHRT_MAX, -1);
		 *vv = value->value.u32_value;
		 return 0;

	case eAosDataType_string:
	case eAosDataType_binary:
		 {
			 int64_t v;
		 	 aos_assert_r(!aos_value_atoll(value->value.string, &v), -1);
			 aos_assert_r(v <= LONG_MAX, -1);
			 *vv = v;
		 }
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_to_int32(struct aos_value *value, int *vv)
{
	return aos_value_to_int(value, vv);
}


int aos_value_to_int64(struct aos_value *value, int64_t *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = (int64_t)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (int64_t)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 *vv = (int64_t)value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = (int64_t)value->value.u16_value;
		 return 0;

	case eAosDataType_int32:
		 *vv = (int64_t)value->value.int32_value;
		 return 0;

	case eAosDataType_u32:
		 *vv = (int64_t)value->value.u32_value;
		 return 0;

	case eAosDataType_int64:
		 *vv = value->value.int64_value;
		 return 0;

	case eAosDataType_u64:
		 aos_assert_r(value->value.u64_value <= LLONG_MAX, -1);
		 *vv = (int64_t)value->value.u64_value;
		 return 0;

	case eAosDataType_string:
	case eAosDataType_binary:
		 aos_assert_r(!aos_value_atoll(value->value.string, vv), -1);
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_to_u8(struct aos_value *value, u8 *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_u8:
		 *vv = value->value.u8_value;
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return 0;
}


int aos_value_to_u16(struct aos_value *value, u16 *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = (u16)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (u16)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 aos_assert_r(value->value.int16_value >= 0, -1);
		 *vv = (u16)value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = value->value.u16_value;
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return 0;
}


int aos_value_to_u32(struct aos_value *value, u32 *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 aos_assert_r(value->value.int8_value >= 0, -1);
		 *vv = (u32)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (u32)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 aos_assert_r(value->value.int16_value >= 0, -1);
		 *vv = (u32)value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = (u32)value->value.u16_value;
		 return 0;

	case eAosDataType_int32:
		 aos_assert_r(value->value.int32_value >= 0, -1);
		 *vv = (u32)value->value.int32_value;
		 return 0;

	case eAosDataType_u32:
	case eAosDataType_ip_addr:
		 *vv = (u32)value->value.u32_value;
		 return 0;

	case eAosDataType_string:
	case eAosDataType_binary:
		 {
			 int64_t v;
		 	 aos_assert_r(!aos_value_atoll(value->value.string, &v), -1);
			 aos_assert_r(v >= 0 && v <= UINT_MAX, -1);
			 *vv = v;
			 return 0;
		 }

	default:
		 aos_alarm("Data incompatible: %s", aos_data_type_2str(value->type));
		 return -1;
	}
	aos_should_never_come_here;
	return 0;
}


int aos_value_to_u64(struct aos_value *value, u64 *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 aos_assert_r(value->value.int8_value >= 0, -1);
		 *vv = (u64)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (u64)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 aos_assert_r(value->value.int16_value >= 0, -1);
		 *vv = (u64)value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = (u64)value->value.u16_value;
		 return 0;

	case eAosDataType_int32:
		 aos_assert_r(value->value.int32_value >= 0, -1);
		 *vv = (u64)value->value.int32_value;
		 return 0;

	case eAosDataType_u32:
		 *vv = (u64)value->value.u32_value;
		 return 0;

	case eAosDataType_int64:
		 aos_assert_r(value->value.int64_value >= 0, -1);
		 *vv = (u64)value->value.int64_value;
		 return 0;

	case eAosDataType_u64:
		 *vv = value->value.u64_value;
		 return 0;

	case eAosDataType_string:
	case eAosDataType_binary:
		 {
			 int64_t v;
		 	 aos_assert_r(!aos_value_atoll(value->value.string, &v), -1);
			 aos_assert_r(v >= 0, -1);
			 *vv = (u64)v;
			 return 0;
		 }

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return 0;
}


int aos_value_to_double(struct aos_value *value, double *vv)
{
	aos_assert_r(value, -1);
	aos_assert_r(vv, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	switch (value->type)
	{    
	case eAosDataType_char:
		 *vv = (double)value->value.int8_value;
		 return 0;

	case eAosDataType_u8:
		 *vv = (double)value->value.u8_value;
		 return 0;

	case eAosDataType_int16:
		 *vv = (double)value->value.int16_value;
		 return 0;

	case eAosDataType_u16:
		 *vv = (double)value->value.u16_value;
		 return 0;

	case eAosDataType_int32:
		 *vv = (double)value->value.int32_value;
		 return 0;

	case eAosDataType_u32:
		 *vv = (double)value->value.u32_value;
		 return 0;

	case eAosDataType_int64:
		 *vv = (double)value->value.int64_value;
		 return 0;

	case eAosDataType_u64:
		 *vv = (double)value->value.u64_value;
		 return 0;

	case eAosDataType_float:
		 *vv = (double)value->value.double_value;
		 return 0;

	case eAosDataType_double:
		 *vv = (double)value->value.double_value;
		 return 0;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return 0;
}


// 
// Description:
// It converts the contents into a string and returns the value the pointer
// pointing to the string. The caller should use the pointer immediately
// and should not modify the contents. 
//
int aos_value_to_str_s(
		aos_value_t *value, 
		char **buff, 
		int *len)
{
	int local_len;
	char * local_buff = aos_get_global_str1(&local_len);
	aos_assert_r(local_buff, -1);
	aos_assert_r(local_len > 8, -1);
	aos_assert_r(buff, -1);

	*buff = local_buff;
	*len = 0;
	switch (value->type)
	{    
	case eAosDataType_binary:
	case eAosDataType_string:
	case eAosDataType_NoDataType:
		 aos_assert_r(value->data_size >= 0, -1);
		 *len = value->data_size;
		 *buff = value->value.string;
		 return 0;

	case eAosDataType_char:
		 aos_assert_r(*len > 1, -1);
		 local_buff[0] = (char)value->value.int8_value;
		 local_buff[1] = 0;
		 *len = 1;
		 return 0;

	case eAosDataType_u8:
		 aos_assert_r(*len > 1, -1);
		 local_buff[0] = (char)value->value.u8_value;
		 local_buff[1] = 0;
		 *len = 1;
		 return 0;

	case eAosDataType_int16:
		 aos_assert_r(local_len > 6, -1);
		 sprintf(local_buff, "%hd", value->value.int16_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_int32:
		 aos_assert_r(local_len > 11, -1);
		 sprintf(local_buff, "%d", value->value.int32_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_int64:
		 aos_assert_r(local_len > 21, -1);
		 sprintf(local_buff, "%lld", value->value.int64_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_u16:
		 aos_assert_r(local_len > 6, -1);
		 sprintf(local_buff, "%hu", (unsigned short)value->value.u16_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_u32:
		 aos_assert_r(local_len > 11, -1);
		 sprintf(local_buff, "%lu", (long unsigned int)value->value.u32_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_u64:
		 aos_assert_r(local_len > 21, -1);
		 sprintf(local_buff, "%llu", (unsigned long long)value->value.u64_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_float:
	case eAosDataType_double:
		 aos_assert_r(local_len > 30, -1);
		 sprintf(local_buff, "%.5f", value->value.double_value);
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_ip_addr: 
		 aos_assert_r(local_len > 17, -1);
		 sprintf(local_buff, "%s", aos_addr_to_str((u32)value->value.u32_value));
		 *len = strlen(local_buff);
		 return 0;

	case eAosDataType_ptr: 
		 aos_alarm("Cannot convert ptr into a string");
		 return -1;

	case eAosDataType_Buffer:
		 aos_alarm("Cannot convert buffer into a string");
		 return -1;

	default:
		 aos_alarm("Data incompatible: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


// 
// Description:
// It converts the contents into a string and returns the value through 'buff'. 
// It copies the results into the buffer 'buff'. The caller is responsible to
// allocate enough memory, which is indicated by 'len'. 
//
// The following types cannot be converted into strings:
//	eAosDataType_ptr, 
//	eAosDataType_Buffer,
//
int aos_value_to_str_b(
		aos_value_t *value, 
		char *buff, 
		int *len)
{
	aos_assert_r(value, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(len, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	int local_len;
	char *local_buff;
	aos_assert_r(!aos_value_to_str_s(value, &local_buff, &local_len), -1);
	aos_assert_r(*len > local_len, -1);
	memcpy(buff, local_buff, local_len);
	buff[local_len] = 0;
	return 0;
}


int aos_value_to_str(
		aos_value_t *value, 
		char **buff, 
		int *len)
{
	aos_assert_r(value, -1);
	aos_assert_r(buff, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	int local_len;
	char *local_buff;
	aos_assert_r(!aos_value_to_str_s(value, &local_buff, &local_len), -1);

	*buff = aos_malloc(local_len + 1);
	aos_assert_r(*buff, -1);
	memcpy(*buff, local_buff, local_len);
	(*buff)[local_len] = 0;
	return 0;
}


int aos_value_to_ptr(struct aos_value *value, void **ptr)
{
	aos_assert_r(value, 0);
	aos_assert_r(ptr, 0);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	aos_assert_r(value->type == eAosDataType_ptr, -1);
	memcpy(ptr, value->value.ptr, 4);
	return 0;
}


int aos_value_clear_memory(aos_value_t *value)
{
	aos_assert_r(value, -1);
	switch (value->type)
	{
	case eAosDataType_string:
	case eAosDataType_binary:
		 if (value->value.string) aos_free(value->value.string);
		 value->value.string = 0;
		 value->type = eAosDataType_NoDataType;
		 return 0;

	default:
		 return 0;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_set_char(struct aos_value *value, const char v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_char;
	value->value.int8_value= v;
	return 0;
}


int aos_value_set_int16(struct aos_value *value, const int16_t v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_int16;
	value->value.int16_value= v;
	return 0;
}


int aos_value_set_int(struct aos_value *value, const int v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_int32;
	value->value.int32_value= v;
	return 0;
}


int aos_value_set_int32(struct aos_value *value, const int32_t v)
{
	return aos_value_set_int(value, v);
}


int aos_value_set_int64(struct aos_value *value, const int64_t v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_int64;
	value->value.int64_value= v;
	return 0;
}


int aos_value_set_u8(struct aos_value *value, const u8 v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_u8;
	value->value.u8_value= v;
	return 0;
}


int aos_value_set_u16(struct aos_value *value, const u16 v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_u16;
	value->value.u16_value= v;
	return 0;
}


int aos_value_set_u32(struct aos_value *value, const u32 v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_u32;
	value->value.u32_value= v;
	return 0;
}


int aos_value_set_u64(struct aos_value *value, const u64 v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_u64;
	value->value.u64_value= v;
	return 0;
}


int aos_value_set_double(struct aos_value *value, const double v)
{
	aos_assert_r(value, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_double;
	value->value.double_value= v;
	return 0;
}


// 
// Description
// It appends a string to the value. If 'str' is null, it serves
// to allocate memory only. If successful, data_size is incremented
// by 'len' (regardless of whether 'str' is null or not). 
//
int aos_value_append_str(
		aos_value_t *value, 
		const char *str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(len > 0, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);
	aos_assert_r(value->type == eAosDataType_string ||
				 value->type == eAosDataType_binary, -1);

	int pos = value->data_size;
	aos_assert_r(!aos_value_set_memory(
			value, value->data_size+len+1, 1), -1);

	value->data_size += len;
	if (str) 
	{
		memcpy(&value->value.string[pos], str, len);
	}
	value->value.string[value->data_size] = 0;
	return 0;
}


// 
// Description
// It creates the memory big enough to hold 'len' number of characters. 
// If 'v' is not null, it copies its contents into the allocated memory.
// If 'v' is null, this function serves to allocate memory.
//
// IMPORTANT: if successful, data_size is set to len regardless of
// whether 'v' is null or not.
//
// IMPORTANT: regardless of what its type was before calling this
// function, it will be set to 'eAosDataType_string'.
//
int aos_value_set_str(
		aos_value_t *value, 
		const char *v, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(len >= 0, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	aos_assert_r(!aos_value_set_memory(value, len+1, 0), -1);

	value->data_size = len;
	if (v) 
	{
		memcpy(value->value.string, v, len);
		value->value.string[len] = 0;
	}
	else
	{
		value->value.string[0] = 0;
	}
	value->type = eAosDataType_string;
	return 0;
}


int aos_value_set_binary(
		aos_value_t *value, 
		const char *v, 
		const int len)
{
	aos_assert_r(!aos_value_set_str(value, v, len), -1);
	value->type = eAosDataType_string;
	return 0;
}


int aos_value_set_ptr(struct aos_value *value, void *ptr)
{
	aos_assert_r(value, -1);
	aos_assert_r(ptr, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);
	aos_assert_r(!aos_value_clear_memory(value), -1);
	value->type = eAosDataType_ptr;
	value->value.ptr = ptr;
	return 0;
}


/*
// 
// Description
// The caller has allocated the memory. This function checks whether 
// the memory is bigger than mem_len. If yes, it copies the contents 
// and frees the memory. Otherwise, it holds the memory. 
//
int aos_value_set_mem(
		struct aos_value *value, 
		char *mem, 
		const int mem_len)
{
	aos_assert_r(value, -1);
	aos_assert_r(mem, -1);
	aos_assert_r(mem_len >= 0, -1);
	aos_assert_r(value->mf->integrity_check(value) == 1, -1);

	aos_assert_r(!aos_value_set_memory(value, mem_len), -1);
	
	memcpy(value->value.string, mem, mem_len);
	value->data_size = mem_len;
	value->type = eAosDataType_string;
	if (mem_len <= AOS_VALUE_DEFAULT_MEMORY_SIZE)
	{
		aos_free(mem);
	}

	return 0;
}
*/


int aos_value_hold(aos_value_t *value)
{
	AOS_RC_OBJ_HOLD(value);
}


int aos_value_put(aos_value_t *value)
{
	AOS_RC_OBJ_PUT(value);
}


int aos_value_release_memory(aos_value_t *value)
{
	aos_assert_r(value, -1);
	if (value->type == eAosDataType_string || value->type == eAosDataType_binary)
	{
		if (value->value.string) aos_free(value->value.string);
	}

	value->data_size = 0;
	if (value->filename) aos_free(value->filename);
	return 0;
}


int aos_value_destroy(aos_value_t *value)
{
	aos_assert_r(value, -1);
	aos_assert_r(!value->mf->release_memory(value), -1);
	aos_free(value);
	return 0;
}



int aos_value_copy(aos_value_t *from, aos_value_t *to)
{
	aos_assert_r(to, -1);
	aos_assert_r(from, -1);
	aos_assert_r(!to->mf->release_memory(to), -1);
	memcpy(&to->value, &from->value, sizeof(from->value));

	if (from->type == eAosDataType_string || from->type == eAosDataType_binary)
	{
		to->value.string = aos_malloc(from->data_size+1);
		aos_assert_r(to->value.string, -1);
		memcpy(to->value.string, from->value.string, from->data_size);
		to->value.string[from->data_size] = 0;
	}
	to->type = from->type;
	to->data_size = from->data_size;
	if (to->filename) aos_free(to->filename);
	to->filename = 0;
	if (from->filename)
	{
		aos_assert_r(!aos_str_set(&to->filename, from->filename, strlen(from->filename)), -1);
	}
	return 0;
}




static int aos_value_set_int16_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, 0);
	aos_assert_r(str, 0);

	char *endptr = 0;
	int64_t vv = strtoll(str, &endptr, 10);
	aos_assert_r(vv >= SHRT_MIN && vv <= SHRT_MAX, 0);
	aos_assert_t(endptr, (int)endptr - (int)str == len, 0);
	aos_assert_r(!value->mf->set_int16(value, (int16_t)vv), -1);
	return 0;
	
}


int aos_value_set_int32_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, 0);
	aos_assert_r(str, 0);

	char *endptr = 0;
	int64_t vv = strtoll(str, &endptr, 10);
	aos_assert_r(vv >= INT_MIN && vv <= INT_MAX, 0);
	aos_assert_t(endptr, (int)endptr - (int)str == len, 0);
	aos_assert_r(!value->mf->set_int32(value, (int32_t)vv), -1);
	return 0;
	
}


int aos_value_set_int64_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, 0);
	aos_assert_r(str, 0);

	char *endptr = 0;
	int64_t vv = strtoll(str, &endptr, 10);
	aos_assert_r(!((vv == LLONG_MIN || vv == LLONG_MAX) && errno == ERANGE), -1);
	aos_assert_t(endptr, (int)endptr - (int)str == len, 0);
	aos_assert_r(!value->mf->set_int64(value, vv), -1);
	return 0;
	
}


int aos_value_set_u16_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(str, -1);

	char *endptr = 0;
	int64_t vv = strtoll(str, &endptr, 10);
	aos_assert_r(vv >= 0 && vv <= USHRT_MAX, -1);
	aos_assert_t(endptr, (int)endptr - (int)str == len, -1);
	aos_assert_r(!value->mf->set_u16(value, vv), -1);
	return 0;
	
}


int aos_value_set_u32_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(str, -1);

	char *endptr = 0;
	int64_t vv = strtoll(str, &endptr, 10);
	aos_assert_r(vv >= 0 && vv <= UINT_MAX, -1);
	aos_assert_t(endptr, (int)endptr - (int)str == len, -1);
	aos_assert_r(!value->mf->set_u32(value, vv), -1);
	return 0;
	
}


int aos_value_set_addr_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(str, -1);

	u32 vv;
	aos_assert_r(!aos_addr_to_u32(str, &vv), -1);
	aos_assert_r(!aos_value_set_u32(value, vv), -1);
	return 0;
	
}


int aos_value_set_u64_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(str, -1);

	char *endptr = 0;
	u64 vv = (u64)strtoll(str, &endptr, 10);
	aos_assert_t(endptr, (int)endptr - (int)str == len, -1);
	aos_assert_r(!value->mf->set_u64(value, vv), -1);
	return 0;
	
}


int aos_value_set_double_from_str(
		aos_value_t *value, 
		const char * const str, 
		const int len)
{
	aos_assert_r(value, -1);
	aos_assert_r(str, -1);

	char *endptr = 0;
	double vv = strtod(str, &endptr);
	aos_assert_t(endptr, (int)endptr - (int)str == len, -1);
	aos_assert_r(!(vv == 0 && endptr == str), -1);
	aos_assert_r(!((HUGE_VAL == vv || -HUGE_VAL == vv) && errno == ERANGE), -1);
	aos_assert_r(!value->mf->set_double(value, vv), -1);
	return 0;
}


int aos_value_set_value(
		struct aos_value *value, 
		const aos_data_type_e type, 
		const char * const buff, 
		const int len)
{
	switch (type)
	{
	case eAosDataType_Unknown:
	case eAosDataType_NoDataType:
		 aos_alarm("Data type unknown");
		 return -1;

	case eAosDataType_char:
		 return value->mf->set_char(value, buff[0]);

	case eAosDataType_int16:
		 return value->mf->set_int16_from_str(value, buff, len);

	case eAosDataType_int32:
		 return value->mf->set_int32_from_str(value, buff, len);

	case eAosDataType_int64:
		 return value->mf->set_int64_from_str(value, buff, len);

	case eAosDataType_u8:
		 return value->mf->set_u8(value, buff[0]);

	case eAosDataType_u16:
		 return value->mf->set_u16_from_str(value, buff, len);

	case eAosDataType_u32:
		 return value->mf->set_u32_from_str(value, buff, len);

	case eAosDataType_u64:
		 return value->mf->set_u64_from_str(value, buff, len);

	case eAosDataType_string:
		 return value->mf->set_str(value, buff, len);

	case eAosDataType_float:
	case eAosDataType_double:
		 return value->mf->set_double_from_str(value, buff, len);

	case eAosDataType_binary:
		 return value->mf->set_binary(value, buff, len);

	case eAosDataType_ip_addr:
		 return value->mf->set_addr_from_str(value, buff, len);

	default:
		 aos_alarm("Cannot set value for type: %s", aos_data_type_2str(type));
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


static aos_value_mf_t sg_mf = 
{
	aos_value_hold,
	aos_value_put,
	aos_value_serialize,
	aos_value_deserialize,
	aos_value_destroy,
	aos_value_release_memory,
	aos_value_to_char,
	aos_value_to_int16,
	aos_value_to_int,
	aos_value_to_int32,
	aos_value_to_int64,
	aos_value_to_u8,
	aos_value_to_u16,
	aos_value_to_u32,
	aos_value_to_u64,
	aos_value_to_double,
	aos_value_to_str,
	aos_value_to_str_b,
	aos_value_to_str_s,
	aos_value_to_ptr,

	aos_value_set_char,
	aos_value_set_int16,
	aos_value_set_int,
	aos_value_set_int32,
	aos_value_set_int64,
	aos_value_set_u8,
	aos_value_set_u16,
	aos_value_set_u32,
	aos_value_set_u64,
	aos_value_set_double,
	aos_value_set_str, 
	aos_value_set_value, 

	aos_value_set_int16_from_str,
	aos_value_set_int32_from_str,
	aos_value_set_int64_from_str,
	aos_value_set_u16_from_str,
	aos_value_set_u32_from_str,
	aos_value_set_u64_from_str,
	aos_value_set_addr_from_str,
	aos_value_set_double_from_str,
	aos_value_set_binary,

	aos_value_append_str, 
	aos_value_set_ptr, 
	aos_value_integrity_check, 
	aos_value_clone,
	aos_value_copy
};


aos_value_t *aos_value_clone(aos_value_t *value)
{
	aos_assert_r(value, 0);
	aos_value_t *obj = aos_malloc(sizeof(aos_value_t));
	aos_assert_r(obj, 0);
	memset(obj, 0, sizeof(aos_value_t));
	obj->mf = &sg_mf;
	aos_assert_g(!aos_value_copy(value, obj), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


int aos_value_init(aos_value_t *value)
{
	aos_assert_r(value, -1);
	memset(value, 0, sizeof(*value));
	value->mf = &sg_mf;
	value->type = eAosDataType_Unknown;
	return 0;
}


aos_value_t * aos_value_create_raw()
{
	aos_value_t *obj = aos_malloc(sizeof(aos_value_t));
	aos_assert_r(obj, 0);
	memset(obj, 0, sizeof(aos_value_t));
	obj->mf = &sg_mf;
	obj->type = eAosDataType_Unknown;
	return obj;
}


aos_value_t * aos_value_create_char(const char value)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_char(obj, value), cleanup);
	obj->mf->hold(obj);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_int16(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_int16_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_int32(
		const char * const value, 
		const int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_int32_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_int64(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_int64_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_u8(const char value) 
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_u8(obj, value), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_u16(
		const char * const value, 
		const int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_u16_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_u32(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_u32_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_u64(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_u64_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_string(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_double(
		const char * const value, 
		int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_double_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_binary(
		const char * const value, 
		const int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_binary(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_value_t * aos_value_create_addr(
		const char * const value, 
		const int len)
{
	aos_value_t *obj = aos_value_create_raw();
	aos_assert_r(obj, 0);
	aos_assert_g(!obj->mf->set_addr_from_str(obj, value, len), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


// 
// Description
// This function creates an instance of aos_value_t based on the
// type and the value. The value is stored as a character string.
// Depending on the type, it will be converted.
//
aos_value_t *aos_value_factory(
		const aos_data_type_e type, 
		char *value, 
		const int len)
{
	switch (type)
	{
	case eAosDataType_char:
	 	 return aos_value_create_char(value[0]);

	case eAosDataType_int16:
	 	 return aos_value_create_int16(value, len);

	case eAosDataType_int32:
	 	 return aos_value_create_int32(value, len);

	case eAosDataType_int64:
	 	 return aos_value_create_int64(value, len);

	case eAosDataType_u8:
	 	 return aos_value_create_u8(value[0]);

	case eAosDataType_u16:
	 	 return aos_value_create_u16(value, len);

	case eAosDataType_u32:
	 	 return aos_value_create_u32(value, len);

	case eAosDataType_u64:
	 	 return aos_value_create_u64(value, len);

	case eAosDataType_Unknown:
	case eAosDataType_NoDataType:
	case eAosDataType_string:
	 	 return aos_value_create_string(value, len);

	case eAosDataType_float:
	case eAosDataType_double:
	 	 return aos_value_create_double(value, len);

	case eAosDataType_binary:
	 	 return aos_value_create_binary(value, len);

	case eAosDataType_ip_addr: 
	 	 return aos_value_create_addr(value, len);

	case eAosDataType_ptr: 
		 aos_alarm("Cannot create ptr from string");
		 return 0;

	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return 0;
	}

	aos_should_never_come_here;
	return 0;
}

aos_value_t *aos_value_create_xml(struct aos_xml_node *node)
{
	aos_value_t *obj = aos_malloc(sizeof(aos_value_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_value_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize(obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


