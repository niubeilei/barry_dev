////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/08/20 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////

#include "JimoProgCheckers/JPSCCheckIndexesAndDocs.h"
#include "SEInterfaces/JimoLogicObjNew.h"
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJPSCCheckIndexesAndDocs_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJPSCCheckIndexesAndDocs(rdata.getPtr(), version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}



AosJPSCCheckIndexesAndDocs::AosJPSCCheckIndexesAndDocs(const AosRundata* rdata,const int version)
:
AosJPSyntaxCheckerObj(version)
{
	mErrMsg = "[ERR] : "; 
}

		
AosJPSCCheckIndexesAndDocs::AosJPSCCheckIndexesAndDocs(const AosJPSCCheckIndexesAndDocs &JPSC)
:
AosJPSyntaxCheckerObj(1)
{
	mErrMsg = JPSC.mErrMsg; 
}


AosJPSCCheckIndexesAndDocs::~AosJPSCCheckIndexesAndDocs()
{

}

AosJimoPtr 
AosJPSCCheckIndexesAndDocs::cloneJimo() const
{
	return OmnNew AosJPSCCheckIndexesAndDocs(*this);
}


bool 
AosJPSCCheckIndexesAndDocs::checkSyntax(AosRundata *rdata, AosJimoProgObj *prog)
{	
	if(!prog) return false;
	map_t jimoLogicMaps = prog->getJimoLogics();
	itr_t itr;
	
	map_t jimoLogicIndex;
	vector<AosJimoLogicObjNewPtr> jimoLogicDoc;

	for(itr = jimoLogicMaps.begin();itr != jimoLogicMaps.end();itr++)
	{
		AosJimoLogicObjNewPtr logic = itr->second;
		if(logic->getJimoLogicType() == AosJimoLogicType::eDataProcIndex)
		{
			jimoLogicIndex[itr->first] = itr->second;
		}
		if(logic->getJimoLogicType() == AosJimoLogicType::eDataProcDoc)
		{
			jimoLogicDoc.push_back(logic);
		}
	}
	bool flag;
	for(itr = jimoLogicIndex.begin();itr != jimoLogicIndex.end();itr++)
	{
		OmnString input = itr->second->getInput();
		OmnString table = itr->second->getTableName();
		flag = false;
		for(size_t i = 0;i < jimoLogicDoc.size(); i++)
		{
			if(jimoLogicDoc[i]->isExist("input",input) && jimoLogicDoc[i]->isExist("table",table))
			{
				flag = true;
				break;
			}
		}
		if(!flag)
		{
			mErrMsg << " dataproc index " << itr->first << " have no matched dataproc doc!";
			rdata->setJqlMsg(mErrMsg);
			aos_assert_r(flag,false);
			return false;
		}
	}
	return true; 	
}


