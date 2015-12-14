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
// This Data Proc Engine takes as its inputs a dataset. It starts a number
// of threads. The threads will retrieve a recordset from the dataset, 
// process all the records, and repeats until all recordsets are processed.
//
// The order in which records are processed is not significant. 
//   
//
// Modification History:
// Created by Linda 2014/03/14
// Copied by Chen Ding, 2015/01/15
////////////////////////////////////////////////////////////////////////////
#include "DataEngine/JoinDataEngine2.h"

#include "DataAssembler/DataAssembler.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "TaskMgr/Task.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"


#include "DataRecord/Recordset.h"

#include "JSON/JSON.h"


#define DELTA	100000

AosJoinDataEngine2::AosJoinDataEngine2(const bool flag)
:
AosTaskAction(AOSACTTYPE_DATAENGINEJOIN2, AosActionType::eJoinDataEngine2, flag), 
mLock(OmnNew OmnMutex()),
mLhsDataset(0),
mRhsDataset(0),
mInput_records(OmnNew AosDataRecordObj*[256]),
mModelType("")
{

	mLogLhsDataset = OmnNew OmnFile("/home/lina/Jimo_Servers0/log/loglhs.txt",OmnFile::eAppend);
	mLogRhsDataset = OmnNew OmnFile("/home/lina/Jimo_Servers0/log/logrhs.txt",OmnFile::eAppend);
}


AosJoinDataEngine2::~AosJoinDataEngine2()
{
}


bool                                                                                 
AosJoinDataEngine2::config(
		const u64 task_docid,     
        const AosXmlTagPtr &conf,                                                         
        const AosRundataPtr &rdata)                                                                          
{                                                                               
	AosTaskObjPtr task = AosTask::getTaskStatic(task_docid, rdata);
    if(!task)   procFailed(__FILE__, __LINE__);                                              
    if(!conf)                                                                            
    {                                                                                    
        AosSetEntityError(rdata, "miss_config", "JoinDataEngine2", "Root conf") << conf << enderr;
        return procFailed(__FILE__, __LINE__);                                 
    }                                                                        
                                                                                     
    mTaskDocid = task_docid;                              

	OmnString key_expr;
	AosExprObjPtr expr;                                              
	key_expr = conf->getAttrStr("LHS");
	OmnString errmsg;
	if (key_expr != "")
	{
		key_expr << ";";
		expr = AosParseExpr(key_expr, errmsg, rdata.getPtr());
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		mLhsFields = expr;
	}

	key_expr = conf->getAttrStr("RHS");
	if (key_expr != "")
	{
		key_expr << ";";
		expr = AosParseExpr(key_expr, errmsg, rdata.getPtr());
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		mRhsFields = expr;
	}

	mModelType = conf->getAttrStr("model_type", "");
                                                         
    bool rslt = configDataset(task, conf, rdata);                         
    if(!rslt)   return procFailed(__FILE__, __LINE__);                                         

    rslt = configEngineProc(rdata, conf);                                             
    if(!rslt)   return procFailed(__FILE__, __LINE__);                                  

    rslt = configDataCollector(rdata, conf);                  
    if(!rslt)   return procFailed(__FILE__, __LINE__);                       
                                                                               
    return true;                                                                                             
}                                                                                                            

bool                                                                           
AosJoinDataEngine2::initAction(                                                
        const AosTaskObjPtr &task,                                             
        const AosXmlTagPtr &sdoc,                                              
        const AosRundataPtr &rdata)                                            
{                                                                              
    //OmnScreen << "JoinDataEngine2 Start!, start time:" << mStartTime << endl;  
	showDataEngineInfo(__FILE__, __LINE__, "start");
                                                                               
    bool rslt = notifyAssemblerStart(rdata);                                   
    if (!rslt) return procFailed(__FILE__, __LINE__);                          
                                                                               
    rslt = notifyDatasetStart(rdata);                                          
    if (!rslt) return procFailed(__FILE__, __LINE__);                          
                                                                               
    rslt = notifyDataProcStart(rdata);                                         
    if (!rslt) return procFailed(__FILE__, __LINE__);                          
    return true;                                                               
}                                                                              


void AosJoinDataEngine2::onThreadInit(const AosRundataPtr &, void **data)         
{                                                                                 
    DataEnginEnv *env = OmnNew DataEnginEnv();                                    
    env->l_recordset = 0;                                                           
    env->r_recordset = 0;                                                           
    *data = env;                                                                  
}


