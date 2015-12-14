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
#include "JQLStatement/JqlStmtIndex.h"

#include "JQLStatement/JqlStmtDataset.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JQLCommon.h"              
#include "JQLExpr/Expr.h"                        

#include "SEUtil/IILName.h"
#include "SEBase/SeUtil.h"                       
#include "SEUtil/Ptrs.h"                         
#include "Rundata/Rundata.h"                     
#include "XmlUtil/XmlTag.h"                      
#include "Debug/Debug.h"                         
#include "Util/OmnNew.h"                         
#include "API/AosApi.h"                          
#include "XmlUtil/Ptrs.h"                        
#include "alarm_c/alarm.h"

AosJqlStmtIndex::AosJqlStmtIndex(const OmnString &errmsg)
:
mFieldIndexColumns(0),
mErrmsg(errmsg)
{
}

AosJqlStmtIndex::AosJqlStmtIndex()
:
mFieldIndexColumns(0),
mRecords(0),
mDefaultIndex(false)
{
	mKeysExpr = 0;
	mWhereConds = 0;
	mLimit = 0;
}



AosJqlStmtIndex::~AosJqlStmtIndex()
{
	if (mFieldIndexColumns.size() > 0) 
	{
		for (u32 i = 0; i < mFieldIndexColumns.size(); i++)
		{
			if (mFieldIndexColumns[i])
			{
				delete mFieldIndexColumns[i];
				mFieldIndexColumns[i] = 0;
			}
		}
	}

	if (mRecords)
	{
		for (u32 i = 0; i < mRecords->size(); i++)
		{
			if ((*mRecords)[i])
			{
				delete (*mRecords)[i];
			}
		}
		delete mRecords;
		mRecords = 0;
	}

	if (mKeysExpr) OmnDelete mKeysExpr;
}


bool
AosJqlStmtIndex::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
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

	if (mOp == JQLTypes::eCreate) return createIndex(rdata);
	if (mOp == JQLTypes::eShow) return showIndexes(rdata);
	if (mOp == JQLTypes::eDrop) return dropIndex(rdata);
	if (mOp == JQLTypes::eDescribe) return describeIndex(rdata);
	if (mOp == JQLTypes::eList) return listIndex(rdata);

	AosSetEntityError(rdata, "JQL_index_run_err", "JQL Index", "")      
		<< "opr is undefined!" << enderr;
	return false;                                                           
}


bool
AosJqlStmtIndex::createIndex(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	//generate the xml data firstly
	if (mIndexName != "" && mRecords)
		return createIndexByRecord(rdata);
	//arvin 2015.08.03
	//JIMODB-232
	if(isExistIndexName(rdata))
	{
		OmnString msg = "[ERR] : Index \"";
		msg << mIndexName <<"\" already exists!";
		rdata->setJqlMsg(msg);
		return 0;
	}
	convertToXml(rdata);
	return true;
}

bool
AosJqlStmtIndex::createIndexByRecord(const AosRundataPtr &rdata)
{
	return true;
}


