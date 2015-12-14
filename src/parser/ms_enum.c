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
// 02/05/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/ms_enum.h"

#include "alarm_c/alarm.h"
#include "parser/cs.h"
#include "parser/ms.h"
#include "parser/msg.h"
#include "parser/parser.h"
#include "parser/xml.h"
#include "util_c/memory.h"


aos_field_t * aos_ms_enum_select_msg(
		aos_msg_selector_t *ms,
		aos_omparser_t *parser)

{
	int index, i;
	aos_assert_r(ms, 0);
	aos_assert_r(ms->type == eAosMSType_Enum, 0);
	aos_ms_enum_t *self = (aos_ms_enum_t *)ms;
	aos_assert_r(self->cont_selector, 0);

	if (self->noe <= 0)
	{
		// 
		// No messages are configured. 
		//
		return 0;
	}

	index = self->cont_selector->mf->select(self->cont_selector, parser);
	for (i=0; i<self->noe; i++)
	{
		if (self->index[i] == index)
		{
			// 
			// Found it.
			//
			return self->msg[i];
		}
	}

	// 
	// Did not find it.
	//
	return 0;
}

int aos_ms_enum_serialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_ms_enum_deserialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_ms_enum_destroy(struct aos_msg_selector *ms)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_ms_enum_integrity_check(struct aos_msg_selector *ms)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_ms_enum_dump_to(
		struct aos_msg_selector *ms,
		char *buff, 
		int *len)
{
	aos_not_implemented_yet;
	return -1;
}


const char * aos_ms_enum_dump(struct aos_msg_selector *ms)
{
	aos_not_implemented_yet;
	return 0;
}


static aos_ms_enum_mf_t sg_mf = 
{
	aos_msg_selector_hold, 
	aos_msg_selector_put,
	aos_ms_enum_select_msg,
	aos_ms_enum_serialize,
	aos_ms_enum_deserialize,
	aos_ms_enum_destroy,
	aos_ms_enum_integrity_check,
	aos_ms_enum_dump_to,
	aos_ms_enum_dump
};


// 
// Description
// This function creates an ms_enum instance based on the 
// xml configuration 'conf'.
//
aos_ms_enum_t *aos_ms_enum_create_xml(struct aos_xml_node *conf)
{
	aos_assert_r(conf, 0);
	aos_ms_enum_t *ms = aos_malloc(sizeof(aos_ms_enum_t));
	aos_assert_r(ms, 0);
	ms->mf = &sg_mf;
	aos_msg_selector_init((aos_msg_selector_t *)ms, eAosMSType_Enum);

	aos_assert_g(!ms->mf->deserialize((aos_msg_selector_t *)ms, conf), cleanup); 
	return (aos_ms_enum_t *)ms;

cleanup:
	if (ms) aos_free(ms);
	return 0;
}

