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
#ifndef Omn_Util_Locale_h
#define Omn_Util_Locale_h

// #include "SEUtil/Ptrs.h"
#include "Util/String.h"


class AosLocale
{
public:
	enum E
	{
		eInvalid, 

		eUSA,
		eChina, 

		eMax
	};

private:
	E					mCountryCode;
	OmnString			mLanguageCode;
	static OmnString	mDftLocaleStr;
	static E			mDftLocale;

public:
	// static bool			config(const AosXmlTagPtr &config);
	static bool			init();
	static OmnString	toStr(const E code);
	static E			toEnum(const OmnString &str);
	static E			getDftLocale() {return mDftLocale;}
	static bool 		isValid(const E id) {return id > eInvalid && id < eMax;}

	OmnString getLanguageCode() const {return mLanguageCode;} 
	static OmnString getDftLanguageCode();
	static AosLocale::E getDefaultLocale() {return mDftLocale;}
};
#endif
