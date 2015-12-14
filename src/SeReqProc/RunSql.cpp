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
// 2015/01/28	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgGloble.h"
#include "SeReqProc/RunSql.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SeReqProc/ReqidNames.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "JQLParser/JQLParser.h"

extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosRunSql::AosRunSql(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RUNSQL, 
		AosSeReqid::eRunSql, rflag)
{
	mFlag = false;	
}


bool 
AosRunSql::proc(const AosRundataPtr &rdata)
{
	//<request>
	//	<contents><![CDATA[select * from t1;]]></contents>
	//	<contentsformat><![CDATA[xxxx]]></contentsformat>
	//</request>
	bool rslt;
	OmnString contents;
	//mKeyWords.clear();
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "missing root request";
		return false;
	}

	AosXmlTagPtr request = root->getFirstChild("request");
	if (!request)
	{
		rdata->setError() << "missing request";
		return false;
	}

	AosXmlTagPtr content = request->getFirstChild("content");
	if (!content)
	{
		rdata->setError() << "missing content";
		return false;
	}

	OmnString jql = content->getNodeText();
	AosXmlTagPtr format = request->getFirstChild("contentformat");
	OmnString format_str;
	if (!format)
	{
		format_str = "xml";
	}
	else
	{
		format_str = format->getNodeText();
	}
	if(jql == "cancelQuery")
	{
		jql = "exit;";
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
/*	char* tempStr = const_cast<char*>(jql.data());
	OmnString firstSubStr, secondSubStr, forthSubStr;
	int len = jql.length();

	sscanf(tempStr, "%s", firstSubStr.data());
	sscanf(tempStr, "%*s%s", secondSubStr.data());
	sscanf(tempStr, "%*s%*s%*s%s", forthSubStr.data());
	if(strncmp("end", tempStr, 3) == 0)
	{
		mJobStr << jql;
		mFlag = false;
		jql = mJobStr;
		mJobStr = "";
	}
	OmnString errMsg = "";
	if(mFlag || (strncmp("create",firstSubStr.data(),6) == 0 && strncmp("job",secondSubStr.data(),3) == 0 && strncmp("begin",forthSubStr.data(),5) == 0))
	{
		if(len < 0 || tempStr[len -1] != ';') 
		{
			errMsg << "missing key word \";\" for this job";
			rdata->setJqlMsg(errMsg);
			return true;                                      
		}
		if(strncmp("run",firstSubStr.data(),3) == 0 && strncmp("job",secondSubStr.data(),3) == 0)
		{
			//AosLogError(rdata, true, "there is missing key word \"end\" for this job") << enderr;
			errMsg << "missing key word \"end\" for this job";
			rdata->setJqlMsg(errMsg);
			return true;                                      
		}
		mJobStr << jql;
		mFlag = true;
		return true;
	}
*/
	vector<AosJqlStatementPtr> statements;
	AosJimoProgObjPtr prog = OmnNew AosJimoProgGloble();
	jimoParser->parse(rdata.getPtr(),prog.getPtr() ,jql,statements);

	OmnString ss;
	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat(format_str);                       
		rslt = statements[i]->run(rdata.getPtrNoLock(), prog.getPtrNoLock(), ss, true);                       
		if(!rslt) break;
	}
	
/*	
	bool rslt = false;
	int rslt_value = -1;

	if (jql.indexOf("{", 0) < 0)
	{
		rslt_value = AosParseJQL(jql.getBuffer(), true);
		if (rslt_value)
		{
			//rdata->setError() << "stdin: syntax error" << enderr;
			AosSetErrorU(rdata, "stdin: syntax error") << enderr;
			return false;
		}
	}

	if (rslt_value != 0)
	{
		//AosSetErrorU(rdata, "stdin: syntax error") << enderr;
		//return false;

		rslt = gAosJQLParser.parseWithRegex(jql, rdata);
		if (!rslt)
		{
			rslt_value = AosParseJQL(jql.getBuffer(), true);
			if (rslt_value)
			{
				//rdata->setError() << "stdin: syntax error" << enderr;
				AosSetErrorU(rdata, "stdin: syntax error") << enderr;
				return false;
			}
		}
	}

	vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
	OmnString errmsg = gAosJQLParser.getErrmsg();
	if (errmsg != "") 
	{
		AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;     
		return false;
	}

	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat(format_str);                       
		statements[i]->run(rdata);
	}
	gAosJQLParser.clearStatements();
	*/
	return true;
}

