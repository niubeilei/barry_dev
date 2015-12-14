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
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/charset.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "rvg_c/rig.h"
#include "rvg_c/rig_basic.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/rc_obj.h"
#include <stdio.h>
#include <string.h>

AOS_DECLARE_RCOBJ_LOCK;

static int aos_charset_hold(struct aos_charset *cs)
{
	AOS_RC_OBJ_HOLD(cs);
}


static int aos_charset_put(struct aos_charset *cs)
{
	AOS_RC_OBJ_PUT(cs);
}


static int aos_charset_integrity_check(struct aos_charset *cs)
{
	int i, j, found;
	aos_assert_r(cs, -1);
	aos_assert_r(cs->mf, -1);
	aos_assert_r(cs->ref_count >= 0, -1);

	for (i=0; i<256; i++)
	{
		if (cs->map[i])
		{
			// It must be in one of the ranges
			found = 0;
			for (j=0; j<cs->ranges_array.noe; j++)
			{
				if (i >= cs->ranges[j]->start && 
					i <= cs->ranges[j]->end)
				{
					found = 1;
					break;
				}
			}

			aos_assert_r(found, -1);
		}
		else
		{
			// It must not be in one of the ranges
			for (j=0; j<cs->ranges_array.noe; j++)
			{
				aos_assert_r(i < cs->ranges[j]->start|| 
						i > cs->ranges[j]->end, -1);
			}
		}
	}
	return 1;
}


static int aos_charset_serialize(
		struct aos_charset *cs, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Charset>
	// 			<Name>			// Optional
	// 			<Description>	// Optional
	// 			<Keyword>		// Optional
	// 			<Ranges>
	// 				<Range>min, max, weight</Range>
	// 				<Range>min, max, weight</Range>
	//				...
	//			</Ranges>
	//		</Charset>
	//		...
	//	</Parent>
	char buff[100];
	aos_assert_r(cs, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);

	aos_xml_node_t *child = parent->mf->append_child_node(parent, "Charset");
	aos_assert_r(child, -1);

	// Set "<Name>"
	if (cs->name)
	{
		aos_assert_r(!child->mf->append_child_str(
				child, "Name", cs->name, 0, 0), -1);
	}

	// Set <Description>
	if (cs->desc)
	{
		aos_assert_r(!child->mf->append_child_str(
				child, "Description", cs->desc, 0, 0), -1);
	}

	// Set <Keyword>
	if (cs->keywords)
	{
		aos_assert_r(!child->mf->append_child_str(
				child, "Keyword", cs->keywords, 0, 0), -1);
	}

	// Set <Ranges>
	aos_xml_node_t *ranges = child->mf->append_child_node(child, "Ranges");
	aos_assert_r(ranges, -1);
	int i;
	aos_charset_range_t *range;
	for (i=0; i<cs->ranges_array.noe; i++)
	{
		// Add <Range>
		range = cs->ranges[i];
		sprintf(buff, "%d, %d, %d", 
			range->start, range->end, range->weight);
		aos_assert_r(!ranges->mf->append_child_str(
				ranges, "Range", buff, 0, 0), -1);
	}

	return 0;
}


static int aos_charset_deserialize(
		struct aos_charset *cs, 
		struct aos_xml_node *node)
{
	int start, end, weight, len;
	aos_assert_r(cs, -1);
	aos_assert_r(node, -1);

	// Retrieve <Name>
	if (cs->name) aos_free(cs->name);
	cs->name = 0;
	aos_assert_r(!node->mf->first_child_str_dft(node, "Name", 
			&cs->name, &len, 0), -1);

	// Retrieve <Description>
	if (cs->desc) aos_free(cs->desc);
	cs->desc = 0;
	aos_assert_r(!node->mf->first_child_str_dft(node, "Description", 
			&cs->desc, &len, 0), -1);

	// Retrieve <Keyword>
	if (cs->keywords) aos_free(cs->keywords);
	cs->keywords = 0;
	aos_assert_r(!node->mf->first_child_str_dft(node, "Keyword", 
			&cs->keywords, &len, 0), -1);

	// Retrieve <Ranges>
	aos_xml_node_t *ranges = node->mf->first_named_child(node, "Ranges");
	aos_assert_r(ranges, -1);

	aos_xml_node_t *range = ranges->mf->first_child(ranges);
	
