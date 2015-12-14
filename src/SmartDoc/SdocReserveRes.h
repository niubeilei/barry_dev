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
// 04/25/2011	Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocReserveRes_h
#define Aos_SmartDoc_SdocReserveRes_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util1/Timer.h"
#include "Util1/TimerObj.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::gregorian;
using namespace boost::posix_time;

class AosSdocReserveRes : public AosSmartDoc, public OmnTimerObj
{
private:
	OmnMutexPtr         mLock;

public:
	AosSdocReserveRes(const bool flag);
	~AosSdocReserveRes();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocReserveRes(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	// TimerObj Interface
	virtual void timeout(const int timerId, const OmnString &timerName, void *parm);

	bool	reserve(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool	cancel(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &sdoc,		
			const AosXmlTagPtr &cmd);
	bool	confirm(const AosRundataPtr &rdata,
			const AosXmlTagPtr &cmd);
	
	bool	getTime(
			const OmnString &time,
			const OmnString &format, 
			int &timerSec, 
			int &timerUser);
	

};
#endif
