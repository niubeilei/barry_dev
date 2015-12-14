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
// 2015/01/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SyncEngineObj_h
#define Aos_SEInterfaces_SyncEngineObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "JimoCall/JimoCall.h"


class AosSyncEngineObj : public AosJimo
{
private:
	static AosSyncEngineObjPtr		smSyncEngine;

public:
	AosSyncEngineObj(const int version);

	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool proc(AosRundata *rdata) = 0;
	virtual bool config(const AosXmlTagPtr &def) = 0;

	static void setSyncEngine(const AosSyncEngineObjPtr &sync_engine) {smSyncEngine = sync_engine;}
	static AosSyncEngineObj *getSyncEngine() {return smSyncEngine.getPtrNoLock();}

	virtual bool addSyncher(AosRundata *rdata, const AosSyncherObjPtr &syncher) = 0;
	virtual AosSyncherObjPtr getSyncher() = 0;
	virtual bool runSyncher(AosRundata *rdata, AosJimoCall &jimo_call) = 0;
};
#endif

