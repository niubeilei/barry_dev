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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryVars_QueryVar_h
#define Aos_QueryVars_QueryVar_h

#include "SEInterfaces/QueryVarObj.h"
#include "Util/String.h"


class AosQueryVar : public AosQueryVarObj
{
protected:
	OmnString		mType;

public:
	AosQueryVar(const OmnString &type, const int version);
	~AosQueryVar();

	virtual bool pickJimo(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);

protected:
	bool procDefault( 	const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);
};
#endif



