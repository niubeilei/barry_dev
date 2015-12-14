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
#include "JQLStatement/JqlSelect.h"

#include "JQLStatement/JqlTableReference.h"
#include "JQLStatement/JqlSelectField.h"
#include "JQLStatement/JqlWhere.h"
#include "JQLStatement/JqlHaving.h"

#include "JQLExpr/Expr.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"

AosJqlSelect::AosJqlSelect()
:
mFieldListPtr(0),
mSelectIntoPtr(0),
mTableReferencesPtr(0)
{
}

AosJqlSelect::~AosJqlSelect()
{
	OmnDelete mFieldListPtr;
	OmnDelete mTableReferencesPtr;
	OmnDelete mSelectIntoPtr;
}


bool
AosJqlSelect::init(const AosRundataPtr &rdata)
{
	bool rslt = false;
		/*
	if (mFieldListPtr->empty())
	{ 
		if (!mTableReferencesPtr)
		{ 
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}                                                                               
		mFieldListPtr->clear();
		for (u32 i = 0; i < mTableReferencesPtr->size(); i++)
		{ 
			AosJqlTableReferencePtr table_reference = (*mTableReferencesPtr)[i];
			rslt = table_reference->getAllField(rdata, select, mFieldListPtr);
			if (!rslt)
			{ 
				AosSetErrorUser(rdata, __func__)
					<< "Failed to get all field." << enderr; 
				OmnAlarm << enderr;
				return false;
			}
		} 
	}
	*/

	AosJqlSelectPtr select = this;
	if (mFieldListPtr->empty()) 
	{
		AosSetErrorUser(rdata, __func__)
			<< "Select Field List Is Empty." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	if (!mTableReferencesPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	bool has_all = false;
	for (u32 i = 0; i < mFieldListPtr->size(); i++)
	{
		if ((*mFieldListPtr)[i]->dumpByNoEscape() == "*")
		{
			has_all = true;
			for (u32 j = 0; j < mTableReferencesPtr->size(); j++)
			{
				AosJqlTableReferencePtr table_reference = (*mTableReferencesPtr)[j];
				rslt = table_reference->getAllField(rdata, select, mFieldListPtr);
				if (!rslt)
				{
					AosSetErrorUser(rdata, __func__)
						<< "Failed to get all field." << enderr;
					OmnAlarm << enderr;
					return false;
				}
				break;
			}
		}
	}

	if (has_all)
	{
		for (u32 i = 0; i < mFieldListPtr->size(); i++)
		{
			if ((*mFieldListPtr)[i]->dumpByNoEscape() == "*")
			{
				mFieldListPtr->erase(mFieldListPtr->begin() + i);
				i--;
			}
		}
	}

	rslt = handleAliasName(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Failed to handle alias name." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	if (mTableReferencesPtr->size() > 1)
	{
		AosTableReferences *table_references  = OmnNew AosTableReferences();
		for (u32 i = 0; i <mTableReferencesPtr->size(); i++)
		{
			table_references->push_back((*mTableReferencesPtr)[i]->clone(rdata));
		}

		AosJqlJoinTablePtr join_table = OmnNew AosJqlJoinTable();
		join_table->mTableReferencesPtr = table_references;
		mTableReferencesPtr->clear();
		AosJqlTableReferencePtr table_reference = OmnNew AosJqlTableReference();
		table_reference->mJoinTable = join_table;
		mTableReferencesPtr->push_back(table_reference);
	}

	AosJqlTableReferencePtr table_reference = (*mTableReferencesPtr)[0];
	rslt = table_reference->init(rdata, this);

	if (!rslt)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Failed to init table reference." << enderr;
		OmnAlarm << enderr;
		return false;
	}
	return  true;
}

bool
AosJqlSelect::generateQueryConf(const AosRundataPtr &rdata, const AosXmlTagPtr &query_conf)
{
	if (!query_conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!mTableReferencesPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	aos_assert_rr(mTableReferencesPtr->size() == 1, rdata, false);
	AosJqlTableReferencePtr table_reference = (*mTableReferencesPtr)[0];
	OmnString table_str = table_reference->generateTableConf(rdata);
	//if (table_str != "") return false;
	AosXmlTagPtr table = AosStr2Xml(rdata.getPtr(), table_str AosMemoryCheckerArgs); 
	if (!table)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Failed to generate table config." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	if (mLimitPtr)
	{
		i64 limit_offset = mLimitPtr->getOffset();
		i64 limit_row_count = mLimitPtr->getRowCount();
		table->setAttr("start_idx", limit_offset);
		table->setAttr("psize", limit_row_count);
	}

	OmnString field_str = generateFieldConf(rdata);
	AosXmlTagPtr field_conf = AosStr2Xml(rdata.getPtr(), field_str AosMemoryCheckerArgs); 
	table->addNode(field_conf);
	query_conf->addNode(table);
	return true;
}

OmnString
AosJqlSelect::generateFieldConf(const AosRundataPtr &rdata)
{
	if (!mFieldListPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	OmnString conf;
	conf << "<fnames>";
	AosJqlSelectFieldPtr field;
	for (u32 i =0; i < mFieldListPtr->size(); i++)
	{
		field = (*mFieldListPtr)[i];
		if (!field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		conf << field->generateFieldConf(rdata);
	}
	conf << "</fnames>";
	return conf;
}


bool
AosJqlSelect::handleAliasName(const AosRundataPtr &rdata) 
{
	if (!mFieldListPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString field_name, as_name;
	for (u32 i = 0; i < mFieldListPtr->size(); i++)
	{
		AosJqlSelectFieldPtr field = (*mFieldListPtr)[i];
		if (!field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		//if (field->checkIsAgrFunc() || field->checkIsFunc())
		//if (!field->isName())
		field_name = field->dumpByNoEscape();
		//else
		//field_name = field->getFieldName(rdata);
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		as_name = field->getAliasName(rdata);
		if (as_name != "")
		{
			AosJqlUtil::escape(field_name);
			AosJqlUtil::escape(as_name);
			AosAliasName alis_name;
			alis_name.mOName = field->dump1();
			alis_name.mCName = as_name;
			alis_name.mField = field;

			mAliasNameMap[as_name] = alis_name;
			mAliasNameMap[field_name] = alis_name;
			mNameToAlias[field_name] = as_name;
		}
	}
	map<OmnString, AosAliasName>::iterator itr;

	//2015-12-05 Pay	JIMODB-1296
	if(mGroupByPtr)
	{
		AosJqlSelectFieldList groupByFields = *(mGroupByPtr->mFieldListPtr);
		mGroupByPtr->mFieldListPtr->clear();
		OmnString grpbyName;
		for(size_t i = 0; i < groupByFields.size(); i++)
		{
			grpbyName = groupByFields[i]->dumpByNoEscape();
			aos_assert_r(grpbyName != "",false);
			itr = mAliasNameMap.find(grpbyName);
			if(itr != mAliasNameMap.end())
			{
				AosJqlSelectFieldPtr groupByField = itr->second.mField;

				mGroupByPtr->mFieldListPtr->push_back(groupByField);
			}
			else
			{
				mGroupByPtr->mFieldListPtr->push_back(groupByFields[i]);
			}
		}
	}

	//2015-12-03 wumeng JIMODB-1275 JIMODB-1276
	//This can replace measure with alias
	//handle having alias
	if(mHavingPtr)
	{
		itr = mAliasNameMap.begin();
		OmnString having_str = mHavingPtr->getHavingConf();
		bool all = true;
		for(;itr != mAliasNameMap.end();itr++)
		{
			OmnString alias;
			alias << (char)0x60 << itr->first << (char)0x60;
			int found = having_str.replace(alias,itr->second.mOName,all);
			if(found > 0)
			{
				mHavingPtr->setHavingConf(having_str);
			}
		}
	}

	//handle orderby alias
	if(mOrderByPtr)
	{
		vector<AosJqlOrderByFieldPtr> orderByFields= *(mOrderByPtr->mOrderFieldList);
		mOrderByPtr->mOrderFieldList->clear();
		OmnString fieldName;
		for(size_t i = 0;i < orderByFields.size();i++)
		{
			if(orderByFields[i]->mField->checkIsAgrFunc())
				fieldName = orderByFields[i]->mField->dumpByStat();
			else
				fieldName = orderByFields[i]->mField->dumpByNoEscape();
			aos_assert_r(fieldName != "",false);
			itr = mAliasNameMap.find(fieldName);
			if(itr != mAliasNameMap.end())
			{
				AosJqlOrderByFieldPtr orderByField = OmnNew AosJqlOrderByField();
				orderByField->mField = itr->second.mField;
				orderByField->mIsAsc = orderByFields[i]->mIsAsc;
				orderByField->mIsInWhereCond = orderByFields[i]->mIsInWhereCond;

				mOrderByPtr->mOrderFieldList->push_back(orderByField);
			}
			else
			{
				mOrderByPtr->mOrderFieldList->push_back(orderByFields[i]);
			}
		}
	}
	return true;
}


bool
AosJqlSelect::getAllTableDef(
		const AosRundataPtr &rdata,
		vector<AosXmlTagPtr> &table_defs)
{
	bool rslt;
	if (!mTableReferencesPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosJqlTableReferencePtr table_reference;
	for (u32 i = 0; i < mTableReferencesPtr->size(); i++)
	{
		table_reference = (*mTableReferencesPtr)[i];
		rslt = table_reference->getAllTableDef(rdata, table_defs);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Failed to get all table config." << enderr;
			OmnAlarm << enderr;
			return false;
		}
	}
	return true;
}

/*
void
AosJqlSelect::optimization()
{
	if (mFieldListPtr && !mGroupByPtr)
	{
		OmnString field_name, as_name, func_name;
		for (u32 i = 0; i < mFieldListPtr->size(); i++)
		{
			AosJqlSelectField *fieldPtr = (*mFieldListPtr)[i];
			if (fieldPtr && fieldPtr->checkIsAgrFunc()) 
			{
				field_name = fieldPtr->getFieldStr(true);
				func_name = fieldPtr->getAgrFuncTypeStr();
				as_name = fieldPtr->mExpr->toString();
				if (fieldPtr->getAliasStr() != "")
					as_name = fieldPtr->getAliasStr();
				mAliasToName[as_name] = field_name; 
				mNameToAlias[field_name] = as_name; 
				(*mFieldListPtr)[i] = fieldPtr->clone();
				(*mFieldListPtr)[i]->mExpr = new AosExprFieldName(field_name.getBuffer());
				delete fieldPtr;
				fieldPtr = 0;
				if (!mLimitPtr) mLimitPtr = new AosJqlLimit;
				mLimitPtr->mOffset = 0; 
				mLimitPtr->mRowCount = 1;
				resetOrderby(func_name, (*mFieldListPtr)[i]);
			}
		}
	}
}


void 
AosJqlSelect::resetOrderby(OmnString func_name, AosJqlSelectField* field)
{
	delete mOrderByPtr;
	mOrderByPtr = new AosJqlOrderBy();
	AosJqlOrderBy::AosJqlOrderByField* orderby_field = new AosJqlOrderBy::AosJqlOrderByField();
	mOrderByPtr->mOrderFieldList = new AosJqlOrderBy::AosJqlOrderByFieldList();
	mOrderByPtr->mOrderFieldList->push_back(orderby_field);
	AosAggregationType::E type = AosAggregationType::toEnum(func_name);
	switch(type)
	{
		case AosAggregationType::eMax:
			orderby_field->mField = field->clone();
			orderby_field->mIsAsc = false;
			break;
		case AosAggregationType::eMin:
			orderby_field->mField = field->clone();
			orderby_field->mIsAsc = true;
			break;
		default:
			delete mOrderByPtr;
			mOrderByPtr = 0;
	}
}
*/

// 2015/9/16
void 
AosJqlSelect::setSubQuery()
{
	(*mTableReferencesPtr)[0]->setSubQuery();
}
