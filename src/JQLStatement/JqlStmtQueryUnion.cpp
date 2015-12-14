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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtQueryUnion.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JQLExpr/Expr.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"
#include "Dataset/DatasetMulti.h"

using namespace AosConf;

AosJqlStmtQueryUnion::AosJqlStmtQueryUnion()
{
	mIsUnionAll = false;
	mQueryList.clear();
}


AosJqlStmtQueryUnion::~AosJqlStmtQueryUnion()
{
	// for(u32 i=0;i<mQueryList.size();i++)
	// {
	//	delete mQueryList[i];
	//	mQueryList[i] = NULL;
	//}
	// mQueryList.clear();
}

bool
AosJqlStmtQueryUnion::runQuery(
		const AosRundataPtr &rdata,
		AosDatasetObjPtr &dataset)
{
	bool rslt;
	AosDatasetMulti *dsMulti = NULL;
    AosDatasetObjPtr ds;

    //get the query result dataset
    aos_assert_r(mQueryList.size() > 0, false);

	//set mSelectPtr as the first query's value. This
	//assumes that all the query's fields info are 
	//same
	setSelect(mQueryList[0]->getSelect());

	//create a multi dataset
	dsMulti = new AosDatasetMulti();
	aos_assert_r(dsMulti, false);
    for (u32 i = 0; i < mQueryList.size(); i++)
    {
		rslt = mQueryList[i]->runQuery(rdata, ds);
		aos_assert_r(rslt, false);
        if (mIsUnionAll)
		{
            rslt = dsMulti->dataUnionAll(rdata, ds);
			if (!rslt)
			{
				OmnScreen << "Err : Union query with different result format." << endl;
				return false;
			}
		}
        else
		{
            rslt = dsMulti->dataUnion(rdata, ds);
			if (!rslt)
			{
				OmnScreen << "Err : Union all query with different result format." << endl;
				return false;
			}
		}
    }

	dsMulti->setPageSize(0);
	dataset = dsMulti;
    return true;
}

bool 
AosJqlStmtQueryUnion::addQuery(AosJqlStmtQuery *query)
{
	aos_assert_r(query, false);
	mQueryList.push_back(query);

	return true;
}
