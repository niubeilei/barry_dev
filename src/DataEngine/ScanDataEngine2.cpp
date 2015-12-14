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
// 2013/12/14 Copied from ImportDataCtlrNorm.cpp by Chen Ding
// Modifyed by Ketty 2013/12/19
////////////////////////////////////////////////////////////////////////////
#include "DataEngine/ScanDataEngine2.h"

#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/Ptrs.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/RecordsetObj.h"

#include "TaskMgr/Task.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadMgr.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Debug/Debug.h"



AosScanDataEngine2::AosScanDataEngine2(const bool flag)
:
AosTaskAction(AOSACTTYPE_DATAENGINE_SCAN2, AosActionType::eScanDataEngine2, flag),
mLock(OmnNew OmnMutex()),
mEngineStatus(eIdle),
mStartTime(0),
mEndTime(0)
{
}


AosScanDataEngine2::~AosScanDataEngine2()
{
}


bool 
AosScanDataEngine2::config(
		const u64 task_docid,      
		const AosXmlTagPtr &conf,               
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(task_docid, rdata);
	if(!task)	procFailed(__FILE__, __LINE__);
	if(!conf)
	{
		AosSetEntityError(rdata, "miss_config", "ScanDataEngine2", "Root conf") << conf << enderr;
		return procFailed(__FILE__, __LINE__);
	}
	
	mTaskDocid = task_docid;

	bool rslt = configDataset(task, conf, rdata);
	if(!rslt)	return procFailed(__FILE__, __LINE__);

	rslt = configEngineProc(rdata, conf);
	if(!rslt)	return procFailed(__FILE__, __LINE__);

	rslt = configDataCollector(rdata, conf);
	if(!rslt)	return procFailed(__FILE__, __LINE__);
	
	mStatistics.clear();
	return true;
}


bool 
AosScanDataEngine2::initAction(                                         
		const AosTaskObjPtr &task,                           
		const AosXmlTagPtr &sdoc,                            
		const AosRundataPtr &rdata)                         
{
	mStartTime = OmnGetSecond();
	OmnScreen << "ScanDataEngine2 Start!, start time:" << mStartTime << endl;
	showDataEngineInfo(__FILE__, __LINE__, "start");

	bool rslt = notifyAssemblerStart(rdata);
	if (!rslt) return procFailed(__FILE__, __LINE__);

	rslt = notifyDatasetStart(rdata);
	if (!rslt) return procFailed(__FILE__, __LINE__);

	rslt = notifyDataProcStart(rdata);
	if (!rslt) return procFailed(__FILE__, __LINE__);
	return true;
}


void AosScanDataEngine2::onThreadInit(const AosRundataPtr &, void **data)
{
	DataEnginEnv *env = OmnNew DataEnginEnv();	
	env->recordset = 0;
	*data = env;
}

bool 
AosScanDataEngine2::runAction(const AosRundataPtr &rdata, void *data)              
{
	AosJimoPtr jimo;
	AosRundata* rdata_raw = rdata.getPtr();
	AosRecordsetObjPtr &record_set = ((DataEnginEnv*)data)->recordset;
	bool rslt;
	if (record_set)
	{
		rslt = mDataset->nextRecordset(rdata, record_set);
		if (!rslt) return procFailed(__FILE__, __LINE__);

		if (AosRecordsetObj::checkEmpty(record_set)) 
			return runFinished(rdata);
	}
	else
	{
		rslt = mDataset->nextRecordset(rdata, record_set);
		if (!rslt) return procFailed(__FILE__, __LINE__);

		if (AosRecordsetObj::checkEmpty(record_set)) 
			return runFinished(rdata);

		mLock->lock();
		vector<AosDataRecordObjPtr> records = record_set->getRecords();
		vector<AosDataProcObjPtr> procs;
		vector<AosDataProcObj*> procs_raw;
		vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > > procs_tmp;
		vector< pair<AosDataProcObj*, vector<AosDataProcObj*> > > procs_tmp_raw;
		for (size_t i = 0; i<records.size(); i++)
		{
			OmnString name = records[i]->getRecordName();

			procs_tmp = mProcGroups[name];
			procs_tmp_raw = mRawProcGroups[name];
			for (size_t j=0; j<procs_tmp.size(); j++)
			{
				aos_assert_rl(procs_tmp[j].first, mLock, false);

				AosDataProcObjPtr proc = procs_tmp[j].first->cloneProc();
				aos_assert_rl(proc, mLock, false);
				procs_tmp[j].second.push_back(proc);
				procs_tmp_raw[j].second.push_back(proc.getPtr());
				procs.push_back(proc);
				procs_raw.push_back(proc.getPtr());
			}
			mProcGroups[name] = procs_tmp;
			mRawProcGroups[name] = procs_tmp_raw;
			records[i]->bindDataProcs(procs_raw);
		}
		mLock->unlock();
	}

	AosRecordsetObj * record_set_raw = record_set.getPtr();
	
	AosScanDataEngine2::Status exitStatus;
	exitStatus = procRecordSet(rdata_raw, record_set_raw, data);

	if (exitStatus == eRunFinished) return runFinished(rdata);
	if (exitStatus == eProcFailed) return procFailed(__FILE__, __LINE__);
	if (exitStatus == eStop) 
	{
		stop(rdata, record_set);
		return runFinished(rdata);
	}
	return true;
}


