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
// 02/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_QueryClientObj_h
#define Aos_SEInterfaces_QueryClientObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosQueryClientObj : virtual public OmnRCObject
{
private:
	static AosQueryClientObjPtr smQueryClient;

public:
	virtual AosQueryReqPtr createQuery(const AosRundataPtr &rdata, const AosXmlTagPtr &def) = 0;

	static AosQueryClientObjPtr getQueryClient() {return smQueryClient;}
	static void setQueryClient(const AosQueryClientObjPtr &d) {smQueryClient = d;}
};

inline bool AosCreateQuery(const AosRundataPtr &rdata, const AosXmlTagPtr &def)
{
	AosQueryClientObjPtr queryclient = AosQueryClientObj::getQueryClient();
	aos_assert_r(queryclient, 0);
	return queryclient->createQuery(rdata, def);
	return true;
}
#endif

#endif
