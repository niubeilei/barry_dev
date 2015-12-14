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
// 02/14/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_types_h
#define aos_rvg_types_h

typedef struct aos_rvg_value_entry
{
	char *	value;
	int		weight;
} aos_rvg_value_entry_t;


typedef enum
{
	eAosRvg_Invalid, 

	eAosRvg_RvgAnd, 
	eAosRvg_RvgOr, 
	eAosRvg_RsgCharset, 
	eAosRvg_RsgEnum, 
	eAosRvg_RigBasic, 
	eAosRvg_RigUnique, 
	eAosRvg_Xml, 

	eAosRvg_Last 
} aos_rvg_type_e;

static inline int aos_rvg_type_check(aos_rvg_type_e type)
{
	return (type > eAosRvg_Invalid && type < eAosRvg_Last);
}

static inline int aos_is_rsg_type(const aos_rvg_type_e type)
{
	return type == eAosRvg_RsgCharset ||
		   type == eAosRvg_RsgEnum;
}

static inline int aos_is_rig_type(const aos_rvg_type_e type)
{
	return type == eAosRvg_RigBasic ||
		   type == eAosRvg_RigUnique;
}


const char * aos_rvg_type_2str(const aos_rvg_type_e type);
aos_rvg_type_e aos_rvg_type_2enum(const char * const type);

///////////////////////////// aos_order_e ///////////////////////////
typedef enum
{
	eAosOrder_Invalid, 

	eAosOrder_NoOrder,
	eAosOrder_Ascending,
	eAosOrder_Descending,

	eAosOrder_Last
} aos_order_e;

static inline int aos_order_check(aos_order_e order)
{
	return (order > eAosOrder_Invalid && order < eAosOrder_Last);
}

const char * aos_order_2str(const aos_order_e order);
aos_order_e aos_order_2enum(const char * const order);


///////////////////////////// aos_domain_check_e ///////////////////
typedef enum
{
	eAosDomainCheck_Invalid, 

	eAosDomainCheck_ShortestMatch,
	eAosDomainCheck_LongestMatch,

	eAosDomainCheck_Last
} aos_domain_check_e;

static inline int aos_domain_check(aos_domain_check_e type)
{
	return (type > eAosDomainCheck_Invalid && 
			type < eAosDomainCheck_Last);
}

#ifdef __cplusplus
extern "C" {
#endif

extern aos_rvg_value_entry_t *aos_rvg_value_entry_create();
extern const char * aos_domain_check_2str(const aos_domain_check_e type);
extern aos_domain_check_e aos_domain_check_2enum(const char * const order);

#ifdef __cplusplus
}
#endif


#endif

