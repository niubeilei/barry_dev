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
#ifndef aos_parser_msg_xml_h
#define aos_parser_msg_xml_h

#include "parser/xml_node.h"

#define AOS_MSG_XML_MEMFUNC_DECL				\

#define AOS_MSG_XML_MEMDATA_DECL				\

typedef struct aos_msg_xml_mf
{
	AOS_FIELD_MEMFUNC_DECL;
	AOS_XML_NODE_MEMFUNC_DECL;
	AOS_MSG_XML_MEMFUNC_DECL;
} aos_msg_xml_mf_t;

typedef struct aos_msg_xml
{
	aos_msg_xml_mf_t *mf;

	AOS_FIELD_MEMDATA_DECL;
	AOS_XML_NODE_MEMDATA_DECL;
	AOS_MSG_XML_MEMDATA_DECL;
} aos_msg_xml_t;

// 
// Global functions
//
extern int aos_msg_xml_integrity_check(
		struct aos_field *field);

extern int aos_msg_xml_parse(struct aos_field *field); 

#endif

