////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEInterfaces_JobSplitterObj_h
#define AOS_SEInterfaces_JobSplitterObj_h

#include "SEInterfaces/JobSplitterId.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <map>
using namespace std;


class AosJobSplitterObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	static AosJobSplitterObjPtr 	smObject;

public:
	virtual AosJobSplitterId::E getJobSplitterId() const = 0;
	virtual void setJobENV(map<OmnString, OmnString> &job_env) = 0;

	// JobSplitterObj Interface
	virtual bool init() = 0;
	virtual void clear() = 0;

	virtual bool splitTasks(
					const AosXmlTagPtr &def,
					const AosTaskDataObjPtr &task_data,
					vector<AosTaskDataObjPtr> &tasks,
					const AosRundataPtr &rdata) = 0;

	virtual AosJobSplitterObjPtr create(
					const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &rdata) const = 0;

	virtual AosJobSplitterObjPtr createJobSplitter(
			        const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
				    const AosRundataPtr &rdata) = 0;

	static bool initStatic();
	static AosJobSplitterObjPtr createJobSplitterStatic(
			    	const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &rdata);

	static void setJobSplitter(const AosJobSplitterObjPtr &obj) {smObject = obj;}
	static AosJobSplitterObjPtr getJobSplitter() {return smObject;}
};
#endif

#endif
