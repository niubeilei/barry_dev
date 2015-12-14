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
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/tester/rvg_xml_tester.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "rvg_c/rvg_xml.h"
#include "util_c/memory.h"
#include "util_c/dyn_array.h"
#include <stdio.h>


int aos_rvg_xml_tester()
{
	aos_xml_node_t *node = aos_xml_node_create_from_file("xml.txt");
	aos_assert_r(node, -1);

	aos_xml_node_t *xmlgen = node->mf->first_named_child(node, "RvgXml");
	aos_assert_r(xmlgen, -1);

	aos_rvg_t *rvg = aos_rvg_factory_xml(xmlgen);
	aos_assert_r(rvg, -1);

	aos_assert_r(rvg->type == eAosRvg_Xml, -1);

	aos_value_t value;
	aos_assert_r(!aos_value_init(&value), -1);
	// aos_dyn_array_t contents;
	// char *ptr;
	// aos_dyn_array_init(&contents, &ptr, 1, 5000, 10000000);

	int i;
	for (i=0; i<10; i++)
	{
		aos_assert_r(!rvg->mf->next_value(rvg, &value), -1);
		printf("Size: %d\n", value.data_size);
	}
	return 0;
} 

