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
#ifndef aos_omni_action_types_h
#define aos_omni_action_types_h

/////////////////////////// aos_action_type_e ////////////////////
typedef enum
{
	eAosActionType_Invalid, 

	eAosActionType_DoNothing,

	eAosActionType_AppendToFile,
	eAosActionType_GenMsgEvent,
	eAosActionType_GenRecvEvent,
	eAosActionType_RecvMsg,
	eAosActionType_SendMsg,
	eAosActionType_SetValue,
	eAosActionType_TcpServer,
	eAosActionType_ToClipboard,
	eAosActionType_WriteToFile,

	eAosActionType_Last 
} aos_action_type_e;

static inline int aos_action_type_check(aos_action_type_e type)
{
	return (type > eAosActionType_Invalid && 
			type < eAosActionType_Last)?1:0;
}

static inline int aos_action_type_is_net(aos_action_type_e type)
{
	return (type == eAosActionType_SendMsg ||
		    type == eAosActionType_RecvMsg)?1:0;
}

extern const char * aos_action_type_2str(aos_action_type_e type);
extern aos_action_type_e aos_action_type_2enum(const char * const type);




////////////////////////// aos_proto_e //////////////////////////
typedef enum
{
	eAosProto_Invalid = 0, 

	eAosProto_Undefined, 
	eAosProto_Tcp, 
	eAosProto_Udp, 
	eAosProto_Unix,

	eAosProto_Last 
} aos_proto_e;

static inline int aos_proto_check(aos_proto_e type)
{
	return (type > eAosProto_Invalid && 
			type < eAosProto_Last)?1:0;
}

extern const char * aos_proto_2str(aos_proto_e type);
extern aos_proto_e aos_proto_2enum(const char * const type);





////////////////////////// aos_act_finish_md_e ////////////
typedef enum
{
	eAosActFinishMd_Invalid = 0, 

	eAosActFinishMd_Immediate, 
	eAosActFinishMd_UponRecvData, 
	eAosActFinishMd_UponRecvMsg, 

	eAosActFinishMd_Last 
} aos_act_finish_md_e;


static inline int aos_act_finish_md_check(aos_act_finish_md_e type)
{
	return (type > eAosActFinishMd_Invalid && 
			type < eAosActFinishMd_Last);
}

extern const char * aos_act_finish_md_2str(aos_act_finish_md_e type);
extern aos_act_finish_md_e aos_act_finish_md_2enum(const char * const type);


////////////////////////// aos_data_src_e ////////////
typedef enum
{
	eAosDataSrc_Invalid = 0, 

	eAosDataSrc_Clipboard, 
	eAosDataSrc_Direct, 
	eAosDataSrc_File, 

	eAosDataSrc_Last 
} aos_data_src_e;


static inline int aos_data_src_check(aos_data_src_e type)
{
	return (type > eAosDataSrc_Invalid && 
			type < eAosDataSrc_Last);
}

extern const char * aos_data_src_2str(aos_data_src_e type);
extern aos_data_src_e aos_data_src_2enum(const char * const type);

#endif

