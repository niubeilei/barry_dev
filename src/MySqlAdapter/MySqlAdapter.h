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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MySqlAdapter_MySqlAdapter_h
#define Aos_MySqlAdapter_MySqlAdapter_h

#include "Conds/Ptrs.h"
#include "SEInterfaces/MySqlAdapterObj.h"
#include "SEUtil/FieldOpr.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"


class AosMySqlAdapter : public AosMySqlAdapterObj
{
	OmnDefineRCObject;
public:
	AosMySqlAdapter();
	~AosMySqlAdapter();
	
	virtual bool procCommand(const AosRundataPtr &rdata, const AosBuffPtr &data); 
	virtual bool procQuery(const AosRundataPtr &rdata, const AosBuffPtr &buff);

protected:
};

#endif
