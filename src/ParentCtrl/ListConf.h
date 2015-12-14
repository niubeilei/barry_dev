//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ListConf.h
// Description:
// Author: Tim 
//
// Modification History:
//	created 12/27/2006
//	
////////////////////////////////////////////////////////////////////////////
#ifndef __LIST_CONF_H__
#define __LIST_CONF_H__

#include "Util/String.h"

enum{
	eListType_bannedextensionlist = 0,
	eListType_bannedmimetypelist,
	eListType_exceptionextensionlist,
	eListType_exceptionmimetypelist,
	eListType_exceptionfilesitelist,
	eListType_exceptionsitelist,
	eListType_exceptionurllist,
	eListType_bannedsitelist,
	eListType_bannedurllist,
	eListType_greysitelist,
	eListType_greyurllist,
	eListType_Max = 16
};

class ListConf
{
public:
	static const char *sgListType[eListType_Max]; 
	
	ListConf();
	~ListConf();

	static int NewListConf(const char *type, const char *filename);
	static int CheckListType(const char *type);
	static OmnString ShowSupportedListType(void);
};

#endif // #ifndef __LIST_CONF_H__
