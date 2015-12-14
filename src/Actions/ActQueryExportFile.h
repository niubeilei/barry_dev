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
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActQueryExportFile_h
#define Aos_SdocAction_ActQueryExportFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/DataAssemblerType.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/SengineImportDocObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Sem.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <map>
#include "Debug/Debug.h"


class AosActQueryExportFile : virtual public AosSdocAction
{
public:
	AosActQueryExportFile(const bool flag);
	~AosActQueryExportFile();

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
};
#endif

