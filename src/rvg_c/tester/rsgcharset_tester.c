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
// 04/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/tester/rsgcharset_tester.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "util_c/memory.h"
#include "util_c/dyn_array.h"
#include <stdio.h>


int aos_rsgcharset_tester()
{
	aos_xml_node_t *node = aos_xml_node_create_from_file("rsg.xml");
	aos_assert_r(node, -1);

	aos_xml_node_t *xn = node->mf->first_child(node);
	aos_assert_r(xn, -1);
	aos_rvg_t *rvg = aos_rvg_factory_xml(xn);
	aos_assert_r(rvg, -1);

	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);

	return 0;
} 

