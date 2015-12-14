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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JimoProgObj_h
#define AOS_SEInterfaces_JimoProgObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "JimoLogicNew/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "SEUtil/JqlTypes.h"
#include <list>

class AosJimoProgObj : virtual public AosJimo
{
public:
	enum ProgType
	{
		eInvalid,

		eJimoJob,

		eJimoService,

		eJimoScript,

		eMaxProgType
	};

protected:
	OmnString		mTest;

public:
	AosJimoProgObj(const int version);
	~AosJimoProgObj();

	virtual bool 		addUserVar(AosRundata *rdata, 
								const OmnString &varname, 
								const AosJimoLogicObjNewPtr &logic) = 0;

	virtual bool 		setUserVarValue(AosRundata *rdata, 
								const OmnString &varname, 
								const AosValueRslt &value) = 0;

	virtual AosValueRslt getUserVarValue(
								AosRundata *rdata, 
								const OmnString &varname) = 0;
	// JimoProgObj Interface
	virtual AosJimoProgObjPtr createJimoProg(
								const AosXmlTagPtr &def,
								AosRundata *rdata) = 0;


	virtual bool appendStatement(AosRundata *rdata,
								const OmnString &section_name,
								const OmnString &statemnet) = 0;

	//Barry 2015/06/05
	virtual	void setDeleteProcName(AosRundata *rdata,
								const OmnString &proc_name) = 0;

	virtual AosJimoProgObjPtr getPrevProg() = 0;
	virtual ProgType getProgType() const = 0;
	virtual bool run(AosRundata *rdata) = 0;
	virtual void setPrevProg(const AosJimoProgObjPtr &prev_prog) = 0;
	virtual bool addJimoLogic(AosRundata *rdata, const AosJimoLogicObjPtr &jimologic) = 0;
	virtual bool addDataProc(AosRundata *rdata, const OmnString &, const OmnString &) = 0;
	virtual bool addStatTask(AosRundata *rdata, const OmnString &, const OmnString &) = 0;
	virtual bool addDataset(AosRundata *rdata, const OmnString &, const OmnString&) = 0;
	virtual bool addTask(AosRundata *rdata, const OmnString &, const OmnString &) = 0;
	virtual bool addJob(AosRundata *rdata, const OmnString &, const OmnString &) = 0;
	virtual AosJimoProgObjPtr getJimoProg(AosRundata *rdata, const OmnString &name) const = 0;
	virtual OmnString getNextName(AosRundata *rdata) const = 0;
	virtual OmnString getStatDocName(AosRundata *rdata) const = 0;
	virtual OmnString getCode() const = 0;

	virtual void setJobName(const OmnString &job_name)= 0;
	virtual AosJimoProgObjPtr createLoadDataProg(
					AosRundata *rdata,
					const AosXmlTagPtr &inputds,
					const AosXmlTagPtr &tabledoc,
					std::list<string> &fields,
					JQLTypes::OpType &op) = 0;

	virtual AosXmlTagPtr createConfig() = 0;
    virtual OmnString getEnv(OmnString name) = 0;
    virtual OmnString getJobname() = 0;

	// Chen Ding, 2015/05/26
	virtual bool addSchema(AosRundata *rdata, 
					const OmnString &name,
					const AosJimoLogicObjNewPtr &schema) = 0;

	// Xia Fan, 2015/5/22
	virtual bool addJimoLogicNew(AosRundata*, OmnString&, AosJimoLogicObjNewPtr&) = 0;
	virtual AosJimoLogicObjNew *getJimoLogic(AosRundata*, const OmnString&) = 0;

	// Young, 2015/08/26
	virtual bool saveLogicDoc(AosRundata *rdata, const OmnString &objid, const OmnString &conf) = 0;
	virtual AosXmlTagPtr getLogicDoc(AosRundata *rdata, const OmnString &objid) = 0;
	
	//arvin ,2015.08.21
	virtual map<OmnString,AosJimoLogicObjNewPtr> getJimoLogics() = 0;

