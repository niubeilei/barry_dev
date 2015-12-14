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
// Modification History:
// 03/15/2008: Created by Chen Dign
//
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rsg_charset_h
#define aos_rvg_rsg_charset_h

#include "rvg_c/rsg.h"

struct aos_rsg_charset;
struct aos_rig;
struct aos_value;
struct aos_charset;
struct aos_xml_node;

#define AOS_RSG_CHARSET_DEFAULT_MINLEN 1
#define AOS_RSG_CHARSET_DEFAULT_MAXLEN 32


#define AOS_RSG_CHARSET_MEMFUNC_DECL				\


#define AOS_RSG_CHARSET_MEMDATA_DECL				\
	int						repeat;					\
	int						ordered;				\
	struct aos_rig *		length_rig;				\
	struct aos_charset *	charset;				\
	int						min_len;				\
	int						max_len

typedef struct aos_rsg_charset_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RSG_MEMFUNC_DECL;
	AOS_RSG_CHARSET_MEMFUNC_DECL;
} aos_rsg_charset_mf_t;

typedef struct aos_rsg_charset
{
	aos_rsg_charset_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RSG_MEMDATA_DECL;
	AOS_RSG_CHARSET_MEMDATA_DECL;
} aos_rsg_charset_t;

extern int aos_rsg_charset_init(aos_rsg_charset_t *);
extern aos_rsg_charset_t *aos_rsg_charset_create_xml(
		struct aos_xml_node *node);

#endif

