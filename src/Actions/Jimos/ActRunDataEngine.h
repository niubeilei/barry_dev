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
// 2013/12/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_Jimos_ActRunDataEngine_h
#define Aos_SdocAction_Jimos_ActRunDataEngine_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/DataEngineListener.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActRunDataEngine : virtual public AosSdocAction,
						 	public AosDataEngineListener
{
public:
	AosActRunDataEngine(const AosRundataPtr &rdata, const OmnString &version);
	~AosActRunDataEngine();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// DataEngineListener interface
    virtual void dataEngineFinished(
						const AosRundataPtr &rdata,
						const bool rslt, 
						const AosXmlTagPtr &status_doc);

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool checkConfig(
			const AosXmlTagPtr &sdoc,
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
	
private:
    void actionFinished(
			const AosRundataPtr &rdata,
			const bool rslt, 
			const AosXmlTagPtr &status_doc);
	AosDataEngineObjPtr createDataEngine(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc);
};
#endif

