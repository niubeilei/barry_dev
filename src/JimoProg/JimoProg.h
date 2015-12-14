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
#ifndef AOS_JimoProg_JimoProg_h
#define AOS_JimoProg_JimoProg_h

#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JimoProgObj.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "SEInterfaces/JimoLogicObjNew.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "JSON/JSON.h"

#include <vector>
#include <set>

class AosJimoProg : public AosJimoProgObj
{
	OmnDefineRCObject;

protected:
	struct SchemaEntry
	{
		OmnString				mName;
		AosJimoLogicObjNewPtr	mSchema;

		SchemaEntry(const OmnString &name, const AosJimoLogicObjNewPtr &schema)
		:
		mName(name),
		mSchema(schema)
		{
		}
	};

	OmnMutexPtr					mLock;
	vector<AosJimoLogicObjPtr>	mJimoLogic;
	vector<JSONValue>			mTasks;
	AosJimoProgObjPtr			mPrevProg;
    OmnString                   mJobName;
	OmnString					mUnionIndexName;
	OmnString					mUnionIILBatchName;
	vector<SchemaEntry> 		mSchemas;
	OmnString					mStatName;
	map<OmnString, AosXmlTagPtr> 			mLogicConfs;
	map<OmnString, AosJimoLogicObjNewPtr> mJimoLogics;
	vector<OmnString>			mConfList;
/*
	vector<OmnString>			mDataProcNameList;
	vector<OmnString>			mDataProcSelectList;
	vector<OmnString>			mDataProcIndexList;
	vector<OmnString>			mDataProcIILBatchOprList;
	vector<OmnString>			mDataProcDocBatchOprList;
	vector<OmnString>			mDataProcIILJoinList;
	vector<OmnString>			mDataProcGroupByList;
	vector<OmnString>			mDataProcStatJoinList;
	vector<OmnString>			mDataProcStatDocList;
	vector<OmnString>			mDataProcStatKeyList;
	vector<OmnString>			mDataProcIILKeyList;
	vector<OmnString>			mUnionSelectNameList;
	vector<OmnString>			mDatasetList;

	map<OmnString, vector<OmnString> > mSelectMap;
	map<OmnString, vector<OmnString> > mUnionMap;
	map<OmnString, vector<OmnString> > mIILJoinMap;
	map<OmnString, vector<OmnString> > mStatMap;
	map<OmnString, vector<OmnString> > mSelectInputMap;
	map<OmnString, vector<OmnString> > mDataProcMap;
	map<OmnString, vector<OmnString> > mDocMap;
	map<OmnString, vector<OmnString> > mIndexMap;
//	map<OmnString, vector<OmnString> > mDatasetMap;
	map<OmnString, vector<OmnString> > mSelectConfMap;
*/
	set<OmnString> mSelectMap;
	map<OmnString, vector<OmnString> > mJoinMap;
	map<OmnString, vector<OmnString> > mDataFlowMap;
	map<OmnString, vector<OmnString> > mOtherNameMap;
	map<OmnString, vector<OmnString> > mNameMap;
	set<OmnString>	mDatasetMap;
public:
	// AosJimoProg(const int version);
	AosJimoProg();
	AosJimoProg(OmnMutexPtr lock);

	~AosJimoProg();

	// JimoProg Interface
	virtual AosJimoProgObjPtr createJimoProg(
								const AosXmlTagPtr &def,
								AosRundata *rdata);

	virtual void setPrevProg(const AosJimoProgObjPtr &prog) {mPrevProg = prog;}
	virtual AosJimoProgObjPtr getPrevProg() {return mPrevProg;}
	virtual ProgType getProgType() const {return AosJimoProgObj::eJimoJob;}

	virtual bool        addUserVar(AosRundata *rdata,               
								const OmnString &varname, 
								const AosJimoLogicObjNewPtr &logic);
	virtual bool 		setUserVarValue(AosRundata *rdata, 
								const OmnString &varname, 
								const AosValueRslt &value);
	virtual AosValueRslt getUserVarValue(
								AosRundata *rdata, 
								const OmnString &varname);

