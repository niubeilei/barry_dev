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
// 12/31/2011   Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActCounterOccurences_h
#define Aos_SdocAction_ActCounterOccurences_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::gregorian;
using namespace boost::posix_time;


class AosActCountOccurences : virtual public AosSdocAction
{
public:
	enum
	{
		eMaxFields = 30
	};

public:
	AosActCountOccurences(const bool flag);
	~AosActCountOccurences();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

