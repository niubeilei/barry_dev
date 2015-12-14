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
// 04/16/2011   Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActVerCode_h
#define Aos_SdocAction_ActVerCode_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::gregorian;
using namespace boost::posix_time;


class AosActVerCode : virtual public AosSdocAction
{
public:
	AosActVerCode(const bool flag);
	~AosActVerCode();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool	checkTime(const OmnString &uid, const int minute, const AosRundataPtr &);
};
#endif