void AosScanDataEngine2::onThreadExit(const AosRundataPtr &, void *data)
{
	OmnDelete (DataEnginEnv*)data;
}


bool 
AosScanDataEngine2::finishedAction(const AosRundataPtr &rdata)         
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	// this func is called by Task.
	// means all thrd has proc finished.
	AosActionObjPtr this_ptr(this, false);

	bool cont = tryFinish();
	if(!cont)
	{
		if(mEngineStatus == eProcFailed)	
		{
			OmnAlarm << enderr;
			return task->actionFailed(this_ptr, rdata);
		}
		OmnAlarm << enderr;
		return false;
	}

	mEndTime = OmnGetSecond();
	OmnScreen << "All ScanDataEngine2 finished:" << endl;
	reportStatus();

	bool rslt = notifyDataProcFinish(rdata);
	aos_assert_r(rslt, false);
	
	rslt = notifyDatasetFinish(rdata);
	aos_assert_r(rslt, false);
	
	rslt = notifyAssemblerFinish(rdata);
	aos_assert_r(rslt, false);

	task->actionFinished(this_ptr, rdata);
	//if (!task->isService())
	clean();

	showDataEngineInfo(__FILE__, __LINE__, "finish");
	return true;
}


bool
AosScanDataEngine2::tryFinish()
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
AosScanDataEngine2::reportStatus()                                     
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


bool
AosScanDataEngine2::notifyDatasetFinish(const AosRundataPtr &rdata)
{
	bool rslt = mDataset->sendFinish(rdata);
	aos_assert_r(rslt, false);	
	return true;
}


bool
AosScanDataEngine2::notifyAssemblerFinish(const AosRundataPtr &rdata)
{
	bool rslt;
	AosRundata *rdata_raw = rdata.getPtrNoLock();
	for(u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		rslt = mRawAssemblers[i]->sendFinish(rdata_raw);	
		if (!rslt) return procFailed(__FILE__, __LINE__);
	}

	for(u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		rslt = mRawAssemblers[i]->waitFinish(rdata_raw);	
		if (!rslt) return procFailed(__FILE__, __LINE__);
	}

	return true;

}


bool
AosScanDataEngine2::notifyDataProcFinish(const AosRundataPtr &rdata)
{
	map<OmnString, 
		vector<
			pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> >
			> >::iterator itr = mProcGroups.begin();

	for(; itr!= mProcGroups.end(); ++itr)
	{
		vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > >& procs = itr->second;
		for (size_t i=0; i<procs.size(); i++)
		{
			procs[i].first->finish(procs[i].second, rdata);
		}
	}
	return true;
}


bool
AosScanDataEngine2::clean()
{
	mAssemblers.clear();
	mRawAssemblers.clear();
	mStatistics.clear();
	return true;	
}


bool AosScanDataEngine2::reset(const AosRundataPtr &rdata)                  
{
	return true;
}

bool
AosScanDataEngine2::configDataCollector(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	bool rslt = false;
	AosXmlTagPtr outputXml;
	vector<boost::shared_ptr<Output> > outputs;
	map<OmnString, vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > > >::iterator itr = mProcGroups.begin();
	for(; itr != mProcGroups.end(); ++itr)
	{
		vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > >& procs = itr->second;
		for (size_t i = 0; i < procs.size(); i++)
		{
			outputs = procs[i].first->getOutputs();
			for (size_t i = 0; i < outputs.size(); i++)
			{
				outputXml = AosXmlParser::parse(outputs[i]->getConfig() AosMemoryCheckerArgs);
				aos_assert_r(outputXml, false);
				rslt = remoteCreateDataCol(outputXml, rdata);
				aos_assert_r(rslt, false);
OmnScreen << outputXml->toString() << endl;
			}
		}
	}

	return true;
}


