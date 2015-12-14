////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 09/27/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Locale.h"

#include "Alarm/Alarm.h"
// #include "SEUtil/XmlTag.h"



static OmnString	sgLocaleNames[AosLocale::eMax];
static OmnString	sgDftLocaleStr = "us";
static OmnString	sgDftLanguageCode = "us";
OmnString AosLocale::mDftLocaleStr = sgDftLocaleStr;
AosLocale::E AosLocale::mDftLocale = AosLocale::eUSA;


/*
bool
AosLocale::config(const AosXmlTagPtr &config)
{
	if (!config) return true;
	AosXmlTagPtr def = config->getFirstChild(AOSCONFIG_LOCALE);
	if (!def) return true;

	mDftLocaleStr = def->getAttrStr(AOSTAG_DFTLOCALE, sgDftLocaleStr);
	mDftLocale = toEnum(mDftLocaleStr);
	if (mDftLocale == eInvalid) mDftLocale = AosLocale::eUSA;
	return true;
}
*/


bool
AosLocale::init()
{
	sgLocaleNames[AosLocale::eUSA] 			= "us";
	sgLocaleNames[AosLocale::eChina] 		= "cn";
	return true;
}


OmnString	
AosLocale::toStr(const E code)
{
	if (code <= eInvalid || code >= eMax) return mDftLocaleStr;
	return sgLocaleNames[code];
}


AosLocale::E
AosLocale::toEnum(const OmnString &str)
{
	int len = str.length();
	if (len <= 0) return mDftLocale;
	const char *data = str.data();

	switch (data[0])
	{
	case 'c':
		 if (len == 2 && data[1] == 'n') return eChina;
		 break;

	case 'u':
		 if (len == 2 && data[1] == 's') return eUSA;
		 break;

	default:
		 return mDftLocale;
	}

	OmnShouldNeverComeHere;
	return eInvalid;
}


OmnString
AosLocale::getDftLanguageCode()
{
	return sgDftLanguageCode;
}

