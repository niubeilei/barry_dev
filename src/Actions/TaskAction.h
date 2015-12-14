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
// 06/04/2013	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_TaskAction_h
#define Aos_Actions_TaskAction_h

#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/ActionObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"

class AosTaskAction : public AosActionObj
{
	OmnDefineRCObject;

protected:
	u64 					mTaskDocid;
	AosXmlTagPtr			mSdoc;

public:
	AosTaskAction(const OmnString &name, const AosActionType::E type, const bool reg);
	AosTaskAction(const AosActionType::E type);
	~AosTaskAction();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// Action Interface
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool run(const char *data, const int len, const AosRundataPtr &rdata);
	virtual bool run(const AosTaskObjPtr &task, const AosXmlTagPtr &sdoc, const AosRundataPtr &rundata);
	virtual bool checkConfig(const AosXmlTagPtr &def, const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	
	virtual bool checkConfig(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf,
			set<OmnString> &data_col_ids);
	virtual void outPutPost() { OmnNotImplementedYet; }
protected:
	bool					remoteCreateDataCol(
								const AosXmlTagPtr &output_xml,
								const AosRundataPtr &rdata);
};
#endif

