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
// 2014/12/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_WordParserObj_h
#define AOS_SEInterfaces_WordParserObj_h

#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosWordParserObj : virtual public OmnRCObject
{
public:
	virtual bool	setSrc(const OmnString &src) = 0;
	virtual bool	setSrc(const char* src, int start, int len) = 0;
	virtual bool	nextWord(OmnString &wordbuff) = 0;
	virtual bool	nextWord(char* wordbuff, int &len) = 0;
};

#endif
