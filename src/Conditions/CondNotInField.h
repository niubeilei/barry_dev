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
// NotInTable condition is used to check whether the generated value
// is in one field of the table. 
//   
//
// Modification History:
// Monday, December 17, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondNotInField_H
#define Aos_Conditions_CondNotInField_H

#include "Conditions/Condition.h"
#include "RVG/Table.h"
#include "RVG/Ptrs.h"

class AosCondNotInField : public AosCondition
{
	OmnDefineRCObject;
protected:
	AosTablePtr			mTable;
	std::string			mTableName;
	std::string			mFieldName;

public:
	AosCondNotInField();
	AosCondNotInField(const std::string& tableName, const std::string& fieldName);
	~AosCondNotInField();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	void setTable(AosTablePtr table);
	AosTablePtr getTable();

	void setTableName(const std::string& name);
	std::string getTableName();

	void setFieldName(const std::string& name);
	std::string getFieldName();

	virtual bool check(const AosExeDataPtr &ed);
	virtual bool check(const AosValue& value);
	virtual bool check(const AosValue& valueLeft, const AosValue& valueRight);

};


#endif


