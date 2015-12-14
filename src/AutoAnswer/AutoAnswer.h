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
// This is a utility to select docs.
//
// Modification History:
// 10/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_AutoAnswer_AutoAnswer_h
#define AOS_AutoAnswer_AutoAnswer_h

#include "AutoAnswer/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosAutoAnswer : public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosAutoAnswer(const bool flag);
	~AosAutoAnswer();

	virtual OmnString checkQuestion(const OmnString &question, const AosRundataPtr &rdata) = 0;
	virtual OmnString checkQuestion(const AosXmlTagPtr &question, const AosRundataPtr &rdata) = 0;

private:
	bool registerAutoAnswer(const AosAutoAnswerPtr &answer);
};
#endif
