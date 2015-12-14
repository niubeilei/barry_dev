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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_MySqlFieldObj_h
#define Aos_SEInterfaces_MySqlFieldObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosMySqlFieldObj : virtual public OmnRCObject
{
public:

	virtual bool encodeHeader(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff) = 0;        

	virtual bool encodeField(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff) = 0;

};
#endif
