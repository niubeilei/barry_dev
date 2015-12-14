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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlQueryInputDataset_H
#define AOS_JQLStatement_JqlQueryInputDataset_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"

#include "AosConf/DataConnector.h"    
#include "AosConf/DataSchemaMultiRecord.h"     
#include "AosConf/DataRecordCtnr.h"           
#include "AosConf/DataScanner.h"           
#include "AosConf/DataSet.h"        
#include "AosConf/DataFieldExpr.h"        
#include "AosConf/DataFieldStr.h"          
#include "AosConf/DataRecord.h"            

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <map>

using boost::shared_ptr;
using namespace AosConf;

class AosJqlQueryInputDataset 
{
public:
	enum E {
		eInvalid,
		eXML,
		eFixBin,
		eCSV,
		eVirtual,
		eMax
	};

public:
	E				mType;
	OmnString 		mTableName;
	OmnString  		mDocType;
	OmnString		mDsche;
	OmnString		mRecordTyoe;
	AosXmlTagPtr	mTableDoc;
	AosRundataPtr	mRundata;
	vector<AosJqlSelectFieldPtr>	mInputFields;

public:
	AosJqlQueryInputDataset();
	AosJqlQueryInputDataset(
		const AosXmlTagPtr &table_doc,
		const OmnString table_name,
		const vector<AosJqlSelectFieldPtr> &inputFields);

	~AosJqlQueryInputDataset();

	bool init(const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getConfig(const AosRundataPtr &rdata);	

private:
	OmnString generateInputDatasetConf(const AosRundataPtr &rdata);

	boost::shared_ptr<DataRecord> generateInputRecord(const AosRundataPtr &rdata);
	boost::shared_ptr<DataScanner> generateScanner(const AosRundataPtr &rdata);
	boost::shared_ptr<DataConnector> generateConnector(const AosRundataPtr &rdata);
	boost::shared_ptr<DataSchema> generateSchema(const AosRundataPtr &rdata);
	boost::shared_ptr<DataSchema> generateMultiRecordSchema(const AosRundataPtr &rdata);
	boost::shared_ptr<DataRecord> generateInputRecordByFixBin(const AosRundataPtr &rdata);
	boost::shared_ptr<DataRecord> generateInputRecordByCSV(const AosRundataPtr &rdata);
	boost::shared_ptr<DataRecord> generateInputRecordByXML(const AosRundataPtr &rdata);

	E toEnum(OmnString type){
		if (type == "xml") return eXML;
		if (type == "fixbin") return eFixBin;
		if (type == "csv") return eCSV;
		return eInvalid;
	}

	bool isValid(){
		return (mType > eInvalid && mType < eMax);
	}

	bool getType();

};

#endif
