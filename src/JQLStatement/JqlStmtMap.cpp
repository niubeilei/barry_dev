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
#include "JQLStatement/JqlStmtMap.h"
#include "JQLStatement/JqlStmtTable.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"

#include "SEInterfaces/AggregationType.h"
#include "SEUtil/JqlUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"
#include "JQLParser/JQLParser.h"                     

extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosJqlStmtMap::AosJqlStmtMap(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mMapName = "";
	mTableName = "";
	mMaxLen = 0;
	mDataType = 0;
	mKeys = NULL;
	mValue = NULL;
	mWhereCond = NULL;

}


AosJqlStmtMap::AosJqlStmtMap()
{
	mMapName = "";
	mTableName = "";
	mMaxLen = 0;
	mDataType = 0;
	mKeys = NULL;
	mValue = NULL;
	mWhereCond = NULL;

}

AosJqlStmtMap::~AosJqlStmtMap()
{
	delete mWhereCond;
	mWhereCond = 0;
	
	OmnDelete(mKeys);

	delete mValue;
	mValue = 0;
}


bool
AosJqlStmtMap::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
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
	if (mOp == JQLTypes::eList) return listMap(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Map", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtMap::createMap(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eMap, mMapName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		msg << "Map " << mMapName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	if (!AosDataType::isValid(AosDataType::toEnum(mDataType)))
	{
		msg << "map type(" << mDataType << ") is invalid.";
		rdata->setJqlMsg(msg);
		return false;
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
		msg << "Failed to create Map doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Map " << mMapName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtMap::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	OmnString dbname = getCurDatabase(rdata);
	OmnString mapIILName = "";
	mapIILName << "_zt44_map_" << dbname << "_" << mTableName << "_" << mMapName;
	doc << "<map zky_type=\"iilmap\" zky_name=\"" << mMapName <<"\"" << " zky_iilname=\"" << mapIILName << "\""
		<< " datatype=\"" << mDataType << "\">";
	doc << 	"<keys>";
	for (u32 i = 0; i < mKeys->size(); i++)
	{
		if ((*mKeys)[i])
			doc << "<key>" << (*mKeys)[i]->dumpByNoEscape() << "</key>";
	}
	doc << 	"</keys>";
	doc << 	"<values>";
	OmnString vv = "";
	if (mValue)
	{
		AosExprObj * expr = mValue;
		AosExprGenFunc* expr_func;
		OmnString func_name;
		OmnString agrtype = "";
		if (expr->getType() == AosExprType::eGenFunc)
		{
			expr_func = dynamic_cast<AosExprGenFunc*>(expr);
			if (expr_func)
			{
				func_name = expr_func->getFuctName();
				AosAggregationType::E type = AosAggregationType::toEnum(func_name.toLower());
				if(AosAggregationType::isValid(type))
				{
					agrtype = func_name;
					vector<AosExprObjPtr> parms;
					expr_func->getParms(rdata.getPtr(), parms);
					vv = parms[0]->getValue(rdata.getPtrNoLock());
					if (func_name == "dist_count")
					{
						vv = "1";
					}
				}
				else 
				{
					vv = expr->dumpByNoEscape();
				}
			}
		}
		else if (vv == "")
			vv = expr->dumpByNoEscape();

		doc << "<value agrtype=\"" << agrtype << "\" "
			//<< " datatype=\"" << mDataType << "\" "
			<< " max_len=\"" << mMaxLen << "\" ";
		if (expr->getType() == AosExprType::eFieldName)
			doc << " type=\"field\" ";
		else
			doc << " type=\"expr\" ";
		doc << " ><![CDATA[" <<  vv << "]]></value>";
	}
	doc << 	"</values>";
	if (mWhereCond)
		doc << 	"<cond><![CDATA[" << mWhereCond->dumpByNoEscape() << "]]></cond>";
	doc << "</map>";

	AosXmlTagPtr map = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	if (!map)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosDataType::E v_type = AosDataType::toEnum(mDataType);
	if (AosJqlUtil::isStr2StrMap(v_type))
	{
		map->setAttr(AOSTAG_NEEDSPLIT, "true");
		map->setAttr(AOSTAG_SEPARATOR, "0x02");
		map->setAttr(AOSTAG_NEEDSWAP, "false");
		map->setAttr(AOSTAG_USE_KEY_AS_VALUE, "false");
	}
	else if (AosJqlUtil::isStr2NumMap(v_type))
	{
		map->setAttr(AOSTAG_NEEDSPLIT, "false");
		map->setAttr(AOSTAG_NEEDSWAP, "false");
		map->setAttr(AOSTAG_USE_KEY_AS_VALUE, "false");
	}
	else
	{
		OmnAlarm << mDataType << enderr;
	}

	map->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	map->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	map->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_MAP_CONTAINER);   

	AosXmlTagPtr table = AosJqlStmtTable::getTable(mTableName, rdata);
	if(mDataType == "datetime" && table)
	{
		OmnString schemaParser = table->getAttrStr("zky_use_schema");
		AosXmlTagPtr schemaDoc = AosJqlStatement::getDocByObjid(rdata, schemaParser);
		AosXmlTagPtr dataFieldsDoc = schemaDoc->getFirstChild("datafields", true);
		AosXmlTagPtr dataFieldDoc = dataFieldsDoc->getFirstChild();
		OmnString dataFormat = "";
		while(dataFieldDoc)
		{
			OmnString fieldName = dataFieldDoc->getAttrStr("zky_name");
			if(fieldName == vv)
			{
				dataFormat = dataFieldDoc->getAttrStr("format");
				break;
			}
			dataFieldDoc = dataFieldsDoc->getNextChild();
		}
		//aos_assert_r(dataFormat != "", false);
		map->setAttr("zky_dataField_format", dataFormat);
	}
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
AosJqlStmtMap::showMaps(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	if ( !getDocs(rdata, JQLTypes::eMap, dfields) )
	{
		msg << "No Map Found! " ;
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
AosJqlStmtMap::describeMap(const AosRundataPtr &rdata)
{
	if (mMapName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eMap, mMapName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display map " << mMapName << " : not found.";
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
AosJqlStmtMap::dropMap(const AosRundataPtr &rdata)
{
	//JIMODB-163 drop map by Levi
	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eMap, mMapName);
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete Map doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop Map " << mMapName << " scessfully";      
	rdata->setJqlMsg(msg);                                  
	return true;                                            
}


AosJqlStatement *
AosJqlStmtMap::clone()
{
	return OmnNew AosJqlStmtMap(*this);
}


void 
AosJqlStmtMap::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtMap::setMapName(OmnString name)
{
	mMapName = name;
}

void                                            
AosJqlStmtMap::setTableName(OmnString name)
{
	mTableName = name;
}

void 
AosJqlStmtMap::setKeys(AosExprList *key_list)
{
	mKeys = key_list;
}

void
AosJqlStmtMap::setValue(AosExprObj *val)
{
	mValue = val;
}

void 
AosJqlStmtMap::setWhereCond(AosExprObj *where_cond)
{
	mWhereCond = where_cond;
}


void 
AosJqlStmtMap::setDataType(AosJQLDataFieldTypeInfo *type)
{
	if (type) mDataType = type->getType();
}

void 
AosJqlStmtMap::setMaxLen(int len)
{
	if (len < 0)
	{
		OmnAlarm << "AosJqlStmtMap :: set max len error." << enderr;
		return;
	}
	mMaxLen = len;
}

bool 
AosJqlStmtMap::listMap(const AosRundataPtr &rdata)
{
	if (mMapName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eMap, mMapName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Map '" << mMapName << "' not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	OmnString mapiilname = doc->getAttrStr("zky_iilname");
	
	OmnString cmd = "";
	cmd << "list index " << mapiilname << ";";

	int val = AosParseJQL((char *)cmd.data(), false);                 
	if (val != 0)                                                          
	{                                                                      
		rdata->setJqlMsg("stdin: syntax error");                           
		return false;                                                      
	}                                                                      
	gAosJQLParser.dump();                                                  
																		   
	vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements(); 
	OmnString errmsg = gAosJQLParser.getErrmsg();                          
	if (errmsg != "")                                                      
	{                                                                      
		AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;  
		return false;                                                      
	}                                                                      
																		   
	for (u32 i=0; i<statements.size(); i++)                                
	{                                                                      
		statements[i]->setContentFormat(mContentFormat);                   
		statements[i]->run(rdata, 0);                                      
	}                                                                      

	return true;
}

