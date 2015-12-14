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
// 06/28/2010: Created by Lynch yang
////////////////////////////////////////////////////////////////////////////
#include "HtmlLayoutMgr/HtmlLayoutTypes.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"


AosHtmlLayoutType 
AosHtmlLayoutType_strToCode(const OmnString &name)
{
	// All GIC tyes are in the form:
	// 		gic_xxx
	const char *data = name.data();
	const int len = name.length();
	aos_assert_r(len > 0, eAosHtmlLayoutType_Invalid);

	switch (data[0])
	{
	case 'a':
		 if (name == AOSHTML_ABSOLUTELAYOUT) return eAosHtmlLayoutType_absolute;
		 if (name == AOSHTML_AUTOLAYOUT) return eAosHtmlLayoutType_auto;

		 break;

	case 'b':
		 if (name == AOSHTML_BORDERLAYOUT) return eAosHtmlLayoutType_border;
		 break;

	case 'c':
		 if (name == AOSHTML_CARDLAYOUT) return eAosHtmlLayoutType_card;
		 break;

	case 'v':
		 if (name == AOSHTML_VHLAYOUT) return eAosHtmlLayoutType_vhbox;
		 break;

	case 'l':
		 if (len < 4) break;
		 if (data[1] != 'm' || data[2] != '_') break;

		 switch (data[3])
		 {
		 case 'a':
		 	  if (name == AOSHTML_ADVERTLAYOUT) return eAosHtmlLayoutType_ad_vert;
			  break;

		 case 'f':
		 	  if (name == AOSHTML_LMFREESTYLELAYOUT) return eAosHtmlLayoutType_lm_freestyle;
			  break;

		 case 'h':
		 	  if (name == AOSHTML_LMHORIZONTAL) return eAosHtmlLayoutType_lm_freestyle;
			  break;

		 case 'v':
		 	  if (name == AOSHTML_LMVERTICAL) return eAosHtmlLayoutType_lm_freestyle;
			  break;

		 case 'w':
		 	  if (name == AOSHTML_WRAP_VERT) return eAosHtmlLayoutType_lm_freestyle;	// Temporarily, Chen Ding, 08/13/2011
			  break;

		 default:
			  break;
		 }
		 break;
	}

	OmnAlarm << "Unrecognized Layout type: " << name << enderr;
	return eAosHtmlLayoutType_Invalid;
}

