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
// 08/09/2011	Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocSchedule_h
#define Aos_SmartDoc_SdocSchedule_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util1/Timer.h"
#include "Util1/TimerObj.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"

class AosSdocSchedule : public AosSmartDoc
{
private:

public:
	AosSdocSchedule(const bool flag);
	~AosSdocSchedule();

	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocSchedule(false);}

};
#endif
