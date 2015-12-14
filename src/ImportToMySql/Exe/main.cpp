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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/AllActions.h"
#include "ValueSel/AllValueSel.h"
#include "SEInterfaces/ValueSelObj.h"
#include "Conds/AllConds.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "DataBlob/BlobRecord.h"
#include "DataField/DataFieldCreator.h"
#include "DataRecord/RecordFixBin.h"
#include "DataScanner/ScannerCreator.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataCollector/DataCollectorCreator.h"
#include "DataCollector/DataCollectorMgr.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "IILTrans/AllIILTrans.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterClt/CounterClt.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/AllCounterProc.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "DataStore/StoreMgr.h"
#include "DbQuery/Query.h"
#include "Debug/Debug.h"
#include "DocProc/DocProc.h"
#include "DocClient/DocClient.h"
#include "DocMgr/Ptrs.h"
#include "DocMgr/DocMgr.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvrConnMgr.h"
#include "DocServer/AllDocSvrProcs.h"
#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "Job/JobMgr.h"
#include "IILTrans/AllIILTrans.h"
#include "MultiLang/LangDictMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/AllTerms.h"
#include "QueryClient/QueryClient.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "ReliableFile/ReplicPolicy.h"
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/FrontEndSvr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/SiteMgr.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataCollectorObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/DocSelRandObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/CounterCltObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/DataCollectorObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SystemMonitor/SysMonitor.h"
#include "DataCollector/DataCollectorCreator.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "SoapServer/SoapServer.h"
#include "TaskMgr/TaskCreator.h"
#include "TaskMgr/TaskDataCreator.h"
#include "TaskMgr/TaskMgr.h"
#include "TransClient/TransClient.h"
#include "Util/OmnNew.h"
#include "Util/ExeReport.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILScanner/IILScanner.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransServer.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "StorageMgr/NetFileClient.h"
#include "StorageMgr/NetFileServer.h"
#include "SmartDoc/SmartDocImpl.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/TcpServerEpoll.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordTransClient/WordTransClient.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "VersionServer/VersionServer.h"
#include "UserMgmt/UserMgmt.h"
#include "Python/Python.h"
#include "Conds/AllConds.h"
#include "SEUtil/SEUtilInit.h"
#include "Thread/ThreadMgr.h"
#include "Thread/ThreadShellMgr.h"
#include "Thread/Thread.h"
#include "TransClient/TransClient.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "StoreQuery/StoreQueryMgr.h"

// Need ContecEcgProc
#include "CustomizedProcs/Contec/Decl.h"
#include "CustomizedProcs/INotify/INotifyMgr.h"
#include "RecordProc/RecordProc.h"
#include "RecordProc/Ptrs.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool filterData(char * data, int &len);
 

bool insertOneByOne(OmnString &stmt, OmnString &table);

bool readFormatFiles(const OmnString &dirname, map<OmnString,AosRecordProcPtr> &drMap);
bool readDataFiles(const OmnString &dirname, map<OmnString,AosRecordProcPtr> &drMap);
bool readOneFile(const OmnString &sourcefilename, map<OmnString,AosRecordProcPtr> &drMap);
bool procData(const AosBuffPtr &buff, map<OmnString,AosRecordProcPtr> &drMap);
bool createDataRecords(const OmnString &fname, map<OmnString,AosRecordProcPtr> &drMap);
bool insertIntoMysql(char *data, int len, map<OmnString,AosRecordProcPtr> &drMap);
bool AosExitSearchEngine();

static OmnString sgBuildNum = "2012-03-03.1";
static OmnString sgBuildTime = "2012/03/13 15:42";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = " 99 : /home/jackie/AOS/src/SEServer/Local/";

static bool needStop = false;
static int sgStatusFreq = 600;

int redirectOutPut();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	//cout << "value: " << value << endl;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012

bool gSvrIsUp = false;           // Ketty 2012/11/28

bool sgIsBigIILServer = false;		// Ken Lee, 2012/07/17
bool gNeedBackup = false;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"

