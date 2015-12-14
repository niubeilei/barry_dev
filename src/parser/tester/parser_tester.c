////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/tester/parser_tester.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "rvg_c/rvg_xml.h"
#include "util_c/memory.h"
#include <stdio.h>


extern int aos_parser_torturer();
extern int aos_parser_special_test_cases();

int aos_parser_tester()
{
	aos_assert_r(!aos_parser_special_test_cases(), -1);
	aos_assert_r(!aos_parser_torturer(), -1);
	return 0;
}


int aos_parser_special_tc1()
{
	aos_xml_node_t *child;

	aos_xml_node_t *node = aos_xml_node_create_from_file("special1.xml", 1);
	aos_assert_r(node, -1);
	child = node->mf->first_named_child(node, "E");
	aos_assert_r(child, -1);
	child = node->mf->first_named_child(node, "YXXcX");
	aos_assert_r(child, -1);
	return 0;
}


int aos_parser_special_test_cases()
{
	aos_assert_r(!aos_parser_special_tc1(), -1);
	return 0;
}


int aos_parser_torturer()
{
	aos_xml_node_t *node = aos_xml_node_create_from_file("rvg.def", 1);
	aos_assert_r(node, -1);
	 
	aos_xml_node_t *xmlgen = node->mf->first_named_child(node, "RvgXml");
	aos_assert_r(xmlgen, -1);
	
	aos_rvg_t *rvg = aos_rvg_factory_xml(xmlgen);
	aos_assert_r(rvg, -1);
	
	aos_assert_r(rvg->type == eAosRvg_Xml, -1);
	
	aos_dyn_array_t contents;
	aos_dyn_array_init(&contents, 0, 1, 5000, 10000000);
	
	aos_value_t value;
	aos_assert_r(!aos_value_init(&value), -1);

	int i;
	for (i=0; i<10000; i++)
	{
		if (i % 100 == 0) printf("i = %d\n", i);
		aos_assert_r(!rvg->mf->next_value(rvg, &value), -1);
		int is_incomplete;
		char *vv;
		int len;
		aos_assert_r(!value.mf->to_str(&value, &vv, &len), -1);
		aos_xml_node_t * node = aos_xml_node_create_from_str(
				0, vv, &len, &is_incomplete, 1);
		if (!node)
		{
			printf("Contents: %s\n", contents.buffer);
			return -1;
		}

		aos_assert_r(!aos_dyn_array_release_memory(&contents), -1);
		aos_assert_r(!node->mf->destroy((aos_field_t *)node), -1);
	}

	return 0;
} 

