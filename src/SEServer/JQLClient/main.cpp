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
// 04/07/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEClient/SEClient.h"
#include "SEClient/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ReadFile.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "SEServer/JQLClient/version.h"
#include "ReadLine/readline.h"
#include "ReadLine/history.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


// global variable
int 			gAosLogLevel = 0;
bool 			gAosShowNetTraffic = false;
u64				gTransID = 0;
AosSEClientPtr 	gConnect = NULL;
OmnString 		gSessionID = "";
OmnString 		gCookieStr = "";
u64				gUrlDocid = 5230;
//vector<bool>	gStmtBeginV;

OmnString 	welcomeStr = \
	"*********************************************************************\n" 
	"*  Welcome to JimoSQL $VERSION                           			  \n" 
	"*  Copyright (c) 2009-2015 Zykie Networks Inc. All rights reserved.  \n"
//	"*  Type \"help\" to list all the commands.							  \n"
	"*********************************************************************\n";

OmnString   prompt = "JQL> ";                 
OmnString   cliNextLineTip = "  -> ";                 
OmnString   helpFile = "help.txt";            
//arvin
OmnString 	tmpjob = "";
bool isCollect = false;


static bool sgIsBegin = false;
static OmnString sgKeepString = "";           
static OmnString sgPasswd = "12345";           
static OmnString sgHostname = "127.0.0.1";    
static OmnString sgUsername = "root";
static int sgRemotePort = 9000;                  
static OmnString sgStmt = "";   //if not empty, run one stat only
static OmnString sgFormat = "print";
static OmnString sgOutFile = "";  //direct output to a file
static OmnString sgInFile = "";  //get statements from a file


// functions
bool initReadLine();
bool loginDB();
bool logoutDB();
OmnString getPasswd(); 
u32 getSubStrTimes(const OmnString &src, const OmnString &sub_str);
bool checkLineBuff(OmnString oneline, const AosRundataPtr &rdata);
AosXmlTagPtr procRequest( 
		const u32 siteid, const OmnString &appname, 
		const OmnString &uname, const OmnString &req);
u64 getTransID() { return gTransID++;}
bool runJql(const OmnString &jql);
static void sigint_proc_func(int sig);	// Add by Young to proc CTRL^C signal
bool collectJob(bool &isCollect,OmnString &stmt);	//arvin

