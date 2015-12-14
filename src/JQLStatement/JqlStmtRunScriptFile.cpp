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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtRunScriptFile.h"
#include "JimoProg/JimoProgGloble.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SeReqProc/RunSql.h"

#include "JQLParser/JQLParser.h"                     
#include "JimoAPI/JimoParserAPI.h"

#include<iomanip>
using namespace std;                                 
extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;


AosJqlStmtRunScriptFile::AosJqlStmtRunScriptFile()
{
	mFileName = "";
	mSuppressFlag = 0;
	//mFlag = false;
	//mJobStr = "";
}

AosJqlStmtRunScriptFile::~AosJqlStmtRunScriptFile()
{
}


bool 
AosJqlStmtRunScriptFile::runCmd(
		const OmnString fname,
		const AosRundataPtr &rdata)
{
	setParametersToAosJQLParser();
	OmnString msg;
	bool bSuppressFlag = mSuppressFlag;
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		msg << "[ERR] : invalid file path \"" << fname << "\"!";
		rdata->setJqlMsg(msg);
		return false;
	}
	OmnString data;
	bool rslt = file.readToString(data);
	data = parser_data(data);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	data.replaceStrWithSpace();

	OmnString allCmdResp = "";
	OmnString curCmdResp = "";
	OmnString cmdResp = "";

	AosStrSplit split(data, ";");
	vector<OmnString> cmdV = split.entriesV();
	OmnString cur_cmd;
	u32 offset = 0;
	//arvin 2015.07.10
	bool isCollect = false;
	OmnString tmpjob;
	bool finish = false;
	cur_cmd = getNextCmd(cmdV, finish, offset, allCmdResp, rdata);
	while (!finish)
	{
		if (cur_cmd != "") 
			cur_cmd << ";";
		else 
		{
			cur_cmd = getNextCmd(cmdV, finish, offset, allCmdResp, rdata);
			continue;
		}
		cur_cmd.replaceStrWithSpace();
		OmnScreen << "cur_cmd: " << cur_cmd << endl;
		if (isIgnore(cur_cmd))
		{
			cur_cmd = getNextCmd(cmdV, finish, offset, allCmdResp, rdata);
			continue;
		}

		if (isSourceCmd(cur_cmd))
		{
			allCmdResp << "\n--------------------------------------------------------";
		}
		
		//arvin 2015/4/19
		AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
		aos_assert_rr(jimoParser,rdata.getPtr(),0);
		//if the jql is following this format like:
		//create job job1()(
		//   ..............;
		//   ..............;
		//   ..............;
		// );
		// end;
		//wo should concat the jql to the whole jql
		rslt = collectJob(isCollect,cur_cmd,tmpjob);
		if(rslt && isCollect)
		{
			cur_cmd = getNextCmd(cmdV, finish, offset, allCmdResp, rdata); 
			continue;
		}
		if(tmpjob != "" && !isCollect)
		{
			cur_cmd = tmpjob;
			tmpjob = "";
		}
	
		AosJimoProgObjPtr prog = OmnNew AosJimoProgGloble();
		vector<AosJqlStatementPtr> statements;
		rslt = jimoParser->parse(rdata.getPtr(),prog.getPtr(), cur_cmd,statements);
		//arvin 2015.07.23
		//JIMODB-53:if parse failed,it will stop 
		if(!rslt) return false;

		OmnString ss;
		for (u32 i=0; i<statements.size(); i++)
		{
			statements[i]->setContentFormat("print");                       
			rslt = statements[i]->run(rdata.getPtrNoLock(), prog.getPtrNoLock(), ss, false);                       
/*			if (!rslt)
			{
				OmnString errmsg = "";		
				errmsg << "[ERR]: " << rdata->getJqlMsg(); 
				curCmdResp << "\n" << errmsg;
				allCmdResp << "\n" << cur_cmd.data() << "\n" << curCmdResp << "\n";
				rdata->setJqlMsg(allCmdResp);
				return true;
			}
*/
			//arvin 2015.07.23
			//JIMODB-53:if parse failed,it will stop 
			if(!rslt) return false;
		}



		/*
		int val = -1;
		if (cur_cmd.indexOf("{", 0) < 0)
			val = AosParseJQL((char *)cur_cmd.data(), true);

		if (val != 0) 
		{
			rslt = gAosJQLParser.parseWithRegex(cur_cmd, rdata);               
			if (!rslt)                                                     
			{                                                              
				allCmdResp << "\nJQL> " << cur_cmd.data() << "\n" << "ERROR : Bad Cmd. Failed to parser."<< "\n";
				break;
		    }                                                              
		}
		gAosJQLParser.dump();

		vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
		OmnString errmsg = gAosJQLParser.getErrmsg();
		if (errmsg != "") 
		{
			AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;               
			return false;
		}
*/
		curCmdResp = rdata->getJqlMsg();
		if (curCmdResp == "")
		{
			curCmdResp =  rdata->getResults();
			if (curCmdResp != "")
			{
				AosXmlTagPtr content = AosXmlParser::parse(curCmdResp AosMemoryCheckerArgs);
				aos_assert_r(content, false);
				curCmdResp = content->getNodeText();
			}
		}
		allCmdResp << "\n" << cur_cmd.data() << "\n" << curCmdResp << "\n";

		if (isSourceCmd(cur_cmd))
		{
			allCmdResp << "--------------------------------------------------------\n";
		}

		cur_cmd = getNextCmd(cmdV, finish, offset, allCmdResp, rdata); 
	
	}
	//SOURCE CMD and Result modified
	//The defalut option of SOURCE CMD is SUPPRESS OFF
	//Add By Mayz 2014/12/04
	if( false == bSuppressFlag )
		rdata->setJqlMsg(allCmdResp);
	else if( true == bSuppressFlag )
	{
		curCmdResp = rdata->getJqlMsg();
		cmdResp = curCmdResp;
		cmdResp.toLower();
		if(strncmp(cmdResp.data(), "error", strlen("error"))==0 ||\
			strncmp(cmdResp.data(), "failed", strlen("failed"))==0 ||\
			strncmp(cmdResp.data(), "no", strlen("no"))==0 ||\
			strncmp(cmdResp.data(), "unknown", strlen("unknown"))==0 ||\
			strstr(cmdResp.data(), "exists")!=NULL)
		{
			rdata->setJqlMsg(curCmdResp);
		}
		else
		{
			cmdResp = "";
			cmdResp << "\n" << "SOURCE " << fname << " finished\n";
			rdata->setJqlMsg(cmdResp);
		}
	}

	return true;
}

