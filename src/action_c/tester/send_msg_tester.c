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
// 02/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "action/tester/send_msg_tester.h"

#include "action/send_msg.h"
#include "alarm/Alarm.h"
#include "parser/xml_node.h"
#include "porting/addr.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "util/buffer.h"
#include "util/memory.h"
#include "util/tracer.h"
#include "util/gen_data.h"
#include <stdio.h>


extern int aos_send_msg_torturer();
extern int aos_send_msg_special_test_cases();

int aos_send_msg_tester()
{
	aos_assert_r(!aos_send_msg_special_test_cases(), -1);
	aos_assert_r(!aos_send_msg_torturer(), -1);
	return 0;
}


int aos_send_msg_special_tc1()
{
	aos_xml_node_t *n = aos_xml_node_create_from_file("sendmsg.xml");
	aos_assert_r(n, -1);
	aos_xml_node_t *node = n->mf->first_named_child(n, "Action_SendMsg");
	aos_assert_r(node, -1);

	aos_action_t *act = aos_act_send_msg_create_xml(node);
	aos_assert_r(act, -1);

	aos_gen_data_t data;
	aos_gen_data_init(&data);

	char data_send[10101];
	memset(data_send, 'c', 10101);

	aos_assert_r(!data.mf->set_str(&data, 0, 
				eAosGenData_DataSent, data_send, 0), -1);
	aos_assert_r(!data.mf->set_int(&data, 0, 
				eAosGenData_DataLen, 10101), -1);

	aos_assert_r(!act->mf->run(act, &data), -1);

	return 0;
}


int aos_send_msg_special_test_cases()
{
	aos_assert_r(!aos_send_msg_special_tc1(), -1);
	return 0;
}


int aos_send_msg_torturer()
{
	return 0;
} 

