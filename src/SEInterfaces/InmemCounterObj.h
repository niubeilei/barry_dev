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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_InmemCounterObj_h
#define Aos_SEInterfaces_InmemCounterObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/InmemCounterType.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosInmemCounterObj : public OmnRCObject
{
	OmnDefineRCObject

protected:
	AosInmemCounterType::E				mType;
	static AosInmemCounterCreatorObjPtr	smCreator;

public:
	AosInmemCounterObj();
	AosInmemCounterObj(
			const OmnString &name, 
			const AosInmemCounterType::E type, 
			const bool flag);
	~AosInmemCounterObj();

	virtual AosInmemCounterObjPtr	clone(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	AosInmemCounterType::E getType() const {return mType;}

	static AosInmemCounterObjPtr createInmemCounter(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
	
	static bool registerInmemCounter(const OmnString &name, AosInmemCounterObj *cacher);

	static void setCreator(const AosInmemCounterCreatorObjPtr &creator) {smCreator = creator;}
	static AosInmemCounterCreatorObjPtr getCreator() {return smCreator;}

protected:
};

#endif

