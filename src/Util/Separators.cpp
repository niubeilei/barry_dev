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
// 01/01/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Separators.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


char AosSeparators::smSeparators[AosSeparators::eMaxLevels] = 
{
	0x01, 
	0x02, 
	0x03, 
	0x04, 
	0x05, 
	0x06, 
	0x07, 
	0x08 
};


bool 
AosSeparators::config(const AosXmlTagPtr &conf)
{
	// The config:
	// 	<config ...>
	// 		...
	// 		<separators
	// 			level0="xxx"
	// 			level1="xxx"
	// 			...
	// 			level7="xxx"
	// 		/>
	// 		...
	// 	</config>
	if (!conf) return true;
	AosXmlTagPtr tag = conf->getFirstChild("separators");
	if (!tag) return true;

	for (int i=0; i<eMaxLevels; i++)
	{
		OmnString name = "level";
		name << i;
		OmnString vv = tag->getAttrStr(name);
		char value;
		if (vv != "")
		{
			bool rslt = convert(vv, value);
			aos_assert_r(rslt, false);
			smSeparators[i] = value;
		}
	}

	return true;
}


bool
AosSeparators::convert(const OmnString &str, char &value)
{
	value = 0;
	if (str.length() != 4) return false;

	char *data = (char *)str.data();
	aos_assert_r(data, false);
	if (data[0] != '0' || data[1] != 'x') return false;

	char c = data[2];
	u32 code1 = 0;
	if ((c >= '0' && c <= '9')) 
	{
		code1 = c - '0';
	}
	else 
	{
		if (c >= 'a' && c <= 'f') code1 = c - 'a' + 10;
		else return false;
	}

	c = data[3];
	u32 code2 = 0;
	if ((c >= '0' && c <= '9')) 
	{
		code2 = c - '0';
	}
	else 
	{
		if (c >= 'a' && c <= 'f') code2 = c - 'a' + 10;
		else return false;
	}

	value = (char)((code1 << 4) + code2);
	return true;
}


char 
AosSeparators::getSeparator(const int level)
{
	aos_assert_r(level >= 0 && level < eMaxLevels, 0);
	return smSeparators[level];
}

