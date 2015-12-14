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
// 06/13/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActLimited_h
#define Aos_SdocAction_ActLimited_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/Opr.h"
#include <vector>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>
using namespace boost::gregorian;
using namespace boost::posix_time;

class AosActLimited : virtual public AosSdocAction
{
private:
	enum
	{
		eDefaultNumVotes = 1,
		eDefaultNumHours = 0,
		eMaxValues = 20
	};

public:
	AosActLimited(const bool flag);
	~AosActLimited();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	OmnString	getCrtTime(
					const AosRundataPtr &rdata,
					ptime &pt1);

	bool	process(
				const AosRundataPtr &rdata,
				const ptime &pcrt_time,
				const OmnString &type,
				const OmnString &cycletime,
				const int hh,
				OmnString &etime);

	bool	runQuery(
				const AosRundataPtr &rdata,
				const OmnString &ctnr_objid,
				const OmnString *anames,
				const OmnString *values,
				const AosOpr *opr,
				const int num_conds,
				AosQueryRsltObjPtr &query_rslt);

	bool	check(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr_objid,
				const int &num,
				int &total,
				const OmnString &crt_time,
				const OmnString &endtime,
				const OmnString &type);

	bool	createLog(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr_objid,
				const OmnString &crttime);

	bool	createLogContainer(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr_objid);

	OmnString 	getContainerObjid(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc);

	OmnString	getLogAttribute(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc);

	bool 	parseTerms(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				OmnString *anames,
				OmnString *values,
				AosOpr *opr,
				int &num_conds); 
};
#endif

