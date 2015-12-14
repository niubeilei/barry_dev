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
// 07/28/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCheckUnique_h
#define Aos_SmartDoc_SdocCheckUnique_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocCheckUnique : public AosSmartDoc
{

public:
	AosSdocCheckUnique(const bool flag);
	~AosSdocCheckUnique();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCheckUnique(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

};
#endif

