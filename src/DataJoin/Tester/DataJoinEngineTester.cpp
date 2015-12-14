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
// 2012/11/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/Tester/DataJoinEngineTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "IILMgr/IILMgr.h"
#include "Job/Job.h"
#include "Job/JobMgr.h"
#include "Porting/Sleep.h"
#include "QueryRslt/QueryRslt.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"

#include <vector>
using namespace std;

static int sgNumIILs = 2;
static int sgMaxNumIILs = 4;
static int sgTries = 0;
static int sgTotalTries = 1000;
static OmnString sgKeyPrefix = "test";
static int sgTotalIILEntry = 200;
static int sgTotalEntry = 0;

map<OmnString, int> sgKeys[4];
map<OmnString, int> sgRslt;

static OmnString sgJobDocObjid;


AosDataJoinEngineTester::AosDataJoinEngineTester()
{
	mName = "DataJoinEngineTester";
}


AosDataJoinEngineTester::~AosDataJoinEngineTester()
{
}


bool 
AosDataJoinEngineTester::start()
{
	OmnScreen << "Start DataJoinEngine Tester" << endl;
	
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setUserid(307);

	bool rslt;
	AosXmlTagPtr job_doc;
	OmnString job_status;
	OmnString args;
	while(sgTries < sgTotalTries)
	{
		system("clear");
		sgTries++;
		OmnScreen << "trie times:" << sgTries << endl;
		sgNumIILs = rand() % (sgMaxNumIILs - 1) + 2;

		rslt = createJobDoc(rdata);
		aos_assert_r(rslt, false);

		rslt = createIILEntry(rdata);
		aos_assert_r(rslt, false);

		args = "objid=";
		args << sgJobDocObjid;
		rslt = AosJobMgr::getSelf()->runJob(args, 0, rdata);
		aos_assert_r(rslt, false);

		while(1)
		{
			job_doc = AosGetDocByObjid(sgJobDocObjid, rdata); 
			aos_assert_r(job_doc, false);

			job_status = job_doc->getAttrStr(AOSTAG_STATUS, AOSJOBSTATUS_IDLE);
			if(job_status == AOSJOBSTATUS_SUCCESS)
			{
				rslt = checkRslt(rdata);
				aos_assert_r(rslt, false);
				break;
			}
			OmnSleep(10);
		}
	}
	return true;
}


