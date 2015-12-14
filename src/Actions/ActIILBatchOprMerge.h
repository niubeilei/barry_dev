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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActIILBatchOprMerge_h
#define Aos_SdocAction_ActIILBatchOprMerge_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"


class AosActIILBatchOprMerge : virtual public AosSdocAction
{

public:
	AosActIILBatchOprMerge(const bool flag);
	~AosActIILBatchOprMerge();

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	
	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

};
#endif