	virtual bool addJimoLogic(AosRundata *rdata, const AosJimoLogicObjPtr &jimologic)
	{
		return false;
	}
	virtual bool addDataProc(AosRundata *rdata, const OmnString &, const OmnString &);

	virtual bool addStatTask(AosRundata *rdata,  const OmnString &, const OmnString &)
	{
		OmnNotImplementedYet;
		return false;
	}

	virtual bool addDataset(AosRundata *rdata, const OmnString &, const OmnString&)
	{
		return false;
	}
	virtual bool addTask(AosRundata *rdata, const OmnString &jsonstr, const OmnString &msg)
	{
		if (jsonstr == "") return false;
		JSONReader reader;
		JSONValue value;
		bool rslt = reader.parse(jsonstr, value);
		aos_assert_r(rslt, false);
		mTasks.push_back(value);
		return true;
	}
	virtual bool addJob(AosRundata *rdata, const OmnString &, const OmnString &)
	{
		return false;
	}
	virtual AosJimoPtr cloneJimo() const {return 0;}
	virtual AosJimoProgObjPtr getJimoProg(AosRundata *rdata, const OmnString &name) const
	{
		return 0;

	}

	virtual OmnString getStatDocName(AosRundata *rdata) const {return "";}
	virtual OmnString getNextName(AosRundata *rdata) const {return "";}

	virtual bool appendStatement(AosRundata*, OmnString const&, OmnString const&)
	{
		return false;
	}
	//Barry 2015/06/05
	virtual void setDeleteProcName(AosRundata*, OmnString const&) 
	{
		OmnNotImplementedYet;
	}

	// Chen Ding, 2015/04/12
	virtual AosJimoProgObjPtr createLoadDataProg(
					AosRundata *rdata,
					const AosXmlTagPtr &inputds,
					const AosXmlTagPtr &tabledoc,
					std::list<string> &fields,
					JQLTypes::OpType &op);
	virtual AosXmlTagPtr createConfig();
    virtual bool run(AosRundata *rdata);
	virtual OmnString getCode() const
	{
		return "";
	}
	virtual void setJobName(const OmnString &job_name)
	{
		OmnNotImplementedYet;
	}

    // 2015/05/06
    virtual OmnString getEnv(OmnString name)
    {
        return "";
    }

    //2015/05/12
    OmnString getJobname()
    {
        return mJobName;
    }

	// Chen Ding, 2015/05/26
	virtual bool addSchema(AosRundata *rdata, 
					const OmnString &name,
					const AosJimoLogicObjNewPtr &schema);

	// Young, 2015/08/26
	virtual bool saveLogicDoc(
					AosRundata *rdata, 
					const OmnString &objid, 
					const OmnString &conf);
	virtual AosXmlTagPtr getLogicDoc(
					AosRundata *rdata, 
					const OmnString &objid);

	// Xia Fan, 2015/05/22
	virtual bool addJimoLogicNew(AosRundata *rdata, OmnString &name, AosJimoLogicObjNewPtr &jimologic);

	virtual AosJimoLogicObjNew * getJimoLogic(AosRundata *rdata, const OmnString &name);