bool
AosScanDataEngine2::configDataset(
        const AosTaskObjPtr &task,
        const AosXmlTagPtr &conf,
        const AosRundataPtr &rdata)
{
    aos_assert_r(task, false);
    map<OmnString, AosDatasetObjPtr> set_map = task->getDatasets();
    aos_assert_r(set_map.size() > 0, false);

    //it assmes that the first dataset will be used
    //by the DataEngine action or map task???
    map<OmnString, AosDatasetObjPtr>::iterator itr = set_map.begin();
    aos_assert_r(itr != set_map.end(), false);
    mDataset = itr->second;
    aos_assert_r(mDataset, false);

	vector<AosDataRecordObjPtr> v;
	mDataset->getRecords(v);
	vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > > procs;
	vector< pair<AosDataProcObj*, vector<AosDataProcObj*> > > procs_raw;
	for (size_t i=0; i<v.size(); i++)
	{
		mProcGroups.insert(make_pair(v[i]->getRecordName(), procs));
		mRawProcGroups.insert(make_pair(v[i]->getRecordName(), procs_raw));
	}

    return true;
}


bool
AosScanDataEngine2::configEngineProc(
        const AosRundataPtr &rdata,
        const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
    AosDataProcObjPtr proc;
    AosXmlTagPtr p_conf = conf->getFirstChild(true);
	vector<AosDataProcObjPtr> v;
	vector<AosDataProcObj*> v_raw;
	std::pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > dp;
	std::pair<AosDataProcObj*, vector<AosDataProcObj*> > dp_raw;

    while(p_conf)
    {
		OmnString input_rcd_name = p_conf->getAttrStr("zky_input_record_name", "");
		if (input_rcd_name == "")
		{
			proc = createDataProc(rdata, p_conf);
			aos_assert_r(proc, false);

			//v.push_back(proc);
			dp = make_pair(proc, v);
			dp_raw = make_pair(proc.getPtr(), v_raw);
			mProcGroups.begin()->second.push_back(dp);
			mRawProcGroups.begin()->second.push_back(dp_raw);

			vector<boost::shared_ptr<Output> > outputs = proc->getOutputs();
			for (size_t i = 0; i < outputs.size(); i++)
			{
				AosDataAssemblerObjPtr asmobj = outputs[i]->getAssembler();
				aos_assert_r(asmobj, false);
				mAssemblers.push_back(asmobj);
				mRawAssemblers.push_back(asmobj.getPtr());
			}
		}
		else
		{
			AosXmlTagPtr gp_conf = p_conf->getFirstChild(true);
			while(gp_conf)
			{
				proc = createDataProc(rdata, gp_conf);
				aos_assert_r(proc, false);

				dp = make_pair(proc, v);
				dp_raw = make_pair(proc.getPtr(), v_raw);
				mProcGroups.begin()->second.push_back(dp);
				mRawProcGroups.begin()->second.push_back(dp_raw);

				vector<boost::shared_ptr<Output> > outputs = proc->getOutputs();
				for (size_t i = 0; i < outputs.size(); i++)
				{
					AosDataAssemblerObjPtr asmobj = outputs[i]->getAssembler();
					mAssemblers.push_back(asmobj);
					mRawAssemblers.push_back(asmobj.getPtr());
				}
				gp_conf = p_conf->getNextChild();
			}
		}
		p_conf = conf->getNextChild();
    }
    return true;
}


bool
AosScanDataEngine2::notifyAssemblerStart(const AosRundataPtr &rdata)
{
	bool rslt;
	AosRundata *rdata_raw = rdata.getPtrNoLock();
	for(u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		mRawAssemblers[i]->setTargetReporter(this);
		rslt = mRawAssemblers[i]->sendStart(rdata_raw);
		if (!rslt) return procFailed(__FILE__, __LINE__);
	}
	return true;
}


bool
AosScanDataEngine2::notifyDatasetStart(const AosRundataPtr &rdata)
{
	bool rslt = mDataset->sendStart(rdata);
	aos_assert_r(rslt, false);	
	return true;
}


bool
AosScanDataEngine2::notifyDataProcStart(const AosRundataPtr &rdata)
{
	map<OmnString, 
		vector< 
			pair<AosDataProcObj*, vector<AosDataProcObj*> > >
			>::iterator itr = mRawProcGroups.begin();

	for(; itr!= mRawProcGroups.end(); ++itr)
	{
		vector< pair<AosDataProcObj*, vector<AosDataProcObj*> > >& procs = itr->second;
		for (size_t i=0; i<procs.size(); i++)
		{
			procs[i].first->setTargetReporter(this);
			procs[i].first->start(rdata);
		}
	}
	return true;
}


AosScanDataEngine2::Status
AosScanDataEngine2::procRecordSet(
		AosRundata *rdata_raw,
		AosRecordsetObj *record_set_raw,
		void *data)
{
	bool rslt;
	AosDataRecordObj *input_record  = 0;
	while (1)
	{
		rslt = record_set_raw->nextRecord(rdata_raw, input_record);
		if (!rslt) return eProcFailed;

		if(!input_record)   break;
		
		Status exitStatu;
		exitStatu = processRecord(rdata_raw, input_record, data);
		if(exitStatu != eRunning) return exitStatu;
	}
	return eFinished;
}


