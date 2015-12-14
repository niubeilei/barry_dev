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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatQueryAnalyzer/StatQueryPattern1.h"
	
#include "API/AosApi.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/QueryTermObj.h"
#include "StatQueryAnalyzer/StatQueryUnit.h"
#include "StatUtil/StatDefineDoc.h"

AosStatQueryPattern1::AosStatQueryPattern1()
{
}

AosStatQueryPattern1::~AosStatQueryPattern1()
{
}


bool
AosStatQueryPattern1::proc(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req,
		const AosStatDefineDocPtr &stat_def)
{
	vector<AosQrUtil::FieldDef> &select_fields = req->getSelectedFields();

	AosStatQueryUnitPtr stat_unit = OmnNew AosStatQueryUnit(
			stat_def, select_fields);

	set<AosStatMeasure> measures;
	bool rslt = findMeasureFields(rdata, req, stat_def, measures);
	aos_assert_r(rslt, false);

	vector<OmnString> &group_by_fields = req->getGroupByFields();
	rslt = stat_unit->initMeasureInfo(measures, group_by_fields);
	aos_assert_r(rslt, false);

	rslt = runQuery(rdata, req, stat_unit);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStatQueryPattern1::findMeasureFields(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req,
		const AosStatDefineDocPtr &stat_def,
		set<AosStatMeasure> &measures)
{
	bool rslt, find;
	
	vector<AosQrUtil::FieldDef> &select_fields = req->getSelectedFields();
	for(u32 i=0; i<select_fields.size(); i++)
	{
		AosQrUtil::FieldDef & s_field = select_fields[i];
		if(!s_field.isStatValueField())	continue;

		AosStatMeasure ms;
		rslt = stat_def->findMeasure(s_field.oname, s_field.mAggregation,
				find, ms);
		if(!rslt || !find)
		{
			OmnAlarm << "This aggregation not support yet."
				<< "measure_field:" << s_field.oname << "; "
				<< "agr_type:" << AosAggregationType::toStr(s_field.mAggregation) << "; "
				<< enderr;
			continue;
		}
	
		measures.insert(ms);
	}
	
	/*
	AosQueryTermObjPtr or_term = req->getOrTerm();
	aos_assert_r(or_term, false);
	
	vector<AosQueryTermObjPtr> cond_terms;
	rslt = or_term->getCondTerms(cond_terms);
	aos_assert_r(rslt, false);

	AosQueryTermObjPtr cond;
	for(u32 i=0; i<cond_terms.size(); i++)
	{
		cond = cond_terms[i];
		if(!cond->isStatValueTerm())	continue;
		
		agr_type = cond->getAggregation();
		if(!AosAggregationType::isValid(agr_type))	continue;
	
		AosStatMeasure ms(cond->getFieldName(), agr_type);
		rslt = stat_def->hasMeasure(ms);
		if(!rslt)
		{
			OmnAlarm << "This aggregation not support yet."
				<< "measure_field:" << cond->getFieldName() << "; "
				<< "agr_type:" << AosAggregationType::toStr(agr_type) << "; "
				<< enderr;
			continue;
		}
		
		measures.insert(ms);
	}
	*/
	return true;
}


bool
AosStatQueryPattern1::runQuery(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req,
		const AosStatQueryUnitPtr &stat_unit)
{
	AosQueryTermObjPtr or_term = req->getOrTerm();
	aos_assert_r(or_term, false);
	
	bool rslt = trimQueryReqTerms(rdata, or_term, stat_unit); 
	aos_assert_r(rslt, false);
	
	u64 t1 = OmnGetTimestamp(); 

	// Ketty temp.
	or_term->setPagesize(20000000);

	rslt = or_term->runQuery(rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "!!!!!!!! run orterm query time:" << OmnGetTimestamp() - t1 << endl;

	u32 docid_num;
	AosBuffPtr docids_buff;
	rslt = collectStatDocids(rdata, or_term, docid_num, docids_buff);
	aos_assert_r(rslt && docid_num, false);
	stat_unit->initStatDataset(rdata, docid_num, docids_buff);

	u64 t2 = OmnGetTimestamp(); 
	rslt = stat_unit->run(rdata);
	aos_assert_r(rslt, false);	
	OmnScreen << "!!!!!!!! run stat query time:" << OmnGetTimestamp() - t2 << endl;
	
	req->setStatQueryUnit(stat_unit);
	return true;
}


bool
AosStatQueryPattern1::trimQueryReqTerms(
		const AosRundataPtr &rdata,
		const AosQueryTermObjPtr &or_term,
		const AosStatQueryUnitPtr &stat_unit)
{
	// This function does the following:
	// 1. Remove time related conditions
	// 2. Add missing compound IIL conditions
	// 3. Set IIL names to all the key field equal-conditions.
	//
	// After this transformation, the query should be ready to
	// run (to pick stat docids).
	
	vector<AosQueryTermObjPtr> value_conds;
	vector<AosQueryTermObjPtr> time_conds;
	set<OmnString> pick_level_fields;
	
	vector<OmnString> & group_by_fields = stat_unit->getGroupByFields();
	for(u32 i=0; i<group_by_fields.size(); i++)
	{
		pick_level_fields.insert(group_by_fields[i]);
	}

	AosStatDefineDocPtr stat_def = stat_unit->getStatDefineDoc();

	vector<AosQueryTermObjPtr> cond_terms;
	bool rslt = or_term->getCondTerms(cond_terms);
	aos_assert_r(rslt, false);

	//OmnString iilname;
	AosQueryTermObjPtr cond;
	for(u32 i=0; i<cond_terms.size(); i++)
	{
		cond = cond_terms[i];
		if(cond->isStatValueTerm())
		{
			value_conds.push_back(cond);	
			continue;
		}
		if(cond->isTimeTerm())
		{
			time_conds.push_back(cond);
			continue;
		}
		
		OmnString key_field = cond->getFieldName();
		rslt = stat_def->hasKeyField(key_field);
		if(!rslt)
		{
			OmnAlarm << "This field not find."
				<< "key_field:" << key_field << "; "
				<< enderr;
			return false;
		}

		//iilname = AosIILName::composeStatKeyIILName(stat_def->getObjid(), key_field);
		//aos_assert_r(iilname != "", false);
		//cond->setIILName(iilname);
		
		pick_level_fields.insert(key_field);
	}

	/*
	vector<AosQrUtil::FieldDef> &select_fields = stat_unit->getSelectFields();
	for(u32 i=0; i<select_fields.size(); i++)
	{
		// need ??
		AosQrUtil::FieldDef & s_field = select_fields[i];
		if(s_field.isStatValueField() || s_field.isTimeField())	continue;
		
		OmnString key_field = s_field.oname;
		rslt = stat_def->hasKeyField(key_field);
		if(!rslt)
		{
			OmnAlarm << "This field not find."
				<< "key_field:" << key_field << "; "
				<< enderr;
			return false;
		}

		pick_level_fields.insert(key_field);
	}
	*/

	AosInternalStat internal_stat(0);
	rslt = true; //stat_def->pickInternalStat(pick_level_fields, internal_stat);
	aos_assert_r(rslt, false);

	u32 internal_id = internal_stat.getStatId();
	OmnString internal_stat_iil_name = AosIILName::composeInternalStatIILName(stat_def->getObjid(), internal_id);
	aos_assert_r(internal_stat_iil_name != "", false);

	or_term->addInternalStatIdTerm(rdata,
			internal_stat_iil_name, internal_id);
	or_term->removeStatValueTerms(rdata, value_conds);
	or_term->removeStatValueTerms(rdata, time_conds);

	stat_unit->initStatKeyFields(internal_stat);
	stat_unit->initStatQryTimeConds(rdata, time_conds);
	stat_unit->initStatQryValueConds(rdata, value_conds);
	return true;
}


bool
AosStatQueryPattern1::collectStatDocids(
		const AosRundataPtr &rdata,
		const AosQueryTermObjPtr &or_term,
		u32 &docid_num,
		AosBuffPtr &docids_buff)
{
	AosQueryRsltObjPtr norm_qry_rslt = or_term->getQueryData();
	aos_assert_r(norm_qry_rslt, false);

	aos_assert_rr(norm_qry_rslt, rdata, false);
	
	u32 total_num = or_term->getTotal(rdata);

	docid_num = norm_qry_rslt->getNumDocs();
	u64 * idx_docids = norm_qry_rslt->getDocidsPtr();
	u32 len = docid_num * sizeof(u64);
	docids_buff = OmnNew AosBuff((char *)idx_docids, len, len AosMemoryCheckerArgs);
	
	OmnScreen << "Total docid num:" << total_num
		<< "; crt docid num:" << docid_num << endl;

	// temp
	//docid_num = 1;
	//docids_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	//for(u32 i=1; i<=docid_num; i++)
	//{
	//	docids_buff->setU64(i);
	//}
	return true;
}

bool
AosStatQueryPattern1::generateRslt(
		const AosRundataPtr &rdata,
		const AosStatQueryUnitPtr &stat_unit,
		OmnString &content)
{
	aos_assert_r(stat_unit, false);
	
	bool rslt = stat_unit->generateRslt(rdata, content);
	aos_assert_r(rslt, false);

	return true;
}


