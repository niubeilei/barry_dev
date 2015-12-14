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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0

#ifndef Aos_SEInterfaces_Scheduler_h
#define Aos_SEInterfaces_Scheduler_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"


class AosScheduler : public AosJimo
{
public:
	AosScheduler(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc, 
			const AosXmlTagPtr &jimo_doc);
	~AosScheduler();

	virtual OmnString toString() const;

	virtual bool addSchedule(const AosXmlTagPtr &sdoc, 
							const AosRundataPtr &rdata);

	virtual bool checkSchedule(const AosXmlTagPtr &sdoc, 
							const AosRundataPtr &rdata);

	virtual bool removeSchedule(const AosXmlTagPtr &sdoc, 
							const AosRundataPtr &rdata);

	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);

	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);
	
	virtual bool config( 	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);
	
protected:
	bool retrieveScheduleInfo(
							const AosXmlTagPtr &sdoc, 
							AosXmlTagPtr &schedule_doc,
							AosXmlTagPtr &rsc_doc,
							AosXmlTagPtr &ctnr_doc,
							const AosRundataPtr &rdata);
};

#endif
#endif

