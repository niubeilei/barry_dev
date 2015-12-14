////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 01/02/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_QueryClient_QueryTrans_h
#define Aos_QueryClient_QueryTrans_h

#include "Query/Ptrs.h"
#include "QueryUtil/QrUtil.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"


class AosQueryTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosQueryReqPtr		mQueryReq;

public:
	AosQueryTrans(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosQueryTrans();

	bool	isNew() const;
	bool	isGood() const;
	void	setQueryType(const OmnString &type);
	void	setStmt(const OmnString &query);
	void	setStartIdx(const OmnString &d);
	void	setPsize(const OmnString &);
	bool 	setOrder(const OmnString &container, 
				const OmnString &order_fname, 
				const OmnString &reverse, 
				const AosRundataPtr &rdata);
	bool	setSubopr(const OmnString &);
	void	setRemoveMeta(const OmnString &ss);
	void	setIdx(const int i);
	bool	setJoins(const OmnString &join, OmnString &errmsg);
	bool	startQuery();
	bool	procPublic(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &cmd,
			OmnString &contents);
	bool	parseSqlPublic(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cmd); 
};
#endif
#endif