bool
AosJqlStmtRunScriptFile::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	runCmd(mFileName, rdata);	
	return true;
}


OmnString
AosJqlStmtRunScriptFile::parser_data(OmnString &data)
{
	OmnString buf = "";
	bool flag = false;
	bool t = false;
	for (i32 i = 0; i < data.length(); i++)
	{
		if ( !flag && ( (data[i] == '-' && data[i+1] == '-')
				|| (data[i] == '/' && data[i+1] == '*')) )
		{
			if (data[i] == '-')
				t = true;
			else
				t = false; 
			flag = true;
			continue;
		}
		if ( flag && ((data[i] == '\n' && t)
				|| (data[i] == '/' && data[i-1] == '*')) )
		{
			flag = false;
			continue;
		}
		if (!flag) buf << data[i];
	}
	return buf;
}


AosJqlStatement *
AosJqlStmtRunScriptFile::clone()
{
	return OmnNew AosJqlStmtRunScriptFile(*this);
}


void 
AosJqlStmtRunScriptFile::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtRunScriptFile::setFileName(OmnString name)
{
	mFileName = name;
}


bool
AosJqlStmtRunScriptFile::isIgnore(OmnString cmd)
{
	if (cmd == "") return false;	
	if (cmd[0] == '#') return true;

	return false;
}


