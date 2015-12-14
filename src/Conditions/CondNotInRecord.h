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
// InTable condition is used to check whether the generated value
// is in one field of the table. If the value is in table, it return true,
// else false.
//   
//
// Modification History:
// December 28, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondNotInRecord_H
#define Aos_Conditions_CondNotInRecord_H

#include "Conditions/Condition.h"
#include "Util/RCObject.h"
#include "RVG/CommonTypes.h"
#include "Conditions/Ptrs.h"
#include "RVG/Table.h"
#include "RVG/Ptrs.h"
#include <map>

class AosCondNotInRecord : public AosCondition
{
	OmnDefineRCObject;
protected:
	AosTablePtr			mTable;
	std::string			mTableName;
	std::map<std::string, std::string>	mParmFieldMap;
	std::map<std::string, AosValuePtr>	mParmValueMap;

public:
	AosCondNotInRecord();
	AosCondNotInRecord(const std::string& tableName);
	~AosCondNotInRecord();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	void setTable(AosTablePtr table);
	AosTablePtr getTable();
	void setTableName(const std::string& name);
	std::string getTableName();

	bool addParmFieldMap(const std::string& parmName, const std::string& fieldName);
	bool getParmFieldMap(const std::string& parmName, std::string& fieldName);
	void delParmFieldMap(const std::string& parmName);

	bool setParmValue(const std::string& parmName, const AosValuePtr& value);
	bool getParmValue(const std::string& parmName, AosValuePtr& value);
	virtual bool check(const AosExeDataPtr &ed);
	virtual bool check(const AosValue& value);
	virtual bool check(const AosValue& valueLeft, const AosValue& valueRight);

};


#endif


