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
#include "util_c/gen_data_tags.h"

#include "alarm_c/alarm.h"
#include <string.h>


char * aos_gen_data_tag_2str(const aos_gen_data_tag_e tag)
{
	switch (tag)
	{
	case eAosGenData_CrtStmch:
		 return "CrtStmch";

	case eAosGenData_DataToSend: 
		 return "DataToSend";

	case eAosGenData_DataLen: 
		 return "DataLen";

	case eAosGenData_LocalAddr: 
		 return "LocalAddr";

	case eAosGenData_LocalPort: 
		 return "LocalPort";

	case eAosGenData_ReadCallback: 
		 return "ReadCallback";

	case eAosGenData_ReadThrdMgr:
		 return "ReadThrdMgr";

	case eAosGenData_RemoteAddr: 
		 return "RemoteAddr";

	case eAosGenData_RemotePort: 
		 return "RemotePort";

	case eAosGenData_ReceivingBuffer: 
		 return "ReceivingBuffer";

	default:
		 aos_alarm("Unrecognized tag: %d", tag);
		 return "Invalid";
	}

	aos_should_never_come_here;
	return "invalid";
}


aos_gen_data_tag_e aos_gen_data_tag_2enum(const char * const tag)
{
	aos_assert_r(tag, eAosGenData_Invalid);
	if (strcmp(tag, "CrtStmch") == 0) return eAosGenData_CrtStmch;
	if (strcmp(tag, "DataToSend") == 0) return eAosGenData_DataToSend;
	if (strcmp(tag, "DataLen") == 0) return eAosGenData_DataLen;
	if (strcmp(tag, "LocalAddr") == 0) return eAosGenData_LocalAddr;
	if (strcmp(tag, "LocalPort") == 0) return eAosGenData_LocalPort;
	if (strcmp(tag, "ReadCallback") == 0) return eAosGenData_ReadCallback;
	if (strcmp(tag, "ReadThrdMgr") == 0) return eAosGenData_ReadThrdMgr;
	if (strcmp(tag, "RemoteAddr") == 0) return eAosGenData_RemoteAddr;
	if (strcmp(tag, "RemotePort") == 0) return eAosGenData_RemotePort;
	if (strcmp(tag, "ReceivingBuffer") == 0) return eAosGenData_ReceivingBuffer;

	return eAosGenData_Invalid;
}

