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
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SqlUtil_SqlReqid
#define Omn_SqlUtil_SqlReqid

#include "Util/String.h"

#define AOSSQLPROC_EXPORTDATA		"exportdata"
#define AOSSQLPROC_IMPORTDATA		"importdata"

class AosSqlReqid
{
public:
	enum E 
	{
		eInvalid,
		eExportData,
		eImportData,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif

