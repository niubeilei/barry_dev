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
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_HtmlServer_HtmlOpr_h
#define Omn_HtmlServer_HtmlOpr_h

#include "Util/String.h"


class AosHtmlOpr
{
public:
	enum E 
	{
		eInvalid,

		eRetrieveFullVpd,
		eRetrieveVpd,
		eRetrieveGic,
		eRetrieveCreator,
		eRetrieveXmlCreator,
		eSimulate, 
		ePrint,
		eRetrieveSite,
		eServerError,
		eRetrieveContainer,

		eMax
	};

	AosHtmlOpr();

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &opr);
	static bool addName(const OmnString &name, const E e);
};

extern AosHtmlOpr gAosHtmlOpr;
#endif