int
AosJqlStmtIndex::convertToXml(const AosRundataPtr &rdata)
{
	//The xml looks like follows:
	//
	//<index type="xxxx" zky_name="ind7" zky_tablename="tab7" zky_objid="_zt4g_iil_ind7" zky_public_doc="true">
	// <columns>
	//   <column zky_name="name" zky_type="charstring" zky_pre_length="4"/>
	//   <column zky_name="age" zky_type="int32" zky_pre_length="4"/>
	// </columns>
	//</index>
	//
	if (mTableName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}	
	bool rslt = false;
	OmnString msg, objid;
	OmnString iil_type;
	OmnString field_name = "";
	AosExprObjPtr expr;
	AosXmlTagPtr table = AosJqlStmtTable::getTable(mTableName, rdata);
	if (!table)
	{
		//arvin 2015.08.03
		//JIMODB-198
		OmnString msg = "[ERR] : Table \"";
		msg << mTableName <<"\" doesn't exist!";
		rdata->setJqlMsg(msg);
		return 0;
	}

	if (!mDefaultIndex)
	{
		//rslt = AosJqlStmtTable::removeDefaultIndex(rdata, table);
		rslt = true;	// Now no default index
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}
	AosXmlTagPtr indexes = table->getFirstChild("indexes");
	if (!indexes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr cmp_indexes = table->getFirstChild("cmp_indexes");
	if (!cmp_indexes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	if (mFieldIndexColumns.size())
	{
		for (u32 i = 0; i < mFieldIndexColumns.size(); i++)
		{
			if (mFieldIndexColumns[i])
			{
				iil_type = "";
				OmnString iil_name = "";
				OmnString node = "";
				OmnString idx_objid = "";
				OmnString str_field = "";
				OmnString index_name = mFieldIndexColumns[i]->mIndexName;
				OmnString type = mFieldIndexColumns[i]->mType;
				if (index_name == "")
				{
					for (u32 j = 0; j < mFieldIndexColumns[i]->mFieldList->size(); j++)
					{   
						str_field = getNameByExprObj((*mFieldIndexColumns[i]->mFieldList)[j]);
						//arvin 2015.08.01
						//JIMODB-198:check field is exist?
						AosJqlColumnPtr column = getTableField(rdata,mTableName,str_field);
						if(column->name != str_field)
						{
							OmnString msg = "[ERR] : Table \"" ;
							msg << mTableName << "\" have no field \"" << str_field << "\"!";
							rdata->setJqlMsg(msg);
							return false;
						}
						if (j != 0)  index_name << "_";
						index_name << str_field;
					}
				}

					iil_name << "_zt44_" << "idx_" << mTableName << "_" << index_name;
				idx_objid << "idx_" << mTableName << "_" << index_name;                                                                                                
				AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eIndexDoc, idx_objid);
				if (doc)
				{
					//Gavin 2015/08/01 JIMODB-199  
					OmnString error;
					error << "Index '" << mIndexName << "' already exists. ";
					rdata->setJqlMsg(error);
					return 0;
				}

				node << "<index zky_iilname=\"" << iil_name << "\""
					 << " zky_name=\"" << index_name << "\"";
				if (mIndexName != "")
					node << " zky_index_name=\"" << mIndexName << "\"";
				node << " zky_tablename=\"" << mTableName << "\""
					 << " zky_type=\"" << type << "\""
					 << " zky_objid=\"" << getObjid(rdata, JQLTypes::eIndexDoc, idx_objid) << "\">";

				node << "<columns>";

				field_name = "";
				for (u32 j = 0; j < mFieldIndexColumns[i]->mFieldList->size(); j++)
				{
					expr = (*(mFieldIndexColumns[i]->mFieldList))[j];
						if (!expr)
					{
						AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
						OmnAlarm << __func__ << enderr;
						return 0;
					}
					str_field = expr->getValue(rdata.getPtrNoLock());
					if (field_name == "") field_name = str_field;
					OmnString fieldtype = getFieldDataType(rdata,field_name);
					node << "<column zky_name=\"" << str_field << "\" type=\"" << fieldtype << "\" />";
				}
				node << "</columns>";

				// Young, 2014/11/18
				if (mWhereConds) 
				{
					AosExprObjPtr where_expr_ptr = mWhereConds->mWhereExpr;
					OmnString exprstr = where_expr_ptr->dump();
					node << "<cond><![CDATA[" << exprstr << "]]></cond>";
				}
				node << "</index>";

				if (mFieldIndexColumns[i]->mFieldList->size() > 1)
				{
					iil_type = "cmp";
					field_name = "";
					OmnString cmp_index = "";
					cmp_index << "<cmp_index zky_iilname=\"" << iil_name << "\" ";
					// 2015/07/12
					if (mIndexName != "")
						cmp_index << "cmp_index_name=\"" << mIndexName<< "\" ";
					cmp_index << ">";

					for (u32 j = 0; j < mFieldIndexColumns[i]->mFieldList->size(); j++)
					{
						str_field = getNameByExprObj((*mFieldIndexColumns[i]->mFieldList)[j]);
						cmp_index << "<" << str_field << " zky_name=\"" << str_field << "\" />";
					}
					cmp_index << "</cmp_index>";
					AosXmlTagPtr t_doc = AosXmlParser::parse(cmp_index AosMemoryCheckerArgs); 
					if (!t_doc)
					{
						AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
						OmnAlarm << __func__ << enderr;
						return 0;
					}
					cmp_indexes->addNode(t_doc);
				}
				doc = AosXmlParser::parse(node AosMemoryCheckerArgs); 
				if (!doc)
				{
					AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
					OmnAlarm << __func__ << enderr;
					return 0;
				}

				doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_INDEX_CONTAINER);
				if ( !createDoc(rdata, JQLTypes::eIndexDoc, idx_objid, doc) )
				{
					msg << "Failed to create Index doc!";
					rdata->setJqlMsg(msg);
					return 0;
				}
				OmnString table_idx;
				// modify by Young, 2015/04/28
				table_idx << "<index zky_type=\"" << iil_type 
					<< "\" zky_name=\"" << str_field
					<< "\" zky_index_objid=\"" << getObjid(rdata, JQLTypes::eIndexDoc, idx_objid)
					<< "\" zky_iilname=\"" << iil_name;
				if( mIndexName != "")
					table_idx << "\" index_name=\"" << mIndexName;
					table_idx << "\" idx_name=\"" << getObjid(rdata, JQLTypes::eIndexDoc, idx_objid)<< "\"/ >";
				AosXmlTagPtr idx_node = AosXmlParser::parse(table_idx AosMemoryCheckerArgs); 
				if (!idx_node)
				{
					AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
					OmnAlarm << __func__ << enderr;
					return 0;
				}
				indexes->addNode(idx_node);

				// 2015/7/21 xiafan
				// JIMODB-46
				msg << "Create Index '" << mIndexName << "' successful.\n";
			}
		}
	}

	else
	{
		AosXmlTagPtr colNodes = table->getFirstChild("columns");
		AosXmlTagPtr colNode = colNodes->getFirstChild("column");
		OmnString fieldName = colNode->getAttrStr("name");

		OmnString indexstr = "";
		indexstr << "<index zky_iilname=\"_zt44_" << mIndexName << "\""
			<< " zky_name=\"" << mIndexName << "\""
			<< " zky_tablename=\"" << mTableName << "\""
			<< " zky_objid=\"" << getObjid(rdata, JQLTypes::eIndexDoc, mIndexName) << "\">"
			<< "<columns><column zky_name=\"" << fieldName << "\"></column></columns>";

		if (mWhereConds) 
		{
			AosExprObjPtr where_expr_ptr = mWhereConds->mWhereExpr;
			OmnString exprstr = where_expr_ptr->dump();
			indexstr << "<cond><![CDATA[" << exprstr << "]]></cond>";
		}
		indexstr << "</index>";
		AosXmlTagPtr indexDoc = AosXmlParser::parse(indexstr AosMemoryCheckerArgs);
		indexDoc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_INDEX_CONTAINER);
		if ( !createDoc(rdata, JQLTypes::eIndexDoc, mIndexName, indexDoc) )
		{
			msg << "Failed to create Index doc!";
			rdata->setJqlMsg(msg);
			return 0;
		}
		
		OmnString table_index_str = "";
		table_index_str << "<index zky_iilname=\"_zt44_" << mIndexName << "\" idx_name=\"" << getObjid(rdata, JQLTypes::eIndexDoc, mIndexName) << "\"/ >";
		AosXmlTagPtr idx_node = AosXmlParser::parse(table_index_str AosMemoryCheckerArgs); 
		if (!idx_node)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		indexes->addNode(idx_node);
	}

	if ( !modifyDoc(rdata, table) )
	{
		msg << "Failed to create Index doc. : modify table msg."; 
		rdata->setJqlMsg(msg);
		return 0;
	}

	rdata->setJqlMsg(msg);
	rdata->setJqlUpdateCount(1);
	return 1;
}


