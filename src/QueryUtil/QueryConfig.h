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
// 01/02/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QueryConfig_h
#define Aos_QueryUtil_QueryConfig_h

#include "XmlUtil/Ptrs.h"



class AosQueryConfig
{
private:
	enum
	{
		eDftQueryPageSize = 50
	};

	static i64		smQueryPageSize;
	static bool		smUsingSmartQuery;

public:
	static bool config(const AosXmlTagPtr &conf);
	
	static bool usingSmartQuery() {return smUsingSmartQuery;}

public:
};
#endif