AosScanDataEngine2::Status
AosScanDataEngine2::processRecord(
		AosRundata *rdata_raw,
		AosDataRecordObj *input_record, 
		void *data)
{
	vector<AosDataProcObj*> &procs = input_record->getBindDataProcs();
	
	AosDataProcStatus::E status;
	u32 ss = procs.size();
	for(u32 i=0; i<ss; i++)
	{
		status = procs[i]->procData(rdata_raw, &input_record, NULL);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 break;
		
		case AosDataProcStatus::eExit:
			 return eRunFinished;
		
		case AosDataProcStatus::eError:
		 	 logProcError(rdata_raw, input_record);
			 OmnAlarm << rdata_raw->getErrmsg() << enderr;
		     return eRunning;

		case AosDataProcStatus::eStop:
			 return eStop;

		default:
		 	 logProcError(rdata_raw, input_record);
		 	 AosSetError(rdata_raw, "internal error") << status << enderr;
			 OmnAlarm << rdata_raw->getErrmsg() << enderr;
			return eProcFailed;
		}
	}
	return eRunning;
}


bool
AosScanDataEngine2::procFailed(const char *fname, const int line)
{
	mEngineStatus = eProcFailed;
	mEndTime = OmnGetSecond();
	OmnAlarm << "error! fname:" << fname 
		<< "; line:" << line << enderr;
	return false;
}


bool
AosScanDataEngine2::runFinished(const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	mEngineStatus = eRunFinished;

	AosActionObjPtr thisptr(this, false);
	task->actionRunFinished(rdata, thisptr);

	mEndTime = OmnGetSecond();
	
	OmnScreen << "One DataEngine thrd run finish." << endl;
	return true;
}


bool
AosScanDataEngine2::stop(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &record_set)
{
	mDataset->stop(rdata, record_set);
	return true;
}


void
AosScanDataEngine2::logProcEntry(const u64 num)
{
	mLock->lock();
	mStatistics.mEntries += num;
	mLock->unlock();
}


AosDataProcObjPtr
AosScanDataEngine2::createDataProc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &proc_conf)
{
	proc_conf->setAttr("version", 1);
	AosDataProcObjPtr proc = AosDataProcObj::createDataProcStatic(
			proc_conf, rdata);
	aos_assert_r(proc, NULL);

	proc->setTaskDocid(mTaskDocid);

	vector<AosDataRecordObjPtr> v;
	mDataset->getRecords(v);
	proc->setInputDataRecords(v);

	bool rslt = proc->config(proc_conf, rdata);
	aos_assert_r(rslt, NULL);
	return proc;
}


void
AosScanDataEngine2::logFiltered(
		AosRundata * rdata_raw,
		AosDataRecordObj *input_record)
{
	// log finish later.	
	mLock->lock();
	mStatistics.mFilterNum++;
	mLock->unlock();
	
	OmnScreen << "filter record:" << input_record->getRecordName() << endl;
}


void
AosScanDataEngine2::logDataError(
		AosRundata * rdata_raw,
		AosDataRecordObj *input_record)
{
	// log finish later.	
	mLock->lock();
	mStatistics.mDataErrorNum++;
	mLock->unlock();
	
	OmnScreen << "data error record:" << input_record->getRecordName() << endl;
}


void
AosScanDataEngine2::logProcError(
		AosRundata * rdata_raw,
		AosDataRecordObj *input_record)
{
	// log finish later.	
	mLock->lock();
	mStatistics.mProcErrorNum++;
	mLock->unlock();
	
	OmnScreen << "proc error record:" << input_record->getRecordName() << endl;
}


void
AosScanDataEngine2::logProcOne(
		AosRundata * rdata_raw,
		AosDataRecordObj *input_record)
{
	mLock->lock();
	mStatistics.mProcNum++;
	mLock->unlock();
}



AosActionObjPtr
AosScanDataEngine2::clone(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata) const 
{
	aos_assert_r(conf, 0);

	try
	{
		 return OmnNew AosScanDataEngine2(false);
	}
	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

int
AosScanDataEngine2::getMaxThreads()
{
	int numThreads;
	map<OmnString, 
		vector<
			pair<AosDataProcObj*, vector<AosDataProcObj*> >
			> >::iterator itr = mRawProcGroups.begin();

	for(; itr!= mRawProcGroups.end(); ++itr)
	{
		vector< pair<AosDataProcObj*, vector<AosDataProcObj*> > >& procs = itr->second;
		for (size_t i=0; i<procs.size(); i++)
		{
			numThreads = procs[i].first->getMaxThreads();
			if(numThreads == 1) return 1;
		}
	}
	return numThreads;
}

//for log
void
AosScanDataEngine2::showDataEngineInfo(
		const char *file,
		const int line,
		const OmnString &action)
{
	report(file, line, action, "ScanDataEngine2");
}

