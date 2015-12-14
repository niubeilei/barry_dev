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
// 07/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HtmlModules_var_expr_h
#define AOS_HtmlModules_var_expr_h

#include "HtmlModules/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


extern int AosRetrieveVar(const AosXmlTagPtr &parm, OmnString &value);

class AosVarEval : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosVarEval();
	~AosVarEval();

	int 
	retrieveVar(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &target);

private:
	int 
	retrieveValPriv(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target);

	int
	retrieveSystem(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target);

	int
	retrieveCanvas(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target);

	int
	retrieveUdata(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target);

	int
	retrievePane(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &object);

	int
	retrieveMember(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &target);

	int
	retrieveSibling(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &object);

	int
	retrieveRecord(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &object);

	int
	composeStr(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target);
};

#endif
