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
//
// Modification History:
// 05/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Job/Testers/JobTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Job/JobMgr.h"
#include "Job/JobNames.h"
#include "Job/JobSchedulerId.h"
#include "Job/JobSplitterNames.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEModules/ObjMgr.h"
#include "TaskMgr/TaskNames.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosJobTester::AosJobTester()
{
	mName = "JobTester";
}


bool 
AosJobTester::start()
{
	cout << "Start Job Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosJobTester::basicTest()
{
	// It converts the data defined in config
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);

	//AosXmlTagPtr tag = config->getFirstChild("raw_data");
	//aos_assert_r(tag, false);

	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setCid("50003");
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(100, rdata));
	bool rslt = createJobdoc(rdata);
	
	OmnString args = "cmdid=runjob,objid=job_t1";

	rslt = AosJobMgr::getSelf()->runCommand(args, 0, rdata);
	aos_assert_r(rslt, false);

	OmnSleep(10);
	
	args = "cmdid=aborttask,objid=job_t1,task_seqno=0";
	rslt = AosJobMgr::getSelf()->runCommand(args, 0, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosJobTester::createJobdoc(const AosRundataPtr &rdata)
{
	OmnString jobIdStr = randomGetJobId();		
	OmnString taskStr = randomGetTaskStr();
	OmnString jobSpliterStr = randomGetJobSplitter();
	OmnString jobSchedulerStr= randomGetJobScheduler();

	// ---temp-----
	OmnString ctnr_str = "<ctnr ";
	ctnr_str << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< AOSTAG_OBJID << "=\"job_ctnr\" "
			<< AOSTAG_PUBLIC_DOC << "=\"true\" "
			<< AOSTAG_CTNR_PUBLIC << "=\"true\" " 
			<< "/>";
	AosXmlTagPtr ctnr = AosCreateDoc(ctnr_str, true, rdata);
	aos_assert_r(ctnr, false);
	
	OmnString log_ctnr_str = "<ctnr ";
	log_ctnr_str << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< AOSTAG_OBJID << "=\"log_ctnr\" "
			<< AOSTAG_PUBLIC_DOC << "=\"true\" "
			<< AOSTAG_CTNR_PUBLIC << "=\"true\" " 
			<< "/>";
	ctnr = AosCreateDoc(log_ctnr_str, true, rdata);
	aos_assert_r(ctnr, false);
	


	OmnString job_str = "<jobdoc ";
	job_str << AOSTAG_JOBID << "=\"" << jobIdStr << "\" "
			<< AOSTAG_OBJID << "=\"job_t1\" "
			 << AOSTAG_CONTAINER_OBJID << "=\"job_ctnr\" " 
			 << AOSTAG_ISPUBLIC	<< "=\"true\" "
			 << AOSTAG_LOG_CONTAINER << "=\"log_ctnr\" "
			 << AOSTAG_PUBLIC_DOC << "=\"true\" "
			 << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			 //<< AOSTAG_JOBSCHEDULER_ID << "=\"" << AOSJOBSCHEDULER_NORM << "\" " 
			 << ">"
			 << jobSchedulerStr
			 << jobSpliterStr
			 << taskStr
			 << "</jobdoc>";
	
	AosXmlTagPtr jobdoc = AosCreateDoc(job_str, true, rdata);
	aos_assert_r(jobdoc, false);

	// ----temp----
	return true;
}


OmnString
AosJobTester::randomGetJobId()
{
	return AOSJOBNAME_ROOTJOB;
}


OmnString
AosJobTester::randomGetTaskId()
{
	return AOSTASKNAME_ACTION;
}


OmnString
AosJobTester::randomGetSpliterId()
{
	return AOSJOBSPLITTER_FILE;
}


OmnString
AosJobTester::randomGetTaskStr()
{
	OmnString task_str;
	OmnString taskid_str = randomGetTaskId();		

	task_str << "<zky_taskdef "
			<< AOSTAG_TYPE << "=\"" << taskid_str << "\" "
			<< " >"
			<< "</zky_taskdef>";
	return task_str;
}


OmnString
AosJobTester::randomGetJobSplitter()
{
	OmnString spliter_str;
	OmnString spliterid_str = randomGetSpliterId();

	spliter_str << "<splitter "
			<< AOSTAG_JOBSPLITTER_ID << "=\"" << spliterid_str << "\" "
			<< AOSTAG_FILENAME << "=\"/home/ketty/AOS/src/Job/Testers/SplitData/aa.txt\" "
			<< "record_size=\"200\" "
			<< "splittype=\"evenly_distribute\" "
			<< " >"
			<< "</splitter>";
	return spliter_str;
}


OmnString
AosJobTester::randomGetJobScheduler()
{
	OmnString scheduler_str;
	scheduler_str << "<scheduler "
			<< AOSTAG_JOBSCHEDULER_ID << "=\"" << AOSJOBSCHEDULER_NORM << "\" " 
			<< " >"
			<< "</scheduler>";
	return scheduler_str;
}
