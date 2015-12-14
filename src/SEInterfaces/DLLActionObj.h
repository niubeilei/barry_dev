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
// 2013/05/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DLLActionObj_h
#define Aos_SEInterfaces_DLLActionObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/ActionObj.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"

class AosDLLActionObj : public AosDLLObj, public AosActionObj
{
private:
	OmnDefineRCObject;

protected:
	OmnString		mActionName;

public:
	AosDLLActionObj(const OmnString &name, 
					const OmnString &libname, 
					const OmnString &method,
					const OmnString &version);
	virtual ~AosDLLActionObj();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// Action interface
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const char *data, const int len, const AosRundataPtr &rdata);
	virtual bool run(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &) const;
	virtual bool run(const AosTaskObjPtr &task, const AosXmlTagPtr &sdoc, const AosRundataPtr &rundata);
	virtual bool checkConfig(
					const AosXmlTagPtr &def, 
					const AosTaskObjPtr &task, 
					const AosRundataPtr &rdata);
	// Ketty 2013/12/26
	virtual bool checkConfig(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf,
			set<OmnString> &data_col_ids);
	
	virtual bool config(const u64 task_docid,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);

	virtual bool 	initAction(
						const AosTaskObjPtr &task, 
						const AosXmlTagPtr &sdoc,
						const AosRundataPtr &rdata);
	virtual bool finishedAction(const AosRundataPtr &rdata);
	virtual bool	run(const AosRundataPtr &rdata);
};
#endif
