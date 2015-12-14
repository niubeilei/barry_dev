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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SuperTable_SuperTable_h
#define Aos_SuperTable_SuperTable_h

#include "SEInterfaces/SuperTableObj.h"


class AosSuperTable : public AosSuperTableObj
{
protected:
	OmnString		mObjid;

public:
	AosSuperTable(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &doc);
	~AosSuperTable();

protected:
	AosXmlTagPtr retrieveTableDoc(const AosRundataPtr &rdata);
};
#endif

