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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DLLObj_h
#define Aos_SEInterfaces_DLLObj_h

#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosDLLObj : virtual public OmnRCObject
{
public:
	enum Type 
	{
		eInvalidObj,

		eDataProc,
		eDataCalc,
		eAction,
		eCondition,

		eMaxDLLType
	};

protected:
	Type		mType;
	OmnString	mLibname;
	OmnString	mMethod;
	OmnString	mVersion;

public:
	AosDLLObj(	const Type type, 
				const OmnString &libname,
				const OmnString &method, 
				const OmnString &version);
	virtual ~AosDLLObj();

	static OmnString enumToString(const Type type);

	OmnString getLibname() const {return mLibname;}
	OmnString getMethod() const {return mMethod;}
	OmnString getVersion() const {return mVersion;}
	Type getType() const {return mType;}

	AosDLLDataProcObjPtr 	convertToDataProc(const AosRundataPtr &rdata);
	AosDataCalcObjPtr 		convertToDataCalc(const AosRundataPtr &rdata);
	AosDLLActionObjPtr 		convertToAction(const AosRundataPtr &rdata);
	AosDLLCondObjPtr 		convertToCondition(const AosRundataPtr &rdata);
};
#endif

