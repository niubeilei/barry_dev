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

#ifndef Aos_RVG_Table_H
#define Aos_RVG_Table_H

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "RVG/CommonTypes.h"
#include "RVG/Column.h"
#include "RVG/Ptrs.h"
#include "XmlLib/tinyxml.h"
#include <map>

class AosColumn;
class AosTable;

class AosTable : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosColumnPtrArray	mColumns;
	int					mRowNum;
	OmnString			mName;
	int					mMaxRows;
	u32					mSelectPct;		// Allowed: [0, 100]
	std::map<OmnString, OmnString>	mParmFieldMap;

public:
	friend class AosColumn;

	AosTable();
	~AosTable();

	bool serialize(TiXmlNode& node);
	bool deserialize(TiXmlNode* node);

	bool getColumn(int index, AosColumnPtr& column);
	bool getColumn(OmnString &columnName, AosColumnPtr& column);
	bool addColumn(int& index, const AosColumn& column);
	bool removeColumn(int columnIndex);

	bool getColumnIndex(int& index, const AosColumnPtr& column);
	bool getColumnIndex(int& index, const OmnString & columnName);

	bool addRows(int startIndex=-1, int numRows=1);
	bool removeRows(int startIndex=0, int numRows=1);
	bool removeRecord(const AosRecord &record);

	bool isEmpty();
	int  getColumnNum();
	int  getRowNum();

	int getMaxRows();
	void setMaxRows(int maxRows);

	OmnString getName();
	void setName(const OmnString &name);

	//
	// get the record in the rowIndex row
	//
	bool getRecord(int rowIndex, AosRecord& record);
	bool getRecordByKey(int &rowIndex, AosRecord &record);
	bool replace(const AosRecord &record, const bool insertFlag);

	//
	// set the record in the rowIndex row
	//
	bool setRecord(int rowIndex, const AosValuePtrArray& record);

	//
	// add a new row at the end of the table, and set all column 
	// field value
	//
	bool addRecord(int& rowIndex, const AosValuePtrArray& record);
	bool addRecord(const AosValuePtrArray& record);

	//
	// add a new row at the end of the table, and return all 
	// column field ptr for users to set the value
	//
	bool getNewRecord(int& rowIndex, AosRecord& record);

	//
	// Randomly get a record
	//
	bool randomGetRecord(int& rowIndex, AosRecord& record);
	bool randomGetRecord(AosRecord &record);
	u32  getSelectPct() const {return mSelectPct;}
	void setSelectPct(const u32 s) {mSelectPct = s;}

	//
	// get data form [row, column] position
	//
	bool getData(int row, int column, AosValuePtr& data);

	bool sortbyColumn(int columnIndex, AosSequenceOrder::E order);
	bool sortbyColumn(OmnString &columnName, AosSequenceOrder::E order);

	bool isFull() const;
	int  getMax() const {return mMaxRows;}
	bool addParmFieldMap(const OmnString &parmName, 
				const OmnString &fieldName);
	bool getParmFieldMap(const OmnString &parmName, 
				OmnString &fieldName);
	void delParmFieldMap(const OmnString &parmName);
	// bool getParmValue(
	// 			const OmnString &parmName, 
	// 			AosValuePtr& value);

	OmnString toString();

	static bool createInstance(const OmnString &configFile, AosTablePtr& table);

protected:
	bool setData(int row, int column, const AosValue& data);
	bool checkRowNum();

};

#endif


