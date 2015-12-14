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
// 2014/12/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoPackage.h"



OmnString 
JimoPackageID::getClassname(const JimoPackageID::E package_id)
{
	switch (package_id)
	{
	case eHelloWorld : return "AosHelloWorldJimoCalls";
	case eDocPackage : return "AosDocPackageJimoCalls";
	case eIILPackage : return "AosIILPackageJimoCalls";
	case eRaft		 : return "AosJimoRaftAPI";
	default: break;
	}
	return "";
}

