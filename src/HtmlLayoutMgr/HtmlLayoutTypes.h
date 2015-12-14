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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlLayoutMgr_HtmlLayoutTypes_h
#define Aos_HtmlLayoutMgr_HtmlLayoutTypes_h

#include "Util/String.h"

#define AOSHTML_VHLAYOUT      		"vhbox"
#define AOSHTML_BORDERLAYOUT		"border"
#define AOSHTML_CARDLAYOUT	    	"card"
#define AOSHTML_ABSOLUTELAYOUT		"absolute"
#define AOSHTML_AUTOLAYOUT			"auto"
#define AOSHTML_LMFREESTYLELAYOUT	"lm_freestyle"
#define AOSHTML_ADVERTLAYOUT		"lm_adv_vert"
#define AOSHTML_LMHORIZONTAL		"lm_hori" 
#define AOSHTML_LMVERTICAL			"lm_vert" 
#define AOSHTML_WRAP_VERT			"lm_wrap_vert" 
enum AosHtmlLayoutType
{
	eAosHtmlLayoutType_Invalid,

	eAosHtmlLayoutType_vhbox,
	eAosHtmlLayoutType_border,
	eAosHtmlLayoutType_card,
	eAosHtmlLayoutType_absolute,
	eAosHtmlLayoutType_auto,

	eAosHtmlLayoutType_lm_freestyle,
	eAosHtmlLayoutType_ad_vert,
	eAosHtmlLayoutType_lm_hori,
	eAosHtmlLayoutType_lm_vert,

	eAosHtmlLayoutType_Max
};

extern AosHtmlLayoutType AosHtmlLayoutType_strToCode(const OmnString &typeName);
#endif

