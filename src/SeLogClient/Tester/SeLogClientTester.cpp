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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogClient/Tester/SeLogClientTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SEUtil/IILName.h"
#include "IILClient/IILClient.h"
#include "TransUtil/RoundRobin.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/OmnNew.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "DocClient/DocClient.h"
#include "SearchEngine/DocMgr.h"
#include "SeLogClient/Tester/LogReqProc.h"
#include "SeLogClient/Tester/LoginLogTest.h"
#include <list>


AosSeLogClientTester::AosSeLogClientTester()
{
}


AosSeLogClientTester::~AosSeLogClientTester()
{
}


bool 
AosSeLogClientTester::start()
{
	OmnScreen << "Start AosSeLogClient Tester ..." << endl;

	// Siteid is random.
	OmnString siteid = "100";//getSiteId(); 

	// Create a super user.
	AosXmlTagPtr userdoc;
	OmnString passwd, username, ctnr;	
	OmnString requester_passwd = "als;kdlkqewrcvqwtiware346d";
	bool rslt = AosSengAdmin::getSelf()->createSuperUser(requester_passwd, userdoc, siteid);
	aos_assert_r(rslt, false);
	aos_assert_r(userdoc, false);
	AosXmlTagPtr passwdnode = userdoc->getFirstChild("passwd");
	passwd = passwdnode->getNodeText();
	aos_assert_r(passwd != "", false);
	username = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_r(username != "", false);
	ctnr = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(ctnr != "", false);
	
	// user super user to login.
	OmnString ssid;
	u64 userid;
	rslt = AosSengAdmin::getSelf()->login(username, 
			passwd, ctnr, ssid, userid, userdoc, siteid, "");
	aos_assert_r(rslt, false);
	mSsid = ssid;
	
	// start thread
	// OmnThreadedObjPtr thisPtr(this, false);
	// OmnThreadPtr thread;
	// u32 threadNum = 1;
	// for(u32 i=0; i<threadNum; i++)
	// {
	// 	thread = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
	// 	thread->start();
	// }
	// 	
	// while(1)
	// {
	// 	OmnSleep(10000);
	// }
	
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(siteid);
	rdata->setSsid(mSsid);
	aos_assert_r(basicTest(rdata), false);
	return true;
}


bool 
AosSeLogClientTester::basicTest(const AosRundataPtr &rdata)
{
	int tries = mTestMgr->getTries();
	aos_assert_r(tries > 0, false);
	
	// Create 20 containers.
	aos_assert_r(createContainer(rdata), false);

	for (int i=0; i<tries; i++)
	{
		int opr = rand() % 100000;
		if (opr < 80000)
		{
			bool rslt = createLog(rdata);
			aos_assert_r(rslt, false);
			continue;
		}

		if (opr < 99000)
		{
			bool rslt = queryLogs(rdata);
			aos_assert_r(rslt, false);
			continue;
		}

		// Need to restart the server
		cout << "Please restart the Server ...: " << i << endl;
		char c;
		cin >> c;
	}
	return true;
}


