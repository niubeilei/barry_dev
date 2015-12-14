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
// This is the super class for all semantic objects. This class defines
// an interface. Common functions for all semantic objects are implemented
// in SemanticObjCommon. 
//   
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_SemanticObj_h
#define Aos_SemanticObj_SemanticObj_h

#include "aosUtil/Types.h"
#include "Event/EventListener.h"
#include "SemanticObj/SOType.h"
#include "SemanticObj/Ptrs.h"
#include "Util/RCObject.h"
#include <list>

class AosSemanticObj : virtual public AosEventListener
{
public:
	virtual AosSOType::E	getType() const = 0;
	virtual std::string		getListnerName() const = 0;
	virtual bool	start() = 0;
	virtual bool	stop() = 0;
	virtual bool	procEvent(const AosEventPtr &event, bool &cont) = 0;
	virtual bool	registerEvents() = 0;
	virtual bool	unregisterEvents() = 0;
	virtual u32		getHashKey() const = 0;
	virtual bool	isSameObj(const AosSemanticObjPtr &rhs) const = 0;
	virtual bool	integrityCheck(std::list<u32> &error_ids) = 0;
};

#endif

