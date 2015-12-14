//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ListConf.cpp
// Description:
//   
// Author: Tim
//
// Modification History: 
//	created 12/27/2006
// 
////////////////////////////////////////////////////////////////////////////
#include "ListConf.h"
#include "aos/aosReturnCode.h"
#include "stdio.h"
#include "Tracer/Tracer.h"
#include "Debug/Debug.h"

#include "Util/String.h"

const char *ListConf::sgListType[eListType_Max] = {
	"bannedextensionlist",
	"bannedmimetypelist",
	"exceptionextensionlist",
	"exceptionmimetypelist",
	"exceptionfilesitelist",
	"exceptionsitelist",
	"exceptionurllist",
	"bannedsitelist",
	"bannedurllist",
	"greysitelist",
	"greyurllist"
};
	

ListConf::ListConf()
{
}

ListConf::~ListConf()
{
}

int ListConf::NewListConf(const char *type, const char *filename)
{
	static FILE *fp;

	fp = fopen(filename, "a+");
	if (!fp) 
	{
		OmnTrace << "failed to new " << filename << endl;
		return -1;
	}
	fclose(fp);
	return eAosRc_Success; 
}

int ListConf::CheckListType(const char *type)
{
	int i;
	for (i = 0; i < eListType_Max; i++) {
		if (OmnString(type) == sgListType[i]) return eAosRc_Success;
	}
	return -1;
}

OmnString ListConf::ShowSupportedListType(void)
{
	OmnString type;
	int i;
	for (i = 0; i < eListType_Max; i++) {
		if (sgListType[i] == 0) continue;
		type += sgListType[i];
		type += "\n";
	}
	return type;
}
