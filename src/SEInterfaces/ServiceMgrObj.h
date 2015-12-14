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
// 2015/08/14 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ServiceMgrObj_h
#define Aos_SEInterfaces_ServiceMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskMgr/TaskErrorType.h"

class AosServiceMgrObj : virtual public OmnRCObject
{
	static AosServiceMgrObjPtr	smServiceMgr;
public:
	virtual bool 	addService(
						const u64 &service_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id) = 0;
	virtual bool stopTaskProcCb(const u32 logic_pid, const int svr_id) = 0;

	static void setServiceMgr(const AosServiceMgrObjPtr &d) {smServiceMgr = d;}
	static AosServiceMgrObjPtr getServiceMgr() {return smServiceMgr;}

	static bool addServiceStatic(
			const u64 &service_docid,
			const AosRundataPtr &rdata);

	static bool startTaskProcCbStatic(const u32 logic_pid, const int proc_pid, const int svr_id);
	static bool stopTaskProcCbStatic(const u32 logic_pid, const int svr_id);
};

#endif
