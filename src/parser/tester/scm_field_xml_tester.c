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
// 02/19/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/tester/scm_field_xml_tester.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/scm_field_xml.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "rvg_c/rvg_xml.h"
#include "util_c/memory.h"
#include <stdio.h>


extern int aos_scm_field_xml_torturer();
extern int aos_scm_field_xml_special_tcs();

int aos_scm_field_xml_tester()
{
	aos_assert_r(!aos_scm_field_xml_special_tcs(), -1);
	aos_assert_r(!aos_scm_field_xml_torturer(), -1);
	return 0;
}


int aos_scm_field_xml_special_tcs()
{
	aos_xml_node_t *node = aos_xml_node_create_from_file("scm.xml", 1);
	aos_assert_r(node, -1);

	aos_scm_field_xml_t *schema = aos_scm_field_xml_create(node);
	aos_assert_r(schema, -1);
	return 0;
}


int aos_scm_field_xml_torturer()
{
	return 0;
} 