OmnString sgIntType[] = {"account_left", "addup_discharge", "addup_limit", "amount", "call_duration", "center_charge_duration", "center_charge_fee", "center_duration", "charge_total", "cid_rent",
						"coolring_rent", "cost", "cycle_id", "data_flowdn_center_charge", "data_flowdn_charge", "data_flowdn", "data_flowup_center_charge", "data_flowup_charge", "data_flowup", "dir_rent",
						"fav_fee", "fee", "fee1", "fee2", "fee3", "fn_rent", "gift_account", "gift_fee", "hmsrent", "incalind_rent",
						"ippkg_rent", "lock_rent", "m_count", "message_num", "msg_length", "mtamount", "mtchargeadd", "mtmoney", "old_fee1", "old_fee2",
						"old_fee3", "t_count", "times", "total_fee", "total_flow", "total_rent", "unidirection_rent"};
int gNUM = 0;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	int index = 1;
	OmnString formatdir;
	OmnString datadir;
	while (index < argc)
	{
		if (strcmp(argv[index], "-formatdir") == 0 && index < argc-1)
		{
			formatdir << argv[index+1]; 	
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-datadir") == 0 && index < argc-1)
		{
			datadir << argv[index+1]; 	
			index += 2;
			continue;
		}

		index++;
	}
	//1.read one record at a time 
	//2.process record (AosDataRecord)
	//3.insert into mysql
	AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());

	OmnDataStoreMySQL::startMySQL(argc, argv);

	map<OmnString,AosRecordProcPtr>	drMap;
	readFormatFiles(formatdir, drMap);

	readDataFiles(datadir, drMap);

	//finish
	map<OmnString, AosRecordProcPtr>::iterator it;
	for ( it=drMap.begin() ; it != drMap.end(); it++ )
	{
		OmnString type = (*it).first;
		if((*it).second->getRecordSize()>0)
		{
			OmnString stmt = "insert into ";
			stmt << type << " values";
			stmt << (*it).second->getBuff();
			OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
			aos_assert_r(store, false);
		OmnScreen << "query :" << stmt << endl;
			if (!store->runSQL(stmt))
			{
				OmnAlarm << "Failed to insert value." << enderr;
				insertOneByOne(stmt, type);
			}
		}
	}
	
	OmnScreen << "=================all finished !=================== " << endl;


	while (1)
	{
		OmnSleep(1);
	}

	theApp.exitApp();
	OmnScreen << "exit now ! ---------" << endl;
	return 0;
} 


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;
	AosSysMonitor::getSelf()->stop();

	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgr::getSelf()->stop();
	OmnSleep(1);

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryClient::getSelf()->stop();

	OmnScreen << "To stop Requester!" << endl;
	AosSeReqProc::stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	OmnScreen << "To stop IILMgr!" << endl;
	AosIILMgr::getSelf()->stop();

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVirtualFileSysMgr::getSelf()->stop();

	OmnScreen << "To TransClients!" << endl;
	AosTransModuleClt::stop();
	OmnSleep(2);
	OmnScreen << "To TransServers!" << endl;
	AosTransModuleSvr::stop();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;
	return true;
}


int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	OmnString filename = "output_";

	FILE* ff = popen("hostname", "r");
	char buff[128];
	int len = fread(buff, 1, 128, ff); 
	buff[len-1]=0;
	filename << buff << ".txt";
	pclose(ff);
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}

bool readFormatFiles(const OmnString &dirname, map<OmnString,AosRecordProcPtr> &drMap)
{
	DIR *dir;
	struct dirent *file;

	if((dir = opendir(dirname.data())) == NULL)
	{
		OmnAlarm << "no format dir" << enderr;
		return false;
	}

	while((file = readdir(dir)) != NULL)
	{
		if(strncmp(file->d_name, ".", 1) == 0)  continue;
		if ((file->d_type & DT_DIR))
		{
			OmnString childPath = dirname;
			childPath << "/" << file->d_name;
			bool rslt = readFormatFiles(childPath, drMap);
			aos_assert_r(rslt, false);
		}

		OmnString fileName = dirname;
		fileName << "/" << file->d_name;
		createDataRecords(fileName, drMap);
	}
	closedir(dir);
}

