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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_SdocAction_h
#define Aos_SdocAction_SdocAction_h

#include "Actions/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SmartDoc/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/XmlRc.h"

#include <vector>
using namespace std;

class AosSdocAction : public AosActionObj
{
	OmnDefineRCObject;

public:
	AosSdocAction(const OmnString &name, const u32 version);
	AosSdocAction(const OmnString &name, const AosActionType::E type, const bool reg);
	AosSdocAction(const AosActionType::E type);
	~AosSdocAction();

	// Action Interface
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool run(const char *data, const int len, const AosRundataPtr &rdata);
	virtual bool run(const AosTaskObjPtr &task, const AosXmlTagPtr &sdoc, const AosRundataPtr &rundata);
	virtual bool checkConfig(const AosXmlTagPtr &def, const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	
	virtual bool 			initAction(
								const AosTaskObjPtr &task, 
								const AosXmlTagPtr &sdoc,
								const AosRundataPtr &rdata);
	virtual bool			finishedAction(const AosRundataPtr &rdata);
	virtual bool 			run(const AosRundataPtr &rdata);

	// Ketty 2013/12/26
	virtual bool checkConfig(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf,
			set<OmnString> &data_col_ids);

	virtual bool config(const u64 task_docid,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);


	virtual AosJimoPtr cloneJimo() const;

	virtual void outPutPost() {OmnNotImplementedYet;}

protected:
	bool			addThreadShellProc(
						const OmnThrdShellProcPtr &runner,
						const AosRundataPtr &rdata);
};
#endif

