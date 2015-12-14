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
//  This class simulates the arithmatic condition. The class identify
//  two fields in the CLI command and checks whether the condition
//  is true.
//
// Modification History:
// 05/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/CondArith.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Parms/Util.h"
#include "TorturerConds/CondData.h"
#include "Util/GenTable.h"

#include <arpa/inet.h>


AosTortCondArith::AosTortCondArith(
			const OmnString &name1, 
			const OmnString &name2, 
			const AosTortCondOpr opr)
:
AosTortCond(opr),
mName1(name1),
mName2(name2)
{
}


AosTortCondArith::~AosTortCondArith()
{
}


// 
// It checks the condition. If anything wrong, it returns false. 
// Otherwise, it returns true and 'rslt' tells whether the condition
// holds.
//
bool	
AosTortCondArith::check(const AosCondData &data, 
						bool &rslt, 
						OmnString &errmsg) const
{
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	AosRVGType::E t1, t2;
	OmnString value1, value2;
	OmnString fieldname;
	int ret;
	if (!data.getValue(mName1, fieldname, value1, t1) ||
		!data.getValue(mName2, fieldname, value2, t2))
	{
		OmnAlarm << "Failed to retrieve the value: " 
			<< value1 << ":" << value2 << enderr;
		errmsg = "Failed to retrieve the value";
		return false;
	}

	if (t1 != t2)
	{
		OmnAlarm << "Type mismatch: " << t1 << ":" << t2 << enderr;
		errmsg = "Type mismatch: ";
		errmsg << t1 << ":" << t2;
		return false;
	}

	switch (t1)
	{
	case eAosRVGType_Int:
		 {
		 	int v1 = atoi(value1.data());
			int v2 = atoi(value2.data());
			bool ret = checkValues(v1, v2, mName1, mName2, rslt, errmsg);
			return ret;
		 }

	case eAosRVGType_U32:
		 {
		 	u32 v1 = (u32)atoi(value1.data());
		 	u32 v2 = (u32)atoi(value2.data());
			return checkValues(v1, v2, mName1, mName2, rslt, errmsg);
		 }

	case eAosRVGType_IP:
		 {
		 	OmnIpAddr addr1(value1);
		 	OmnIpAddr addr2(value2);
			u32 v1 = htonl(addr1.getIPv4());
			u32 v2 = htonl(addr2.getIPv4());
			return checkValues(v1, v2, mName1, mName2, rslt, errmsg); 
		 }

	case eAosRVGType_Str:
	case eAosRVGType_StrEnum:
	case eAosRVGType_Netmask:
	case eAosRVGType_Mac:
		 OmnAlarm << "Cannot compare two strings: " << mName1
		 	<< ":" << mName2 << enderr;
		 errmsg = "Cannot compare";
		 return false;

	case eAosRVGType_TimeStr:
		 ret = AosTimeStr_compare(value1, value2, errmsg);
		 if (ret == -100)
		 {
		 	OmnAlarm << "Failed to compare time strings: " 
				<< value1 << ":" << value2 << enderr;
			errmsg << "Program Error: Time string not in the right format: ";
			errmsg << "Value1: " << value1
				<< " and Value2: " << value2;
			return false;
		 }

    	 switch (mOpr)
	     {
		 case eAosTortCondOpr_Less:
			  if (ret < 0) return rslt = true;
			  errmsg << value1 << " < " << value2 << " does not hold. ";
			  rslt = false;
			  return true;
			
		 case eAosTortCondOpr_LessEqual:
			  if (ret <= 0) return rslt = true;
			  errmsg << value1 << " <= " << value2 
			  	<< " does not hold. " << ret;
			  rslt = false;
			  return true;
			
		 case eAosTortCondOpr_Equal:
			  if (ret == 0) return rslt = true;
			  errmsg << value1 << " == " << value2 << " does not hold. ";
			  rslt = false;
			  return true;
			
	     case eAosTortCondOpr_LargerEqual:
			  if (ret >= 0) return rslt = true;
			  errmsg << value1 << " >= " << value2 << " does not hold. ";
			  rslt = false;
			  return true;
			
	     case eAosTortCondOpr_Larger:
			  if (ret > 0) return rslt = true;
			  errmsg << value1 << " > " << value2 << " does not hold. ";
			  rslt = false;
			  return true;
			
	     case eAosTortCondOpr_NotEqual:
			  if (ret) return rslt = true;
			  errmsg << value1 << " != " << value2 << " does not hold. ";
			  rslt = false;
			  return true;
			
		 default:
		      OmnAlarm << "Unrecognized operator: " << mOpr << enderr;
			  errmsg << "Unrecognized operator: " << mOpr;
			  rslt = false;
			  return true;
		 }

	default:
		 OmnAlarm << "Unrecognized parm type: " << t1 << enderr;
		 errmsg = "Unrecognized type";
		 return false;
	}

	*/
	return false;
}

