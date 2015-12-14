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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/MetaExtension.h"

OmnString AosMetaExtension::mNames[AosMetaExtension::eMax+1];


static AosMetaExtension sgMetaExtension;


AosMetaExtension::AosMetaExtension()
{
	init();
}


bool
AosMetaExtension::init()
{
	mNames[eInvalid] 			= "invalid";

	mNames[eCloudid] 			= "cloudid";
	mNames[eDocid] 				= "docid";
	mNames[eObjid] 				= "objid";
	mNames[ePrivate] 			= "private";
	mNames[eCreateByTemplate] 	= "cbtpl@";

	mNames[eMax] 				= "invalid";

	return true;
}


OmnString
AosMetaExtension::getTemplatePrefix()
{
	return mNames[eCreateByTemplate];
}


int
AosMetaExtension::getTemplatePrefixLen()
{
	return mNames[eCreateByTemplate].length();
}

AosMetaExtension::E
AosMetaExtension::toEnum(const OmnString &theopr)
{
	if (theopr.length() < 5) return eInvalid; 
	const char *data = theopr.data();
	char cc = data[0];
	OmnString opr = theopr;
	if (data[0] == '{')
	{
		// It is the form: 
		// 		{xxx}
		cc = data[1];
		opr.assign(&data[1], opr.length()-1);
	}

	switch (cc)
	{
	case 'c':
		 if (opr == mNames[eCloudid]) return eCloudid;
		 if (opr == mNames[eCreateByTemplate]) return eCreateByTemplate;
		 break;

	case 'd':
		 if (opr == mNames[eDocid]) return eDocid;
		 break;

	case 'o':
		 if (opr == mNames[eObjid]) return eObjid;
		 break;

	case 'p':
		 if (opr == mNames[ePrivate]) return ePrivate;
		 break;

	default:
		 break;
	}

	return eInvalid;
}


OmnString	
AosMetaExtension::toString(const E opr)
{
	if (opr <= eInvalid || opr >= eMax) return mNames[eInvalid];
	return mNames[opr];
}


