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
// Generate multiple values from a record which is selected randomly 
// from the table. For example, if the table contains four fields, 
// "a", "b", "c", "d" we want to get "b" and "c" values from the table 
// and the values are in the same record.  It is similar as "select 
// b,c from tablename", then randomly select
// one record in the select statement result.
//   
//
// Modification History:
// December 26, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RVG_RVGMultiFromTable_H
#define Aos_RVG_RVGMultiFromTable_H

#if 0
#include "RVG/RVG.h"
#include "RVG/Table.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlLib/tinyxml.h"
#include <map>

class AosRVGMultiFromTable : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnString			mName;

	u32					mSelectPct;		// Allowed: [0, 100]
	AosTablePtr			mTable;
	OmnString			mTableName;
	std::map<OmnString, OmnString>	mParmFieldMap;

	AosValuePtrArray	mGenValues;

public:
	AosRVGMultiFromTable();
	~AosRVGMultiFromTable();

	//
	// Randomly select a record from the table.
	//
	virtual bool nextValue();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	u32 getSelectPct();
	void setSelectPct(u32 pct);
	OmnString getName();
	void setName(const OmnString &name);

	AosTablePtr getTable();
	OmnString getTableName();
	void setTable(AosTablePtr table);
	void setTableName(const OmnString &name);

	bool addParmFieldMap(const OmnString &parmName, const OmnString &fieldName);
	bool getParmFieldMap(const OmnString &parmName, OmnString &fieldName);
	void delParmFieldMap(const OmnString &parmName);

	bool isEmpty();

	//
	// after invoke "nextValue()", you can call this function to get the value of the parameter
	//
	bool getParmValue(const OmnString &parmName, AosValuePtr& value);

};
#endif

#endif

