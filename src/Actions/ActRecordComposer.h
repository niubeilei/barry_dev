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
// 06/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_ActRecordComposer_h
#define Aos_Actions_ActRecordComposer_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "RemoteBackupClt/Ptrs.h"
#include <map>

class AosActRecordComposer : virtual public AosSdocAction
{
private:

public:
	AosActRecordComposer(const bool flag);
	AosActRecordComposer(const AosXmlTagPtr &def, const AosRundataPtr &rdata); 
	~AosActRecordComposer();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const char *record, const int len, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata); 
};
#endif

