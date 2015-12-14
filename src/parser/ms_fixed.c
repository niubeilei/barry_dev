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
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/ms_fixed.h"

#include "alarm_c/alarm.h"
#include "parser/msg.h"
#include "parser/parser.h"
#include "parser/xml.h"
#include "util_c/memory.h"


aos_field_t * aos_ms_fixed_select_msg(
		aos_msg_selector_t *ms,
		aos_omparser_t *parser)

{
	aos_assert_r(ms, 0);
	aos_assert_r(ms->type == eAosMSType_Fixed, 0);
	aos_ms_fixed_t *self = (aos_ms_fixed_t *)ms;
	aos_assert_r(self->msg, 0);
	return self->msg;
}

int aos_ms_fixed_serialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_ms_fixed_deserialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_ms_fixed_mf_t sg_mf = 
{
	aos_msg_selector_hold,
	aos_msg_selector_put,
	aos_ms_fixed_select_msg,
	aos_ms_fixed_serialize,
	aos_ms_fixed_deserialize
};


// 
// Description
// This function creates an ms_fixed instance based on the 
// xml configuration 'conf'.
//
aos_ms_fixed_t *aos_ms_fixed_create_xml(struct aos_xml_node *conf)
{
	aos_assert_r(conf, 0);
	aos_ms_fixed_t *ms = aos_malloc(sizeof(aos_ms_fixed_t));
	aos_assert_r(ms, 0);
	memset(ms, 0, sizeof(aos_ms_fixed_t));
	ms->mf = &sg_mf;

	aos_assert_g(!ms->mf->deserialize((aos_msg_selector_t *)ms, conf), cleanup); 
	return ms;

cleanup:
	if (ms) aos_free(ms);
	return 0;
}