	if (cs->ranges_array.noe > 0)
	{
		int i;
		for (i=0; i<cs->ranges_array.noe; i++)
		{
			aos_free(cs->ranges[i]);
		}
		aos_assert_r(!cs->ranges_array.mf->remove_all(
				&cs->ranges_array, (char **)&cs->ranges), -1); 
	}

	while (range)
	{
		/*
		int index = 0;
		buff_len = 100;
		aos_assert_r(!range->mf->get_str((aos_field_t *)range, buff, &buff_len), -1);
		aos_assert_r(!aos_str_get_int(buff, 100, &index, &start), -1);
		aos_assert_r(!aos_str_get_int(buff, 100, &index, &end), -1);
		aos_assert_r(!aos_str_get_int(buff, 100, &index, &weight), -1);
		*/
		char *ss = range->mf->get_attr((aos_field_t *)range, "start");
		aos_assert_rm(!aos_atoi(ss, strlen(ss), &start), -1, "ss = %s", ss);

		char *ee = range->mf->get_attr((aos_field_t *)range, "end");
		aos_assert_r(!aos_atoi(ee, strlen(ee), &end), -1);
		
		char *ww = range->mf->get_attr((aos_field_t *)range, "weight");
		aos_assert_r(!aos_atoi(ww, strlen(ww), &weight), -1);
		
		aos_assert_r(!cs->mf->add_range(cs, start, end, weight), -1);
		aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);
		range = ranges->mf->next_sibling(ranges);
	}

	aos_assert_r(cs->ranges_array.noe > 0, -1);
	return 0;
}


static char aos_charset_next_char(aos_charset_t *cs)
{
	aos_assert_r(cs, -1);
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);

	int idx;
    aos_assert_r(!cs->range_selector->mf->next_int(
			cs->range_selector, &idx), -1);

	aos_assert_r(idx >= 0 && idx <cs->ranges_array.noe, -1);
	return (char)aos_next_int(cs->ranges[idx]->start, 
			cs->ranges[idx]->end);
}


// 
// Description
// It generates a random string of length 'len', in the order
// as the one defined by the character set. Characters can be repeated
// at at most 'repeat' number of times. If 'repeat' == 0, characters 
// can be repeated any number of times. 
// 
// Algorithm:
// Startnig with an array[0..255]. Randomly pick characters based on the
// character set. Every time a character is picked, the corresponding
// cell in the array[0.255] is incremented by one. If the character
// can no longer be picked due to the repeat limitation, it will select
// the next selectable character by incrementing the character index.
//
static int aos_charset_next_ascending(
		aos_charset_t *cs, 
		char *str, 
		const int len, 
		const int the_repeat)
{
	int array[256];
	int i, idx1, idx2;
	int repeat = (the_repeat == 0)?len:the_repeat;
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);

	// 1. Construct the array
	memset(array, 0, sizeof(array));
	for (i=0; i<len; i++)
	{
    	aos_assert_r(!cs->range_selector->mf->next_int(
				cs->range_selector, &idx1), -1);
		idx2 = aos_next_int(cs->ranges[idx1]->start, cs->ranges[idx1]->end);
		aos_assert_r(idx2 >= 0 && idx2 < 256, -1);
		if (array[idx2] < repeat) 
		{
			array[idx2]++;
		}
		else
		{
			u8 try = (u8)(idx2+1);
			while (try != (u8)idx2)
			{
				if (array[try] < repeat)
				{
					array[try]++;
					break;
				}
				try++;
			}
			aos_assert_r(try != (u8)idx2, -1);
		}
	}

	// 2. Assemble the string
	int str_idx = 0;
	for (i=0; i<cs->ranges_array.noe; i++)
	{
		int start = cs->ranges[i]->start; 
		int end = cs->ranges[i]->end, j;
		for (j=start; j<end; j++)
		{
			int occurance = array[j], k;	
			for (k=0; k<occurance; k++)
			{
				aos_assert_r(str_idx < len, -1);
				str[str_idx++] = (char)j;
			}
			array[j] = 0;
		}
	}

	aos_assert_r(str_idx == len, -1);
	aos_assert_r(aos_memcmp_equal(array, 0, 256) == 1, -1);
	return 0;
}


