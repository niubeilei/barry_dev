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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_GenericObj_h
#define Aos_GenericObjs_GenericObj_h

#include "SEInterfaces/GenericObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"


class AosGenericObject : public AosGenericObj
{
	OmnDefineRCObject;

protected:
	OmnString		mObjType;

public:
	AosGenericObject(int version);
	~AosGenericObject();

	virtual OmnString getObjType() const {return mObjType;}
	virtual bool config( 	AosRundata *rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);

	virtual bool execute(AosRundata *rdata, 
						const OmnString &verb_name, 
						const OmnString &obj_name,  
						const OmnString &json) {return false;}
};
#endif

