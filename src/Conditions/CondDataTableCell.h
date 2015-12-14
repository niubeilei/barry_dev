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
// This class wraps the cell data of the table
//   
//
// Modification History:
// Wednesday, January 02, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondDataTableCell_H
#define Aos_Conditions_CondDataTableCell_H

#include "Conditions/CondData.h"
#include "RVG/CommonTypes.h"

class AosCondDataTableCell : public AosCondData
{
private:
	AosTablePtr	mTable;
	std::string mTableName;
	std::string mFieldName;
	int			mRowIndex;

public:
	AosCondDataTableCell();
	~AosCondDataTableCell();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	bool getData(AosValuePtr& value, const AosExeDataPtr &ed);

	std::string getTableName();
	void setTableName(const std::string& name);

	std::string getFieldName();
	void setFieldName(const std::string& name);

	int getRowIndex();
	void setRowIndex(int index);

};

#endif
