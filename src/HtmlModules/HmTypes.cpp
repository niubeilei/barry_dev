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
#include "HtmlModules/HmTypes.h"

#include "Alarm/Alarm.h"



AosParmId AosParmId2Enum(const OmnString &name)
{
	switch (name.data()[0])
	{
	case 'c':
		 if (name == "cfunc") return eAosParmId_CallFunc;
		 if (name == "compstr") return eAosParmId_CompStr;
		 if (name == "const") return eAosParmId_Const;
		 if (name == "clipvar") return eAosParmId_Clipvar;
		 if (name == "canvas") return eAosParmId_Canvas;
		 if (name == "chntf") return eAosParmId_ChangeNotifier;
		 break;

	case 'm':
		 if (name == "member") return eAosParmId_Member;
		 break;

	case 'l':
		 if (name == "logininfo") return eAosParmId_LoginInfo;
		 break;

	case 'p':
		 if (name == "pane") return eAosParmId_Pane;
		 break;

	case 'r':
		 if (name == "record") return eAosParmId_Record;
		 break;

	case 's':
		 if (name == "sibling") return eAosParmId_Sibling;
		 if (name == "system") return eAosParmId_System;
		 break;

	case 'u':
		 if (name == "udata") return eAosParmId_Udata;
		 break;

	default:
		 OmnAlarm << "Unrecognized parm ID: " << name << enderr;
		 return eAosParmId_Invalid;
	}

	OmnShouldNeverComeHere;
	return eAosParmId_Invalid; 
}


AosParmMid AosParmMid2Enum(const OmnString &name)
{
	switch (name.data()[0])
	{
	case 'c':
		 if (name == "caller") return eAosParmMid_Caller;
		 if (name == "canvas") return eAosParmMid_Canvas;
		 break;

	case 'f':
		 if (name == "func") return eAosParmMid_Func;
		 break;

	case 'i':
		 if (name == "id") return eAosParmMid_Id;
		 break;

	case 'l':
		 if (name == "level") return eAosParmMid_Level;
		 break;

	case 'p':
		 if (name == "parent") return eAosParmMid_Parent;
		 break;

	case 's':
		 if (name == "self") return eAosParmMid_Self;
		 if (name == "sysgic") return eAosParmMid_Sysgic;
		 if (name == "sysvar") return eAosParmMid_Sysvar;
		 break;

	case 'w':
		 if (name == "window") return eAosParmMid_Window;
		 break;

	default:
		 break;
	}

	OmnAlarm << "Invalid Parm MID: " << name << enderr;
	return eAosParmMid_Invalid;
}

