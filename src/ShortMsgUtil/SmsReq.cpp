////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/SmsReq.h"

#include "Debug/Debug.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

static int sgNumObjs = 0;

AosSmsReq::AosSmsReq()
{
	sgNumObjs++;
}


AosSmsReq::~AosSmsReq()
{
	sgNumObjs--;
}