bool
AosJqlStmtIndex::showIndexes(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> indexes;
	if ( !getDocs(rdata, JQLTypes::eIndexDoc, indexes) )
	{
		OmnString error = "No index found";
		rdata->setJqlMsg(error);
		return true;
	}

	int totalNum = indexes.size();
	rdata->setJqlUpdateCount(totalNum);

	OmnString content = "<content>";
	for (int i=0; i<totalNum; i++)
	{
		OmnString idx_name = indexes[i]->getAttrStr("zky_index_name");
		OmnString table_name = indexes[i]->getAttrStr("zky_tablename");
		content << "<record zky_name=\"" << idx_name << "\" zky_tablename=\"" << table_name << "\"/>";
	}
	content << "</content>";

	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("zky_name");
	fields.push_back("zky_tablename");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_name"] = "IndexName";
	alias_name["zky_tablename"] = "TableName";

	AosXmlTagPtr doc = AosXmlParser::parse(content AosMemoryCheckerArgs);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	
	return true;
}


bool
AosJqlStmtIndex::dropIndex(const AosRundataPtr &rdata)
{
	OmnString msg;                                          
	string objid = getObjid(rdata, JQLTypes::eIndexDoc, mIndexName);                    
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete Index doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop Index " << mIndexName << " scessful";      
	rdata->setJqlMsg(msg);                                  

	if ( !deleteIIL(rdata, mIndexName) )
	{
		msg << "Failed to delete Index Data !";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Drop Index " << mIndexName << " scessful";
	rdata->setJqlMsg(msg);
	return true;  
}


bool
AosJqlStmtIndex::listIndex(const AosRundataPtr &rdata)
{
	bool rslt;
	if (mIndexName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	//fix list index bug jimodb-59 
	//2015-07-23 rain
	vector<AosXmlTagPtr> indexes;
	OmnString indexIILName = mIndexName;
	if(strncmp(mIndexName.data(), "_zt", 3) != 0)
	{
		//normal index not like _zt44_idx_tableName_keyName
		rslt = getDocs(rdata, JQLTypes::eIndexDoc, indexes);
		if(!rslt)
		{
			OmnString error = "No index found";
			rdata->setJqlMsg(error);
			return true;
		}
		//get the right iilname in indexDoc
		for(size_t i = 0; i < indexes.size(); i++)
		{
			OmnString idxName = indexes[i]->getAttrStr("zky_index_name");
			if(idxName == mIndexName)
			{
				mIndexName = indexes[i]->getAttrStr("zky_iilname");
				break;
			}
		}
	}
	

	const char *pos = strstr((const char*)mIndexName.data(), "__$group"); 
	if (pos)
	{
		rslt = listShuffleIndex(rdata);
		mIndexName = indexIILName;
		return rslt;
	}
	vector<i64> docids;
	vector<string> values;
	i64 page_size = 1;
	i64 black_size = 20;
	if (mLimit){
		page_size = mLimit->getOffset();
		black_size = mLimit->getRowCount();
	}

	string str_term;
	if (mWhereConds) 
	{
		OmnString iil_name = OmnString(&mIndexName[6]);
		AosExprObjPtr where_expr_ptr = mWhereConds->mWhereExpr;
		if (!where_expr_ptr)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			mIndexName = indexIILName;
			return false;
		}
		where_expr_ptr->createConds(rdata, mConds, "");
		AosJqlQueryWhereConds query_where_conds;
		AosXmlTagPtr conds, where_filter;
		if (mKeysExpr)
		{
			query_where_conds.init(rdata, mConds, mIndexName, mKeysExpr);
		}
		else
		{
			OmnString objid = getObjid(rdata, JQLTypes::eIndexDoc, iil_name);
			AosXmlTagPtr doc = getDocByObjid(rdata, objid);
			if (!doc)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				mIndexName = indexIILName;
				return false;
			}
			query_where_conds.init(rdata, mConds, doc);
		}
		query_where_conds.getConfig(rdata, conds, where_filter);
		mCond = query_where_conds.getCond();
		if (mCond->mOpr == "eq")
			mCond->mOpr = "pf";

		if (!mCond)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			mIndexName = indexIILName;
			return false;
		}

		if (mCond->mOpr == "eq") mCond->mOpr = "pf";

		if (mCond->mLHS.toLower() == "docid")
		{
			AosJqlQueryWhereCond::QueryFilter query_filter; 
			query_filter.mType = "value";
			query_filter.mFieldIdx = 0;
			query_filter.mOpr = mCond->mOpr;
			query_filter.mValue = mCond->mRHS;
			query_filter.mValue2 = mCond->mValue2;
			mCond = OmnNew AosJqlQueryWhereCond();
			mCond->mIILName = mIndexName;
			mCond->mOpr = "an";
			mCond->mRHS = "*";
			mCond->addQueryFilter(query_filter);
		}
	}
	else
	{
		mCond = OmnNew AosJqlQueryWhereCond();
		mCond->mIILName = mIndexName;
		mCond->mOpr = "an";
		mCond->mRHS = "*";
	}
	AosXmlTagPtr term = createTerm(rdata);
	if (!term)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		mIndexName = indexIILName;
		return false;
	}
	str_term = term->toString();