	virtual map<OmnString,AosJimoLogicObjNewPtr> getJimoLogics(){return mJimoLogics;}
	// Levi 2015/08/18
	virtual bool setConf(OmnString& confList);
	virtual vector<OmnString> getConfList();
/*

	virtual bool setDataProcName(OmnString& name);
	virtual vector<OmnString> getDataProcNameList();  
	
	virtual bool setDataProcSelect(OmnString& name);
	virtual vector<OmnString> getDataProcSelectList();  

	virtual bool setDataProcIndex(OmnString& name);
	virtual vector<OmnString> getDataProcIndexList();
	
	virtual bool setDataProcIILBatchOpr(OmnString& name);
	virtual vector<OmnString> getDataProcIILBatchOprList();
	
	virtual bool setDataProcDocBatchOpr(OmnString& name);
	virtual vector<OmnString> getDataProcDocBatchOprList();

	virtual bool setDataProcIILJoin(OmnString& name);
	virtual vector<OmnString> getDataProcIILJoinList();
	
	virtual bool setDataProcGroupBy(OmnString& name);
	virtual vector<OmnString> getDataProcGroupByList();
	
	virtual bool setDataProcStatJoin(OmnString& name);
	virtual vector<OmnString> getDataProcStatJoinList();

	virtual bool setDataProcStatDoc(OmnString& name);
	virtual vector<OmnString> getDataProcStatDocList();
	
	virtual bool setDataProcStatKey(OmnString& name);
	virtual vector<OmnString> getDataProcStatKeyList();

	virtual bool setDataProcIILKey(OmnString& name);
	virtual vector<OmnString> getDataProcIILKeyList();
	
	virtual bool setDataset(OmnString& name);
	virtual vector<OmnString> getDatasetList();

	virtual void setUnionIndexName(const OmnString &name);
	virtual OmnString getUnionIndexName();

	virtual void setUnionIILBatchName(const OmnString &name);
	virtual OmnString getUnionIILBatchName();

	virtual void setUnionSelectName(const OmnString &name);
	virtual vector<OmnString> getUnionSelectName();
	
	virtual bool setSelectMap(map<OmnString, vector<OmnString> > &mSelectMap);
	virtual map<OmnString, vector<OmnString> > getSelectMap();
	virtual bool insertToSelectMap(OmnString &name, vector<OmnString> &selectList);

	virtual bool setUnionMap(map<OmnString, vector<OmnString> > &mUnionMap);
	virtual map<OmnString, vector<OmnString> > getUnionMap();
	
	virtual bool setIILJoinMap(map<OmnString, vector<OmnString> > &mIILJoinMap);
	virtual map<OmnString, vector<OmnString> > getIILJoinMap();

	virtual bool setStatMap(map<OmnString, vector<OmnString> > &mIILJoinMap);
	virtual map<OmnString, vector<OmnString> > getStatMap();
	virtual bool insertToStatMap(OmnString &name, vector<OmnString> &statList);
	
	virtual bool insertToSelectInputMap(OmnString input_name, OmnString join_name);
	virtual  map<OmnString, vector<OmnString> > getSelectInputMap();
	
	virtual bool insertToDataProcMap(OmnString name, vector<OmnString> map_list);
	virtual map<OmnString, vector<OmnString> > getDataProcMap();

	virtual bool insertToDocMap(OmnString ds_name, vector<OmnString> doc_list);
	virtual map<OmnString, vector<OmnString> > getDocMap();
	
	virtual bool insertToIndexMap(OmnString index_name, vector<OmnString> index_list);
	virtual map<OmnString, vector<OmnString> > getIndexMap();
	//virtual map<OmnString, vector<OmnString> > getDatasetMap();
	
	virtual bool insertToSelectConfMap(OmnString sel_name, OmnString sel_conf);
	virtual map<OmnString, vector<OmnString> > getSelectConfMap();
*/
	virtual bool setJoinMap(map<OmnString, vector<OmnString> > &mJoinMap);
	virtual map<OmnString, vector<OmnString> > getJoinMap();

	virtual bool setStat(OmnString& name);
	virtual bool getStat(OmnString& name);

	virtual bool setDatasetMap(const OmnString &ds);
	virtual set<OmnString> getDatasetMap(){return mDatasetMap;}

	virtual bool insertToDataFlowMap(const OmnString &input_name, const OmnString &output_name);
	virtual map<OmnString, vector<OmnString> > getDataFlowMap(){return mDataFlowMap;}
	
	virtual bool insertToOtherNameMap(const OmnString &input_name, const OmnString &output_name);
	virtual map<OmnString, vector<OmnString> > getOtherNameMap(){return mOtherNameMap;}
	
	virtual bool insertToNameMap(const OmnString &input_name, const OmnString &output_name);
	virtual map<OmnString, vector<OmnString> > getNameMap(){return mNameMap;}

	virtual bool hasSelect(const OmnString &selectname);
	virtual void setSelectMap(const OmnString &selname);
};
#endif

