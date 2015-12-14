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
#ifndef OMN_SEUtil_MetaExtension_h
#define OMN_SEUtil_MetaExtension_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosMetaExtension : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eInvalid,

		eCloudid,
		eDocid,
		ePrivate,
		eObjid,
		eCreateByTemplate,

		eMax
	};

	enum
	{
		eIdChar = '$',
		eSepChar = '.'
	};

private:
	static OmnString	mNames[eMax+1];

public:
	AosMetaExtension();
	~AosMetaExtension() {}

	static E 			toEnum(const OmnString &opr);
	static OmnString	toString(const E opr);
	static OmnString	getAccessTag(const E opr) {return toString(opr);}
	static OmnString	getTemplatePrefix();
	static int			getTemplatePrefixLen();

private:
	bool	init();
};
#endif