OmnScreen << str_term << endl;

	i64 num_docs = 0;

	rslt = querySafeOnIIL(rdata,page_size,
			black_size, str_term, docids, values, num_docs );
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		mIndexName = indexIILName;
		return false;
	}
	if (!docids.size() == values.size())
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		mIndexName = indexIILName;
		return false;
	}

	OmnString content = "";
	content << "<content total=\"" << num_docs << "\">";
	for (u32 i=0; i<docids.size(); i++)
	{
		content << "<record>"
			   	<< 	"<zky_docid><![CDATA[" << docids[i]<< "]]></zky_docid>"
				<<   "<value><![CDATA[" << values[i] << "]]></value>"
				<< "</record>";
	}
	content << "</content>";

	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		mIndexName = indexIILName;
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("zky_docid");
	fields.push_back("value");
	map<OmnString, OmnString> alias_name;
	alias_name["zky_docid"] = "Docid";
	alias_name["value"] = "Value";
	AosXmlTagPtr doc = AosXmlParser::parse(content AosMemoryCheckerArgs);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		mIndexName = indexIILName;
		return false;
	}

	OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	mIndexName = indexIILName;
	return true;
}


AosXmlTagPtr
AosJqlStmtIndex::createTerm(const AosRundataPtr &rdata)
{
	if (!mCond)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	OmnString term = "";
	term << "<term>"
		<< "<opr><![CDATA[" << mCond->mOpr << "]]></opr>"
		<< "<iilname><![CDATA[" << mCond->mIILName << "]]></iilname>"
		<< "<value1><![CDATA[" << mCond->mRHS << "]]></value1>"
		<< "<value2><![CDATA[" << mCond->mValue2 << "]]></value2>"
		<< "<query_filters>"
		<< AosJqlQueryWhereConds::generateQuerFilterConf(rdata, mTableName, mCond) 
		<< "</query_filters>"
		<< "</term>";
	return AosXmlParser::parse(term AosMemoryCheckerArgs);             
}

