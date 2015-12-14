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
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicVirtualField.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JQLStatement/JqlStatement.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataFieldMath.h"
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataField.h"

#include <boost/make_shared.hpp>
using namespace AosConf;
using boost::make_shared;
using boost::shared_ptr;


AosJLVirtualField::AosJLVirtualField(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &vfconf)
:AosJimoLogic(1)
{
	// virtualfield's configure format:
	// <column>
	// 	<keys>
	// 		<key>...<key>
	// 		<key>...<key>
	//		...
	// 	</keys>
	// 	<map>
	// 		<keys>
	// 			<key>...</key>
	// 			<key>...</key>
	// 			...
	// 		</keys>
	// 		<values>
	// 			<value>...</value>
	// 		</values>
	// 	</map>
	// </column>
	aos_assert(vfconf);
	
	//felicia, 2014/11/21
	OmnString column_type = vfconf->getAttrStr("type", "");
	aos_assert(column_type != "");
	if (column_type == "expr")
	{
		boost::shared_ptr<DataFieldExpr> fieldObj = boost::make_shared<DataFieldExpr>(vfconf);
		mInputFields.push_back(fieldObj);
		return;
	}


	AosXmlTagPtr keys = vfconf->getFirstChild("keys");		
	aos_assert(keys);
	AosXmlTagPtr key = keys->getNextChild();
	OmnString filedname = "cps";
	while (key)
	{
		OmnString str = key->getNodeText();
		aos_assert(str != "");
		filedname << "_" << str;
		key = keys->getNextChild();
	};


	boost::shared_ptr<DataRecord> datarecord = inputds->getSchema()->getRecord();
	vector<boost::shared_ptr<DataField> > &v = datarecord->getFields();		
	
	// This program will  construct two datafieldObj
	// one is keyField, and the other one is valueField
	OmnString vfname = vfconf->getAttrStr("name");
	AosXmlTagPtr mapNode = vfconf->getFirstChild("map");
	aos_assert(mapNode);
	OmnString datatype = mapNode->getAttrStr("datatype");
	//aos_assert(datatype != "");
	AosXmlTagPtr keysNode = vfconf->getFirstChild("keys");
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	aos_assert(keysNode && valuesNode);

	// 1. key field
	u32 keyLen = 0;
	OmnString keyName, valueName;
	keyName << vfname << "_key";
	valueName = vfname;

	boost::shared_ptr<DataFieldMath> keyDataFieldObj = boost::make_shared<DataFieldMath>();
	keyDataFieldObj->setAttribute("zky_opr", "concat");
	keyDataFieldObj->setAttribute("zky_name", keyName);
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");
	int times = 0;
	while (keyNode)
	{
		times++;
		OmnString fieldname = keyNode->getNodeText();
		aos_assert(fieldname != "");

		boost::shared_ptr<DataFieldExpr> fieldExprObj = boost::make_shared<DataFieldExpr>();
		OmnString tmpExprName = "";
		tmpExprName << keyName << "_f" << times;
		fieldExprObj->setAttribute("zky_name", tmpExprName);
		fieldExprObj->setExpr(fieldname);
		mInputFields.push_back(fieldExprObj);
		keyDataFieldObj->setFieldName(tmpExprName);

		for (size_t i=0; i<v.size(); i++)
		{
			if (fieldname == v[i]->getAttribute("zky_name"))
			{
				int len = atoi(v[i]->getAttribute("zky_length").data());	
				if (len <= 0) len = 50;
				keyLen += len;
				break;
			}
		}	
		if (keyLen <= 0) keyLen = 50;
		keyNode = keysNode->getNextChild("key");
	}

	// 2. value field
	OmnString mapIILName = mapNode->getAttrStr("zky_iilname");
	OmnString isNeedSplit = "true";
	u32 valueLen = 0;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if  (valueNode)
	{
		valueLen = valueNode->getAttrU32("max_len", 0);
		aos_assert(valueLen > 0);
		OmnString agr_type = valueNode->getAttrStr("agrtype");  
		if (agr_type != "")
			isNeedSplit = "false";
		else
			agr_type = "invalid";
	}

	OmnString mapValueNodeText = valueNode->getNodeText();

	if (mapValueNodeText == "zky_docid" || mapValueNodeText == "`zky_docid`")
	{
		isNeedSplit = "false";
	}

	boost::shared_ptr<DataField> valueDataFieldObj = boost::make_shared<DataField>();
	valueDataFieldObj->setAttribute("type", "iilmap");
	valueDataFieldObj->setAttribute("datatype", datatype);

	OmnString dftvalue = vfconf->getAttrStr(AOSTAG_VALUE_DFT_VALUE, "");
	if(dftvalue != "")
	{
		valueDataFieldObj->setAttribute(AOSTAG_VALUE_DFT_VALUE, dftvalue);
	}

	valueDataFieldObj->setAttribute("zky_name", vfname);
	valueDataFieldObj->setAttribute("zky_iilname", mapIILName);
	valueDataFieldObj->setAttribute("zky_value_from_field", keyName);
	valueDataFieldObj->setAttribute("zky_ignore_serialize", "true");
	valueDataFieldObj->setAttribute("zky_needswap", "false");
	valueDataFieldObj->setAttribute("zky_needsplit", isNeedSplit);

	keyDataFieldObj->setAttribute("zky_sep", "0x01");
	valueDataFieldObj->setAttribute("zky_sep", "0x02");

	mInputFields.push_back(keyDataFieldObj);
	mInputFields.push_back(valueDataFieldObj);


	// ice, 2014/09/05
	OmnString type = vfconf->getAttrStr("datatype", "");
	OmnString nfname = AosDataType::getFieldName(vfname, type);                                    
	if (type == "u64")
	{
		boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();
		docidFieldObj->setAttribute("zky_name", nfname);
		docidFieldObj->setAttribute("type", "u64");
		docidFieldObj->setAttribute("zky_value_from_field", vfname);
		mInputFields.push_back(docidFieldObj);
	}

	//Young........................
	// virtual field 
	boost::shared_ptr<DataField> dfObj = boost::make_shared<DataFieldStr>();
	dfObj->setAttribute("type", "str");
	dfObj->setAttribute("zky_length", keyLen);
	dfObj->setAttribute("zky_name", vfname);
	dfObj->setAttribute("zky_otype", "datafield");
	mOutputFields.push_back(dfObj);

}


AosJLVirtualField::AosJLVirtualField(int version)
:AosJimoLogic(version)
{
}


AosJLVirtualField::~AosJLVirtualField()
{
}
