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
// 09/29/2012 Created by Chen Ding
// 2013/05/29 Renamed to Jimo by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_Jimo_h
#define Aos_Jimo_Jimo_h

#include "Jimo/MethodDef.h"
#include "Jimo/GlobalFuncs.h"
#include "Jimo/JimoUtil.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JimoType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosJimo : virtual public OmnRCObject
{
public:
	enum
	{
		eDefaultVersion = 1
	};

protected:
	u32			mJimoType;
	int			mJimoVersion;
	OmnString	mJimoName;

public:
	AosJimo();
	AosJimo(const u32 type, const int version);
	AosJimo(const AosJimo &rhs)
	:
	mJimoType(rhs.mJimoType),
	mJimoVersion(rhs.mJimoVersion)
	{
	}

	~AosJimo();

	virtual bool run(const AosRundataPtr &rdata);
	virtual bool run(const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);

	void *getMethod( 		const AosRundataPtr &rdata, 
							const char *name, 
							AosJimoUtil::funcmap_t &map, 
							AosMethodId::E &method_id);
	virtual u32 getJimoType() const {return mJimoType;}
	virtual u32 getVersion() const {return mJimoVersion;}
	virtual AosJimoPtr cloneJimo() const = 0;

	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);

	virtual void * getMethod(const AosRundataPtr &rdata, 
							const char *name, 
							AosMethodId::E &method_id);

	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							AosBuff *buff);

	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							AosBuff *buff);

	virtual bool queryInterface(
							const AosRundataPtr &rdata, 
							const char *interface_objid) const;

	bool registerMethod( 	const char *name, 
							AosJimoUtil::funcmap_t &map,
							const AosMethodId::E method_id,
							void *func);

	// Chen Ding, 2015/06/01
	void setJimoName(const OmnString &name) {mJimoName = name;}
	OmnString getJimoName() const {return mJimoName;}
};
#endif

