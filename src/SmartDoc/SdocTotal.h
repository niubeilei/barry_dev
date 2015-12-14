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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SDocTotal_SDocTotal_h
#define Aos_SDocTotal_SDocTotal_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocTotal : public AosSmartDoc
{
private:

public:
	AosSdocTotal(const bool flag);
	~AosSdocTotal();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocTotal(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	// virtual bool procDoc(
	// 		const OmnString &siteid,
	// 		const AosWebRequestPtr &req,
	// 		const AosXmlTagPtr &doc,
	// 		const AosSessionPtr &session,
	// 		OmnString &contents,
	// 		AosXmlRc &errcode,
	// 		OmnString &errmsg);
};
#endif