// 
// Description
// It generates a random string of length 'len', in the reversed order
// as the one defined by the character set. Characters can be repeated
// at at most 'repeat' number of times. If 'repeat' == 0, characters 
// can be repeated any number of times. 
// 
// Algorithm:
// Startnig with an array[0..255]. Randomly pick characters based on the
// character set. Every time a character is picked, the corresponding
// cell in the array[0.255] is incremented by one. If the character
// can no longer be picked due to the repeat limitation, it will select
// the next selectable character by incrementing the character index.
//
static int aos_charset_next_descending(
		aos_charset_t *cs, 
		char *str, 
		const int len, 
		const int the_repeat)
{
	int array[256];
	int i, idx1, idx2;
	int repeat = (the_repeat == 0)?len:the_repeat;
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);

	// 1. Construct the array
	memset(array, 0, sizeof(array));
	for (i=0; i<len; i++)
	{
    	aos_assert_r(!cs->range_selector->mf->next_int(
				cs->range_selector, &idx1), -1);
		idx2 = aos_next_int(cs->ranges[idx1]->start, cs->ranges[idx1]->end);

		aos_assert_r(idx2 >= 0 && idx2 < 256, -1);
		if (array[idx2] < repeat) 
		{
			array[idx2]++;
		}
		else
		{
			u8 try = (u8)(idx2+1);
			while (try != (u8)idx2)
			{
				if (array[try] < repeat)
				{
					array[try]++;
					break;
				}
				try++;
			}
			aos_assert_r(try != (u8)idx2, -1);
		}
	}

	// 2. Assemble the string
	int str_idx = 0;
	for (i=cs->ranges_array.noe-1; i>=0; i--)
	{
		int start = cs->ranges[i]->start; 
		int end = cs->ranges[i]->end;
		int j;
		for (j=start; j>=end; j--)
		{
			int occurance = array[j];	
			int k;
			for (k=0; k<occurance; k++)
			{
				aos_assert_r(str_idx < len, -1);
				str[str_idx++] = (char)j;
			}
			array[j] = 0;
		}
	}

	aos_assert_r(str_idx == len, -1);
	aos_assert_r(aos_memcmp_equal(array, 0, 256) == 1, -1);
	return 0;
}


// 
// Description
// It generates a random string of length 'len'. 
//
static int aos_charset_next_no_order(
		aos_charset_t *cs, 
		char *str, 
		const int len, 
		const int the_repeat)
{
	int i, idx1, idx2;
	int repeat = (the_repeat == 0)?len:the_repeat;
	int array[256];

	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);
	memset(array, 0, sizeof(array));

	for (i=0; i<len; i++)
	{
		aos_assert_r(!cs->range_selector->mf->next_int(
				cs->range_selector, &idx1), -1);
		idx2 = aos_next_int(cs->ranges[idx1]->start, cs->ranges[idx1]->end);

		if (array[idx2] < repeat) 
		{
			array[idx2]++;
			str[i] = (char)idx2;
		}
		else
		{
			u8 try = idx2+1;
			while (try != idx2)
			{
				if (array[try] < repeat)
				{
					array[try]++;
					break;
				}
				try++;
			}
			aos_assert_r(try != idx2, -1);
			str[i] = (char)try;
		}
	}

	str[len] = 0;
	return 0;
}


// 
// Description
// It generates a random string based on the character set. 
// If 'repeat' is not zero, no characters will appear more than
// 'repeat' number of times. If order == 'eAosOrder_Ascending',
// characters in the string will appear in the same order as
// the character set defines. if order == 'eAosOrder_Descending', 
// characters in the string will appear in the reversed order
// as the character set defines. Otherwise, no order is assumed.
//
// IMPORTANT: the caller should guarantee that 'str' has valid
// memory of at least 'len' size. 
//
static int aos_charset_next_str(
		aos_charset_t *cs, 
		char *str, 
		const int len, 
		const int repeat, 
		const aos_order_e order)
{
	aos_assert_r(cs, -1);
	aos_assert_r(str, -1);
	aos_assert_r(len >= 0, -1);
	aos_assert_r(repeat >= 0, -1);
	aos_assert_r(aos_order_check(order) == 1, -1);

	switch (order)
	{
	case eAosOrder_NoOrder:
		 aos_assert_r(!aos_charset_next_no_order(
					 cs, str, len, repeat), -1);
		 break;

	case eAosOrder_Ascending:
		 aos_assert_r(!aos_charset_next_ascending(
					 cs, str, len, repeat), -1);
		 break;

	case eAosOrder_Descending:
		 aos_assert_r(!aos_charset_next_descending(
					 cs, str, len, repeat), -1);
		 break;

	default:
		 aos_alarm("Unsupported order: %d:%s", order, 
				 aos_order_2str(order));
		 return -1;
	}

	return 0;
}