bool
AosSeLogClientTester::queryLogs(const AosRundataPtr &rdata)
{
	// This function randomly determines how many times to query.
	if (mLogids.size() <= 0) return true;
	int nn = rand() % 30;
	for (int i=0; i<nn; i++)
	{
		/*
		int idx = rand() % mLogids.size();
		u64 logid = mLogids[idx];
		OmnString docid;
		docid << logid;
		*/
		int nn = rand()%10;
		OmnString query = getQueryStr(false);
		OmnString fname = getFieldName();
OmnScreen << "QueryLog: " << query << " : " << fname << endl;
		AosXmlTagPtr doc = AosSengAdmin::getSelf()->queryDocs(
				rdata->getSiteid(), 0, nn, query, fname, 0);
		if (!doc)
		{
			rdata->setError() << "Failed to query: " << query;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// The results should be in the form:
		// 	<Contents>
		// 		<record .../>
		// 		<record .../>
		//		...
		//	</Contents>
		//
		// Retrieve all the records, then the logids, then retrieve the logs.
		AosXmlTagPtr records = doc->getFirstChild("Contents");
		aos_assert_r(records, false);
		AosXmlTagPtr record = records->getFirstChild();
		//int idx = mLogids.size()-1;
		while (record)
		{
			u64 logid = record->getAttrU64("zky_logid", 0);
			aos_assert_r(logid, false);
			AosXmlTagPtr entry = AosSengAdmin::getSelf()->retrieveLog(rdata->getSiteid(), logid);
			aos_assert_r(entry, false);
			
			// entry should be this format:
			// <doc container="xxx" 
			// 		zky_ctmepo__d="xxx"
			// 		zky_logid="xxx>
			// 		<content>
			// 			<entry entry_id="xxx">Testing</entry>
			// 		</content>
			// 	</doc>
			AosXmlTagPtr content = entry->getFirstChild("contents");
			aos_assert_r(content, false);
			AosXmlTagPtr body = content->getFirstChild();
			aos_assert_r(body, false);

			int entry_id = body->getAttrInt("entry_id", -1);
			aos_assert_r(entry_id >= 0, false);
			aos_assert_r(entry_id < mLogids.size(), false);
			//aos_assert_r(entry_id == idx, false);
			aos_assert_r(mLogids[entry_id] == logid, false);
			record = records->getNextChild();
		//	idx--;
		}
OmnScreen << "QueryLog success: " << query << " : " << fname  << " : " << doc->toString() << endl;
	}
	return true;
}


OmnString
AosSeLogClientTester::getQueryStr(const bool &reverse)
{
	OmnString rvs;
	rvs = (reverse ? "true" : "false");

	int nn = rand()%10;
	OmnString query;
	OmnString container = getContainer();
	OmnString logname = getLogname();
	query << "<term type=\"logs\" "
		<<          "reverse=\"" << rvs << "\" >"
		<<       "<selector " 
		<<            "type=\"log\" "
		<< 		      "container=\"" << container << "\" "
		<<            "logname=\"" << logname << "\" "
        <<            "number=\"" << nn << "\" />"
		<<   "</term>";
	return query;
}


OmnString
AosSeLogClientTester::getFieldName()
{
	OmnString fname;
	fname << "<fname type=\"1\" >"
		<< "<oname><![CDATA[zky_logid]]></oname>"
		<< "<cname><![CDATA[zky_logid]]></cname>"
		<< "</fname>";
	return fname;
}



OmnString 
AosSeLogClientTester::getLogname()
{
	OmnString logname;
	int idx = rand() % 100;
	if (idx < 40) 
	{
		logname = "";
	}
	else if (idx < 80)
	{
		logname = "test_1";
	}
	else
	{
		logname = "test_2";
	}
	return logname;
}


OmnString 
AosSeLogClientTester::getContainer()
{
	// retrieve a container to add log
	u32 size = mCobjids.size();
	aos_assert_r(size>0, false);
	u32 idx = rand()%size;
	OmnString container = mCobjids[idx];
	aos_assert_r(container != "", false);
	return container;
}


u64 
AosSeLogClientTester::createLog(
		const OmnString &container,
		const OmnString &logname,
		const AosRundataPtr &rdata)
{
	aos_assert_r(container != "", 0);
	OmnString contents = "<entry entry_id=\"";
	contents << mLogids.size() << "\">Testing</entry>";
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->createLog(rdata->getSiteid(),
			                                logname, container, contents);
	if(!doc)
	{
		rdata->setError() << "Failed to create log"
			<< ". Container objid: " << container 
			<< ". Log name: " << logname 
			<< ". Siteid: " << rdata->getSiteid();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	OmnString logidstr = doc->getNodeText();
	aos_assert_rr(logidstr != "", rdata, false);
	u64 logid = (u64) atoll(logidstr);
	mLogids.push_back(logid);
	return logid;
}

	
bool
AosSeLogClientTester::createContainer(const AosRundataPtr &rdata)
{
	// It creates a container that defines three logs:
	// 	<container ...>
	// 		<AOSTAG_LOG_DEFINITION>
	// 			<AOSTAG_DFT_LOGNAME 
	// 				AOSTAG_AGING_TYPE="agecap"
	// 				max_filesize="50000000"
	// 				max_files="6"/>
	//			<test_1 
	// 				AOSTAG_AGING_TYPE="agecap"
	// 				max_filesize="20000000"
	// 				max_files="1"/>
	//			<test_2 
	// 				AOSTAG_AGING_TYPE="agecap"
	// 				max_filesize="600"
	// 				max_files="100"/>
	// 		</AOSTAG_LOG_DEFINITION>
	// 	</container>
	
	// Get container objid
	OmnString cobjid;
	OmnString container;
	char keyChar[50];
	for (int i=0; i<20; i++)
	{
		// Get a random objid
		cobjid = OmnRandom::word(1, 20);
		while (cobjid == "")
		{
			cobjid = OmnRandom::word(1, 20);
			if (cobjid != "") break;
		}
	
		container = "";
		// Get a container body
		container << "<container "<< "zky_objid=\"" << cobjid 
			<< "\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_otype =\"zky_ctnr\"><"
			<< AOSTAG_LOG_DEFINITION << ">"
				<< "<" << AOSTAG_DFT_LOGNAME << ">"
				<< "<ageconfig "
					"max_filesize=\"50000000\" "
					"max_files=\"6\" "
				<< AOSTAG_AGING_TYPE << "=\"agecap\">"
				<< "</ageconfig>"
				<< "</" << AOSTAG_DFT_LOGNAME << ">"
			<< "<test_1>"
			<< "<ageconfig max_filesize=\"20000000\" max_files=\"1\" aging_type=\"agecap\"/>"
			<< "</test_1>"
			<< "<test_2>"
			<< "<ageconfig max_filesize=\"600\" max_files=\"100\" aging_type=\"agecap\"/>"
			<< "</test_2>"
			<< "</" << AOSTAG_LOG_DEFINITION << ">"
			<< "</container>";
		AosXmlParser parser;
		AosXmlTagPtr container_xml = parser.parse(container, "" AosMemoryCheckerArgs);
		aos_assert_r(container_xml, false);
	
		AosXmlTagPtr resp = AosSengAdmin::getSelf()->createDoc(
							rdata->getSiteid(), container_xml, mSsid, "", "", "");
		aos_assert_r(resp, false);
		
		// Add container objid into mCobjids.
		mCobjids.push_back(cobjid);
OmnScreen << cobjid << endl;
	}
	return true;
}


OmnString 
AosSeLogClientTester::getSiteId()
{
	// This function get a random siteid.
	OmnString siteid;
	int s = rand() % 100;
	siteid << s;
	return siteid;
}

bool
AosSeLogClientTester::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	AosRundataPtr rdata = OmnApp::getRundata();

	u32 tid = thread->getLogicId();
	u32 tries = 100;
	while (state == OmnThrdStatus::eActive)
	{	
		for(u32 i=0; i<tries; i++)
		{
			basicTest(rdata);
		}

		OmnScreen << "Thread:" << tid << "Existing!" << endl;
		state = OmnThrdStatus::eIdle;
		return true;
	}
	return true;
}


/*	
bool 
AosSeLogClientTester::addVersion(const AosRundataPtr &rdata)
{
	// This function create a doc first, then will retrieve the doc, 
	// add version.
	OmnString objid = "vpd_brian";
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< "zky_public_doc=\"true\" zky_public_ctnr=\"true\" "
		<< "contents=\"hello,brian!\" "
		<< AOSTAG_PARENTC << "=\"" << mPctrObjid << "\" "
		<< "zky_testver=\"" << ++sgVersion << "\" /> ";
	AosXmlParser parser;
	AosXmlTagPtr adddoc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(adddoc, false);
	bool rslt = AosSengAdmin::getSelf()->sendCreateObjReq2(rdata->getSiteid(),
								adddoc, rdata->getSsid());
	aos_assert_r(rslt, false);

	for (u32 i=0; i< mTries; i++)
	{
		OmnScreen << "Tires: " << i << endl;
		OmnString resp;
		AosXmlTagPtr rcvdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(rdata->getSiteid(),
						rdata->getSsid(), objid, resp, true);
		aos_assert_r(rcvdoc, false);
	
		rcvdoc->setAttr("zky_count", i);
		rslt = AosSengAdmin::getSelf()->sendModifyReq(rdata->getSiteid(),
								rdata->getSsid(), rcvdoc->toString(), rdata, true);
	
		rcvdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(rdata->getSiteid(),
							rdata->getSsid(), objid, resp, true);
		aos_assert_r(rcvdoc, false);

		rslt = AosSengAdmin::getSelf()->addVersion(rcvdoc, rdata->getSiteid());
		if (!rslt)
		{
			rdata->setError() << "Add version failed";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	return true;
}


bool
AosSeLogClientTester::basicTest1(const AosRundataPtr &rdata)
{
	// This function login first then will retrieve doc by docid,
	// modify it, and send it to server modify.
	
	for(u32 docid=5000; docid<500000; docid++)
	{
		OmnScreen << "Retrieve doc: " << docid << endl;
		AosXmlTagPtr rcvdoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
								rdata->getSiteid(), rdata->getSsid(), docid, true);
		if (!rcvdoc) continue;
		rcvdoc->setAttr("zky_count", docid);
		bool rslt = AosSengAdmin::getSelf()->sendModifyReq(rdata->getSiteid(),
								rdata->getSsid(), rcvdoc->toString(), rdata, true);
		if (!rslt)
		{
			OmnAlarm << "Failed to modify: " << docid << enderr;
			continue;
		}
	}
	return true;	
}
*/

/*
bool
AosSeLogClientTester::addLogTest()
{
	OmnScreen << "~~~~~~~~~addlog~~~~~~~~~~~~~~~~~~";
	if (mPctrObjids.size() <= 20)
	{
		int n = rand() % 100;
		if (n < mCreateCtnrWeight)
		{
			if(!createContainer())
			{
				OmnScreen << "~~~~~~~~~~~~~~~~~~~~~~~filed to createContainer~~~~~~~~~~~~~~~~~~";
				return true;
			}
		}
	}

	int nn = rand() % 100;
	OmnString pctr_objid;
	bool right_pctr = true;
	if (nn < mUseContainerWeight && mPctrObjids.size() > 0)
	{
		// Use a valid container
		int idx = rand() % mPctrObjids.size();
		pctr_objid = mPctrObjids[idx];
	}
	else
	{
		// Need to either use an empty container objid or a container
		// objid that does not exist. 
		pctr_objid = "222_";
		pctr_objid << randomStr(3, 20);
		right_pctr = false;
	}
		

	nn = rand()%100;
	OmnString log_typeid = "";
	if(nn<33)
	{
		log_typeid = "test_1";
	}
	else if(nn < 66)
	{
		log_typeid = "test_2";
	}
	OmnString opr = "add";
	AosRundataPtr rdata = OmnApp::getRundata();

	rdata->setSsid(mSsid);

	OmnScreen << "~~~~~~~~add log ssid~~~~~~~~~~" << rdata->getSsid()<< endl;
	rdata->setSiteid("100");
	AosXmlTagPtr response;
	
	OmnString str = randomStr(500, 3000);
	nn = rand()%100;
	if(nn<10)
	{
		str = randomStr(1, 500);
	}
	else if(nn >90)
	{
		str = randomStr(3000,10000);
	}
	
	nn = rand()%100;
	OmnString xmltag = randomStr(3, 10);
	OmnString loginfo;

	loginfo << "<root><" << xmltag << "><![CDATA[" << str << "]]></" << xmltag << "></root>";
	 
	AosXmlParser parser;
	AosXmlTagPtr loginfo_xml = parser.parse(loginfo, "");
	aos_assert_r(loginfo_xml, false);

OmnScreen << "\n~~~~~~~~~pctr_objid~~~~~~~~~" << pctr_objid
			<< "\n~~~~~~~~~~~log_typeid~~~~~~~~" << log_typeid <<endl;
//			<< "\n~~~~~~~~~~~loginfo~~~~~~~~~~~" << loginfo << endl;

	if(nn <50)
	{
		mSeLogClient->addLog(pctr_objid, log_typeid, loginfo_xml, response, rdata);
	}
	else
	{
		mSeLogClient->addLog(pctr_objid, log_typeid, loginfo, response, rdata);
	}


//	map<OmnString , map<OmnString, list<u32> > >	mMapContainer;
	if(!response)
	{
		OmnAlarm << "~~~~~~~~~~not get response~~~~~~~~~~~" << pctr_objid<< enderr;
		return false;
	}

	mLock->lock();	
	OmnString logidstr = response->getNodeText();
	u64 ulogid = atoll(logidstr);
	u32 seqno = AosXmlDoc::getDocidSeqno(ulogid);
	if(mMapContainer.count(pctr_objid) == 0)
	{
		list<u32> tmp;
		mMapContainer[pctr_objid][log_typeid] = tmp;
	}
	
	bool found = false;
	for(list<u32>::iterator iter = mMapContainer[pctr_objid][log_typeid].begin(); 
			iter != mMapContainer[pctr_objid][log_typeid].end();
			iter++)
	{
		if(*iter == seqno)
		{
			found = true;
			break;
		}
	}
	if(!found)
	{
		mMapContainer[pctr_objid][log_typeid].push_back(seqno);
	}


	if(mMapContainer[pctr_objid][log_typeid].size()>7)
	{
		mMapContainer[pctr_objid][log_typeid].pop_front();
	}
	mLock->unlock();	


	nn = rand() % 100;
	if(!response)
	{
		OmnAlarm << "~~~~~~~~~~not get response~~~~~~~~~~~" << pctr_objid<< enderr;
		return false;
	}
	if (nn < mSaveLogWeight)
	{
		OmnString logid = response->getNodeText();
		if(!right_pctr)
		{
			OmnScreen << "~~~~~~~~~error pctr_objid~~~~~~~~" << pctr_objid << endl;
			return true;
		}
		OmnScreen << "~~~~~response~~~~~" << logid << endl;
		
		mLock->lock();
		if(mLogId.size()>100000)
		{
			int dey = 90000;
			vector<OmnString>::iterator iter = mLogId.begin();
			mLogId.erase(iter, iter+dey);
			iter = mLogTypeid.begin();
			mLogTypeid.erase(iter, iter+dey);
			iter = mLogPctrObjid.begin();
			mLogPctrObjid.erase(iter, iter+dey);
			iter = mLogInfo.begin();
			mLogInfo.erase(iter, iter+dey);
		}
		mLogId.push_back(logid);
		OmnScreen << "~~~~map seqno~~~" << AosXmlDoc::getDocidSeqno(atoll(logid)) 
			<< "~~~~~logid~~~~~~~~~" << logid 
			<< "~~~~pctr_objid~~"<< pctr_objid << endl;
		mLogTypeid.push_back(log_typeid);
		mLogPctrObjid.push_back(pctr_objid);
		mLogInfo.push_back(loginfo); 
		mLock->unlock();
	}
	return true;
}


bool 
AosSeLogClientTester::retrieveLogTest()
{
	if (mLogId.size() <= 0) return true;

	OmnString opr = "retrieve";
	
	mLock->lock();
	int nn = rand() % mLogId.size();
	int error_nn = rand() % mLogId.size();

	while(error_nn == nn && mLogId.size()>3)
	{
		error_nn = rand() % mLogId.size();
	}

	OmnString logid = mLogId[nn];
	OmnString pctr_objid = mLogPctrObjid[nn];
	OmnString log_typeid = mLogTypeid[nn];
	OmnString loginfo = mLogInfo[nn];
	OmnScreen << "~~~~map seqno~~~" << AosXmlDoc::getDocidSeqno(atoll(logid)) 
			<< "~~~~~logid~~~~~~~~~" << logid 
			<< "~~~pctr_objid~~~"<< pctr_objid << endl;

	bool right_request = true;
	
	list<u32>::iterator iter = mMapContainer[pctr_objid][log_typeid].begin();
	u32 seqno = AosXmlDoc::getDocidSeqno(atoll(logid));
OmnScreen << "~~~~~~min sqno~~~~~" << *iter 
		<< "~~~ctr seqno~~~" << seqno 
		<< "~~~~~~logid~~~~~~~~~~"<< logid << endl;

	u32 min_seqno = *iter;
	if(seqno < min_seqno)
	{
OmnScreen << "~~~~~~eqno < *iter~~~~" << endl;
		
		right_request = false;
	}
	

	if(mLogId.size()>3)
	{
		nn = rand() % 100;
		if(nn<10)
		{
			pctr_objid = "333_";
			pctr_objid<<randomStr(3, 8);
			right_request = false;
		}	
		else if(nn<20)
		{
			logid = mLogId[error_nn];
			right_request = false;
		}
		else if(nn<30)
		{
			log_typeid = "error_type_";
			log_typeid << randomStr(3,5);
			right_request = false;
		}
	}
	
	mLock->unlock();

	AosRundataPtr rdata = OmnApp::getRundata();
OmnScreen << "!!!!!!!!!!ssid!!!!!!!!!!" << mSsid <<endl;
	rdata->setSsid(mSsid);
	rdata->setSiteid("100");
	AosXmlTagPtr response;
	mSeLogClient->retrieveLog(pctr_objid, log_typeid, logid, response, rdata);
	//OmnScreen << "~~~~~~~~~response~~~~~~~~~~~~~~~~~" << response->toString() << endl;
	if(!response)
	{
		OmnAlarm << "~~~~~~~~~~not get response~~~~~~~~~~~" << enderr;
		return false;
	}
	AosXmlTagPtr res = response->getFirstChild();
	aos_assert_r(res, false);
	AosXmlTagPtr respLog = res->getFirstChild();
	
	if(!right_request)
	{
		OmnScreen << "\n~~~~~~~~~~~~error request~~~~~~~~~~~"   << endl;
		return true;
	}
	if(!respLog)
	{
		OmnAlarm << "\n~~~~~~~~~~error~~~~~~~~~~~" <<  res->toString() <<enderr;
		return false;	
	}
	if(right_request && loginfo != respLog->toString())
	{

		OmnScreen << "~~~~~~~~~~~~right~~~~~~~~~~~~~~" <<  loginfo <<"\n" 
		 << "~~~~~~~~~~~~~~~error~~~~~~~~~~~" << respLog->toString()  << endl;
		OmnAlarm << "\n~~~~~~~~~~not get loginfo~~~~~~~~~~~" << enderr;
		return false;
	}
	
	return true;
}

OmnString
AosSeLogClientTester::randomStr(const u64 min, const u64 max)
{
	if(min<=0 || max<min) return 0;
	int len1 = (rand() % (max-min)) + min;
	char data[max+5];
	AosRandomLetterStr(len1, data);
	OmnString str(data);
	return str;
}


bool
AosSeLogClientTester::actionCreateLog()
{
	mLock->lock();
	int nn = rand() % mLogPctrObjid.size();
	OmnString pctr_objid = mLogPctrObjid[nn];
	OmnString log_typeid = mLogTypeid[nn];
	mLock->unlock();	
	OmnScreen << "~~~~~~~~~actionCreateLog~~~~~~~~~~`" << endl;
	OmnString sdoc;
	
	OmnString objid = "sdoc_1";
	objid << randomStr(4,6);
	sdoc << "<smartdoc zky_sdoctp=\"runaction\" zky_public_doc=\"true\" zky_otype=\"zky_smtdoc\" zky_objid=\""<< objid <<"\">"
		<< "<actions>"
		<< "	<action zky_actid=\"createdoc\" type=\"createdoc\" zky_xpath=\"\" zky_value_type=\"const\" zky_data_type=\"XmlDoc\" zky_iscreate=\"false\">"
		<< "			<attrs>"
		<< "				<attr>"
		<< "					<xpath>"
		<< "						<zky_valuedef zky_value_type=\"const\">zky_pctrs</zky_valuedef>"
		<< " 					</xpath>"
		<< "					<value>"
		<< "						<zky_valuedef zky_value_type=\"const\">1111</zky_valuedef>"
		<< "					</value>"
		<< "				</attr>"
		<< "				<attr>"
		<< "					<xpath>"
		<< "						<zky_valuedef zky_value_type=\"const\">zky_tag</zky_valuedef>"
		<< "					</xpath>"
		<< "					<value>"
		<< "  						<zky_valuedef zky_value_type=\"const\">8888</zky_valuedef>"
		<< "					</value>"
		<< "				</attr>"
		<< "			</attrs>"
		<< "	</action>"
		<< "	<action zky_actid=\"createlog\" type=\"createlog\" zky_ctnr_objid=\""<< pctr_objid <<"\" zky_logtype=\""<< log_typeid <<"\">"
		<< "		<"<< AOSTAG_DOCSELECTOR <<" zky_docselector_type=\"createdoc\"/>"
		<< " 	</action>"
		<< "</actions>"
		<< "</smartdoc>";
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(sdoc, "");
	aos_assert_r(xml, false);
	
	AosRundataPtr rdata = OmnApp::getRundata();
	OmnString siteid = "100";
	rdata->setSsid(mSsid);
	rdata->setSiteid("100");
	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createDoc(siteid, xml, mSsid, "", "", "");
	if(!resp) return false;
	OmnString id = resp->getAttrStr("zky_objid");

	//AosXmlTagPtr getDoc = AosSengAdmin::getSelf()-> retrieveDocByObjid(siteid, mSsid, id);
	//OmnScreen << getDoc->toString()<< endl;
	//AosXmlTagPtr getDoc = AosSengAdmin::getSelf()-> actionAddLog(siteid, id, mSsid);
	//OmnScreen << getDoc->toString()<< endl;
	
	return true;
}


bool
AosSeLogClientTester::queryLog()
{
	mLock->lock();
	int nn = rand() % mLogPctrObjid.size();
	OmnString pctr_objid = mLogPctrObjid[nn];
	OmnString log_typeid = mLogTypeid[nn];
	mLock->unlock();	
	OmnScreen << "~~~~~~~~~queryLog~~~~~~~~~~`" << endl;
	OmnString siteid = "100";
	AosXmlTagPtr resp;// = AosSengAdmin::getSelf()->queryLog(siteid, pctr_objid, log_typeid, 0, 0);
	if(!resp)
	{
		OmnScreen << "response:" <<  resp->toString() << endl; 
		OmnAlarm << "error" << enderr;
		return false;
	}
	return true;
}
*/


bool
AosSeLogClientTester::createLog(const AosRundataPtr &rdata)
{
	// 1. Determine logname
	OmnString logname = getLogname();

	// 2. retrieve a container to add log
	OmnString container = getContainer();

OmnScreen << "CreateLog: " << container << " : " << logname << endl;
	// 3. Add log
	u64 logid = createLog(container, logname, rdata);
	aos_assert_r(logid>0, false);

OmnScreen << "CreateLog success : " << logid << endl;

	return true;
}

