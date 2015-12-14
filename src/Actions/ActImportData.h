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
#if 0
#ifndef Aos_SdocAction_ActImportData_h
#define Aos_SdocAction_ActImportData_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "ImportData/ImportListener.h"
#include "Rundata/Ptrs.h"


class AosActImportData : virtual public AosSdocAction,
						 public AosImportListener
{
public:
	AosActImportData(const bool flag);
	~AosActImportData();

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
	
	// Import data listener interface
    virtual void importFinished(
			const bool rslt, 
			const AosXmlTagPtr &status_doc,
			const AosRundataPtr &rdata);

};
#endif

#endif