bool readDataFiles(const OmnString &dirname, map<OmnString,AosRecordProcPtr> &drMap)
{
	DIR *dir;
	struct dirent *file;

	if((dir = opendir(dirname.data())) == NULL)
	{
		OmnAlarm << "no data dir" << enderr;
		return false;
	}

	while((file = readdir(dir)) != NULL)
	{
		if(strncmp(file->d_name, ".", 1) == 0)  continue;
		if ((file->d_type & DT_DIR))
		{
			OmnString childPath = dirname;
			childPath << "/" << file->d_name;
			bool rslt = readDataFiles(childPath, drMap);
OmnScreen << "filePath : " << childPath << endl;
			aos_assert_r(rslt, false);
			continue;
		}

		OmnString fileName = dirname;
		fileName << "/" << file->d_name;
		readOneFile(fileName, drMap);
	}
	closedir(dir);
	return true;
}

bool
createDataRecords(const OmnString &fname, map<OmnString,AosRecordProcPtr> &drMap)
{
	//1.read file and change to xml
	//2.create datarecord
	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		OmnAlarm << "file is not good!" << enderr;
		return false;
	}
	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "read file error!" << enderr;
		return false;
	}
	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(contents, "" AosMemoryCheckerArgs);
	if(!config)
	{
		OmnAlarm << "config is not exist!" << enderr;
		return false;
	}
	OmnString id = config->getAttrStr("id", "");
	if(id == "")
	{
		OmnAlarm << "config has not id !" << enderr;
		return false;
	}
	AosRundataPtr rdata = OmnApp::getRundata();
	AosRecordProcPtr datarecord = OmnNew AosRecordProc();;
	datarecord->config(config, rdata);
	drMap[id] = datarecord;
		
	return true;
}

bool readOneFile(const OmnString &sourcefilename, map<OmnString,AosRecordProcPtr> &drMap)
{
	const char * scfile = sourcefilename.data();
	OmnFilePtr sourcefile = OmnNew OmnFile(scfile, OmnFile::eReadOnly AosMemoryCheckerArgs);;
	aos_assert_r(sourcefile->isGood(), false);

	u64 pos = 0;
	const int memsize = 5000000;
	AosBuffPtr  buff = OmnNew AosBuff(memsize AosMemoryCheckerArgs);
	char *data = buff->data();
	while(1)
	{
		int bytesread = sourcefile->readToBuff(pos, memsize, data);
		aos_assert_r(bytesread > 0, false);
		int p = bytesread;
		while(1)
		{
			aos_assert_r(p >= 1, false);
			if (buff->data()[p-1] == 0x0a) 
				break;
			p--;
		}
		buff->setDataLen(p);
		pos += p;	
		procData(buff, drMap);	
		if (bytesread != memsize)
			break;
	}
	sourcefile->closeFile();
}

bool procData(const AosBuffPtr &buff, map<OmnString,AosRecordProcPtr> &drMap)
{
	char *data = buff->data();
	while(data < buff->data()+buff->dataLen())
	{
		int p = 0;
		while(1)
		{
			if (data[p] == 0x0a) 
			{	p++;
				break;
			}
			p++;
		}

		if(filterData(data, p))
		{
			data += p;
			continue;
		}

		//process one record
		insertIntoMysql(data, p, drMap);

		data += p;
	}
	return true;
}

