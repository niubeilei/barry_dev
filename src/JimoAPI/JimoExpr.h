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
// 2015/05/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoCubeMap_h
#define Aos_JimoAPI_JimoCubeMap_h

//#include "JimoObj/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include <vector>

class AosRundata;
class AosNameValueDoc;

namespace Jimo
{
	AosExprObjPtr jimoCreateNameValueExpr(
						const OmnString &name, 
						const OmnString &value);

	AosExprObjPtr jimoCreateNameValueExpr(
						const OmnString &name, 
						const AosExprObjPtr &expr);

	AosExprObjPtr jimoCreateNameValueExpr(
						const OmnString &name, 
						const std::vector<AosExprObjPtr> &exprs);

	AosExprObjPtr jimoCreateNameValueExpr(
						const std::vector<AosExprObjPtr> &exprs);

	AosExprObjPtr jimoCreateStrExpr(const OmnString &value);
};

#endif
