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
// Generate value from the field of the table, and provide find condition 
// list to constraint the selective range.
// 	1. find condition list is empty: no condition 
//	2. find condition list has one element: one condition 
//	3. find condition list has more than one element: multiple conditions 
//
// The find condition list can be dynamically specified by users. it provides
// different ways to select value from table.
//   
//
// Modification History:
// Monday, December 17, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RVG_RVGFromTable_H
#define Aos_RVG_RVGFromTable_H

#if 0
#include "RVG/RVG.h"
#include "RVG/Table.h"
#include "Conditions/Ptrs.h"

class AosRVGFromTable : public AosRVG
{
	OmnDefineRCObject;
public:
	typedef struct FindCond
	{
		OmnString mFieldName;
		AosConditionPtr mCond;	
	}FindCond;

	//
	// a functor to find Field Name
	//
	typedef struct FunctorFindField: public std::binary_function<FindCond, OmnString, bool>
	{
		bool operator()(const FindCond& findCond, const OmnString &name) const
		{
			return (findCond.mFieldName == name);
		}
		
	} FunctorFindField;

	typedef std::list<FindCond> AosFindCondList;

private:
	AosTablePtr			mTable;
	OmnString 			mTableName;
	OmnString 			mFieldName;

	//
	// This key field list is used for searching value
	// if the list is empty, there is no any key, All the
	// specified field values will be probably selected.
	// 
	// if the list has one element, it means a single key,
	// select values according to the single key value
	//
	// if the list has more than one element, it means using
	// multiple keys to select values.
	//
	// we can config this key value list from GUI
	//
	AosFindCondList		mFindConds;

public:
	AosRVGFromTable();
	~AosRVGFromTable();

	virtual bool nextValue(AosValue& value);
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	AosTablePtr getTable();
	OmnString getFieldName();
	OmnString getTableName();
	void setTable(AosTablePtr table);
	void setFieldName(const OmnString &field);
	void setTableName(const OmnString &name);

	void getConds(AosRVGFromTable::AosFindCondList& conds);
	void addCond(const OmnString &fieldName, const AosConditionPtr& cond);
	void removeCond(const OmnString &fieldName);
	
protected:
	void getAvailRowIndexes(std::vector<int>& rowIndexes);

};
#endif

#endif