bool insertIntoMysql(char *data, int len, map<OmnString,AosRecordProcPtr> &drMap)
{
	gNUM++;
	AosRundataPtr rdata = OmnApp::getRundata();
	OmnString type;
	//proc head
	if(len == 120)
	{
		type = "cpn"; 
	}
	else if(len == 25)
	{
		type = "phy";
	}
	else if(len == 20)
	{
		type = "upn"; 
	}
	else
	{
		OmnString prestr(data,2);
		type = prestr;
	}

	AosRecordProcPtr obj = drMap[type]; 
	obj->reset(rdata);
	obj->setIndexIncrease();
	aos_assert_r(obj, false);
	obj->setData(data, len, false);
	OmnString value;
	int total = obj->getNumFields();
	OmnString record;
	record << "(" << gNUM << ",";
	AosValueRslt vrslt;
	for(int i=0; i<total; i++)
	{
		obj->getField(i, vrslt, true, rdata);
		AosDataFieldType::E t = obj->getFieldType(i);
		value = vrslt.getValueStr1();
		bool isInt = false;
		for(int j=0; j<47; j++)
		{
			if(sgIntType[j] == obj->getFieldName(i))
			{
				isInt = true;
				break;
			}
		}
		if(i==total-1)
		{
			if(isInt)
			{
				if(value.isEmptyLine())
				{
					value = "null";
				}
				record << value;
			}
			else
			{
				record << "'" << value << "'";
			}
		}
		else
		{
			if(isInt)
			{
				if(value.isEmptyLine())
				{
					value = "null";
				}
				record << value << ",";
			}
			else
			{
				record << "'" << value << "',";
			}
		}
	}
	record << ")";
	if(obj->getRecordSize() == 1000)
	{
		OmnString stmt = "insert into ";
		stmt << type << " values";
		stmt << obj->getBuff();
		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		aos_assert_rr(store, rdata, false);
	OmnScreen << "query :" << stmt << endl;
		if (!store->runSQL(stmt))
		{
			AosSetError(rdata, "failed_set_sql_value_001");
			OmnAlarm << "Failed to insert value." << enderr;
			insertOneByOne(stmt, type);
		}
		obj->resetBuff(rdata);
	}
	record << ",";
	obj->setRecordToBuff(record);

	return true;
}

