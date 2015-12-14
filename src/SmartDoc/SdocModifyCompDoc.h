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
// 12/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocModifyCompDocs_h
#define Aos_SmartDoc_SdocModifyCompDocs_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>


class AosSdocModifyCompDoc : public AosSmartDoc
{
private:
	enum
	{
		eMaxModifiedAttrs = 100
	};

public:
	AosSdocModifyCompDoc(const bool flag);
	~AosSdocModifyCompDoc();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocModifyCompDoc(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};

#endif