bool
AosJqlStmtIndex::listShuffleIndex(const AosRundataPtr &rdata)
{
	vector<i64> docids;
	vector<string> values;
	vector<OmnString> iilnames;
	
	OmnString iilname = mIndexName;
	const char *pos = strstr((const char*)iilname.data(), "__$group"); 
	iilname = OmnString(iilname.data(), pos-iilname.data()); 
	int mun_cubes = AosGetNumCubes();
	for (int i = 0; i < mun_cubes; i++)
	{
		iilnames.push_back(AosIILName::composeCubeIILName(i, iilname));
	}

	i64 page_size = 1;
	i64 black_size = 20;
	if (mLimit){
		page_size = mLimit->getOffset();
		black_size = mLimit->getRowCount();
	}

	string str_term;
	if (mWhereConds) 
	{
		iilname = OmnString(&mIndexName[6]);
		AosExprObjPtr where_expr_ptr = mWhereConds->mWhereExpr;
		if (!where_expr_ptr)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		where_expr_ptr->createConds(rdata, mConds, "");
		AosJqlQueryWhereConds query_where_conds;
		AosXmlTagPtr conds, where_filter;
		if (mKeysExpr)
		{
			query_where_conds.init(rdata, mConds, mIndexName, mKeysExpr);
		}
		else
		{
			OmnString objid = getObjid(rdata, JQLTypes::eIndexDoc, iilname);
			AosXmlTagPtr doc = getDocByObjid(rdata, objid);
			if (!doc)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			query_where_conds.init(rdata, mConds, doc);
		}
		query_where_conds.getConfig(rdata, conds, where_filter);
		mCond = query_where_conds.getCond();
		if (mCond->mOpr == "eq")
			mCond->mOpr = "pf";

		if (!mCond)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		if (mCond->mOpr == "eq") mCond->mOpr = "pf";

		if (mCond->mLHS.toLower() == "docid")
		{
			AosJqlQueryWhereCond::QueryFilter query_filter; 
			query_filter.mType = "value";
			query_filter.mFieldIdx = 0;
			query_filter.mOpr = mCond->mOpr;
			query_filter.mValue = mCond->mRHS;
			query_filter.mValue2 = mCond->mValue2;
			mCond = OmnNew AosJqlQueryWhereCond();
			mCond->mIILName = mIndexName;
			mCond->mOpr = "an";
			mCond->mRHS = "*";
			mCond->addQueryFilter(query_filter);
		}
	}
	else
	{
		mCond = OmnNew AosJqlQueryWhereCond();
		mCond->mIILName = mIndexName;
		mCond->mOpr = "an";
		mCond->mRHS = "*";
	}

	vector<vector<i64> > all_docids;
	vector<vector<string> > all_values;
	i64 num_docs = 0, all_num_docs = 0;
	for (u32 i = 0; i < iilnames.size(); i++)
	{
		docids.clear();
		values.clear();
		mCond->mIILName = iilnames[i];
		AosXmlTagPtr term = createTerm(rdata);
		if (!term)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		str_term = term->toString();

		OmnScreen << str_term << endl;


		bool rslt = querySafeOnIIL(rdata,page_size,
				black_size, str_term, docids, values, num_docs);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		if (!docids.size() == values.size())
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		all_docids.push_back(docids);
		all_values.push_back(values);
		all_num_docs = all_num_docs + num_docs;
	}

	OmnString content = "";
	content << "<content total=\"" << num_docs << "\">";
	for (u32 i = 0; i < all_docids.size(); i++)
	{
		docids = all_docids[i];
		values = all_values[i];
		for (u32 j = 0; j < docids.size(); j++)
		{
			content << "<record>"
				   	<< 	"<zky_docid><![CDATA[" << docids[j]<< "]]></zky_docid>"
					<<   "<value><![CDATA[" << values[j] << "]]></value>"
					<<   "<shuffle_id><![CDATA[" << i << "]]></shuffle_id>"
					<< "</record>";
		}
	}
	content << "</content>";

	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("zky_docid");
	fields.push_back("value");
	fields.push_back("shuffle_id");
	map<OmnString, OmnString> alias_name;
	alias_name["zky_docid"] = "Docid";
	alias_name["value"] = "Value";
	alias_name["shuffle_id"] = "ShuffleId";
	AosXmlTagPtr doc = AosXmlParser::parse(content AosMemoryCheckerArgs);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}



