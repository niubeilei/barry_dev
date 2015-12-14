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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermHitType_h
#define Aos_Query_TermHitType_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"


class AosTermHitType : public AosTermIILType
{

public:
	AosTermHitType(const AosQueryType::E type);
	AosTermHitType(const OmnString &name, const AosQueryType::E type, const bool flag);

	virtual bool setOrder(
					const OmnString &container,
					const OmnString &name, 
					const bool reverse, 
					const AosRundataPtr &) {return false;}
	virtual bool getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool	collectInfo(const AosRundataPtr &rdata);

private:
};
#endif