bool                                                                   
AosJoinDataEngine2::runAction(const AosRundataPtr &rdata, void *data)  
{
	aos_assert_r(mLhsDataset, false);
	aos_assert_r(mRhsDataset, false);

	bool rslt;
	if (mModelType == "simple")
	{
		rslt = procSimple(rdata.getPtr(), data);
	}
	else
	{
		//OmnAlarm << endl;
		//rslt = proc(rdata.getPtr(), data);
	}
	if (!rslt) return procFailed(__FILE__, __LINE__);

	return runFinished(rdata);

}


void 
AosJoinDataEngine2::onThreadExit(const AosRundataPtr &, void *data)         
{                    
	OmnDelete (DataEnginEnv*)data;
}


bool                                                         
AosJoinDataEngine2::finishedAction(const AosRundataPtr &rdata)
{                                                                   
	// this func is called by Task.
	// means all thrd has proc finished.

	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	AosActionObjPtr this_ptr(this, false);

	bool cont = tryFinish();
	if(!cont)
	{
		if(mEngineStatus == eProcFailed)    return task->actionFailed(this_ptr, rdata);
		return false;
	}

	mEndTime = OmnGetSecond();
	OmnScreen << "All ScanDataEngine2 finished:" << endl;
	reportStatus();

	bool rslt;
	rslt = notifyDataProcFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = notifyDatasetFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = notifyAssemblerFinish(rdata);
	aos_assert_r(rslt, false);

	task->actionFinished(this_ptr, rdata);
	clean();

	showDataEngineInfo(__FILE__, __LINE__, "finish");
	return true;
}

