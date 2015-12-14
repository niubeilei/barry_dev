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
// Modification History:
// 2015/05/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoExpr.h"

#include "SEInterfaces/ExprObj.h"

namespace Jimo
{

AosExprObjPtr jimoCreateNameValueExpr(
		const OmnString &name, 
		const AosExprObjPtr &expr)
{
	AosExprObj *expr_obj = AosExprObj::getExprStatic();
	aos_assert_r(expr_obj, 0);
	return expr->createNameValueExpr(name, expr);
}


AosExprObjPtr jimoCreateNameValueExpr(
		const OmnString &name, 
		const OmnString &value)
{
	AosExprObj *expr_obj = AosExprObj::getExprStatic();
	aos_assert_r(expr_obj, 0);

	AosExprObjPtr str_expr = expr_obj->createStrExpr(value);
	aos_assert_r(str_expr, 0);

	return expr_obj->createNameValueExpr(name, str_expr);
}


AosExprObjPtr jimoCreateNameValueExpr(
		const OmnString &name, 
		const std::vector<AosExprObjPtr> &exprs)
{
	AosExprObj *expr_obj = AosExprObj::getExprStatic();
	aos_assert_r(expr_obj, 0);

	AosExprObjPtr array_expr = expr_obj->createArrayExpr(exprs);
	aos_assert_r(array_expr, 0);
	return expr_obj->createNameValueExpr(name, array_expr);
}


AosExprObjPtr jimoCreateNameValueExpr(
		const std::vector<AosExprObjPtr> &exprs)
{
	AosExprObj *expr_obj = AosExprObj::getExprStatic();
	aos_assert_r(expr_obj, 0);

	return expr_obj->createArrayExpr(exprs);
}



AosExprObjPtr jimoCreateStrExpr(const OmnString &value)
{
	AosExprObj *expr_obj = AosExprObj::getExprStatic();
	aos_assert_r(expr_obj, 0);

	return expr_obj->createStrExpr(value);
}


};

