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
// 02/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_gen_data_tags_h
#define aos_util_gen_data_tags_h

typedef enum
{
	eAosGenData_Invalid, 

	eAosGenData_CrtStmch, 
	eAosGenData_DataToSend, 
	eAosGenData_DataLen, 
	eAosGenData_LocalAddr, 
	eAosGenData_LocalPort, 
	eAosGenData_ReadCallback, 
	eAosGenData_ReadThrdMgr,
	eAosGenData_RemoteAddr, 
	eAosGenData_RemotePort, 
	eAosGenData_ReceivingBuffer, 

	eAosGenData_Last
} aos_gen_data_tag_e;	

static inline int aos_gen_data_tag_check(const aos_gen_data_tag_e tag)
{
	return (tag > eAosGenData_Invalid && tag < eAosGenData_Last);
}

extern char * aos_gen_data_tag_2str(const aos_gen_data_tag_e tag);
extern aos_gen_data_tag_e aos_gen_data_tag_2enum(const char * const tag);


#endif