	// Levi 2015/08/18
	virtual bool setConf(OmnString& confList) = 0;
	virtual vector<OmnString> getConfList() = 0;
	/*
	virtual bool setDataProcName(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcNameList() = 0;
	
	virtual bool setDataProcSelect(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcSelectList() = 0;
	
	virtual bool setDataProcIndex(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcIndexList() = 0;
	
	virtual bool setDataProcIILBatchOpr(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcIILBatchOprList() = 0;
	
	virtual bool setDataProcDocBatchOpr(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcDocBatchOprList() = 0;

	virtual bool setDataProcIILJoin(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcIILJoinList() = 0;
	
	virtual bool setDataProcGroupBy(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcGroupByList() = 0;
	
	virtual bool setDataProcStatJoin(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcStatJoinList() = 0;

	virtual bool setDataProcStatDoc(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcStatDocList() = 0;
	
	virtual bool setDataProcStatKey(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcStatKeyList() = 0;

	virtual bool setDataProcIILKey(OmnString& name) = 0;
	virtual vector<OmnString> getDataProcIILKeyList() = 0;
	
	virtual void setUnionIndexName(const OmnString &name) = 0;
	virtual OmnString getUnionIndexName() = 0;

	virtual void setUnionIILBatchName(const OmnString &name) = 0;
	virtual OmnString getUnionIILBatchName() = 0;

	virtual void setUnionSelectName(const OmnString &name) = 0;
	virtual vector<OmnString> getUnionSelectName() = 0;

	virtual bool setDataset(OmnString& name) = 0;
	virtual vector<OmnString> getDatasetList() = 0;
	
	virtual bool setSelectMap(map<OmnString, vector<OmnString> >& name) = 0;
	virtual map<OmnString, vector<OmnString> > getSelectMap() = 0;

	virtual bool insertToSelectMap(OmnString &name, vector<OmnString> &selectList) = 0;
	
	virtual bool setUnionMap(map<OmnString, vector<OmnString> >& name) = 0;
	virtual map<OmnString, vector<OmnString> > getUnionMap() = 0;
	
	virtual bool setIILJoinMap(map<OmnString, vector<OmnString> >& name) = 0;
	virtual map<OmnString, vector<OmnString> > getIILJoinMap() = 0;

	virtual bool setStatMap(map<OmnString, vector<OmnString> >& name) = 0;
	virtual map<OmnString, vector<OmnString> > getStatMap() = 0;

	virtual bool insertToStatMap(OmnString &name, vector<OmnString> &statList) = 0;
	virtual bool insertToSelectInputMap(OmnString input_name, OmnString join_name) = 0;
	virtual map<OmnString, vector<OmnString> > getSelectInputMap() = 0;
	
	virtual bool insertToDataProcMap(OmnString name, vector<OmnString> map_list) = 0;
	virtual map<OmnString, vector<OmnString> > getDataProcMap() = 0;
	
	virtual bool insertToDocMap(OmnString ds_name, vector<OmnString> doc_list) = 0;
	virtual map<OmnString, vector<OmnString> > getDocMap() = 0;
	
	virtual bool insertToIndexMap(OmnString index_name, vector<OmnString> index_list) = 0;
	virtual map<OmnString, vector<OmnString> > getIndexMap() = 0;
	
	virtual bool insertToSelectConfMap(OmnString select_name, OmnString sel_conf) = 0;
	virtual map<OmnString, vector<OmnString> > getSelectConfMap() = 0;
*/
	virtual bool setJoinMap(map<OmnString, vector<OmnString> >& name) = 0;
	virtual map<OmnString, vector<OmnString> > getJoinMap() = 0;

	virtual bool setStat(OmnString& name) = 0;
	virtual bool getStat(OmnString& name) = 0;

	virtual bool setDatasetMap(const OmnString &ds) = 0;
	virtual set<OmnString> getDatasetMap() = 0;

	virtual bool insertToDataFlowMap(const OmnString &input_name, const OmnString &output_name) = 0;
	virtual map<OmnString, vector<OmnString> > getDataFlowMap() = 0;

	virtual bool insertToOtherNameMap(const OmnString &input_name, const OmnString &output_name) = 0;
	virtual map<OmnString, vector<OmnString> > getOtherNameMap() = 0;
	
	virtual bool insertToNameMap(const OmnString &input_name, const OmnString &output_name) = 0;
	virtual map<OmnString, vector<OmnString> > getNameMap() = 0;
	
	virtual bool hasSelect(const OmnString &selectname) = 0;
	virtual void setSelectMap(const OmnString &selname) = 0;
};
#endif