bool
AosJqlStmtIndex::describeIndex(const AosRundataPtr &rdata)
{

	if (mIndexName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString msg = "";
	string objid = getObjid(rdata, JQLTypes::eIndexDoc, mIndexName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to find Index doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}

	doc = doc->getFirstChild("columns");
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	//output Index fields
    cout << "Index columns: " << endl;

	vector<OmnString> fields;
	fields.push_back("zky_name");
	fields.push_back("zky_type");
	fields.push_back("zky_pre_length");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_name"] = "Name";
	alias_name["zky_type"] = "Type";
	alias_name["zky_pre_length"] = "Pre-Length";

	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
    	
	return true;
}


void 
AosJqlStmtIndex::setWhereConds(AosJqlWhere *where_conds)
{
	mWhereConds = where_conds;
}

AosJqlStatement *
AosJqlStmtIndex::clone()
{
	return OmnNew AosJqlStmtIndex(*this);
}


void 
AosJqlStmtIndex::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtIndex::setTableName(OmnString name)
{
	mTableName = name;
}

void 
AosJqlStmtIndex::setIndexName(OmnString name)
{
	mIndexName = name;
}

/*
void
AosJqlStmtIndex::setFieldIndexColumns(vector<AosJqlStmtIndex::AosFieldIdxCols*> *fieldIdxCols)
{
	mFieldIndexColumns = fieldIdxCols;
	(*mFieldIndexColumns)[0]->mIndexName = mIndexName;
}
*/


void
AosJqlStmtIndex::setFieldList(AosJqlStmtIndex::AosFieldIdxCols* field_list)
{
	mFieldIndexColumns.clear();
	mFieldIndexColumns.push_back(field_list);
}



OmnString
AosJqlStmtIndex::getObjName()
{
	OmnString objName = "";

	objName << mIndexName;
	return objName;
}


void
AosJqlStmtIndex::setLimit(AosJqlLimit *limit)
{
	mLimit = limit;
}

void 
AosJqlStmtIndex::setIndexRecords(vector<AosExprList*> *records)
{
	mRecords = records;
}


void
AosJqlStmtIndex::setKeysExpr(AosExprList *keys_list)
{
	mKeysExpr = keys_list;
}

bool
AosJqlStmtIndex::isExistIndexName(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> index_docs;
	bool rslt = getDocs(rdata, JQLTypes::eIndexDoc, index_docs);
	if(!rslt)
	{
		OmnString msg = "No index found";
		rdata->setJqlMsg(msg);
		return true;
	}
	for(size_t i = 0; i < index_docs.size(); i++)
	{
		OmnString name = index_docs[i]->getAttrStr("zky_index_name","");
		if(name == mIndexName) return true;
	}
	return false;
}