static int aos_charset_next_value(
		aos_charset_t *cs, 
		aos_value_t *value,
		const int len, 
		const int repeat, 
		const aos_order_e order)
{
	aos_assert_r(value, -1);

	int pos = value->data_size;
	aos_assert_r(!value->mf->append_str(value, 0, len), -1);
	aos_assert_r(!aos_charset_next_str(cs, &value->value.string[pos], 
				len, repeat, order), -1);
	return 0;
}


static int aos_charset_is_char_in(
		struct aos_charset *cs, 
		const unsigned char c)
{
	aos_assert_r(cs, -1);
	return cs->map[c];
}


// 
// Description
// It checks whether the string 'str' is made of the characters
// from this charset. If yes, it returns 1. If not it returns 0.
// If errors, it returns -1. 
//
static int aos_charset_is_valid_str(
		struct aos_charset *cs, 
		const char * const str, 
		const int len)
{
	int i;
	aos_assert_r(cs, -1);
	aos_assert_r(str, -1);
	aos_assert_r(len >= 0, -1);

	for (i=0; i<len; i++)
	{
		if (!cs->map[(unsigned char)str[i]]) return 0;
	}

	return 1;
}


// 
// Description
// It adds a character range with a weight. The weight is used to
// control how often the characters in this range are used when 
// generating chars.
//
static int aos_charset_add_range(
		struct aos_charset *cs, 
		const u8 start, 
		const u8 end, 
		const int weight)
{
	int i;
	aos_assert_r(cs, -1);
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);
	aos_assert_r(weight >= 0 && weight <= AOS_CHARSET_MAX_WEIGHT, -1);
	aos_assert_r(start <= end, -1);

	aos_charset_range_t *range = aos_malloc(sizeof(*range));
	aos_assert_r(range, -1);
	range->start = start;
	range->end = end;
	range->weight = weight;

	int noe = cs->ranges_array.noe;
	aos_assert_g(cs->range_selector, cleanup);
	aos_assert_g(!cs->range_selector->mf->add_range(
			cs->range_selector, noe, noe, weight), cleanup);

	aos_assert_g(!cs->ranges_array.mf->add_element(
			&cs->ranges_array, 1, (char **)&cs->ranges), cleanup);
	cs->ranges[noe] = range;
	cs->ranges_array.noe++;

	for (i=start; i<=end; i++)
	{
		cs->map[i] = 1;
	}

	return 0;

cleanup:
	aos_free(range);
	return -1;
}


static int aos_charset_release_memory(aos_charset_t *cs)
{
	int i;
	aos_assert_r(cs, -1);

	if (cs->name) aos_free(cs->name);
	cs->name = 0;

	if (cs->desc) aos_free(cs->desc);
	cs->desc = 0;

	if (cs->keywords) aos_free(cs->keywords);
	cs->keywords = 0;

	for (i=0; i<cs->ranges_array.noe; i++)
	{
		aos_free(cs->ranges[i]);
	}

	aos_assert_r(!cs->ranges_array.mf->release_memory(
			&cs->ranges_array), -1); 
	cs->ranges = 0;

	aos_assert_r(cs->range_selector, -1);
	cs->range_selector->mf->put((aos_rvg_t *)cs->range_selector);
	cs->range_selector = 0;
	return 0;
}


static char * aos_charset_get_desc(aos_charset_t *cs)
{
	aos_assert_r(cs, 0);
	return cs->desc;
}


static char * aos_charset_get_keywords(aos_charset_t *cs)
{
	aos_assert_r(cs, 0);
	return cs->keywords;
}


static int aos_charset_destroy(aos_charset_t *cs)
{
	aos_assert_r(cs, -1);
	aos_assert_r(cs->mf->integrity_check(cs) == 1, -1);
	aos_assert_r(!cs->mf->release_memory(cs), -1);
	aos_free(cs);
	return 0;
}