bool
AosJoinDataEngine2::configDataCollector(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	//Jozhi 2015-03-25 implement BuffArrayVar
	AosXmlTagPtr outputXml;
	vector<boost::shared_ptr<Output> > outputs;
	bool rslt = false;
	outputs = mProc->getOutputs();
	for (size_t i = 0; i < outputs.size(); i++)
	{
		outputXml = AosXmlParser::parse(outputs[i]->getConfig() AosMemoryCheckerArgs);
		aos_assert_r(outputXml, false);
		rslt = remoteCreateDataCol(outputXml, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool                                                                             
AosJoinDataEngine2::configDataset(                                               
        const AosTaskObjPtr &task,                                               
        const AosXmlTagPtr &conf,                                                
        const AosRundataPtr &rdata)                                              
{                                                                                
    aos_assert_r(task, false);                                                   

	map<OmnString, AosDatasetObjPtr> datasets = task->getDatasets();
	aos_assert_r(datasets.size() == 2, false);

	map<int, OmnString> indexs = task->getIndexs();
	aos_assert_r(indexs.size() == 2, false);

	map<int, OmnString>::iterator itr_index;
	itr_index = indexs.find(0);
	aos_assert_r(itr_index != indexs.end(), false);

	map<OmnString, AosDatasetObjPtr>::iterator itr;
	itr = datasets.find(itr_index->second);
	aos_assert_r(itr != datasets.end(), false);
	mLhsDataset = itr->second;

	itr_index = indexs.find(1);
	aos_assert_r(itr_index != indexs.end(), false);
	itr = datasets.find(itr_index->second);
	mRhsDataset = itr->second;
    return true;                                                                 
}                                                                                


bool 
AosJoinDataEngine2::configEngineProc(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &conf)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	aos_assert_r(conf, false);

	AosDataProcObjPtr proc;
	AosXmlTagPtr p_conf = conf->getFirstChild(true);
	aos_assert_r(p_conf, false);

	proc = createDataProc(rdata, p_conf);
	aos_assert_r(proc, false);
	mProc = proc;

	vector<boost::shared_ptr<Output> > outputs = proc->getOutputs();
	for (size_t i = 0; i < outputs.size(); i++)
	{
		AosDataAssemblerObjPtr asmobj = outputs[i]->getAssembler();
		aos_assert_r(asmobj, false);
		mAssemblers.push_back(asmobj);
	}

	//arvin 2015.09.11
	OmnString config = p_conf->getNodeText();
	JSONReader reader;
	JSONValue json;
	reader.parse(config,json);
	OmnString type = json["type"].asString("");
	if(type.toLower() == "statjoin" )
	{
		JSONValue key = json["keys"];
		OmnString errmsg;
		for(size_t i =0 ; i < key.size();i++)
		{
			OmnString key_str = key[i].asString();
			key_str << ";";
			AosExprObjPtr expr = AosParseExpr(key_str,errmsg,rdata.getPtr());
			aos_assert_r(expr,false);
			mKeyExpr.push_back(expr);	
		}
	}
	return true;
}

bool                                                                       
AosJoinDataEngine2::notifyAssemblerStart(const AosRundataPtr &rdata)       
{                                                                          
    bool rslt;                                                             
    AosRundata *rdata_raw = rdata.getPtrNoLock();                          
    for(u32 i = 0; i < mAssemblers.size(); i++)                            
    {                                                                      
		mAssemblers[i]->setTargetReporter(this);
        rslt = mAssemblers[i]->sendStart(rdata_raw);                       
        if (!rslt) return procFailed(__FILE__, __LINE__);                  
    }                                                                      
    return true;
}                                                                          


bool
AosJoinDataEngine2::notifyDatasetStart(const AosRundataPtr &rdata)
{
	bool rslt = mLhsDataset->sendStart(rdata);
	aos_assert_r(rslt, false);

	rslt = mRhsDataset->sendStart(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool                                                                                
AosJoinDataEngine2::notifyDataProcStart(const AosRundataPtr &rdata)                 
{                                                                                   
	mProc->setTargetReporter(this);
	mProc->start(rdata);                                                 
	return true;                                                                    
}                                                                                   


bool                                                                      
AosJoinDataEngine2::notifyAssemblerFinish(const AosRundataPtr &rdata)     
{                                                                         
    bool rslt;                                                            
    AosRundata *rdata_raw = rdata.getPtrNoLock();                         
    for(u32 i = 0; i < mAssemblers.size(); i++)                           
    {                                                                     
        rslt = mAssemblers[i]->sendFinish(rdata_raw);                     
        if (!rslt) return procFailed(__FILE__, __LINE__);                 
    }                                                                     
	for(u32 i = 0; i < mAssemblers.size(); i++)
	{
		rslt = mAssemblers[i]->waitFinish(rdata_raw);	
		if (!rslt) return procFailed(__FILE__, __LINE__);
	}

    return true;                                                          
}                                                                         

bool
AosJoinDataEngine2::notifyDatasetFinish(const AosRundataPtr &rdata)
{
	bool rslt = mLhsDataset->sendFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = mRhsDataset->sendFinish(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool                                                                                 
AosJoinDataEngine2::notifyDataProcFinish(const AosRundataPtr &rdata)                 
{                                                                                    
	vector<AosDataProcObjPtr> procs;
	mProc->finish(procs, rdata);                                                 
    return true;                                                                     
}                                                                                    
                                                                                     


bool
AosJoinDataEngine2::clean()
{
	mAssemblers.clear();       
	return true;               
}


bool 
AosJoinDataEngine2::reset(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosJoinDataEngine2::procSimple(AosRundata *rdata, void *data)
{
	memset(mInput_records, 0, sizeof(AosDataRecordObj*[256]));               

	Input lhs_input, rhs_input;

	bool rslt;
	rslt = getNextRecord(rdata, lhs_input, mLhsDataset);
	if (!rslt) return procFailed(__FILE__, __LINE__);
	mInput_records[0] = lhs_input.record;

	rslt  = getNextRecord(rdata, rhs_input, mRhsDataset);
	if (!rslt) return procFailed(__FILE__, __LINE__);
	mInput_records[1] = rhs_input.record;

	while(1)
	{

		rslt = procDataSimple(lhs_input, rhs_input, rdata);
		aos_assert_r(rslt, false);

		if (mDataProcStatus == AosDataProcStatus::eExit)
			break;
	}
	return true;
}


bool
AosJoinDataEngine2::procDataSimple(
				Input &lhs_input, 
				Input &rhs_input, 
				AosRundata* rdata)
{
	bool rslt;
	mDataProcStatus = mProc->procData(rdata, mInput_records, NULL);

	switch (mDataProcStatus)
	{
	case AosDataProcStatus::eLT:
		rslt = getNextRecord(rdata, lhs_input, mLhsDataset);
		if (!rslt) return procFailed(__FILE__, __LINE__);
		mInput_records[0] = lhs_input.record;
		 break;

	case AosDataProcStatus::eGT:
		rslt  = getNextRecord(rdata, rhs_input, mRhsDataset);
		if (!rslt) return procFailed(__FILE__, __LINE__);
		mInput_records[1] = rhs_input.record;
		 break;

	case AosDataProcStatus::eEQ:
	case AosDataProcStatus::eContinue:
		rslt = getNextRecord(rdata, lhs_input, mLhsDataset);
		if (!rslt) return procFailed(__FILE__, __LINE__);
		mInput_records[0] = lhs_input.record;

		rslt  = getNextRecord(rdata, rhs_input, mRhsDataset);
		if (!rslt) return procFailed(__FILE__, __LINE__);
		mInput_records[1] = rhs_input.record;

		 break;

	case AosDataProcStatus::eExit:
		 return true;
	
	default:
		 return procFailed(__FILE__, __LINE__);
	}

	return true;
}

bool
AosJoinDataEngine2::getNextRecord(
					AosRundata* rdata,
					Input &input,
					const AosDatasetObjPtr &dataset)
{
	aos_assert_r(dataset, false);
	input.finished = false;

	bool rslt;
	if (!input.record_set)
	{
		rslt = dataset->nextRecordset(rdata, input.record_set);
		aos_assert_r(rslt, false);
		if (!input.record_set || input.record_set->size() <= 0)
		{
			input.finished = true;
			return true;
		}
		rslt = input.record_set->nextRecord(rdata, input.record);
	}
	else
	{
		rslt = input.record_set->nextRecord(rdata, input.record);
	}

	if (!input.record)
	{
		rslt = dataset->nextRecordset(rdata, input.record_set);
		aos_assert_r(rslt, false);
		if (!input.record_set || input.record_set->size() <= 0)
		{
			input.finished = true;
			return true;
		}
		rslt = input.record_set->nextRecord(rdata, input.record);
	}

	aos_assert_r(rslt, false);

	if (!input.record)
	{
		input.finished = true;
		return true;
	}

	return true;
}


bool
AosJoinDataEngine2::getRecordset(
					AosRundata* rdata,
					Input &input,
					const AosDatasetObjPtr &dataset,
					std::string type)
{
	aos_assert_r(dataset, false);
	input.record = 0;
	input.finished = false;
	input.record_set = 0;
	bool rslt = dataset->nextRecordset(rdata, input.record_set);
	aos_assert_r(rslt, false);

	if (!input.record_set || input.record_set->size() <= 0)
	{
		input.finished = true;
	}
	return true;
}



AosActionObjPtr
AosJoinDataEngine2::clone(
	const AosXmlTagPtr &def, 
	const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosJoinDataEngine2(false);
	}
	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosJoinDataEngine2::procFailed(const char *fname, const int line)
{
    mEngineStatus = eProcFailed;
    mEndTime = OmnGetSecond();
    OmnAlarm << "error! fname:" << fname
        << "; line:" << line << enderr;
    return false;
}


bool
AosJoinDataEngine2::runFinished(const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
    // means this thrd run has finished.
    mEngineStatus = eRunFinished;

    AosActionObjPtr thisptr(this, false);
    task->actionRunFinished(rdata, thisptr);

    OmnScreen << "One DataEngine thrd run finish." << endl;
    return true;
}


bool
AosJoinDataEngine2::tryFinish()
{
    bool cont = false;

    mLock->lock();
    switch(mEngineStatus)
    {
    case eRunFinished:
        cont = true;
        mEngineStatus = eFinished;
        break;

    case eIdle:
    case eStart:
    case eRunning:
    case eProcFailed:
        OmnAlarm << "error!" << enderr;
        break;

    case eFinished:
    default:
        break;
    }
    mLock->unlock();
    return cont;
}


OmnString
AosJoinDataEngine2::reportStatus()
{
    OmnString docstr;
    docstr << "<dataengine_report "
           << AOSTAG_STATUS << "=\"" << mEngineStatus << "\" "
           << AOSTAG_START_TIME << "=\"" << mStartTime << "\" ";
    if(mEndTime)
    {
        docstr << AOSTAG_END_TIME << "=\"" << mEndTime << "\" "
           << AOSTAG_TIME_TAKEN << "=\"" << mEndTime - mStartTime << "\" ";
    }

    docstr << AOSTAG_TIME_TAKEN << "=\"" << mEndTime - mStartTime << "\" "
           << ">"
           << "<records_proc_info "
           << " proc_num:" << mStatistics.mProcNum
           << " proc_entries:" << mStatistics.mEntries
           << " filter_num:" << mStatistics.mFilterNum
           << " data_error_num:" << mStatistics.mDataErrorNum
           << " proc_error_num:" << mStatistics.mProcErrorNum
           << " >"
           << "</dataengine_report>";

    OmnScreen << "report DataEngine status: " << endl << docstr << endl;
    return docstr;
}


AosDataProcObjPtr
AosJoinDataEngine2::createDataProc(
        const AosRundataPtr &rdata,
        const AosXmlTagPtr &proc_conf)
{
    proc_conf->setAttr("version", 1);
    AosDataProcObjPtr proc = AosDataProcObj::createDataProcStatic(
            proc_conf, rdata);
    aos_assert_r(proc, 0);
	proc->setTaskDocid(mTaskDocid);

	vector<AosDataRecordObjPtr> v1, v2, v;
	mLhsDataset->getRecords(v1);
	mRhsDataset->getRecords(v2);

	v = v2;
	v.insert(v.begin(), v1.begin(), v1.end());
	proc->setInputDataRecords(v);

	proc->config(proc_conf, rdata);
    return proc;
}

//for log
void
AosJoinDataEngine2::showDataEngineInfo(
		const char *file,
		const int line,
		const OmnString &action)
{
	report(file, line, action, "JoinDataEngine2");
}

