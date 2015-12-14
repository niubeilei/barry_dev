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
//
// Modification History:
// 4/8/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RandomValueGen_RvgContainer_h
#define Aos_RandomValueGen_RvgContainer_h

#include "Parms/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosRvgContainer : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	OmnDynArray<AosRVGPtr>	mParms;

public:
	AosRvgContainer();
	~AosRvgContainer();

	bool	parse(const OmnString &filename);
	bool	parse(const OmnXmlItemPtr &def);
	bool	nextValue(const AosGenTablePtr &table);

	bool	getU16(const u16 index, u16 &value);
	bool	getU32(const u16 index, u32 &value);
	bool	getStr(const u16 index, OmnString &value);
	u32		entries() const {return mParms.entries();}

private:
};
#endif

