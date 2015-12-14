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
#include "action_c/types.h"

#include "alarm_c/alarm.h"
#include <string.h>


const char * aos_action_type_2str(aos_action_type_e type)
{
	switch (type)
	{
	case eAosActionType_DoNothing:
		 return "DoNothing";

	case eAosActionType_SendMsg:
		 return "SendMsg";

	case eAosActionType_SetValue:
		 return "SetValue";

	case eAosActionType_RecvMsg:
		 return "RecvMsg";

	case eAosActionType_TcpServer:
		 return "TcpServer";

	case eAosActionType_GenMsgEvent:
		 return "GenMsgEvent";

	case eAosActionType_GenRecvEvent:
		 return "GenRecvEvent";

	default:
		 break;
	}

	aos_alarm("Unrecognized action type: %d", type);
	return "invalid";
}


aos_action_type_e aos_action_type_2enum(const char * const type)
{
	aos_assert_r(type, eAosActionType_Invalid);

	if (strcmp(type, "DoNothing") == 0) return eAosActionType_DoNothing;
	if (strcmp(type, "SendMsg") == 0) return eAosActionType_SendMsg;
	if (strcmp(type, "RecvMsg") == 0) return eAosActionType_RecvMsg;
	if (strcmp(type, "TcpServer") == 0) return eAosActionType_TcpServer;
	if (strcmp(type, "GenMsgEvent") == 0) return eAosActionType_GenMsgEvent;
	if (strcmp(type, "GenRecvEvent") == 0) return eAosActionType_GenRecvEvent;
	if (strcmp(type, "SetValue") == 0) return eAosActionType_SetValue;

	return eAosActionType_Invalid;
}


const char * aos_proto_2str(aos_proto_e type)
{
	switch (type)
	{
	case eAosProto_Undefined:
		 return "Undefined";

	case eAosProto_Tcp:
		 return "TCP";

	case eAosProto_Udp: 
		 return "UDP";

	case eAosProto_Unix:
		 return "UNIX";

	default:
		 break;
	}

	aos_alarm("Unrecognized proto type: %d", type);
	return "Invalid";
}


aos_proto_e aos_proto_2enum(const char * const type)
{
	aos_assert_r(type, eAosProto_Invalid);

	if (strcmp(type, "Undefined") == 0) return eAosProto_Undefined;
	if (strcmp(type, "TCP") == 0) return eAosProto_Tcp;
	if (strcmp(type, "UDP") == 0) return eAosProto_Udp;
	if (strcmp(type, "UNIX") == 0) return eAosProto_Unix;

	aos_alarm("Unrecognized type: %s", type);
	return eAosProto_Invalid;
}


const char * aos_data_src_2str(aos_data_src_e type)
{
	switch (type)
	{
	case eAosDataSrc_File:
		 return "File";

	case eAosDataSrc_Direct:
		 return "Direct";

	case eAosDataSrc_Clipboard:
		 return "Clipboard";

	default:
		 break;
	}

	aos_alarm("Unrecognized data src type: %d", type);
	return "Invalid";
}


aos_data_src_e aos_data_src_2enum(const char * const type)
{
	aos_assert_r(type, eAosProto_Invalid);

	if (strcmp(type, "File") == 0) return eAosDataSrc_File;
	if (strcmp(type, "Direct") == 0) return eAosDataSrc_Direct;
	if (strcmp(type, "Clipboard") == 0) return eAosDataSrc_Clipboard;

	aos_alarm("Unrecognized type: %s", type);
	return eAosDataSrc_Invalid;
}


const char * aos_act_finish_md_2str(aos_act_finish_md_e type)
{
	switch (type)
	{
	case eAosActFinishMd_UponRecvData:
		 return "UponRecvData";

	case eAosActFinishMd_UponRecvMsg:
		 return "UponRecvMsg";

	case eAosActFinishMd_Immediate:
		 return "Immediate";

	default:
		 break;
	}

	return "Invalid";
}


aos_act_finish_md_e aos_act_finish_md_2enum(const char * const type)
{
	aos_assert_r(type, eAosActFinishMd_Invalid);

	if (strcmp(type, "Immediate") == 0) 
		return eAosActFinishMd_Immediate;

	if (strcmp(type, "UponRcvdData") == 0) 
		return eAosActFinishMd_UponRecvData;

	if (strcmp(type, "UponRcvdMsg") == 0) 
		return eAosActFinishMd_UponRecvMsg;

	return eAosActFinishMd_Invalid;

}


