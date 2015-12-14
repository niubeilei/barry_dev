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
// 2015/05/04 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtDistributionMap.h"
#include "JQLStatement/JqlStmtTable.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"

#include "SEInterfaces/AggregationType.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

AosJqlStmtDistributionMap::AosJqlStmtDistributionMap(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mMapName = "";
	mTableName = "";
	mMaxLen = 0;
	mMaxNums = 0;
	mSpliteNum = 0;
	mDataType = 0;
	mKeys = NULL;
	mWhereCond = NULL;

}


AosJqlStmtDistributionMap::AosJqlStmtDistributionMap()
{
	mMapName = "";
	mTableName = "";
	mMaxLen = 0;
	mMaxNums = 0;
	mSpliteNum = 0;
	mDataType = 0;
	mKeys = NULL;
	mWhereCond = NULL;

}

AosJqlStmtDistributionMap::~AosJqlStmtDistributionMap()
{
	delete mWhereCond;
	mWhereCond = 0;
	
	OmnDelete(mKeys);
}


bool
AosJqlStmtDistributionMap::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createMap(rdata);
	if (mOp == JQLTypes::eShow) return showMaps(rdata);
	if (mOp == JQLTypes::eDrop) return dropMap(rdata);
	if (mOp == JQLTypes::eDescribe) return describeMap(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Distribution Map", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDistributionMap::createMap(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	OmnString objid = getObjid(mMapName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		msg << "Distribution Map " << mMapName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc->isRootTag())
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr("zky_objid", objid);

	if ( !createDoc(rdata, JQLTypes::eMap, mMapName, doc) )
	{
		msg << "Failed to create Distribution Map doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Distribution Map " << mMapName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDistributionMap::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	OmnString  mapIILName = "";
	mapIILName << "_zt44_map_" << mTableName << "_" << mMapName;
	doc << "<map zky_type=\"iilmap\" zky_name=\"" << mMapName <<"\"" << " zky_iilname=\"" << mapIILName << "\" "
		<< " nums=\"" << mMaxNums << "\" "
		<< " splite=\"" << mSpliteNum << "\">";

	doc << 	"<keys>";
	for (u32 i = 0; i < mKeys->size(); i++)
	{
		if ((*mKeys)[i])
			doc << "<key>" << (*mKeys)[i]->dump() << "</key>";
	}
	doc << 	"</keys>";

	if (mWhereCond)
		doc << 	"<cond><![CDATA[" << mWhereCond->dump() << "]]></cond>";

	doc << "</map>";

	AosXmlTagPtr map = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	if (!map)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	if (mDataType == "str")
	{
		map->setAttr(AOSTAG_NEEDSPLIT, "true");
		map->setAttr(AOSTAG_SEPARATOR, "0x02");
		map->setAttr(AOSTAG_NEEDSWAP, "false");
		map->setAttr(AOSTAG_USE_KEY_AS_VALUE, "false");
	}
	map->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	map->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	map->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_MAP_CONTAINER);   

	AosXmlTagPtr table = AosJqlStmtTable::getTable(mTableName, rdata);
	if (table)
	{
		AosXmlTagPtr maps = table->getFirstChild("maps");
		AosXmlTagPtr map_tmp = map->clone(AosMemoryCheckerArgsBegin);
		maps->addNode(map_tmp);

		bool rslt = modifyDoc(rdata, table);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
	}
	return map;
}


bool
AosJqlStmtDistributionMap::showMaps(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	if ( !getDocs(rdata, JQLTypes::eMap, dfields) )
	{
		msg << "No Distribution Map Found! " ;
		rdata->setJqlMsg(msg);
		return true;
	}
	int total = dfields.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<dfields.size(); i++)
	{
		OmnString vv = dfields[i]->getAttrStr("zky_name", "");
		if (vv != "")
		{
			content << "<record name=\"" << vv << "\"/>";
		}
	}
	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml") 
	{
		rdata->setResults(content);
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("name");
	map<OmnString, OmnString> alias_name;
	alias_name["name"] = "MapName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtDistributionMap::describeMap(const AosRundataPtr &rdata)
{
	if (mMapName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(mMapName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display Distribution map " << mMapName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("zky_type");
	fields.push_back("datatype");

	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["datatype"] = "DataType";

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtDistributionMap::dropMap(const AosRundataPtr &rdata)
{
	OmnString msg;                                          
	string objid = getObjid(mMapName);                    
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete Distribution Map doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop Distribution Map " << mMapName << " scessful";      
	rdata->setJqlMsg(msg);                                  
	return true;                                            
}



OmnString
AosJqlStmtDistributionMap::getObjid(const OmnString name)
{
	OmnString objid = "";
	objid << AOSZTG_MAP_IIL << "_" << name;
	return objid;
}

AosJqlStatement *
AosJqlStmtDistributionMap::clone()
{
	return OmnNew AosJqlStmtDistributionMap(*this);
}


void 
AosJqlStmtDistributionMap::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDistributionMap::setMapName(OmnString name)
{
	mMapName = name;
}

void                                            
AosJqlStmtDistributionMap::setTableName(OmnString name)
{
	mTableName = name;
}

void 
AosJqlStmtDistributionMap::setKeys(AosExprList *key_list)
{
	mKeys = key_list;
}


void 
AosJqlStmtDistributionMap::setWhereCond(AosExprObj *where_cond)
{
	mWhereCond = where_cond;
}


void 
AosJqlStmtDistributionMap::setDataType(OmnString data_type)
{
	mDataType = data_type;
}


void 
AosJqlStmtDistributionMap::setMaxLen(int len)
{
	if (len < 0)
	{
		OmnAlarm << "AosJqlStmtDistributionMap :: set max len error." << enderr;
		return;
	}
	mMaxLen = len;
}


void 
AosJqlStmtDistributionMap::setMaxNums(int nums)
{
	mMaxNums = nums;
}


void 
AosJqlStmtDistributionMap::setSpliteNum(int splite)
{
	mSpliteNum = splite;
}

