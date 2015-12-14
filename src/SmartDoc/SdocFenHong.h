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
// 08/19/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocFenHong_h
#define Aos_SmartDoc_SdocFenHong_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocFenHong : public AosSmartDoc
{

	enum 
	{
		eMaxNumDocs = 500
	};

public:
	AosSdocFenHong(const bool flag);
	~AosSdocFenHong();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocFenHong(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

};
#endif

