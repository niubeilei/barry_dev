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
// 01/09/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/SEClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/DocTags.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include <deque>



AosSEClient::AosSEClient()
:
mRemotePort(0),
mSiteid(0),
mIsGood(true),
mLock(OmnNew OmnMutex())
{
}


AosSEClient::AosSEClient(
		const OmnIpAddr &remoteAddr, 
		const int remotePort)
:
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mSiteid(0),
mIsGood(true),
mLock(OmnNew OmnMutex())
{
}


AosSEClient::AosSEClient(
		const OmnIpAddr &remoteAddr, 
		const int remotePort, 
		const u32 siteid, 
		const OmnString &appname, 
		const OmnString &username)
:
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mSiteid(siteid),
mAppname(appname),
mUsername(username),
mIsGood(true),
mLock(OmnNew OmnMutex())
{
}


AosSEClient::~AosSEClient()
{
}


bool			
AosSEClient::procRequest(
		const u32 siteid, 
		const OmnString &appname, 
		const OmnString &uname,
		const OmnString &sqlquery, 
		OmnString &resp,
		OmnString &errmsg)
{
	mSiteid = siteid;
	mAppname = appname;
	mUsername = uname;
	mQueryid = "";
	OmnString req;
	OmnStrParser1 parser(sqlquery, ", ", true, false);
	OmnString word = parser.nextWord();
	if (word == "addobj")
	{
		aos_assert_r(addObj(req, sqlquery, resp, errmsg), false);
	}
	else if (word == "update")
	{
		aos_assert_r(updateObj(req, sqlquery, errmsg), false);
	}
	else if (word == "deleteobj")
	{
		aos_assert_r(deleteObj(req, sqlquery, errmsg), false);
	}
	else if (word == "select")
	{
		aos_assert_r(parseSelect(req, parser, errmsg), false);
	}
	else
	{
		req = sqlquery;
	}
		
	OmnTcpClientPtr conn = getConn();
	aos_assert_r(conn, false);

	// mLock->lock();
	// if (!mConn)
	// {
	// 	mConn = OmnNew OmnTcpClient("nn", mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
	// 	aos_assert_rl(mConn->connect(errmsg), mLock, false);
	// }

	int guard = 2;
	bool rslt = false;
	OmnConnBuffPtr buff;
	while (guard--)
	{
		aos_assert_rl(conn->smartSend(req.data(), req.length()), mLock, false);
		rslt = conn->smartRead(buff);
		if (rslt) break;
		
		if (guard >= 1)
		{
			conn->closeConn();
			if (!conn->connect(errmsg))
			{
				OmnAlarm << "Failed connecting: " << errmsg << enderr;
				return false;
			}
		}
	}
	returnConn(conn);

	if (!rslt || !buff)
	{
		errmsg = "Failed to read response!";
		return false;
	}

	// We need to simulate the real server. WebServer will return 
	// the response contents, which are composed by the jsp in the
	// form:
	// 	<response>
	// 		<the contents returned here>
	// 	</response>
		
	OmnString str(buff->getData(), buff->getDataLength());
	resp = "<response>";
	resp << str << "</response>";
	return true;
}


bool			
AosSEClient::sql(
		OmnString &req,
		const OmnString &stmt, 
		OmnString &resp,
		OmnString &errmsg)
{
	// 'sqlquery' is
	// It constructs a request of the following format:
	// 	<request>
	// 		<item name="operation">sql</item>
	// 		<cmd>
	// 			<![CDATA[
	// 			the sql statement here
	// 			]]>
	// 		</cmd>
	// 	</request>
	
	req = "<request><item name=\"operation\">sql</item><cmd><![CDATA[";
	req << stmt << "]]></cmd></request>";
	return true;
}