static int aos_charset_domain_check(
		aos_charset_t *cs, 
		aos_value_t *value)
{
	aos_assert_r(cs, -1);
	aos_assert_r(value, -1);
	aos_assert_r(value->type == eAosDataType_string ||
				 value->type == eAosDataType_binary, -1);

	int i, j;
	char *data = value->value.string;
	for (i=0; i<value->data_size; i++)
	{
		char c = data[i];
		int found = 0;
		for (j=0; j<cs->ranges_array.noe; j++)
		{
			if (c >= cs->ranges[j]->start && c <= cs->ranges[j]->end)
			{
				found = 1;
				break;
			}
		}

		if (!found) return 0;
	}

	return 1;
}


// 
// Description:
// Match at least 'min_len' and at most 'max_len'. If 'max_len'
// is 0, it matches as much as it can. 
//
// If not matched 'min_len', it returns 0 and 'idx' is not changed.
//
static int aos_charset_domain_check_len(
		aos_charset_t *cs, 
		aos_value_t *value, 
		int *idx, 
		aos_domain_check_e type, 
		const int min_len, 
		const int max_len)
{
	aos_assert_r(cs, -1);
	aos_assert_r(value, -1);
	aos_assert_r(value->type == eAosDataType_string ||
				 value->type == eAosDataType_binary, -1);

	int i, j, found, len;
	int ii = *idx;
	aos_assert_r(min_len >= 0 && min_len + ii <= value->data_size, -1);
	aos_assert_r((max_len == 0 || (min_len <= max_len && 
				  max_len + ii <= value->data_size)), -1);

	aos_assert_r(ii >= 0 && ii < value->data_size, -1);
	char *data = &value->value.string[ii];

	switch (type)
	{
	case eAosDomainCheck_ShortestMatch:
	case eAosDomainCheck_LongestMatch:
		 // Need to match at least min_len number of characters
		 for (j=0; j<min_len; j++)
		 {
		 	found = 0;
		 	for (i=0; i<cs->ranges_array.noe; i++)
		 	{
				if (data[ii] >= cs->ranges[i]->start && 
					data[ii] <= cs->ranges[i]->end)
				{
					found = 1;
					break;
				}
			}

			if (!found) 
			{
				return 0;
			}
			ii++;
		 }

		 if (type == eAosDomainCheck_ShortestMatch) 
		 {
			 *idx = ii;
			 return 1;
		 }

		 // Try match as many as 'max_len'. 
		 len = (max_len == 0)?value->data_size:max_len;
		 for (j=min_len; j<len; j++)
		 {
		 	found = 0;
		 	for (i=0; i<cs->ranges_array.noe; i++)
		 	{
				if (data[ii] >= cs->ranges[i]->start && 
					data[ii] <= cs->ranges[i]->end)
				{
					found = 1;
					break;
				}
			}

			if (!found) 
			{
				break;
			}
			ii++;
		 }

		 *idx = ii;
		 return 1;

	default:
		 aos_alarm("Unsupported domain check type: %d", type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


static aos_charset_mf_t sg_mf = 
{
	aos_charset_hold,
	aos_charset_put,
	aos_charset_release_memory,
	aos_charset_destroy,
	aos_charset_integrity_check,
	aos_charset_serialize,
	aos_charset_deserialize,
	aos_charset_next_char,
	aos_charset_next_value,
	aos_charset_next_str,
	aos_charset_is_char_in,
	aos_charset_is_valid_str,
	aos_charset_add_range,
	aos_charset_domain_check, 
	aos_charset_domain_check_len,
	aos_charset_get_desc,
	aos_charset_get_keywords
};


int aos_charset_init(aos_charset_t *obj)
{
	aos_assert_r(obj, -1);
	memset(obj, 0, sizeof(*obj));
	obj->mf = &sg_mf;
	aos_assert_r(!aos_dyn_array_init(&obj->ranges_array, 
			(char **)&obj->ranges, 4, 5, AOS_CHARSET_MAX_RANGES), -1);
	obj->range_selector = (aos_rig_t *)aos_rig_basic_create();
	aos_assert_r(obj->range_selector, -1);
	return 0;
}


aos_charset_t *aos_charset_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_charset_t *obj = aos_malloc(sizeof(aos_charset_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_charset_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize(obj, node), cleanup);
	obj->mf->hold(obj);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_charset_t *aos_charset_create_str(const char * const contents)
{
	aos_assert_r(contents, 0);
	int is_incomplete;
	int len = strlen(contents);
	aos_xml_node_t *node = aos_xml_node_create_from_str(0, 
			contents, &len, &is_incomplete, 0);
	aos_assert_r(node, 0);
	return aos_charset_create_xml(node);
}

