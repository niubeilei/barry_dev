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
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/event_proc.h"

#include "alarm/Alarm.h"
#include "util/memory.h"


int aos_event_proc_serialize(
		struct aos_event_proc *proc, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<a_name>
	// 			<Condition>...</Condition>
	// 			<NextState>...</NextState>
	// 			<Schema>...</Schema>		optional
	// 		</a_name>
	// 		...
	// 	</Parent>
	aos_not_implemented_yet;
	return -1;
}


int aos_event_proc_deserialize(
		struct aos_event_proc *proc, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_event_proc_destroy(
		struct aos_event_proc *proc)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_event_proc_mf_t sg_mf = 
{
	aos_event_proc_serialize, 
	aos_event_proc_deserialize, 
	aos_event_proc_destroy
};


int aos_event_proc_init(aos_event_proc_t *proc)
{
	aos_assert_r(proc, -1);
	return 0;
}


aos_event_proc_t *aos_event_proc_create_xml(struct aos_xml_node *node)
{
	aos_assert_r(node, 0);
	aos_event_proc_t *proc = aos_malloc(sizeof(aos_event_proc_t));
	aos_assert_r(proc, 0);
	memset(proc, 0, sizeof(aos_event_proc_t));
	proc->mf = &sg_mf;

	aos_assert_g(!proc->mf->deserialize(proc, node), cleanup);
	return proc;

cleanup:
	aos_free(proc);
	return 0;
}

