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
// 03/24/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_domain_types_h
#define aos_omni_domain_types_h

/////////////////////////// aos_domain_e ////////////////////
typedef enum
{
	eAosDomain_Invalid, 

	eAosDomain_Int,

	eAosDomain_Last 
} aos_domain_e;

static inline int aos_domain_check(aos_domain_e type)
{
	return (type > eAosDomain_Invalid && 
			type < eAosDomain_Last)?1:0;
}

extern const char * aos_domain_2str(aos_domain_e type);
extern aos_domain_e aos_domain_2enum(const char * const type);


#endif

