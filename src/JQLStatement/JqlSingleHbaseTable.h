//////////////////////////////////////////////////////////////////////////
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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlSingleHbaseTable_H
#define AOS_JQLStatement_JqlSingleHbaseTable_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSingleTable.h"
#include "JQLStatement/JqlSelectTable.h"
#include "JQLStatement/JqlSelectField.h"
#include "JQLExpr/ExprGenFunc.h"
#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "AosConf/DataFieldStr.h"          
#include "AosConf/HbaseDataSet.h"          
#include "AosConf/DataRecord.h"            
#include "AosConf/DataSet.h"            
#include <vector>
#include <map>

using boost::shared_ptr;
using namespace AosConf;

class AosJqlSingleHbaseTable : public AosJqlSingleTable
{
	public:
	//	AosExprObj* 				mTableName;  // by SingleQuery 
		OmnString 					mRawKey;
		AosRundataPtr				mRundata;

	public:
		virtual OmnString generateInputDatasetConf(const AosRundataPtr &rdata);
		static bool checkIsHbaseTable(const AosRundataPtr &rdata, AosExprObjPtr table, AosJqlSelectPtr select);
		boost::shared_ptr<HbaseDataSet> createHbaseDataSet();
		boost::shared_ptr<DataSet> createDataSet();
		boost::shared_ptr<DataScanner> createScanner();
		boost::shared_ptr<DataConnector> createConnector();
		boost::shared_ptr<DataSchema> createSchema();
		boost::shared_ptr<DataRecord> createRecordByTag(const AosRundataPtr &rdata, OmnString tag_name, OmnString per_tagname);
		bool getHbaseConnMsg(OmnString &db_name, OmnString &db_addr, OmnString &db_port, OmnString &t_name);

};

#endif
