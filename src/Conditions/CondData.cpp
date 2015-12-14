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
// 06/08/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondData.h"

#include "alarm/Alarm.h"
#include "Conditions/CondDataConstant.h"
#include "Conditions/CondDataParm.h"
#include "Conditions/CondDataTableCell.h"
#include "Conditions/CondDataTable.h"
#include "Conditions/CondDataVar.h"
#include "Parms/RVG.h"
#include "Util/VarList.h"

AosCondDataPtr 
AosCondData::factory(TiXmlNode* node)
{
	aos_assert_r(node, 0);
	AosCondDataPtr data;
	AosCondDataType::E type = AosCondDataType::str2Enum(node->ValueStr());
	switch (type)
	{
	case AosCondDataType::eConstant:
		 data = OmnNew AosCondDataConst();
		 break;

	case AosCondDataType::eVariable:
		 data = OmnNew AosCondDataVar();
		 break;

	case AosCondDataType::eParmValue:
		 data = OmnNew AosCondDataParm();
		 break;

	case AosCondDataType::eTableCell:
		 data = OmnNew AosCondDataTableCell();
		 break;

	case AosCondDataType::eTable:
		 data = OmnNew AosCondDataTable();
		 break;

	default:
		 OmnAlarm << "Unrecognized condition data type: " 
			 << node->ValueStr() << enderr;
		 return 0;
	}

	aos_assert_r(data, 0);
	if (!data->deserialize(node))
	{
		OmnAlarm << "Failed to create node" << enderr;
		return 0;
	}

	return data;
}

