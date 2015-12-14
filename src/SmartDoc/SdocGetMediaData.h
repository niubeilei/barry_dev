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
// 12/22/2010	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocGetMediaData_h
#define Aos_SmartDoc_SdocGetMediaData_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocGetMediaData : public AosSmartDoc
{
public:
	enum
	{
		eMaxTypes = 50
	};

	AosSdocGetMediaData(const bool flag);
	~AosSdocGetMediaData();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocGetMediaData(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool getData(OmnString &contents, const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
};
#endif

