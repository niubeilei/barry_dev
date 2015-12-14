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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IILQueryReqObj_h
#define Aos_SEInterfaces_IILQueryReqObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosIILQueryReqObj : virtual public OmnRCObject
{
private:
	static AosIILQueryReqObjPtr smObject;

public:
	static AosIILQueryReqObjPtr getObject() {return smObject;}
	static void setObject(const AosIILQueryReqObjPtr &d) {smObject = d;}

};

#endif