bool
AosDataJoinEngineTester::createJobDoc(const AosRundataPtr &rdata)
{
	OmnString zky_datacolid;
	zky_datacolid << "testiil_001_" << sgTries;
	
	OmnString task_sdoc_str;
	OmnString task_doc_objid = "data_join_test_task_";
	task_doc_objid << sgTries;

	task_sdoc_str << "<sdoc zky_objid=\"" << task_doc_objid << "\" zky_pctrs=\"sdoc_unicom_doc\" "
				  << 		"zky_starttype=\"auto\" type=\"task_action\" "
				  <<		"zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
				  << 	"<actions>"
				  <<		"<action zky_type=\"datajoin\">"
				  <<			"<datajoin type=\"engine\" splittype=\"key\" zky_needsplit=\"false\">"
				  <<				"<zky_iilscanners>";
	for(int i=0; i<sgNumIILs; i++)
	{
		OmnString iilname = "_zt19_iil00";
		iilname << i << "_" << sgTries;
		task_sdoc_str <<				"<iilscanner zky_iilname=\"" << iilname << "\" zky_matchtype=\"key\" "
					  <<						"zky_ignorematcherr=\"false\" pagesize=\"100000\" "
					  <<						"zky_nomatch_reject=\"true\">"
					  <<					"<zky_valuecond zky_value_type=\"str\" zky_opr=\"an\">"
					  <<						"<zky_value1>*</zky_value1>"
					  <<					"</zky_valuecond>"
					  <<					"<zky_selectors>";
		if(i == 0) task_sdoc_str <<				"<selector zky_entryselector=\"first\" zky_value_type=\"key\" zky_fieldidx=\"0\" />";
		task_sdoc_str << 					"</zky_selectors>"
					  <<				"</iilscanner>";
	}
	task_sdoc_str <<				"</zky_iilscanners>"
				  <<				"<record type=\"variable\" zky_sep=\"$\"/>"
				  <<				"<zky_iilassmblr zky_datacolid=\"" << zky_datacolid << "\" />"
				  <<			"</datajoin>"
				  <<		"</action>"
				  <<	"</actions>"
				  << "</sdoc>";
	
	AosXmlParser parser;
	AosXmlTagPtr task_tag = parser.parse(task_sdoc_str, "" AosMemoryCheckerArgs);
	aos_assert_r(task_tag, false);

	AosXmlTagPtr task_doc = AosCreateDoc(task_tag, rdata);
	aos_assert_r(task_doc, false);

	task_doc_objid = task_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(task_doc_objid != "", false);
	
	OmnString log_ctnr_str;
	OmnString log_ctnr_objid = "log_ctnr_";
	log_ctnr_objid << sgTries;
	log_ctnr_str << "<doc zky_objid=\"" << log_ctnr_objid << "\" zky_otype=\"zky_ctnr\" "
				 << 		"zky_public_doc=\"true\" zky_public_ctnr=\"true\" />";

	AosXmlTagPtr log_ctnr_tag = parser.parse(log_ctnr_str, "" AosMemoryCheckerArgs);
	aos_assert_r(log_ctnr_tag, false);

	AosXmlTagPtr log_ctnr_doc = AosCreateDoc(log_ctnr_tag, rdata);
	aos_assert_r(log_ctnr_doc, false);

	log_ctnr_objid = log_ctnr_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(log_ctnr_objid != "", false);
	
	OmnString job_sdoc_str;
	OmnString job_sdoc_objid = "data_join_test_job_";
	job_sdoc_objid << sgTries;
	job_sdoc_str << "<sdoc zky_jobid=\"rootjob\" zky_objid=\"" << job_sdoc_objid << "\" "
				 << 		"zky_ctnrobjid=\"task_ctnr\" zky_ispublic=\"true\" "
				 << 		"log_ctnr=\"" << log_ctnr_objid << "\" zky_pctrs=\"sdoc_unicom_doc\" "
				 << 		"zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
				 << 	"<scheduler zky_numslots=\"1\" zky_jobschedulerid=\"norm\" />"
				 << 	"<zky_tokens/>"
				 << 	"<data_collectors>"
				 << 		"<data_collector zky_datacolid=\"" << zky_datacolid << "\" zky_type=\"iil\">"
				 << 			"<iilasm type=\"strattr\" zky_iilname=\"_zt19_" << zky_datacolid << "\" zky_length=\"30\"/>"
				 << 		"</data_collector>"
				 << 	"</data_collectors>"
				 << 	"<splitter zky_isready=\"true\" zky_jobsplitterid=\"file\" splittype=\"num_files\">"
				 << 		"<files />"
				 << 	"</splitter>"
				 << 	"<tasks>"
				 << 		"<task zky_tasksobjid=\"" << task_doc_objid << "\" zky_taskid=\"join_001\" type=\"norm\" zky_physicalid=\"0\" />"
				 << 	"</tasks>"
				 << "</sdoc>";
	
	AosXmlTagPtr job_sdoc_tag = parser.parse(job_sdoc_str, "" AosMemoryCheckerArgs);
	aos_assert_r(job_sdoc_tag, false);

	AosXmlTagPtr job_sdoc = AosCreateDoc(job_sdoc_tag, rdata);
	aos_assert_r(job_sdoc, false);

	job_sdoc_objid = job_sdoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(job_sdoc_objid != "", false);
	
	OmnString job_doc_str;
	sgJobDocObjid = "data_join_test_job_doc_";
	sgJobDocObjid << sgTries;
	job_doc_str << "<doc zky_objid=\"" << sgJobDocObjid << "\" zky_sdoc_objid=\"" << job_sdoc_objid << "\" "
				<< 		"zky_jobid=\"rootjob\" zky_otype=\"obj\" "
				<< 		"zky_public_doc=\"true\" zky_public_ctnr=\"true\" />";

	AosXmlTagPtr job_doc_tag = parser.parse(job_doc_str, "" AosMemoryCheckerArgs);
	aos_assert_r(job_doc_tag, false);

	AosXmlTagPtr job_doc = AosCreateDoc(job_doc_tag, rdata);
	aos_assert_r(job_doc, false);
	
	sgJobDocObjid = job_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(sgJobDocObjid != "", false);

	return true;
}