bool			
AosSEClient::addObj(
		OmnString &req,
		const OmnString &stmt, 
		OmnString &resp,
		OmnString &errmsg)
{
	// 'stmt' is in the form:
	// addobj <the object to add>, or
	// addobj -f <filename>

	const char *data = stmt.data();
	if (stmt.length() < 9)
	{
		errmsg = "To add an object but statement is incorrect: ";
		errmsg << stmt;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString theobj;
	if (strncmp(&data[7], "-f", 2) == 0)
	{
		// The object is defined in a file. 
		OmnFile ff(&data[10], OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!ff.isGood())
		{
			errmsg = "To add an object from a file but failed to read the file: ";
			errmsg << stmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		if (!ff.readToString(theobj))
		{
			errmsg = "Failed to read the object: ";
			errmsg << stmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}
	else
	{
		theobj = &data[7];
	}

	// 	<request>
	// 		<item name="operation">serverCmd</item>
	// 		<cmd operation="createcobj"/>
	// 		<xmlobj>
	// 			the object to create
	// 		</xmlobj>
	// 	</request>
	
	req = "<request><item name=\"operation\">serverCmd</item>";
	req << "<cmd opr=\"createcobj\" "
		<< AOSTAG_USERNAME << "=\"chen\" "
		<< AOSTAG_SITEID << "=\"142\"/>"
		<< "<xmlobj>" << theobj << "</xmlobj></request>";
	return true;
}


bool			
AosSEClient::updateObj(
		OmnString &req,
		const OmnString &stmt, 
		OmnString &errmsg)
{
	// 'stmt' is in the form:
	// updateobj <the object to add>, or
	// updateobj -f <filename>

	const char *data = stmt.data();
	if (stmt.length() < 11)
	{
		errmsg = "To update an object but statement is incorrect: ";
		errmsg << stmt;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString theobj;
	if (strncmp(&data[10], "-f", 2) == 0)
	{
		// The object is defined in a file. 
		OmnFile ff(&data[13], OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!ff.isGood())
		{
			errmsg = "To update an object from a file but failed to read the file: ";
			errmsg << stmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		if (!ff.readToString(theobj))
		{
			errmsg = "Failed to read the object: ";
			errmsg << stmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}
	else
	{
		theobj = &data[10];
	}

	// 	<request>
	// 		<item name="operation">serverCmd</item>
	// 		<cmd opr="saveobj"/>
	// 		<xmlobj>
	// 			the object to be modified
	// 		</xmlobj>
	// 	</request>
	
	req = "<request><item name=\"operation\">serverCmd</item>";
	req << "<cmd opr=\"saveobj\" "
		<< AOSTAG_USERNAME << "=\"chen\" "
		<< AOSTAG_SITEID << "=\"142\"/>"
		<< "<xmlobj>" << theobj << "</xmlobj></request>";
	return true;
}


bool			
AosSEClient::deleteObj(
		OmnString &req,
		const OmnString &stmt, 
		OmnString &errmsg)
{
	// 'stmt' is in the form:
	// deleteobj <the docid to delete>

	const char *data = stmt.data();
	if (stmt.length() < 11)
	{
		errmsg = "To delete an object but statement is incorrect: ";
		errmsg << stmt;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// 	<request>
	// 		<item name="operation">serverCmd</item>
	// 		<item name="docid">docid</item>
	// 	</request>
	req = "<request><item name=\"operation\">delObject</item>";
	req << "<item name=\"docid\">" << &data[10] << "</item>";
	return true;
}


bool	
AosSEClient::parseConds(
		OmnStrParser1 &parser, 
		OmnString &conds, 
		OmnString &startidx, 
		OmnString &psize,
		OmnString &order,
		OmnString &errmsg)
{
	// It converts the conditions from a string into the form:
	// 	<conds>
	// 		<cond type="AND">
	// 			<term type="word">xxx</term>
	// 			<term type="tag">xxx</term>
	// 			<term type="eq|ne|lt|le|gt|ge">
	// 				<lhs><![CDATA[xxx]]></lhs>
	// 				<rhs><![CDATA[xxx]]></rhs>
	// 			</term>
	// 		</cond>
	// 	</conds>
	//
	// It is in the form:
	// 	<conds> limit start, psize order by fname1, fname2
	//
	// 	<conds> ::= <and> OR <and> ...
	//  <and> ::= <term> AND <term> ...
	//  <term> ::= <term1> | <term2> | ...
	//  <term1> ::= <attrname> <op> <value>
	// It creates all the terms. It then uses 'order' to re-arrange
	// the terms so that the 'ordered' terms are put in front, 
	// serving as the controlling terms. 
	mIsGood = false;

	OmnString word;
	OmnString lhs, rhs, opr;
	int numTerms = 0;
	OmnString sep;
	while ((word = parser.nextWord("")) != "")
	{
		if (word == "limit")
		{
			// Process the page size and start idx
			//		limit start, size
			startidx = parser.nextWord("");
			psize = parser.nextWord("");
			if (startidx == "")
			{
				errmsg = "Query syntax error. The limit phrase incorrect!";
				OmnAlarm << errmsg << enderr;
				return false;
			}

			if (psize == "")
			{
				errmsg = "Query syntax error. The limit phrase missing size!";
				OmnAlarm << errmsg << enderr;
				return false;
			}
			continue;
		}

		if (word == "_#queryid")
		{
			// "_#queryid = xxx"
			char c = parser.nextChar();
			aos_assert_r(c == '=', false);
			mQueryid = parser.nextWord();
			// req << "queryid=\"" << word << "\"/></request>";
			return true;
		}

		if (word == "_#appname")
		{
			// "_#appname = xxx"
			char c = parser.nextChar();
			aos_assert_r(c == '=', false);
			mAppname = parser.nextWord();
			word = parser.nextWord();
			if (word == "") break;
			aos_assert_r(word.toLower() == "and", false);
			continue;
		}

		if (word == "_#username")
		{
			// "_#username = xxx"
			char c = parser.nextChar();
			aos_assert_r(c == '=', false);
			mUsername = parser.nextWord();
			word = parser.nextWord();
			if (word == "") break;
			aos_assert_r(word.toLower() == "and", false);
			continue;
		}

		if (word == "_#siteid")
		{
			// "_#siteid = xxx"
			char c = parser.nextChar();
			aos_assert_r(c == '=', false);
			mSiteid = atol(parser.nextWord().data());
			word = parser.nextWord();
			if (word == "") break;
			aos_assert_r(word.toLower() == "and", false);
			continue;
		}

		if (word == "order")
		{
			// "order by <fields>"
			// Currently we only support one order field
			order = parser.nextWord("");
			if (order != "by")
			{
				errmsg = "Query syntax error: expecting 'by' after 'order'!";
				OmnAlarm << errmsg << enderr;
				return false;
			}

			order = "";
			while ((word = parser.nextWord()) != "")
			{
				if (order != "") order << ",";
				order << word;
			}

			if (order == "")
			{
				errmsg = "Query syntax error: expecting a field name after"
					" 'order by'";
				OmnAlarm << errmsg << enderr;
				return false;
			}

			// This should be the last one
			if (conds != "") conds << "</cond></conds>";
			return true;
		}

		// It should be <name> <opr> <value>
		OmnString opr = parser.nextWord();
		if (opr == "")
		{
			errmsg = "Expecting the operator in a condition but failed!";
			OmnAlarm << errmsg << enderr;
			return false;
		}

		if (conds == "") conds = "<conds><cond type=\"AND\">";
		aos_assert_r(parseTerm(word, opr, conds, parser, errmsg), false);
		numTerms++;

		word = parser.nextWord();
		if (word == "") break;
		if (word.toLower() != "and")
		{
			errmsg = "Expecting 'and' but failed";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}
	if (conds != "") conds << "</cond></conds>";
	return true;
}


bool	
AosSEClient::parseTerm(
		const OmnString &lhs, 
		const OmnString &opr1, 
		OmnString &conds, 
		OmnStrParser1 &parser, 
		OmnString &errmsg)
{
	// It constructs a term of one of the forms:
	// 			<term type="word">xxx</term>
	// 			<term type="tag">xxx</term>
	// 			<term type="eq|ne|lt|le|gt|ge">
	// 				<lhs><![CDATA[xxx]]></lhs>
	// 				<rhs><![CDATA[xxx]]></rhs>
	// 			</term>
	//
	// It can be one of the following:
	// 	<lhs> <opr> <rhs>
	//  <lhs> in-range ([min, max])
	int len = opr1.length();
	char *data = (char *)opr1.data();
	OmnString opr;
	if (len == 1)
	{
		if (data[0] == '<') opr = "lt";
		else if (data[0] == '>') opr = "gt";
		else if (data[0] == '=') opr = "eq";
		else
		{
			errmsg = "Expecting an operation in a condition but failed!";
			OmnAlarm << enderr;
			return false;
		}
	}
	else if (len == 2)
	{
		if (data[0] == '<' && data[1] == '=') opr = "le";
		else if (data[0] == '=' && data[1] == '<') opr = "le";
		else if (data[0] == '>' && data[1] == '=') opr = "ge";
		else if (data[0] == '=' && data[1] == '>') opr = "ge";
		else if (data[0] == '=' && data[1] == '=') opr = "eq";
		else if (data[0] == '!' && data[1] == '=') opr = "ne";
		else
		{
			errmsg = "Expecting an operaion in a condition but failed!";
			OmnAlarm << enderr;
			return false;
		}
	}
	else if (opr1 == "in-range")
	{
		aos_assert_r(parseInrange(lhs, conds, parser, errmsg), false);
		return true;
	}
	else
	{
		errmsg = "Expecting an operator in a condition but failed!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString rhs = parser.nextWord();
	conds << "<term type=\"" << opr << "\"><lhs><![CDATA["
		<< lhs << "]]></lhs>"
		<< "<rhs><![CDATA[" << rhs << "]]></rhs>"
		<< "</term>";
	return true;
}


bool	
AosSEClient::parseInrange(
		const OmnString &lhs,
		OmnString &conds, 
		OmnStrParser1 &parser, 
		OmnString &errmsg)
{
	// It constructs a term of one of the forms:
	// 		<term type="eq|ne|lt|le|gt|ge">
	// 			<lhs><![CDATA[xxx]]></lhs>
	// 			<rhs><![CDATA[xxx]]></rhs>
	// 		</term>
	// 		<term type="eq|ne|lt|le|gt|ge">
	// 			<lhs><![CDATA[xxx]]></lhs>
	// 			<rhs><![CDATA[xxx]]></rhs>
	// 		</term>
	//
	// The input should be:
	// 	[(min, max)]
	char left = parser.nextChar();
	OmnString min = parser.nextWord();
	if (min == "")
	{
		errmsg = "Expecting the 'min' value but failed!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString opr;
	if (left == '(')
	{
		opr = "lt"; 
	}
	else if (left == '[')
	{
		opr = "le";
	}
	else
	{
		errmsg << "Invalid left bracket: " << opr;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	conds << "<term type=\"" << opr << "\"><lhs><![CDATA[" << lhs << "]]></lhs>"
		<< "<rhs><![CDATA[" << min << "]]></rhs></term>";

	OmnString max = parser.nextWord();
	char right= parser.nextChar();
	if (max == "")
	{
		errmsg = "Expecting the 'max' value but failed!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (right == ')')
	{
		opr = "gt";
	}
	else if (right == ']')
	{
		opr = "ge";
	}
	else
	{
		 errmsg << "Invalid right bracket: " << opr;
		 OmnAlarm << errmsg << enderr;
		 return false;
	}
	conds << "<term type=\"" << opr << "\"><lhs><![CDATA[" << lhs << "]]></lhs>"
		<< "<rhs><![CDATA[" << max << "]]></rhs></term>";
	return true;
}


/*
bool
AosSEClient::parseUpdate(
		OmnStrParser1 &parser, 
		OmnString &errmsg)
{
	// update <container> set <name> = <value>, ... where <conds>
	// The 'update' word has already been parsed. 
	//
	// If conditions are not specified, <container> must be present, 
	// which means to update the entire container. 
	//
	// If conditions are present but not <container>, it is to update
	// objects in any container that match the condition. 
	
	OmnString word = parser.nextWord("");
	if (word != "set")
	{
		// It must be a container name. 
		aos_assert_r(createContainerTerms(word, errmsg), false);

		word = parser.nextWord("");
		if (word != "set")
		{
			errmsg = "Query syntax error: updating data but the word 'set' "
				"is not specified: ";
			errmsg << mStmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}

	// The remaining should be in the form:
	// 	<name> = <value>, ..., <name> = <value> [where <conds>]
	OmnString lhs, rhs;
	int state = 0;
	mNumAssign = 0;
	while ((word = parser.nextWord("")) != "")
	{
		if (word == "where") 
		{
			mIsGood = parseConds(parser, errmsg);
			if (mIsGood) mSqlType = eUpdate;
			return mIsGood;
		}

		if (word == ",") 
		{
			if (state != 3)
			{
				errmsg = "Update statement not ending correctly. Statement: ";
				errmsg << mStmt;
				OmnAlarm << errmsg << enderr;
				return false;
			}

			// Now we got one assignment: 
			if (mNumAssign >= eMaxAssign)
			{
				errmsg = "Too many assignments in the update statement. ";
				errmsg << "Maximum allowed: " << eMaxAssign;
				OmnAlarm << errmsg << enderr;
				return false;
			}

			if (lhs == "" || rhs == "")
			{
				errmsg = "Update statement syntax error. One of the assignment is empty!";
				errmsg << " Statement: " << mStmt;
				OmnAlarm << errmsg << enderr;
				return false;
			}

			mLHS[mNumAssign] = lhs;
			mRHS[mNumAssign] = rhs;
			mNumAssign++;
		}

		if (word == "=")
		{
			if (lhs == "" || state != 1)
			{
				errmsg = "Update statement incorrect, phrase started with =";
				errmsg << ". Statement: " << mStmt;
				OmnAlarm << errmsg << enderr;
				return false;
			}

			state = 2;
			continue;
		}

		if (state == 0) 
		{
			lhs = word;
			continue;
		}

		if (state != 2)
		{
			errmsg = "Update statement syntax error!";
			errmsg << " Statement: " << mStmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		rhs = word;
		state = 3;
	}

	if (mNumAssign == 0)
	{
		errmsg = "Update statement missing the assignment phrases: ";
		errmsg << mStmt;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mIsGood = true;
	mSqlType = eUpdate;
	return true;
}
*/


/*
bool
AosSEClient::parseDelete(
		OmnStrParser1 &parser, 
		OmnString &errmsg)
{
	// "delete [from <container>] [where <conds>]"
	// The 'delete' word has already been parsed. 
	//
	// If conditions are not specified, <container> must be present, 
	// which means to delete all entries in the container.
	//
	// If conditions are present but not <container>, it is to delete 
	// objects in any container that match the condition. 
	
	OmnString word = parser.nextWord("");

	if (word.toLower() == "from")
	{
		word = parser.nextWord("");
		if (word == "")
		{
			errmsg = "Delete statement, expecting a container name after the"
				" word 'from'. Statement: ";
			errmsg << mStmt;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		aos_assert_r(createContainerTerms(word, errmsg), false);

		word = parser.nextWord("");
	}

	if (word == "where") 
	{
		mIsGood = parseConds(parser, errmsg);
		if (mIsGood) mSqlType = eDelete;
		return mIsGood;
	}

	errmsg = "'delete' statement syntax error! ";
	errmsg << mStmt;
	OmnAlarm << errmsg << enderr;
	return false;
}
*/


bool
AosSEClient::parseSelect(
		OmnString &req,
		OmnStrParser1 &parser, 
		OmnString &errmsg)
{
	// It parses a select statement. The word 'select' has been 
	// parsed. 
	// 	<request>
	// 		<item name="operation"><![CDATA[serverCmd]]></item>
	// 		<item name="zky_uname"><![CDATA[xxx]]></item>
	// 		<item name="zky_appnm"><![CDATA[xxx]]></item>
	// 		<item name="zky_siteid"><![CDATA[xxx]]></item>
	// 		<cmd opr="retlist"
	// 			queryid="xxx"
	// 			qtype="sql"
	// 			query="xxx"
	// 			order="xxx"
	// 			dir="xxx"       // either 'asc' or 'des', dft: 'asc'
	// 			startidx="xxx"
	// 			psize="xxx">
	// 			<conds .../>
	// 			<fnames .../>
	// 		</cmd>
	// 	</request>
	//
	// The statement should be:
	// 	select fieldnames from <container> where <conds> order by <fieldnames>
	// 		limit startidx, num
	// If <fieldnames> is "_#queryid=xxx, it is a subsequent query. All others
	// will be ignored.

	OmnString word = parser.nextWordEx("", ",=: ");
	OmnString cname, ftype;
	OmnString fields;
	OmnString queryid;
	while (word != "from" && word != "where")
	{
		if (fields == "" && word == "x")
		{
			fields = "<fnames><fname type=\"x\"/>";
			word = parser.nextWord();
			break;
		}

		// It should be a field. Field can be in the form:
		// 	oname:cname:type
		// where 'oname' is the original field name, 'cname' is the
		// converted field name, and 'type' can be '1' for attribute,
		// '2' for subtag, '3' for CDATA, and 'x' for XML object itself.
		// Fields are separated by commas. 

		switch (parser.crtChar())
		{
		case ':':
			 // It should be in the form:
			 // 	oname:cname:type
			 cname = parser.nextWordEx("", ":");
			 aos_assert_r(cname != "", false);
			 parser.nextChar(false);
			 ftype = parser.nextWordEx("", ", ");
			 aos_assert_r(ftype != "", false);
			 break;

		case ',':
			 cname = word;
			 ftype = "1";
			 break;

		case ' ':
			 break;

		default:
			 errmsg = "Syntax error!";
			 OmnAlarm << errmsg << enderr;
			 return false;
		}

		if (word == "from" || word == "where") break;

		if (fields == "") fields = "<fnames>";
		fields << "<fname type=\"" << ftype << "\">"
			<< "<oname><![CDATA[" << word << "]]></oname>"
			<< "<cname><![CDATA[" << cname << "]]></cname>"
			<< "</fname>";

		word = parser.nextWordEx("", ",=: ");
	}
	if (fields != "") fields << "</fnames>";

	OmnString conds;
	if (word == "from")
	{
		// Retrieve the container
		OmnString ctnr = parser.nextWord("");
		if (ctnr == "")
		{
			errmsg = "Statement incorrect. Missing the container name!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	
		conds = "<conds><cond type=\"AND\"><term type=\"tag\"><![CDATA[";
		conds << AOSZTG_CONTAINER << ctnr << "]]></term>";

		word = parser.nextWord();
	}

	// Should be 'where'
	if (word.toLower() != "where")
	{
		errmsg = "Statement incorrect. Missing the word 'where'";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString order, psize, startidx;
	aos_assert_r(parseConds(parser, conds, startidx, psize, order, errmsg), false);

	req = "<request>";
	req << "<item name=\"operation\"><![CDATA[serverCmd]]></item>"
		<< "<item name=\"" << AOSTAG_USERNAME << "\"><![CDATA[" << mUsername << "]]></item>"
		<< "<item name=\"" << AOSTAG_APPNAME << "\"><![CDATA[" << mAppname << "]]></item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\"><![CDATA[" << mSiteid << "]]></item>"
		<< "<cmd opr=\"retlist\""
		<< " qtype=\"sql\"";

	if (mQueryid != "")
	{
		req << "queryid=\"" << mQueryid << "\"/></request>";
		return true;
	}

	if (startidx != "") req << " startidx=\"" << startidx << "\"";
	if (psize != "") req << " psize=\"" << psize << "\"";
	if (order != "") req << " order=\"" << order << "\"";
	req << ">";
	if (conds != "") req << conds;
	if (fields != "") req << fields;
	req << "</cmd></request>";
	return true;
}


/*
bool
AosSEClient::parseFields(
	const OmnString *fnames,
	FieldDef *fields, 
	const int numFields)
{
	// Each field name can be in the form:
	// 	<xmlname>|$|<convertedname>|$|<type>
	
	for (int i=0; i<numFields; i++)
	{
		AosStrSplit items(fnames[i].data(), "|$|", 3);
		fields[i].xmlname = items[0];
		fields[i].cname = (items.entries()>1)?items[1]:fields[i].xmlname;
		char type = (items.entries()>2)?items[2].data()[0]:AOS_ATTR_FLAG;
		aos_assert_r(type == AOS_ATTR_FLAG ||
				type == AOS_TEXT_FLAG ||
				type == AOS_CDATA_FLAG, false);
		fields[i].type = type;
	}
	return true;
}
*/


OmnTcpClientPtr
AosSEClient::getConn()
{
	OmnTcpClientPtr conn;
	mLock->lock();
	if (mIdleConns.size() > 0)
	{
		deque<OmnTcpClientPtr>::reverse_iterator itr;
		itr = mIdleConns.rbegin();
		if (itr != mIdleConns.rend())
			conn = *itr;
		mIdleConns.pop_back();
		mLock->unlock();
		return conn;
	}
	else
	{
		conn = OmnNew OmnTcpClient("nn", 
				mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
		OmnString errmsg;
		if (!conn->connect(errmsg))
		{
			mLock->unlock();
			return 0;
		}
	 	//aos_assert_rl(conn->connect(errmsg), mLock, 0);
		mLock->unlock();
		return conn;
	}
	mLock->unlock();
	return conn;
}



void 
AosSEClient::returnConn(const OmnTcpClientPtr &conn)
{
	mLock->lock();
	mIdleConns.push_back(conn);
	mLock->unlock();
}
