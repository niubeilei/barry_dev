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
#ifndef AOS_JQLStatement_JqlQueryWhereConds_H
#define AOS_JQLStatement_JqlQueryWhereConds_H

#include "JQLStatement/JqlDataStruct.h"
#include "JQLStatement/JqlQueryWhereCond.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/Ptrs.h"

#include "SEInterfaces/ExprObj.h"
#include "Rundata/Rundata.h"
#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "Debug/Debug.h"
#include "Util/Opr.h"

#include <vector>
using namespace std;

class AosJqlQueryWhereConds : public AosJqlDataStruct
{
private:
	typedef OmnString SingleIILStr;
	typedef OmnString CmpIILStr;
	typedef OmnString FieldStr;

	typedef map<FieldStr, SingleIILStr>::iterator SingleIILMapItr;
	typedef map<CmpIILStr, vector<FieldStr> >::iterator CmpIILMapItr;
	typedef map<FieldStr, vector<CmpIILStr> >::iterator FieldCmpIILMapItr;
	typedef map<FieldStr, vector<AosJqlQueryWhereCondPtr> >::iterator FieldCondsMapItr;
	typedef map<u32, vector<CmpIILStr> >::iterator mCmpIILMapByPriorityLevelItr;

public:
	AosRundataPtr 									mRundata;
	AosXmlTagPtr									mTableDoc;
	OmnString										mTableName;
	AosDataRecordObjPtr								mRecord;
	char 											mSep;

	vector<AosJqlQueryWhereCondPtr>					mConds;
	vector<AosJqlQueryWhereCondPtr>					mCmpIILConds;
	vector<AosJqlQueryWhereCondPtr>					mKeyWordConds;
	vector<AosJqlQueryWhereCondPtr>					mSingleIILConds;
	vector<AosJqlQueryWhereCondPtr>					mNotAllowedUseIndexConds;

	map<FieldStr, SingleIILStr>						mSingleIILMap;
	map<CmpIILStr, vector<FieldStr> >				mCmpIILMap;
	map<u32, vector<CmpIILStr> >					mCmpIILMapByPriorityLevel;
	map<FieldStr, vector<CmpIILStr> >				mFieldCmpIILMap;
	map<FieldStr, vector<AosJqlQueryWhereCondPtr> >	mFieldCondsMap;
	u32												mFieldNum;

	bool 											mInited;
	bool											mIsAllSingle;
	bool											mIsBad;

	// for list index
	const AosExprList										*mKeysExpr;


public:
	AosJqlQueryWhereConds();
	~AosJqlQueryWhereConds();

	bool init(
			const AosRundataPtr &rdata,
			vector<AosJqlQueryWhereCondPtr> &conds,
			const OmnString &table_name,
			const AosDataRecordObjPtr &record);

	bool init(
			const AosRundataPtr &rdata,
			vector<AosJqlQueryWhereCondPtr> &conds,
			const AosXmlTagPtr &index_doc);

	bool init(
			const AosRundataPtr &rdata,
			vector<AosJqlQueryWhereCondPtr> &conds,
			const OmnString &iilname,
			const AosExprList *fields);

	bool initByStat(
			const AosRundataPtr &rdata,
			vector<AosJqlQueryWhereCondPtr> &qry_conds,
			vector<OmnString> &stat_keys);

	bool getConfig(
			const AosRundataPtr &rdata,
			AosXmlTagPtr &conds,
			AosXmlTagPtr &where_filter);

	int getFieldIdx(
			const AosRundataPtr &rdata,
			const OmnString &cmpiil_name, 
			const OmnString &field_name);

	AosJqlQueryWhereCondPtr getCond();
	OmnString generateKeyWordTermCond(const AosJqlQueryWhereCondPtr &cond);

	static OmnString generateDefaultTermConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &table_doc, 
			const OmnString &table_name);

	static bool generateDefaultCond(
			const AosRundataPtr &rdaya,
			AosXmlTagPtr &table_doc, 
			const OmnString &table_name,
			vector<AosJqlQueryWhereCondPtr> &conds);

	static OmnString generateTermConf(
			const AosRundataPtr &rdata,
			const OmnString &table_name,
			const AosJqlQueryWhereCondPtr &cond,
			const bool is_single = false);

	static OmnString generateQuerFilterConf(
			const AosRundataPtr &rdata,
			const OmnString &table_name,
			const AosJqlQueryWhereCondPtr &cond);

	static int getFieldIdx(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &index_doc,
			const OmnString &cmpiil_name,
			const OmnString &field_name);

	static OmnString generateWhereFilterConf(
			const AosRundataPtr &rdata,
			const OmnString &expr_str);

	bool checkIsLastField(
		const AosRundataPtr &rdata,
		const OmnString &cmpiil_name,
		const OmnString &field_name);


private:
	bool initCmpIILMap();
	bool initSingleIILMap();
	bool initFieldCondsMap();

	bool splitConds();
	bool optimizeByCompIIL();
	bool mergeRedundantConds();
	bool sortCmpIILByPriorityLevel();
	bool handleCmpIIL(const AosRundataPtr &rdata, vector<CmpIILStr> &vec_cmpiils);
	bool handleAllSingleConds(const AosRundataPtr &rdata, vector<AosJqlQueryWhereCondPtr> &conds);

	bool longitudinalMergeCond(AosJqlQueryWhereCondPtr &to_cond, AosJqlQueryWhereCondPtr &from_cond);
	bool transverseMergeCond(AosJqlQueryWhereCondPtr &to_cond, AosJqlQueryWhereCondPtr &cond);

	OmnString generateCondsConf();
	OmnString generateWhereFilterConf();

	static OmnString trimSep(OmnString &value)
	{
		char ch;
		bool flag;
		OmnString new_str;
		for (int i = 0; i < value.length(); i ++)
		{
			ch = value[i];
			if (ch != '\1')
			{
				flag = false;
				new_str << ch ;
				continue;
			}
			if (ch == '\1' && flag == false)
			{
				new_str << ch ;
				flag = true;
				continue;
			}
		}
		return new_str;
	}

	template<typename Key, typename Value>
	void addEntry(
			map<Key, vector<Value> > &map_ctnr,
			Key key, 
			Value value)
	{
		if (map_ctnr.find(key) == map_ctnr.end())
		{
			vector<Value> vec;
			vec.push_back(value);
			map_ctnr[key] = vec;
		}
		else{
			map_ctnr[key].push_back(value);
		}
	}

	bool gt_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool ge_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool le_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool lt_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool eq_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool in_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);

	bool like_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool ne_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);
	bool other_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2);

public:
	void setIsAllSingle(bool flag){mIsAllSingle = flag;}
	bool checkCondsDataType(const AosRundataPtr &rdata,
						const AosXmlTagPtr &tabledoc,
						vector<AosJqlQueryWhereCondPtr> &conds);

};
#endif
