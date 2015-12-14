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
// 10/24/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_Update_h
#define Aos_Query_Update_h

#include "Query/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include <stdexcept>

class AosUpdate : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum UpdateType
	{
		eUnknow,
		eInc,
		eDec,
		eSet,
		eRemove,
		eSum,
		eScript
	};

	class UpdateException : public runtime_error
	{
	public:
		UpdateException(const string &msg = "Update Command Exception!"):runtime_error(msg){}
	};

private:
	UpdateType type;
	OmnDynArray<AosUpdatePtr, 10, 10, 50> mUpdates; 

public:	
	AosUpdate(){}
	AosUpdate(AosXmlTagPtr &update);
	virtual ~AosUpdate(){}
	
	virtual bool updateData(AosXmlTagPtr &xml){return false;};
	bool update(AosXmlTagPtr &xml);

private:
	AosUpdatePtr getInstance(AosXmlTagPtr &command);
	UpdateType getUpdateType(OmnString &type);
};


class UpdateInc:public AosUpdate
{
	OmnDefineRCObject ;
private:
	OmnString 	mPath;
	i64			mValue;
public:
	UpdateInc(){}
	UpdateInc(AosXmlTagPtr &);
	~UpdateInc(){}
	bool updateData(AosXmlTagPtr &xml);
};

#endif

