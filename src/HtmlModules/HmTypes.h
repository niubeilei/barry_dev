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
// 07/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HtmlModules_HmTypes_h
#define AOS_HtmlModules_HmTypes_h

#include "Util/String.h"


enum AosParmId
{
	eAosParmId_Invalid, 

	eAosParmId_CallFunc, 
	eAosParmId_Canvas,
	eAosParmId_ChangeNotifier, 
	eAosParmId_Clipvar, 
	eAosParmId_CompStr, 
	eAosParmId_Const,
	eAosParmId_LoginInfo, 
	eAosParmId_Member, 
	eAosParmId_Pane, 
	eAosParmId_Record,
	eAosParmId_Sibling,
	eAosParmId_System,
	eAosParmId_Udata,
};
extern AosParmId AosParmId2Enum(const OmnString &name);

enum AosParmMid
{
	eAosParmMid_Invalid,

	eAosParmMid_Caller, 
	eAosParmMid_Canvas, 
	eAosParmMid_Func,
	eAosParmMid_Id, 
	eAosParmMid_Level, 
	eAosParmMid_Parent, 
	eAosParmMid_Self, 
	eAosParmMid_Sysgic, 
	eAosParmMid_Sysvar, 
	eAosParmMid_Window
};
extern AosParmMid AosParmMid2Enum(const OmnString &name);


#endif