int main(int argc, char **argv)
{
	initReadLine();
	signal(SIGINT, sigint_proc_func);
	
	//get the version
	OmnString version = AOS_JIMO_VERSION;	
	welcomeStr.replace("$VERSION", version, true);

	bool debugFlag = false;
	int index = 0;
	OmnApp::appStart(argc, argv);             

	while (index < argc)
	{
		OmnString parm = argv[index++];
		parm.toLower();

		if (parm == "-s")
		{
			OmnTracer::mStdio = false;
			continue;
		}
		
		if (parm == "-cli")
		{
			debugFlag = true;
			continue;
		}

		if (!strncmp(parm.data(), "-u", strlen("-u")))
		{
			if (parm == "-u")
			{
				if (index >= argc)
				{
					cout << "lost username" << endl;
				}
				OmnString username(argv[index++]);
				if (username.length() <= 0)
				{
					cout << "Incorrect -u argument: Username is null" << endl;
					return false;
				}
				sgUsername = username;
			}
			else
			{
				sgUsername = parm.data() + strlen("-u");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-p", strlen("-p")))
		{
			if (parm == "-p")
			{
				if (index >= argc)
				{
					cout << "lost port" << endl;
				}
				OmnString port(argv[index++]);
				if (port.length() <= 0)
				{
					cout << "Incorrect -p argument: Port is null" << endl;
					return false;
				}
				sgRemotePort = port.toInt();
			}
			else
			{
				OmnString port = parm.data() + strlen("-p");
				sgRemotePort = port.toInt();
			}
			if (sgRemotePort <= 0)
			{
				cout << "Port(" << sgRemotePort << ") error!!!" << endl;
				return false;
			}

			continue;
		}

		if (!strncmp(parm.data(), "-h", strlen("-h")))
		{
			if (parm == "-h")
			{
				if (index >= argc)
				{
					cout << "lost host" << endl;
				}
				OmnString host(argv[index++]);
				if (host.length() <= 0)
				{
					cout << "Incorrect -h argument: Host is null" << endl;
					return false;
				}
				sgHostname = host;
			}
			else
			{
				sgHostname = parm.data() + strlen("-h");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-t", strlen("-t")))
		{
			if (parm == "-t")
			{
				if (index >= argc)
				{
					cout << "lost single JQL statement" << endl;
					return false;
				}
				OmnString stmt(argv[index++]);
				if (stmt.length() <= 0)
				{
					cout << "Incorrect -1 argument: single statement is null" << endl;
					return false;
				}
				sgStmt = stmt;
			}
			else
			{
				sgStmt = parm.data() + strlen("-t");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-i", strlen("-i")))
		{
			if (parm == "-i")
			{
				if (index >= argc)
				{
					cout << "lost input file name" << endl;
					return false;
				}
				OmnString fname(argv[index++]);
				if (fname.length() <= 0)
				{
					cout << "Incorrect -i argument: input filename is null" << endl;
					return false;
				}
				sgInFile = fname;
			}
			else
			{
				sgInFile = parm.data() + strlen("-i");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-o", strlen("-o")))
		{
			if (parm == "-o")
			{
				if (index >= argc)
				{
					cout << "lost output file name" << endl;
					return false;
				}
				OmnString fname(argv[index++]);
				if (fname.length() <= 0)
				{
					cout << "Incorrect -1 argument: output file name is null" << endl;
					return false;
				}
				sgOutFile = fname;
			}
			else
			{
				sgOutFile = parm.data() + strlen("-o");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-f", strlen("-f")))
		{
			if (parm == "-f")
			{
				if (index >= argc)
				{
					cout << "lost format string" << endl;
					return false;
				}
				OmnString format(argv[index++]);
				if (format.length() <= 0)
				{
					cout << "Incorrect -1 argument: format string is null" << endl;
					return false;
				}
				sgFormat = format;
			}
			else
			{
				sgFormat = parm.data() + strlen("-f");
			}
			continue;
		}

		if (!strncmp(parm.data(), "-v", strlen("-v")) || 
		 	!strncmp(parm.data(), "-version", strlen("-version")) ) 
		{
			//the user asks the version only. Print the 
			//version and exit
			cout << "JimoSQL version " << version  <<endl;
			exit(0);
		}
	}

	if (sgUsername != "root")
	{
		sgPasswd = getPasswd();
	}

	bool rslt = true;
	AosRundataPtr rdata = OmnApp::getRundata(); 
	rdata->setSiteid(100);                      

	// login database
	OmnString req, resp, err;
	gConnect = OmnNew AosSEClient(sgHostname, sgRemotePort);
	int trys = 10;
	OmnTcpClientPtr conn;
	while(trys)
	{
		conn = gConnect->getConn();
		if (!conn)
		{
			cout << "waiting ..." << endl;
			trys--;
			sleep(1);
			continue;
		}
		break;
	}
	if (!conn)
	{
		cout << "Failed to connect server" << endl;
		exit(1);
	}
	if (!loginDB()) 
	{
		cout << "Login JimoDB Failed!!!" << endl;
		exit(1);
	}

	if (sgStmt != "") {
		//we need to run specified JQL statement only
		OmnString stmt = sgStmt;

		bool rslt = checkLineBuff(stmt, rdata);	
		if (!rslt)
			cout << "Failed to run statement: " << stmt << endl;
	
		logoutDB();
		exit(0);
	}

	char* input_buff, shell_prompt[1000];
	char* first_tip  = (char*)"JQL>";
	char* second_tip = (char*)"  ->";
	char* input_tip = first_tip;
	cout << welcomeStr << endl;
	
	while (1)
	{
		input_tip = first_tip;
		sgKeepString = "";	
		while (1)
		{
			snprintf(shell_prompt, sizeof(shell_prompt), "%s ", input_tip);
			input_buff = readline(shell_prompt);
			OmnString tmpbuff(input_buff, strlen(input_buff)); 
			tmpbuff.replace("\n", "", true);
			rslt = checkLineBuff(tmpbuff, rdata);	
			if (rslt) break;
			input_tip = second_tip;	
		}
	}
	return 0;
} 

bool checkLineBuff(OmnString oneline, const AosRundataPtr &rdata)
{
	bool flag = false;
	OmnString magicStr = "#^&@#@$";
	OmnString str1;

	oneline.replaceStrWithSpace();

	// Modify by Young for (begin ... end), 2015/05/25	
	
	//if (strstr(tmpdata.data(), " begin ")) sgIsBegin = true;
	//if (strstr(tmpdata.data(), " end"))
	//{
	//	sgIsBegin = true;
	//	if (sgIsBegin)
	//}

	//u32 times = 0;
	//if (times = getSubStrTimes(oneline, " begin ") )
	//{
	//	for (u32 i=0; i<times; i++) gStmtBeginV.push_back(true);
	//	sgKeepString << oneline;
	//	return false;
	//}
	//else if (times = getSubStrTimes(oneline, " end"))
	//{
	//	if (!gStmtBeginV.empty())
	//	{
	//		gStmtBeginV.push_back(true);
	//	}
	//}

	if (oneline == "" && sgKeepString == "" && !isCollect) return true;
	if (oneline == "" && isCollect) return false;
	if (strcmp(oneline.getBuffer(), "exit") == 0 || 
	    strcmp(oneline.getBuffer(), "exit;") == 0)
	{
		logoutDB();
		cout << "Bye" << endl; 
		exit(0);
	}

	oneline.replace("\\;", magicStr, true);
	AosStrSplit split(oneline, ";");
	vector<OmnString> strV = split.entriesV();

	if (strV.size() <= 0)
	{
		oneline.replace(magicStr, "\\;", true);
		sgKeepString << oneline;
		return false;
	}

	u32 len = oneline.length();
	if (oneline[len-1] == ';') flag = true;
	for (u32 i=0; i<strV.size(); i++)
	{
		//keep escaped statement in str1
		str1 = strV[i];
		str1.replace(magicStr, "\\;", true);
		str1.replaceStrWithSpace();

		strV[i].replace(magicStr, ";", true);
		if((i != 0) && (strV[i] == "")) continue;

		OmnString buff = "";
		if (i == 0) buff = sgKeepString;
		if ((i == strV.size() - 1) && (!flag))
		{
			sgKeepString << str1 << " ";
			break;
		}
		

		buff << str1 << ";";
		//arvin
		bool rslt = collectJob(isCollect,buff);
		if(rslt && isCollect)
		{
			flag =false;
			sgKeepString = "";
			continue;
		}
		if(tmpjob != "" && !isCollect)
		{
			buff = tmpjob;
			tmpjob = "";
		}

		//OmnString tmpdata = buff;
		//tmpdata.toLower();
		//if (strstr(tmpdata.data(), " begin ")) 
		//{
		//	sgIsBegin = true;
		//}
		//if (strstr(tmpdata.data(), " end"))
		//{
		//	sgIsBegin = false;
		//	break;		
		//}
		//if (sgIsBegin) break;


		buff.replaceStrWithSpace();
		add_history(buff.getBuffer());
		sgKeepString = "";
		
		// Add by Young, 2014/11/25
		// for JQL command "source ..." 
		OmnString tmpBuff = buff;
		tmpBuff.toLower();
		
		//Source CMDS results will be achieved in the JqlStmtRunScriptFile.cpp
		//Modified by Mayz, 2014/12/05
		//if (strncmp(tmpBuff.data(), "source ", strlen("source ")) == 0)
		if(false)
		{
			int start = buff.indexOf(0, '"', false) + 1;
			int end = buff.indexOf(0, '"', true) - 1;
			aos_assert_r(start < end, false);
			if (start >= end)
			{
				cout << "check the path of source file" << endl;
				return true;
			}
			OmnString fname = buff.substr(start, end); 
			OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
			if (!file || !file->isGood())
			{
				file->closeFile();
				cout << "file is not good" << endl;
				return true;
			}

			OmnString fileContents = "";
			file->readToString(fileContents);
			file->closeFile();
			checkLineBuff(fileContents, rdata);
		}
		else
		{
			//remove "\" before running the statement since
			//the command need ';' only
			buff.replace("\\;", ";", true);

			runJql(buff);
			//OmnCout <<  buff << endl;
		}
	}

	return flag;
}

OmnString getPasswd()
{
	OmnString passwd = getpass("Enter passwd: ");
	return passwd;
}

AosXmlTagPtr procRequest(const u32 siteid, const OmnString &appname,
		const OmnString &uname, const OmnString &req)
{
	OmnString resp, errmsg;
	AosBuffPtr buff = OmnNew AosBuff(req.length()+12 AosMemoryCheckerArgs);
	aos_assert_r(buff, NULL);
	buff->setU64(0);
	buff->setBuff(req.data(), req.length());
	OmnString sqlquery(buff->data(), buff->dataLen());
	gConnect->procRequest(siteid, appname, uname, sqlquery, resp, errmsg);

	AosXmlParser parser;
	AosXmlTagPtr respXml = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(respXml, NULL);

	return respXml;
}

bool
loginDB()
{
	OmnString req;
	req << "<request>"
		<< "<item name='operation'><![CDATA[serverreq]]></item>"
		<< "<item name='zky_siteid'><![CDATA[100]]></item>"
		<< "<item name='transid'><![CDATA[" << getTransID() << "]]></item>"
		<< "<item name='reqid'><![CDATA[jql_login]]></item>"
		<< "<item name='zkyurldocdid'><![CDATA[" << gUrlDocid << "]]></item>"
		<< "<item name='loginobj'><![CDATA[true]]></item>"
		<< "<request>"
		<< "<username><![CDATA[" << sgUsername << "]]></username>"
		<< "<pwd><![CDATA[" << sgPasswd << "]]></pwd>"
		<< "<ctnr><![CDATA[zky_sysuser]]></ctnr>"
		<< "</request>"
		<< "</request>";
	OmnString errmsg;                                                          
	OmnString resp;
	if (!gConnect)
	{
		return false;
	}
	bool rslt = gConnect->procRequest(100, "", "", req, resp, errmsg);
	if (!rslt || resp == "")
	{
		return false;
	}
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	if (!root)
	{
		return false;
	}
	AosXmlTagPtr contents = root->getFirstChild("Contents");
	if (!contents)
	{
		return false;
	}
	AosXmlTagPtr loginobj = contents->getFirstChild("zky_lgnobj");
	if (!loginobj)
	{
		return false;
	}
	gSessionID = loginobj->getAttrStr("zky_ssid", "");
	if (gSessionID == "")
	{
		return false;
	}
	gCookieStr = "<zky_cookies>";
	gCookieStr << "<cookie zky_name=\"zky_ssid_" << gUrlDocid << "\"><![CDATA[" << gSessionID << "]]></cookie></zky_cookies>";
	return true;
}


bool runJql(const OmnString &jql)
{
	OmnString req;
	req << "<request>"
		<< "<item name='operation'><![CDATA[serverreq]]></item>"
		<< "<item name='zky_siteid'><![CDATA[100]]></item>"
		//<< "<item name='username'><![CDATA[nonameyet]]></item>"
		<< "<item name='transid'><![CDATA[" << getTransID() << "]]></item>"
		<< "<item name='reqid'><![CDATA[runsql]]></item>"
		//<< "<item name='sendto'><![CDATA[seserver]]></item>"
		<< "<item name='zkyurldocdid'><![CDATA[" << gUrlDocid << "]]></item>"
		//<< "<item name='OsName'><![CDATA[Linux]]></item>"
		//<< "<item name='userAgent'><![CDATA[Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)]]></item>"
		//<< "<item name='subOpr'><![CDATA[]]></item>"
		//<< "<item name='OsArch'><![CDATA[amd64]]></item>"
		<< "<item name='loginobj'><![CDATA[true]]></item>"
		//<< "<item name='OsVersion'><![CDATA[3.2.0-23-generic]]></item>"
		<< "<request>"
		<< "<content><![CDATA[" << jql << "]]></content>"
		<< "<contentformat>" << sgFormat << "</contentformat>"
		<< "</request>"
		<< gCookieStr
		<< "</request>";
	OmnString errmsg;                                                          
	OmnString resp;
	aos_assert_r(gConnect, false);
	aos_assert_r(gConnect->procRequest(100, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);

	AosXmlTagPtr status = root->getFirstChild("status");
	aos_assert_r(status, false);
	//arvin 2015.07.23
	//JIMODB-50:donot need file error
/*	//YanYan 2015/07/21 
	//JIMODB-1
	AosXmlTagPtr tablestatus = status->getFirstChild("tablestatus");
	if(tablestatus)
	{
		OmnString tname = tablestatus->getAttrStr("tname", "");
		bool hastable = tablestatus->getAttrBool("hastable", true);
		if(!hastable)
		{
			cout << "ERROR : Table '" << tname << "' doesn't exist\n" << endl;
			return true;
		}
	}
	bool is_error = status->getAttrBool("error");
	if (is_error)
	{
		AosXmlTagPtr error = status->getFirstChild("error");
		aos_assert_r(error, false);
		OmnString err_str;
		OmnString error_str = "";
		//AosXmlTagPtr child = error->getFirstChild();
		//aos_assert_r(child, false);
		
		error_str << "**********ERROR**********\n"
				  << " ID   : " << error->getAttrStr("errmsg_id") << "\n"
				  << " File : " << error->getAttrStr("fname") << ":" << error->getAttrStr("line") << "\n"
				  << " MSG  : " << status->getNodeText() << "\n"
				  << "*************************";
		cout << error_str << "\n" << endl;
				
		return true;
	}
*/
	AosXmlTagPtr messageNode = root->getFirstChild("message");
	if (messageNode)
	{
		OmnString message = messageNode->getNodeText();
		if (message != "") 
		{
			cout << message << "\n" << endl;
			return true;
		}
	}

	AosXmlTagPtr contents_tag = root->getFirstChild("content");
	if (contents_tag)
	{
		if (sgFormat == "xml")
		{
			cout << contents_tag->toString() << endl;
			return true;
		}
		OmnString contents = contents_tag->getNodeText();

		// Add by Young, covert XML CDATA
		contents.replace("0x0333333", "<![CDATA[", true);
		contents.replace("0x0444", "]]>", true);
		cout << "\n" << contents << "\n" << endl;
	}
	return true;
}


bool logoutDB()
{
	return true;
	OmnString requestInfo = "<request>";
	requestInfo << "<reqid>logout</reqid>"
			 << "<siteid>100</siteid>"
			 << "<transid>" << getTransID() << "</transid>"
			 << "<ssid>" << gSessionID << "</ssid>" 
			 << "</request>";

	AosXmlTagPtr respXml = procRequest(100, "", "", requestInfo);
	//aos_assert_r(respXml, false);
	//AosXmlTagPtr messageNode = respXml->getFirstChild("message");
	//aos_assert_r(messageNode, false);
//OmnScreen << "\n" << respXml->toString() << endl;

	return true;
}


bool initReadLine()
{
	rl_bind_key('\t', rl_insert);
	return true;
}

void sigint_proc_func(int sig)
{
	OmnString jql = "cancelQuery";
	runJql(jql);
	exit(0);
}


u32 getSubStrTimes(const OmnString &src, const OmnString &sub_str)
{
	u32 times = 0;
	int len = src.length();	
	int start_pos = 0;
	while ( len > sub_str.length() )
	{
		if (start_pos = src.findSubString(sub_str, start_pos, false))
		{
			times++;			
			len -= start_pos + sub_str.length();
		}
	}

	return times;
}

bool collectJob(bool &isCollect,OmnString &stmt)
{	
	int idx = 0;
	bool reverse = false;
	OmnString jql = stmt;
	jql.toLower();
	if(!isCollect)
	{
		idx = jql.findSubString("create",0,reverse);
		if(idx != 0)
			return false;
		idx = jql.findSubString("procedure",6,reverse);
		if(idx != 7)
		{
			//arvin 2015.10.15
			//JIMODB-965
			idx = jql.findSubString("service",6,reverse);
			if (idx !=7 )
				idx = jql.findSubString("job",6,reverse);
			if(idx != 7) return false;
		}
		idx = jql.findSubString("begin",10,reverse);
		if(idx == -1)
			return false;
		isCollect = true;			
	}
	tmpjob << stmt;
	idx = jql.findSubString("end;",0,reverse);
	//arvin 2015.07.23
	//JIMODB-53
	if(idx != -1)
	{
		isCollect = false;
	}
	else		
	{
		idx = jql.findSubString("end ;",0,reverse); 
		if(idx != -1) isCollect = false;
	}
	idx = jql.findSubString("endif;",0,reverse);
	if(idx != -1)
	{
		isCollect = false;
	}
	else
	{
		idx = jql.findSubString("endif ;",0,reverse); 
		if(idx != -1) isCollect = false;
	}
	return true;
}


