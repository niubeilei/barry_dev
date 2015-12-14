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
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoProg_JimoProgService_h
#define Aos_JimoProg_JimoProgService_h

#include "JimoProg/JimoProg.h"

class AosJimoProgService : public AosJimoProg
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, vector<OmnString>, Omn_Str_hash, compare_str> map_t;
    typedef hash_map<const OmnString, vector<OmnString>, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t									mSections;
	OmnString								mServiceName;
	OmnString								mJobName;
	OmnString								mCode;
	vector<JSONValue>						mTasks;
	vector<OmnString>						mDeleteProcNames;
	map<OmnString, AosJimoLogicObjNewPtr>	mUserVarMap;
	OmnString								mStatName;

public:
	AosJimoProgService(const int version);
	~AosJimoProgService();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// JimoProg Interface
	virtual ProgType 	getProgType() const {return eJimoService;}
	virtual bool 		run(AosRundata *rdata);
	virtual OmnString 	getCode() const {return mCode;}
	virtual OmnString 	generateCode(AosRundata *rdata);
	virtual void 		setJobName(const OmnString &Service_name);
	virtual bool 		appendStatement(AosRundata *rdata,
								const OmnString &section_name,
								const OmnString &statement);
	virtual	void 		setDeleteProcName(AosRundata *rdata,
								const OmnString &proc_name);
	virtual bool        addUserVar(AosRundata *rdata,               
								const OmnString &varname, 
								const AosJimoLogicObjNewPtr &logic);
	virtual bool 		setUserVarValue(AosRundata *rdata, 
								const OmnString &varname, 
								const AosValueRslt &value);
	virtual AosValueRslt getUserVarValue(
								AosRundata *rdata, 
								const OmnString &varname);

private:
	vector<JSONValue> combineTasks(AosRundata *rdata,vector<JSONValue> &taskV);
	bool handleError(AosRundata *rdata);

	bool generateInputs(const AosXmlTagPtr &def, AosRundata *rdata);
	bool generateTasks(const AosXmlTagPtr &def, AosRundata *rdata);
	bool generateCode(AosRundata *rdata, const OmnString &section_name);
	bool generateServiceCode(AosRundata *rdata);

	bool separateJoinTask(
						vector<JSONValue> &join_tasks,
						vector<JSONValue> &normal_tasks);


	bool getJSONTasks(AosRundata* rdata,
					const vector<OmnString> &tasks_str);

	bool getSameTypeTasks(
	 					AosRundata *rdata,
						vector<JSONValue> &taskV,
						map<string, vector<JSONValue> > &sameTypeTasks);

	
	bool getSameTypeInputTask(
						AosRundata *rdata,
						const vector<JSONValue> &tasks,
						vector<JSONValue> &tmpV);

	map<string,string> getInputsFromTask(
								AosRundata* rdata,
								const JSONValue &task);

	bool composeDataProcs(
					AosRundata *rdata,
					JSONValue &task1,
					const JSONValue &task2);

	bool compare(
			const map<string,string> &lhs,
			const map<string,string> &rhs);


	bool collectInputOutput(
			AosRundata *rdata,
			vector<JSONValue> &tasks,
			map<OmnString,int> &outputs,
			map<OmnString,int> &inputs);
	
	bool putIntoMap(
			const JSONValue &values,
			map<OmnString,int> &map);

	bool checkTaskOrder(
			AosRundata *rdata,
			map<OmnString,int> &outputs,
			vector<JSONValue>  &tasks,
			vector<JSONValue>  &finish_tasks);
	
	bool inputsIsReady(
			AosRundata* rdata,
			map<OmnString,int> &outputs_map,
			vector<OmnString> &inputs_str,
			JSONValue &task);

	bool updateTaskOutputs(
			AosRundata* rdata, 
			JSONValue &task, 
			map<OmnString,int> &outputs_map);


	bool pickReadyTasks(
			AosRundata *rdata, 
			map<OmnString,int> &outputs_map,
			vector<JSONValue>  &tasks,
			vector<JSONValue>  &finish_tasks);

	bool getTaskInputsOrOutputs(
			AosRundata *rdata,
			const OmnString &parm,
			JSONValue &task,
			vector<OmnString> &inputs_str);

	bool deleteInvalidTask(
			AosRundata *rdata,
			vector<JSONValue> &tasks,
			vector<OmnString> &invalid_outputs);

	bool isDelete(
			AosRundata *rdata,
			const vector<OmnString> &outputs,
			const vector<OmnString> &invalid_outputs);
	
	bool setStat(OmnString& name);
	bool getStat(OmnString& name);

};
#endif

