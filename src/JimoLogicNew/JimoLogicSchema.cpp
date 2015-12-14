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
// 2015/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicSchema.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"



/*
bool
AosJimoLogicSchema::parseField(
		AosRundata *rdata, 
		AosJimoLogicSchema::Field &field,
		const AosExprObjPtr &field_def)
{
	// The field definition should be in the following format:
	// 	(name: <name>, type: <type>, max_len: <ddd>)
	if (field_def->getType() != AosExprType::eBrackets)
	{
		AosLogError(rdata, true, "invalid_field_def")
			<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
		return false;
	}

	vector<AosExprObjPtr> exprs = field_def->getExprList();
	if (exprs.size() <= 0)
	{
		AosLogError(rdata, true, "invalid_field_def")
			<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
		return false;
	}

	for (u32 i=0; i<exprs.size(); i++)
	{
		if (exprs[i]->getType() != AosExprType::eNameValue)
		{
			AosLogError(rdata, true, "invalid_field_def")
				<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
			return false;
		}

		OmnString name = exprs[i]->getName().toLower();
		AosExprObjPtr value = exprs[i]->getValueAsExpr();
		if (name == "" || !value)
		{
			AosLogError(rdata, true, "invalid_name_value_pair")
				<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
			return false;
		}

		if (name == "field_name")
		{
			field.field_name = value->getValue(rdata);
			if (field.field_name == "")
			{
				AosLogError(rdata, true, "field_name_empty")
					<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
				return false;
			}
		}
		else if (name == "type")
		{
			field.field_type = value->getValue(rdata);
			if (field.field_type == "") field.field_type = "str";
		}
		else if (name == "max_len")
		{
			field.max_len = atoi(value->getValue(rdata).data());
		}
	}

	OmnString errmsg;
	if (!field.isValid(errmsg))
	{
		AosLogError(rdata, true, errmsg) << enderr;
		return false;
	}

	return true;
}


bool
AosJimoLogicSchema::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr thisptr(this, false);
	prog->addSchema(rdata, mSchemaName, thisptr);
	return true;
}

*/