bool
AosJqlStmtRunScriptFile::isSourceCmd(const OmnString cmd)
{
	OmnString tmp_cmd = cmd;
	tmp_cmd.replaceStrWithSpace();
	tmp_cmd.toLower();
	if ( strncmp(tmp_cmd.data(), "source", strlen("source")) == 0 )
	{
		return true;
	}
	return false;
}

OmnString 
AosJqlStmtRunScriptFile::getSourceFileName(const OmnString source_cmd)
{
	AosStrSplit split(source_cmd, "\"");
	vector<OmnString> strV = split.entriesV();
	if (strV.size() != 3) 
	{
		OmnCout << "SOURCE ERROR, Please check sytax: [" << source_cmd  << "]" << endl;
		return "";
	}

	return strV[1];
}

void
AosJqlStmtRunScriptFile::setSuppress(bool suppress_flag)
{
	mSuppressFlag = suppress_flag; 
}

bool
AosJqlStmtRunScriptFile::getSuppress()
{
	return mSuppressFlag;
}


void 
AosJqlStmtRunScriptFile::setParameters(AosExprList *parms)
{
	mParms = parms;
}


void 
AosJqlStmtRunScriptFile::setParametersToAosJQLParser()
{
	gAosJQLParser.cleanSourceParms();
	if (mParms)
	{
		for (u32 i = 0; i < mParms->size(); i++)
		{
			gAosJQLParser.setSourceParms((*mParms)[i]);
		}
	}
}


OmnString 
AosJqlStmtRunScriptFile::getNextCmd(
		vector<OmnString> &cmdV, 
		bool &finish,
		u32 &offset,
		OmnString &allCmdResp,
		const AosRundataPtr &rdata)
{
	if (offset >= cmdV.size()) 
	{
		finish = true;
		return "";
	}

	std::stack<int> mystack;
	OmnString cmd = "", tmp_cmd = "", prefix = "", prefix2 = "";
	for (; offset < cmdV.size(); offset++)
	{
		tmp_cmd = cmdV[offset];
		//tmp_cmd.toUpper();
		tmp_cmd.replaceStrWithSpace();
		prefix = OmnString(tmp_cmd.data(), 2);
		prefix2 = OmnString(tmp_cmd.data(), 6);
		if (prefix.toUpper() =="IF")
		{
			mystack.push(1);
		}
		else if (prefix2.toUpper() == "END IF")
		{
			if (mystack.empty())
			{
				allCmdResp << "\nbad cmd: " << cmd << tmp_cmd.data() << "\n";
				offset++;
				return "";
			}
			mystack.pop();
		}
		cmd << cmdV[offset];
		if (!mystack.empty())
			cmd << ";";
		if (mystack.empty())
		{
			offset++;
			return cmd;
		}
	}

	if (cmd != "")
	allCmdResp << "\nbad cmd: " <<  cmd.data() << "\n";
	finish = true;
	return "";
}


bool 
AosJqlStmtRunScriptFile::collectJob(bool &isCollect,OmnString &stmt, OmnString &tmpjob)
{	
	int idx = 0;
	bool reverse = false;
	OmnString jql = stmt;
	jql.toLower();
	if(!isCollect)
	{
		idx = jql.findSubString("if ",0,reverse);
		if(idx == 0)
			isCollect = true;
	}
	if(!isCollect)
	{
		idx = jql.findSubString("create",0,reverse);
		if(idx != 0)
			return false;
		idx = jql.findSubString("service",6,reverse);
		if(idx == -1)
		{
			idx = jql.findSubString("job",6,reverse);
			if(idx != 7)
				return false;
		}
		idx = jql.findSubString("begin",10,reverse);
		if(idx == -1)
			return false;
		isCollect = true;			
	}
	tmpjob << stmt;
	idx = jql.findSubString("end;",0,reverse);
	if(idx != -1)
	{
		isCollect = false;
	}
	else		
	{
		//arvin 2015.07.23
		//JIMODB-53
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
		//arvin 2015.07.23
		//JIMODB-53
		idx = jql.findSubString("endif ;",0,reverse); 
		if(idx != -1) isCollect = false;
	}
	return true;
}