bool
AosDataJoinEngineTester::createIILEntry(const AosRundataPtr &rdata)
{

	for(int i=0; i<sgMaxNumIILs; i++)
	{
		sgKeys[i].clear();
	}

	OmnString iilname;
	bool unique;
	bool rslt;
	OmnString key;
	int num;
	u64 iilid;
	AosIILType iiltype;
	AosIILPtr iil;
	for(int i=0; i<sgNumIILs; i++)
	{
		iilname = "_zt19_iil00";
		iilname << i << "_" << sgTries;

		iilid = AosIILMgr::getSelf()->getIILID(iilname, true, rdata);
		rslt = AosIILMgr::getSelf()->createIILPublic1(
			iilid, 100, eAosIILType_Str, false, false, rdata);
		aos_assert_r(rslt, false);
		
		iil = AosIILMgr::getSelf()->loadIILByIDPublic(
			iilid, 100, iiltype, false, false, rdata);
		aos_assert_r(iil, false);
		AosIILStr *striil = (AosIILStr*)iil.getPtr();

		unique = rand() % 2;
		for(int j=0; j<sgTotalIILEntry; j++)
		{
			key = sgKeyPrefix;
			key << j;
			num = 0;
			if(rand() % 100 <= 90)
			{
				rslt = striil->addDocSafe(key, 0, false, false, false, rdata);
				aos_assert_r(rslt, false);
				num++;
			
				if(!unique)
				{
					while(rand() % 100 > 80)
					{
						rslt = striil->addDocSafe(
							key, 0, false, false, false, rdata);
						aos_assert_r(rslt, false);
						num++;
					}
				}
			}
			sgKeys[i][key] = num;
		}
		AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	}
		
	sgRslt.clear();
	sgTotalEntry = 0;
	for(int i=0; i<sgTotalIILEntry; i++)
	{
		key = sgKeyPrefix;
		key << i;

		num = sgKeys[0][key];
		for(int j=1; j<sgNumIILs; j++)
		{
			num *= sgKeys[j][key]; 
		}
		if(num != 0) sgRslt[key] = num;
		sgTotalEntry += num;
	}
	return true;
}


bool
AosDataJoinEngineTester::checkRslt(const AosRundataPtr &rdata)
{
	OmnString iilname;
	iilname << "_zt19_testiil_001_" << sgTries;
	
	AosQueryRsltPtr query_rslt;
	AosQueryContextObjPtr query_context = OmnNew AosQueryContext();
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	query_context->setBlockSize(100000);
		
	AosBuffPtr buff;
	u64 num;
	char * key;
	int len;
	int ss;
	int total = 0;
	bool first = true;
	bool rslt;
	map<OmnString, int>::iterator itr;
	while(first || !query_context->finished())
	{
		first = false;
		query_rslt = OmnNew AosQueryRslt();
		query_rslt->setWithValues(true);
		rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
		aos_assert_r(rslt, false);
	
		buff = query_rslt->getValueBuff();
		num = query_rslt->getNumDocs();
		total += num;
		for(u64 i=0; i<num; i++)
		{
			key = buff->getStr(len);
			OmnString vv(key, len);
			aos_assert_r(vv != "", false);

			itr = sgRslt.find(vv);
			aos_assert_r(itr != sgRslt.end(), false);
			OmnScreen << "find key:" << vv << endl;
			ss = itr->second;
			aos_assert_r(ss > 0, false);
			--ss;
			if(ss == 0)
			{
				sgRslt.erase(vv);
			}
			else
			{
				sgRslt[vv] = ss;
			}
		}
	}
	aos_assert_r(sgTotalEntry == total, false);
	if(sgRslt.size() != 0)
	{
		itr = sgRslt.begin();
		while(itr != sgRslt.end())
		{
			OmnScreen << "key:" << itr->first << ",num:" << itr->second << endl;
			itr++;
		}
		OmnAlarm << "sgRslt.size = " << sgRslt.size() << enderr;
	}

	return true;
}

