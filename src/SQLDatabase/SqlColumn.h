////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 01/04/2014 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SQLDatabase_SqlColumn_h
#define Aos_SQLDatabase_SqlColumn_h

#include "SQLDatabase/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/DataTypes.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/ValueObj.h"

class AosSqlColumn
{
	OmnDefineRCObject;

protected:
	OmnString mName;
	AosDataType::E mType;
	//the default value needs to be changed to real value
	OmnString mDefault;
	OmnString mComment;

public:
	//constructors/destructors
	AosSqlColumn();
	AosSqlColumn(OmnString name);
	virtual ~AosSqlColumn();

	//getters/setters
	OmnString getName() const;
	OmnString getTypeStr() const;
	AosDataType::E getType() const;
 	virtual void setName(OmnString name);
 	virtual void setComment(OmnString comment);
 	virtual void setType(AosDataType::E type);
 	virtual void setDefault(OmnString defaultVal);

	//member functions
};
#endif

