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
#ifndef Aos_SmartDoc_SdocVote_h
#define Aos_SmartDoc_SdocVote_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

using namespace boost::gregorian;
using namespace boost::posix_time;

class AosSdocVote : public AosSmartDoc
{

	enum
	{
		eDefaultNumVotes = 1,
		eDefaultNumHours = 0
	};

public:
	AosSdocVote(const bool flag);
	~AosSdocVote();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocVote(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool	queryCond(
				OmnString &query,
				const OmnString &etime,
				const OmnString &crttime,
				const u64 &docid,
				const OmnString &type,
				const OmnString &collection, 
				const OmnString &ctnr_name, 
				const AosRundataPtr &rdata);
	bool	createLog(
				const u64 &docid,
				const OmnString &crttime,
				const OmnString &opr,
				const OmnString &addattr,
				const AosXmlTagPtr &objdef,
				const OmnString &collection,
				const OmnString &ctnr_name,
				const AosRundataPtr &rdata);

	bool	parsequery(
				const OmnString &query,
				OmnString &contents,
				const OmnString &opr,
				const OmnString &collection,
				const OmnString &ctnr_name,
				const AosRundataPtr &rdata);
	bool	getSeqno(u64 &seqno, const AosRundataPtr &rdata);
	bool	process(
				OmnString &crttime,
				OmnString &etime,
				const OmnString &opr_stime,
				const OmnString &opr_etime,
				const int hour,
				const OmnString &type,
				OmnString &cytime,
				const AosRundataPtr &rdata);
};

#endif

