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
// 2012/07/30 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActDataJoin_h
#define Aos_SdocAction_ActDataJoin_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataJoin/DataJoinListener.h"
#include "DataJoin/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActDataJoin : virtual public AosSdocAction,
					  public AosDataJoinListener
{

public:
	AosActDataJoin(const bool flag);
	~AosActDataJoin();

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
	
	// DataJoinListener interface
    virtual void joinFinished(
			const bool rslt, 
			const AosXmlTagPtr &status_doc,
			const AosRundataPtr &rdata);
	
};
#endif

#endif
