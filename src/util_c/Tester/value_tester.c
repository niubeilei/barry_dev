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
// 04/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/Tester/value_tester.h"

#include "alarm_c/alarm.h"
#include "util_c/value.h"
#include <string.h>


int aos_value_ico_set_value(aos_value_ico_t *obj, aos_data_type_e type, void *value, int len)
{
	obj->type = type;
	switch (type)
	{
	case eAosDataType_Unknown:
	case eAosDataType_NoDataType:
	case eAosDataType_string:
	case eAosDataType_binary:
	case eAosDataType_Buffer:
		 aos_assert_r(len <= AOS_VALUE_ICO_MAX_STR_LEN, -1);
		 memcpy(obj->str_value, value, len);
		 obj->str_value[len] = 0;
		 obj->data_len = len;
		 return 0;

	case eAosDataType_char:
		 obj->int_value = *((char*)value);
		 return 0;

	case eAosDataType_int16:
		 obj->int_value = *((int16_t *)value);
		 return 0;

	case eAosDataType_int32:
		 obj->int_value = *((int32_t *)value);
		 return 0;

	case eAosDataType_int64:
		 obj->int_value = *((int64_t *)value);
		 return 0;

	case eAosDataType_u8:
		 obj->int_value = *((u8*)value);
		 return 0;

	case eAosDataType_u16:
		 obj->int_value = *((u16*)value);
		 return 0;

	case eAosDataType_u32:
	case eAosDataType_ip_addr:
		 obj->int_value = *((u32*)value);
		 return 0;

	case eAosDataType_u64:
		 obj->int_value = *((u64*)value);
		 return 0;

	case eAosDataType_float:
		 obj->int_value = *((float*)value);
		 return 0;

	case eAosDataType_double:
		 obj->int_value = *((double*)value);
		 return 0;

	case eAosDataType_ptr:
		 obj->ptr = value;
		 return 0;

	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_ico_comp(aos_value_ico_t *obj, aos_value_t *value)
{
	aos_assert_r(obj->type == value->type, -1);
	switch (value->type)
	{
	case eAosDataType_Unknown:
	case eAosDataType_NoDataType:
	case eAosDataType_string:
	case eAosDataType_binary:
	case eAosDataType_Buffer:
		 aos_assert_r(obj->data_len == value->data_size, -1);
		 aos_assert_r(memcmp(obj->str_value, value->value.string, value->data_size) == 0, -1);
		 aos_assert_r(value->mem_size >= value->data_size, -1);
		 aos_assert_r(value->mem_size - value->data_size <= AOS_VALUE_TOO_BIG, -1);
		 return 1;

	case eAosDataType_char:
		 aos_assert_r(obj->int_value == value->value.int8_value, -1);
		 return 1;

	case eAosDataType_int16:
		 aos_assert_r(obj->int_value == value->value.int16_value, -1);
		 return 1;

	case eAosDataType_int32:
		 aos_assert_r(obj->int_value == value->value.int32_value, -1);
		 return 1;

	case eAosDataType_int64:
		 aos_assert_r(obj->int_value == value->value.int64_value, -1);
		 return 1;

	case eAosDataType_u8:
		 aos_assert_r(obj->int_value == value->value.u8_value, -1);
		 return 1;

	case eAosDataType_u16:
		 aos_assert_r(obj->int_value == value->value.u16_value, -1);
		 return 1;

	case eAosDataType_u32:
	case eAosDataType_ip_addr:
		 aos_assert_r(obj->int_value == value->value.u32_value, -1);
		 return 1;

	case eAosDataType_u64:
		 aos_assert_r(obj->int_value == value->value.u64_value, -1);
		 return 1;

	case eAosDataType_float:
		 aos_assert_r(obj->double_value == value->value.double_value, -1);
		 return 1;

	case eAosDataType_double:
		 aos_assert_r(obj->double_value == value->value.double_value, -1);
		 return 1;

	case eAosDataType_ptr:
		 aos_assert_r(obj->ptr == value->value.ptr, -1);
		 return 1;

	default:
		 aos_alarm("Unrecognized type: %d", value->type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_value_ico_init(aos_value_ico_t *obj)
{
	memset(obj, 0, sizeof(*obj));
	return 0;
}


int aos_value_test_one(aos_value_ico_t *ico, aos_value_t *value)
{
	aos_assert_r(aos_value_ico_comp(ico, value) == 1, -1);

	aos_data_type_e type = (aos_data_type_e)aos_next_int(eAosDataType_Invalid+1, eAosDataType_Last-1);
	
	obj->type = type
	switch (type)
	{
	case eAosDataType_Unknown:
	case eAosDataType_NoDataType:
		 break;

	case eAosDataType_char:
		 {
			 char c = aos_next_int(CHAR_MIN, CHAR_MAX);
			 aos_value_ico_set_value(obj, type, &c);
			 aos_assert_r(!value->mf->set_char(value, c), -1); 
			 break;
		 }

	case eAosDataType_int16:
	case eAosDataType_int32:
	case eAosDataType_int64:
	case eAosDataType_u8:
	case eAosDataType_u16:
	case eAosDataType_u32:
	case eAosDataType_u64:
	case eAosDataType_string:
	case eAosDataType_float:
	case eAosDataType_double:
	case eAosDataType_binary:
	case eAosDataType_ip_addr: 
	case eAosDataType_ptr: 
	case eAosDataType_Buffer:

	default:
		 aos_alarm("Unrecognized type: %d", type);
		return -1;
	}

	aos_assert_r(aos_value_ico_comp(ico, value) == 1, -1);
	return 0;
}


int aos_value_tester()
{
	aos_value_ico_t ico;
	aos_value_ico_init(&ico);

	int i=0; 
	while (i < 1000000)
	{
		aos_assert_r(!aos_value_test_one(&ico, &value), -1);
		i++;
	}

	return 1;
}

