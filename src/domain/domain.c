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
// This is the super class for all domains.
//   
//
// Modification History:
// 03/24/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "domain/domain.h"

#include "alarm/Alarm.h"
#include "domain/types.h"
#include "parser/xml_node.h"
#include "porting/mutex.h"
#include "util/rc_obj.h"
#include "util/memory.h"
#include "util/tracer.h"
#include "util/magic.h"
#include "util/global_data.h"
#include "util/value.h"


AOS_DECLARE_RCOBJ_LOCK;

static aos_lock_t *	sg_lock = 0;
static int			sg_init_flag = 0;


static int sg_init()
{
	aos_global_lock();
	if (sg_init_flag) 
	{
		aos_global_unlock();
		return 0;
	}

	sg_lock = aos_malloc(sizeof(*sg_lock));
	aos_init_lock(sg_lock);
	sg_init_flag = 1;
	aos_global_unlock();
	return 0;
}


// 
// Description
// It serializes its part. This function should be called
// by its derived class. 
//
int aos_domain_serialize(
		aos_domain_t *domain, 
		aos_xml_node_t *node)
{
	// 
	// 	<domain>
	// 		<Type>
	// 		<Name>			optional
	// 		...
	// 	</domain>
	//
	aos_assert_r(domain, -1);
	aos_assert_r(node, -1);

	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, "Type", 
			aos_domain_2str(domain->type), 0, 0), -1);

	// Set <Name>
	if (domain->name)
	{
		aos_assert_r(!node->mf->append_child_str(node, "Name", 
			domain->name, 0, 0), -1);
	}

	return 0;
}


int aos_domain_deserialize(
		struct aos_domain *domain, 
		aos_xml_node_t *node)
{
	char buff[100];
	int buff_len = 100;
	aos_assert_r(domain, -1);
	aos_assert_r(node, -1);

	// Retrieve <Type>
	buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(node, "Type", 
			buff, &buff_len, 0), -1);
	domain->type = aos_domain_2enum(buff); 
	aos_assert_r(aos_domain_check(domain->type) == 1, -1);

	// Retrieve <Name>
	if (node->name) 
	{
		aos_free(node->name);
		aos_assert_r(!node->mf->first_child_str_dft(node, "Name", 
			&domain->name, &buff_len, 0), -1);
	}

	return 0;
}


int aos_domain_destroy(struct aos_domain *domain)
{
	aos_assert_r(domain, -1);
	aos_assert_r(!domain->mf->release_memory(domain), -1);
	aos_free(domain);
	return 0;
}


int aos_domain_hold(aos_domain_t *domain)
{
	AOS_RC_OBJ_HOLD(domain);
}


int aos_domain_put(aos_domain_t *domain)
{
	AOS_RC_OBJ_PUT(domain);
}


int aos_domain_release_memory(struct aos_domain *domain)
{
	aos_assert_r(domain, -1);
	if (domain->name) 
	{
		aos_free(domain->name);
	}
	domain->name = 0;
	return 0;
}


int aos_domain_init(aos_domain_t *sm)
{
	aos_assert_r(sm, -1);
	sm->magic = AOS_DOMAIN_MAGIC;
	if (!sg_init_flag) sg_init();
	return 0;
}


aos_domain_t *aos_domain_factory(aos_xml_node_t *node)
{
	char type[100];
	int type_len = 100;
	aos_assert_r(node, 0);
	aos_assert_r(!node->mf->first_child_str_b(
			node, "Type", type, &type_len, 0), 0);

//	if (strcmp(type, "SendMsg") == 0)
//	{
//		return aos_act_send_msg_create_xml(node);
//	}

	aos_alarm("Unrecognized type: %s", type);
	return 0;
}

