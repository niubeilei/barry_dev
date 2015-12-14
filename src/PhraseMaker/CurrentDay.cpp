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
// Modification History:
// 2013/05/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "PhraseMaker/PhraseMaker.h"

#include "Porting/GetTime.h"



AosPhrmkCurrentDay::AosPhrmkCurrentDay()
{
}
	

AosPhrmkCurrentDay::~AosPhrmkCurrentDay()
{
}


bool
AosPhrmkCurrentDay::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


OmnString 
AosPhrmkCurrentDay::create(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	// 'def' is:
	// 	<phrasemaker
	//		type=”current_day”
	//		fixlength=”true|false”
	//		format=”xxx”/>
	// where 'format' is:
	// 		'yyyymmdd'
	// 		'yymmdd'
	// 		'mddyyyy'
	// 		'mmddyy'
	// 		'MMMddyyyy'
	// 		'Monthddyy'
	//
	aos_assert_rr(def, rdata, "");

	OmnString format = def->getAttrStr("format");
	aos_assert_rr(format != "", rdata, "");
	const char *data = format.data();
	int len = format.length();
	switch (data[0])
	{
	case 'y':
		 // yyyymmdd
		 if (length == 8)
		 {
			 if (format == FORMAT_yyyymmdd) return AosGetTime_yyyymmdd();
		 }
		 else if (length == 6)
		 {
			 if (format == FORMAT_yymmdd) return AosGetTime_yymmdd();
		 }
		 break;

	case 'm':
		 // 'mmddyyyy'
		 // 'mmddyy'
		 if (length == 8)
		 {
			 if (format == FORMAT_mmddyyyy) return AosGetTime_mmddyyyy();
		 }
		 else if (length == 6)
		 {
			 if (format == FORMAT_mmddyy) return AosGetTime_mmddyy();
		 }
		 break;

	case 'M':
		 // 'MMMddyyyy'
		 // 'Monthddyy'
		 if (length == 8)
		 {
			 if (format == FORMAT_MMMddyyyy) return AosGetTime_MMddyyyy();
		 }
		 else if (length == 6)
		 {
			 if (format == FORMAT_Monthddyy) return AosGetTime_Monthddyy();
		 }
		 break;

	default:
		 break;
	}

	AosSetErrorU(rdata, "invalid_format") << format << enderr;
	return "";
}