bool insertOneByOne(OmnString &stmt, OmnString &table)
{
		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		aos_assert_r(store, false);
		char c = '(';
		int index = stmt.find(c, false);
		OmnString str1(stmt.data(), index);
		OmnString str2(stmt.data()+index, stmt.length()-index);
		OmnStrParser1 parser(str2, "),(", false, false);
	vector<OmnString> vv;
	int nn = AosStrSplit::splitStrBySubstr(str2, "),(", vv, 100000);
		OmnString record;
		for(int i=0; i<vv.size(); i++)
		{
			record = vv[i];
			OmnString insertcmd;
			if(i == 0)
			{
				insertcmd << str1 << record << ")";
			}
			else if(i == vv.size()-1)
			{
				insertcmd << str1 << "(" << record;
			}
			else
			{
				insertcmd << str1 << "(" << record << ")";
			}
			if (!store->runSQL(insertcmd))
			{
				OmnAlarm << "Failed to insert one record : " << insertcmd << enderr;
				//only update cpn, upn, phy
				OmnString updatecmd;	
				updatecmd << "update " << table << " set ";  
				if(table == "cpn")
				{
					OmnString cpnvalue;
					if(i == 0)
					{
						OmnString value(record.data()+1, record.length());
						cpnvalue << value;
					}
					else if(i == vv.size()-1)
					{
						OmnString value(record.data(), record.length()-1);
						cpnvalue << value;
					}
					else
					{
						OmnString value(record.data(), record.length());
						cpnvalue << value;
					}
					OmnStrParser1 parser2(cpnvalue, ",", false, false);
					OmnString v;
					int w_index=0;
					map<int, OmnString> wds;
					wds[0] = "uid";
					wds[1] = "H1H2H3";
					wds[2] = "LONG_CODE";
					wds[3] = "LONG_CODE_LOC";
					wds[4] = "TYPE";
					wds[5] = "CHANGE_DATE";
					wds[6] = "LONG_CODE_NEW";
					wds[7] = "LONG_CODE_LOC_NEW";
					wds[8] = "TYPE_NEW";
					wds[9] = "BEGIN_DATE";
					wds[10] = "END_DATE";
					wds[11] = "TIME_TYPE";
					wds[12] = "NOTE";
					wds[13] = "AUDIT_FLAG";
					OmnString primary;
					while((v = parser2.nextWord()) != "")
					{
						if(w_index != 1)
						{
							updatecmd << wds[w_index] << "=" << v << ",";
						}
						else
						{
							primary << v;
						}
						w_index++;
					}
					OmnString cmd(updatecmd.data(), updatecmd.length()-1);
					cmd << " where H1H2H3=" << primary;
					if (!store->runSQL(cmd))
					{
						OmnAlarm << "failed to update." << cmd << enderr;
					}
				}
				else if(table == "upn")
				{
					OmnString upnvalue;
					if(i == 0)
					{
						OmnString value(record.data()+1, record.length());
						upnvalue << value;
					}
					else if(i == vv.size()-1)
					{
						OmnString value(record.data(), record.length()-1);
						upnvalue << value;
					}
					else
					{
						OmnString value(record.data(), record.length());
						upnvalue << value;
					}
					OmnStrParser1 parser2(upnvalue, ",", false, false);
					OmnString v;
					int w_index=0;
					map<int, OmnString> wds;
					wds[0] = "uid";
					wds[1] = "PHONE_NO";
					wds[2] = "REGION_CDE";
					wds[3] = "CITY_CDE";
					OmnString primary;
					while((v = parser2.nextWord()) != "")
					{
						if(w_index != 1)
						{
							updatecmd << wds[w_index] << "=" << v << ",";
						}
						else
						{
							primary << v;
						}
						w_index++;
					}
					OmnString cmd(updatecmd.data(), updatecmd.length()-1);
					cmd << " where PHONE_NO=" << primary;
					if (!store->runSQL(cmd))
					{
						OmnAlarm << "failed to update." << cmd << enderr;
					}
				}
				else if(table == "phy")
				{
					OmnString phyvalue;
					if(i == 0)
					{
						OmnString value(record.data()+1, record.length());
						phyvalue << value;
					}
					else if(i == vv.size()-1)
					{
						OmnString value(record.data(), record.length()-1);
						phyvalue << value;
					}
					else
					{
						OmnString value(record.data(), record.length());
						phyvalue << value;
					}
					OmnStrParser1 parser2(phyvalue, ",", false, false);
					OmnString v;
					int w_index=0;
					map<int, OmnString> wds;
					wds[0] = "uid";
					wds[1] = "HEAD_ID";
					wds[2] = "GRP_REGION_ID";
					wds[3] = "AREA_CODE";
					wds[4] = "REGION_CDE";
					wds[5] = "REGION_NAME";
					wds[6] = "CHARR_CDE";
					OmnString primary;
					while((v = parser2.nextWord()) != "")
					{
						if(w_index != 1)
						{
							updatecmd << wds[w_index] << "=" << v << ",";
						}
						else
						{
							primary << v;
						}
						w_index++;
					}
					OmnString cmd(updatecmd.data(), updatecmd.length()-1);
					cmd << " where HEAD_ID=" << primary;
					if (!store->runSQL(cmd))
					{
						OmnAlarm << "failed to update." << cmd << enderr;
					}
				}
			}
		}
		return true;
}

bool
filterData(char * data, int &len)
{
	OmnString type(data, 2);
	if( type == "ac" || type == "br" || type == "de" || type == "hl" || type == "ib" || type == "id" || type == "iy" || type == "ml" ||
		type == "ph" || type == "pt" || type == "sg" || type == "sr" || type == "ss" || type == "vc" || type == "ve" || type == "vf" ||
		type == "vh" || type == "vn" || type == "vr" || type == "vs" || type == "vt" || type == "bj" || type == "cr" || type == "cw" ||
		type == "gg" || type == "gn" || type == "hp" || type == "hm" || type == "ia" || type == "ig" || type == "ij" || type == "im" ||
		type == "ir" || type == "mm" || type == "nd" || type == "on" || type == "pg" || type == "pr" || type == "vb" || type == "vd" ||
		type == "vi" || type == "vv" || type == "vw" || type == "wa" || type == "wn" || type == "vj" || type == "dl" || type == "va" ||
		len == 120 	 || len	 == 25   || len  == 20)
	{
		return false;
	}
	return true;
}


