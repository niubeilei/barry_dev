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
// 06/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_ActDataCombiner_h
#define Aos_Actions_ActDataCombiner_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "RemoteBackupClt/Ptrs.h"
#include <map>

class AosActDataCombiner : virtual public AosSdocAction
{
private:

public:
	AosActDataCombiner(const bool flag);
	~AosActDataCombiner();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

