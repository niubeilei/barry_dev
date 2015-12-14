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
// 2015/04/23 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicMap2.h"

#include "JimoAPI/JimoParserAPI.h"
#include "Rundata/Rundata.h"

AosJLMap2::AosJLMap2(
		const AosRundataPtr &rdata,
		 const AosJimoProgObjPtr &prog,
		const string &inputds_name, 
		//const AosXmlTagPtr &def,
		const string &tablename,
		const bool prime)
:AosJimoLogic(1)
{
	//aos_assert(def);
	mTableName = tablename;
	//mMapName = def->getAttrStr("zky_name");
	mInput = inputds_name;
	//AosXmlTagPtr cond = def->getFirstChild("cond");

	//if (cond){
	//	mCondsStr = cond->getNodeText();
	//}

	//AosXmlTagPtr keys = def->getFirstChild("keys");
	//if (keys)
	//{
	//	AosXmlTagPtr key = keys->getFirstChild(true);
	//	while (key)
	//	{
	//		mKeys.push_back(key->getNodeText());
	//		key = keys->getNextChild();
	//	}
	//}
	//AosXmlTagPtr values = def->getFirstChild("values");
	//if (values)
	//{
	//	AosXmlTagPtr value = values->getFirstChild(true);
	//	if (value)
	//	{
	//		mValue = value->getNodeText();
	//	}
	//}

	bool rslt = createJsonStr(rdata, prog);
	if (!rslt)
	{
		OmnAlarm << "error ." << enderr;
	}
}

bool 
AosJLMap2::createJsonStr(
		const AosRundataPtr &rdata,
		const AosJimoProgObjPtr &prog)
{
	aos_assert_r(prog, false);
	OmnString str;
	//str << "create map " << mMapName << "(";
	OmnString jobName = prog->getJobname();
	str << "dataproc map " << jobName << "_dp_map_" << mTableName << "(";

	if (mTableName != "") {
		str << "table : \"" << mTableName << "\",";
	}

	//str << " key_fields : [";

	//for (u32 i = 0; i < mKeys.size(); i++) {
	//	if (i != 0) 
	//		str << ",";

	//	str << " \"" << mKeys[i] << "\" ";
	//}

	//str << "],";
	//if (mCondsStr != "")
	//	str << "condition : " << "\"" << mCondsStr << "\",";

	//if (mValue != "")
	//	str << "value : " << "\"" << mValue << "\",";

	str << "inputs : \"" << mInput << "\"";
	str << ");";
//OmnScreen << str << endl;	

	AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
	aos_assert_rr(jimoParser, rdata.getPtr(), 0);
	vector<AosJqlStatementPtr> statements;
	jimoParser->parse(rdata.getPtr(), prog.getPtr(), str, statements);

	OmnString ss;
	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat("xml");
		statements[i]->run(rdata.getPtrNoLock(), prog.getPtrNoLock(), ss, false);
	}
	return true;
}

AosJLMap2::AosJLMap2(int version)
:AosJimoLogic(version)
{
}


AosJLMap2::~AosJLMap2()
{
}


AosJimoPtr 
AosJLMap2::cloneJimo() const
{
	OmnNotImplementedYet;
	return NULL;
}
