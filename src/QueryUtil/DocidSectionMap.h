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
// 2013/01/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_QueryUtil_DocidSectionMap_h
#define AOS_QueryUtil_DocidSectionMap_h

#include "SEInterfaces/DocidSectionMapObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"


OmnDefineSingletonClass(AosDocidSectionMapSingleton,
						AosDocidSectionMap,
						AosDocidSectionMapSelf,
						OmnSingletonObjId::eDocidSectionMap,
						"DocidSectionMap");

class AosDocidSectionMap : virtual public AosDocidSectionMapObj
{
	OmnDefineRCObject;

private:

public:
	AosDocidSectionMap();
	~AosDocidSectionMap();

    // Singleton class interface
    static AosDocidSectionMap *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	virtual int getPhysicalId(const u64 &section_id);
};
#endif
