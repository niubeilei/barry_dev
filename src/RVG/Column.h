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
// Tuesday, December 18, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RVG_Column_H
#define Aos_RVG_Column_H

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "RVG/CommonTypes.h"
#include "RVG/Ptrs.h"
#include "XmlLib/tinyxml.h"

class AosColumn;
class AosTable;
typedef std::vector<AosColumnPtr> AosColumnPtrArray;
typedef std::vector<AosTablePtr> AosTablePtrArray;

class AosColumn : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	OmnString mColumnName;
	AosValueType::E		mValueType;
	AosValuePtrArray	mValues;
	AosSequenceOrder::E	mOrderState;

	AosTablePtr			mTable;

public:
	friend class AosTable;

	AosColumn();
	AosColumn(const AosColumn& column);
	~AosColumn();
	bool serialize(TiXmlNode& node);
	bool deserialize(TiXmlNode* node);
	AosColumn& operator=(const AosColumn& column);

	int getRowNum();

	void setTable(AosTablePtr table);

	OmnString getColumnName();
	void setColumnName(OmnString &name);

	AosValueType::E getValueType();
	void setValueType(AosValueType::E type);

	bool addRows(int startIndex=-1, int numRows=1);
	bool removeRows(int startIndex, int& numRows);

	bool getRowData(int rowIndex, AosValuePtr& data);
	bool setRowData(int rowIndex, const AosValue& data);

	//
	// Randomly get data from all possible column values
	//
	bool randomGetData(AosValuePtr& data);

	bool findData(int& rowIndex, const AosValue& data);
	bool findData(std::vector<int>& rowIndexes, const AosValue& data);
	void sort(const AosSequenceOrder::E order);

protected:
	bool addRowData(int& rowIndex, const AosValue& data);


};

#endif

