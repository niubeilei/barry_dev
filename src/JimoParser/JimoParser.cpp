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
// 2015/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoParser/JimoParser.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoJQLParser.h"
#include "JimoAPI/JimoParserAPI.h"
#include "JimoParser/Parserlet.h"
#include "JQLParser/JQLParser.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprBracketsObj.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/NameValueParser.h"
#include "Util/NameValueDoc.h"

extern int  AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoParser_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoParser(rdata.getPtr(), version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosJimoParser::AosJimoParser(AosRundata *rdata,const int version)
:
AosJimoParserObj(version)
{
	mJimoVersion = version;
	mJimoType = AosJimoType::eJimoParser;

	OmnScreen << "================== JimoParser Created!" << endl;
	mJimoType = AosJimoType::eJimoParser;
	mLock = OmnNew OmnMutex();
	mRootParserlet = OmnNew AosParserlet();
	if (!config(rdata))
	{
	 	OmnThrowException("failed_loading_parserlets");
	 	return;
	}
}

AosJimoParser::AosJimoParser(const AosJimoParser &parser)
:
AosJimoParserObj(1)
{
	mJimoVersion = 1;
	mJimoType = AosJimoType::eJimoParser;

	mRootParserlet = parser.mRootParserlet;
	mNameCharset = parser.mNameCharset;
	mKeywordCharset = parser.mKeywordCharset;
	mExprCharset = parser.mExprCharset;
	mLock = OmnNew OmnMutex();
}



AosJimoParser::~AosJimoParser()
{
	OmnScreen << "================== JimoParser Deleted!" << endl;
}


AosJimoPtr
AosJimoParser::cloneJimo() const
{
	return OmnNew AosJimoParser(*this);
}


OmnString
AosJimoParser::getObjType(AosRundata *rdata)
{
	return "JimoParser;";
}


AosJimoParserObjPtr
AosJimoParser::createJimoParser(AosRundata *rdata)
{
	return OmnNew AosJimoParser(*this);
}


bool
AosJimoParser::config(AosRundata *rdata)
{
	// In the current implementations, it assumes JimoParser
	// is configured through the configuration. In the future,
	// it will be defined by a doc.
	//
	// The configuration should be:
	// 	<jimo_parser>
	// 		<parserlet>
	// 		</parserlet>
	// 	</jimo_parser>

	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_rr(conf, rdata, false);

	AosXmlTagPtr jimo_parser_tag = conf->getFirstChild("jimo_parser");
	if (!jimo_parser_tag) return true;
	AosXmlTagPtr parser_tag = jimo_parser_tag->getFirstChild("parserlet");
	if (!parser_tag) return true;


	bool rslt = mRootParserlet->config(rdata, parser_tag);
	if (!rslt)
	{
		//	OmnDelete root_parserlet;
		return false;
	}

	// Configure charsets
	//	<parserlets>
	//		<charsets>
	//			<charset .../>
	//			<charset .../>
	//			...
	//			<charset .../>
	//		</charsets>
	//	</parserlets>
	AosXmlTagPtr charset_tags = jimo_parser_tag->getFirstChild("charsets");
	if (!charset_tags)
	{
		AosLogError(rdata, true, "missing_charset_conf") << enderr;
		return false;
	}

	AosXmlTagPtr name_charset_tag = charset_tags->getFirstChild("name_charset");
	if (!name_charset_tag)
	{
		AosLogError(rdata, true, "missing_name_charset") << enderr;
		return false;
	}

	if (!mNameCharset.config(rdata, name_charset_tag))
	{
		AosLogError(rdata, true, "name_charset_config_failed") << enderr;
		return false;
	}

	AosXmlTagPtr keyword_charset_tag = charset_tags->getFirstChild("keyword_charset");
	if (!keyword_charset_tag)
	{
		AosLogError(rdata, true, "missing_keyword_charset") << enderr;
		return false;
	}

	if (!mKeywordCharset.config(rdata, keyword_charset_tag))
	{
		AosLogError(rdata, true, "keyword_charset_config_failed") << enderr;
		return false;
	}
	AosXmlTagPtr expr_charset_tag = charset_tags->getFirstChild("expr_charset");
	if (!expr_charset_tag)
	{
		AosLogError(rdata, true, "missing_expr_charset") << enderr;
		return false;
	}

	if (!mExprCharset.config(rdata, expr_charset_tag))
	{
		AosLogError(rdata, true, "expr_charset_config_failed") << enderr;
		return false;
	}

	return true;
}


bool
AosJimoParser::getNameValueListStr(
		AosRundata *rdata,
		int &start_index,
		int &end_index)
{
	// This function looks for the pattern:
	//	'(' + arbitrary string that contains no ')' + ')'
	// If found, start_index is set to '(' and 'end_index'
	// is set to the character right after ')' if ')'
	// if not the last character, or -1.
	//
	// If no such pattern is found, 'start_index' is set to -1.
	int count = 0;

	start_index = -1;

	// Looking for '('
	while (mCrtIdx <= mDataLen)
	{
		char c = mData[mCrtIdx];
		if ('(' == c)
		{
			count++;
			start_index = mCrtIdx;
			mCrtIdx++;
			break;
		}
		mCrtIdx++;
	}

	// Looking for ')'
	while(mCrtIdx < mDataLen)
	{
		char c = mData[mCrtIdx];
		if ('(' == c )
		{
			count++;
		}

		if (')' == c)
		{
			if(0 == --count)
			{
				if (mCrtIdx == mDataLen)
				{
					end_index = -1;
					break;
				}

				mCrtIdx++;
				end_index = mCrtIdx;
				break;
			}
		}
		mCrtIdx++;
	}

	if (count > 0)
	{
		// There is '(' but no matching ')' (this is an error)
		start_index = -1;
	}
	return true;
}

bool
AosJimoParser::parse(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		const OmnString &stmt,
		vector<AosJqlStatementPtr> &statements,
		bool dft)
{
	// This function parses 'stmt', which may contain one or more
	// statements. 'stmt' is the string entered through a JQL client
	// or from a script file.
	mLock->lock();
	if (stmt == "")
	{
		mLock->unlock();
		return true;
	}
	mOrigStmt = stmt;
	mData = (char*)stmt.data();
	mDataLen = stmt.length();
	mCrtIdx = 0;
	bool parsed =  false;
	AosJqlStatementPtr ss;
	vector<AosJimoParserObjPtr> genobjs;
	while (mCrtIdx < mDataLen)
	{
		// This loop extracts one keyword each time. It uses the
		// keyword to find a Parserlet.

		int start_idx = mCrtIdx;
		bool rslt = getGenericObjsLocked(rdata, genobjs);
		//arvin 2015.07.24
		//JIMODB-106:clear incorrect message
		rdata->setJqlMsg("");

		aos_assert_rr(rslt, rdata, false);
		if (genobjs.size() > 0)
		{
			parsed = false;
			for (u32 i=0; i<genobjs.size(); i++)
			{
				// This may be handled by a generic obj. Note that it is possible
				// that there are multiple matching generic objs for this statement.
				// This should happen very rarely but possible. The first generic
				// obj that successfully parsed the statement will stop trying the
				// next generic obj.
				if (!genobjs[i]->parseJQL(rdata, this, prog, parsed, dft)) 
				{
					parsed = false;
				}
				if (parsed)
				{
					OmnString orig_stmt(&mData[start_idx], mCrtIdx - start_idx);
					ss = dynamic_cast<AosJqlStatement*>(genobjs[i].getPtr());
					ss->setOrigStatement(orig_stmt);
					if (!ss)
					{
						mLock->unlock();
						AosLogError(rdata, true, "internal_error") << enderr;
						return false;
					}
					//if((mCrtIdx+1) < mDataLen)
					//{
					//int durLen = mDataLen - mCrtIdx;
					//if(durLen <= 0)return false;
					if(mData[mCrtIdx] != ';') 
					{
						while(mCrtIdx <= mDataLen)
						{	
							if(mData[mCrtIdx] == ';') break;
							if(mData[mCrtIdx] == ' ') 
							{
								mCrtIdx++;
								continue;
							}
							mErrmsg << ss->getErrmsg() << " : misss \";\"";
							rdata->setJqlMsg(mErrmsg);
							mLock->unlock();
							return false;
						}
					}
					statements.push_back(ss);
					break;
				}
			}
			if (!parsed)
			{
				if (!parseOneJQLStatement(rdata, statements,start_idx))
				{
					mLock->unlock();
					//AosLogError(rdata, true, "failed_parsing_statement") << enderr;
					return false;
				}
			}
			mCrtIdx++;
			continue;
		}

		// No generic obj is found. It is a normal JQL statement
		// This is not a Jimo Statement. Use JQLParser to parse it.
		if (!parseOneJQLStatement(rdata, statements,start_idx))
		{
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();
	return true;
}


bool
AosJimoParser::registerParserlet(
		AosRundata *rdata,
		const OmnString &parserlet_name,
		const OmnString &jimo_name,
		const int version,
		bool &override_flag)
{
	// This function adds the generic obj 'jimo_name' to the parser
	// tree.
	aos_assert_rr(mRootParserlet, rdata, false);

	vector<OmnString> keywords;
	AosCharset::ErrorCode errcode;
	int nn = mKeywordCharset.parseWords(parserlet_name.data(),
				parserlet_name.length(), keywords, errcode);
	if (nn <= 0)
	{
		AosLogError(rdata, true, "failed_parse_keywords")
			<< AosFN("Keywords") << parserlet_name << enderr;
		return false;
	}

	mLock->lock();

	int idx = 0;
	bool rslt = mRootParserlet->registerParserlet(rdata,
			keywords, idx, jimo_name, version, override_flag);
	mLock->unlock();
	aos_assert_rr(rslt, rdata, false);
	return true;
	/*
	while (next_keyword != "")
	{
		keywords.push_back(crt_keyword);
		AosParserletPtr pp = parserlet->getParserlet(rdata, crt_keyword);
		if (!pp)
		{
			// There is no parserlet for keyword 'crt_keyword'. If 'next_keyword'
			// is empty, this is a leaf node. Add the generic obj t oit.
			// Otherwise, add a parserlet [crt_keyword, parserlet], and
			// repeat for the next keyword.
			if (next_keyword == "")
			{
				if (!parserlet->addGenericObj(rdata, keywords, jimo_name))
				{
					mLock->unlock();
					AosLogError(rdata, true, "failed_registering_parserlet")
						<< AosFN("keywords") << parserlet_name << enderr;
					return false;
				}

				mLock->unlock();
				return true;
			}

			// next_keyword is not empty but the current one does not exist
			// in 'parserlet'. Need to add the keyword to it.
			pp = parserlet->addParserlet(rdata, crt_keyword,crt_keyword);
			if (!pp)
			{
				mLock->unlock();
				AosLogError(rdata, true, "internal_error")
					<< AosFN("Keywords") << parserlet_name << enderr;
				return false;
			}

			crt_keyword = next_keyword;
			parserlet = pp;
			continue;
		}

		// Found the parserlet for crt_keyword.
		if (next_keyword == "")
		{
			// The parserlet already exists.
			if (!parserlet->appendGenericObj(rdata, jimo_name))
			{
				mLock->unlock();
				AosLogError(rdata, true, "failed_registering_parserlet")
					<< AosFN("keywords") << parserlet_name << enderr;
				return false;
			}

			mLock->unlock();
			return true;
		}

		parserlet = pp;
		crt_keyword = next_keyword;
	}
	*/

	mLock->unlock();
	AosLogError(rdata, true, "internal_error")
		<< AosFN("ParserletName") << parserlet_name << enderr;
	return false;
}

// arvin 2015/3/31

bool 
AosJimoParser::nextStatement(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &expr_str,
		AosJqlStatementPtr &statement,
		bool dft)
{
	int start_idx = mCrtIdx;
	OmnString keyword = nextKeyword(rdata);
	if(keyword.toLower() == "end" || keyword.toLower() == "endif")
	{
		return true;
	}
	if(keyword.toLower() == "else")
	{
		expr_str = "else";
		start_idx = mCrtIdx;
	}
	int end_idx = mOrigStmt.indexOfWithEscape(mCrtIdx,';');
	if(end_idx == -1)
	{
		end_idx = mDataLen;
	}
	OmnString stmt = OmnString(&mData[start_idx],end_idx-start_idx+1);
	if(stmt == "")
	{
		mErrmsg << "miss the statement!";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser,rdata,0);
	vector<AosJqlStatementPtr> statements;
	jimo_parser->parse(rdata,prog, stmt, statements, dft);
	if(statements.size() != 1)
	{
		mErrmsg = rdata->getJqlMsg();
		if(mErrmsg == "")
		{
			mErrmsg << stmt << " parse failed!";
		}
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	statement = statements[0];
	mCrtIdx = end_idx + 1;
	return true;
}

bool 
AosJimoParser::peekNextChar(const char character)
{
	if(mData[mCrtIdx] == character)
		return true;
	return false;
}

bool 
AosJimoParser::parseProcedureParms(AosRundata *rdata,vector<OmnString> &parms)
{
	parms.clear();
	int start_idx = mCrtIdx, end_idx;
	bool rslt = getNameValueListStr(rdata, start_idx, end_idx);
	aos_assert_rr(rslt, rdata, false);
	mCrtIdx = start_idx+1;
	while(mCrtIdx <  end_idx-1)
	{
		OmnString procdure_parm = nextProcdureParm(rdata);
		if(procdure_parm == "")
		{
			//no parm exist
			break;
		}
		parms.push_back(procdure_parm);
	}
	mCrtIdx = end_idx;
	return true;
}



bool
AosJimoParser::getNameValueList(
		AosRundata *rdata,
		vector<AosExprObjPtr> &name_values)
{
	// This function assumes the following:
	//	keyword ... keyword
	//	(
	//		name:value,
	//		name:value,
	//		...
	//	);
	// The parser stops right after the last keyword, which can be
	// white spaces or '('.
	

	AosNameValueParser parser;
	AosNameValueDoc doc;
	OmnScreen << "11111111111" << endl;
	if (!parser.parse(rdata, mData, mDataLen, mCrtIdx, doc)) return false;

	doc.reset();
	AosExprObjPtr expr;
	while (doc.hasMore())
	{
		bool rslt = doc.nextField(rdata, expr);
		if (!rslt)
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
		aos_assert_rr(expr->getType() == AosExprType::eNameValue, rdata, false);

		name_values.push_back(expr);
	}
	return true;
/*
	// Chen Ding, 2015/05/25

	AosJQLParserObj* jql_parser = Jimo::jimoGetJQLParser(rdata);
	aos_assert_rr(jql_parser, rdata, false);
	
	int start_idx, end_idx;
	bool rslt = getNameValueListStr(rdata, start_idx, end_idx);
	aos_assert_rr(rslt, rdata, false);
	AosExprObjPtr expr;
	if (start_idx >= 0)
	{
		if (end_idx == -1)
		{
			// This means the end of the pattern is also the end
			// of the string. This is normally incorrect.
			expr = jql_parser->parseJQL(OmnString(&mData[start_idx]), true, rdata);
		}
		else
		{
			aos_assert_rr(end_idx >= 0 && end_idx < mDataLen, rdata, false);
			char c1 = mData[end_idx];
			mData[end_idx] = ';';

			char c2 = mData[end_idx+1];
			mData[end_idx+1] = 0;

			expr = jql_parser->parseJQL(OmnString(&mData[start_idx]), true, rdata);
			mData[end_idx] = c1;
			mData[end_idx+1] = c2;
		}

		if (!expr)
		{
			//AosLogError(rdata, true, "failed_parsing_name_value_list")
			//	<< AosFN("Statement") << mData
			//	<< AosFN("Position") << start_idx << enderr;
			
			return false;
		}

OmnScreen << "Expr1: " << expr->dump() << endl;
		if (expr->getType() != AosExprType::eBrackets)
		{
			AosLogError(rdata, true, "expr_type_incorrect")
				<< AosFN("exprType") << expr->getType()
				<< enderr;
			return false;
		}

		AosExprBracketsObj* bracket = dynamic_cast<AosExprBracketsObj*>(expr.getPtr());

		name_values = bracket->getExprList(rdata);
		for (u32 i=0; i<name_values.size(); i++)
		{
OmnScreen << "Expr: " << name_values[i]->dump() << endl;
			if (name_values[i]->getType() != AosExprType::eNameValue)
			{
				AosLogError(rdata, true, "not_name_value") << enderr;
				return false;
			}
		}
		return true;
	}
	return false;
	*/
}


OmnString
AosJimoParser::getParmStr(
		AosRundata *rdata,
		const OmnString &parm,
		vector<AosExprObjPtr> &name_values)
{
	if(name_values.size() <= 0) return "";
	OmnString pname = parm;
	OmnString value;
	AosExprObjPtr expr;
	for(itr = name_values.begin(); itr != name_values.end();itr++)
	{
		expr = *itr;
		OmnString name = expr->getName();
		if(pname.toLower() == name.toLower())
		{
			value = expr->getValueAsExpr()->dumpByNoQuote();
			name_values.erase(itr);
			break;
		}
	}
	return value;
}


int
AosJimoParser::getParmInt(
		AosRundata *rdata,
		const OmnString &parm,
		vector<AosExprObjPtr> &name_values)
{
	if(name_values.size() <= 0)	return -1;

	OmnString pname = parm;
	OmnString value_str;
	AosExprObjPtr expr;
	int value = -1;
	for(itr = name_values.begin(); itr != name_values.end();itr++)
	{
		expr = *itr;
		OmnString name = expr->getName();
		if(pname.toLower() == name.toLower())
		{
			//value = name_values[i]->dump();
			value_str = expr->getValueAsExpr()->dumpByNoQuote();
			if (value_str == "")
				value = -1;
			else
				value = value_str.toInt();
			name_values.erase(itr);
			break;
		}
	}
	return value;
}


AosExprObjPtr
AosJimoParser::getParmExpr(
		AosRundata *rdata,
		const OmnString &parm,
		vector<AosExprObjPtr> &name_values)
{
	if(name_values.size() <= 0)	return 0;
	
	OmnString pname = parm;
	AosExprObjPtr expr,expr_value;
	for(itr = name_values.begin();itr != name_values.end();itr++)
	{
		expr = *itr;
		OmnString name = expr->getName();
		if(pname.toLower() == name.toLower())
		{
			expr_value = expr->getValueAsExpr();
			name_values.erase(itr);
			break;
		}
	}
	return expr_value;
}


bool
AosJimoParser::parmExist(
		AosRundata *rdata,
		const OmnString &parm,
		vector<AosExprObjPtr> &name_values)
{
	if(name_values.size() <= 0) return false;
	OmnString pname = parm;
	AosExprObjPtr expr;
	aos_assert_r(name_values.size()>0,false);
	for(itr = name_values.begin(); itr != name_values.end();itr++)
	{
		expr = *itr;
		OmnString name = expr->getName();
		if(pname.toLower() == name.toLower())
		{
			return true;
		}
	}
	return false;
}


bool
AosJimoParser::getParmArray(
		AosRundata *rdata,
		const OmnString &name,
		vector<AosExprObjPtr> &name_values,
		vector<AosExprObjPtr> &values)
{
	// This function assumes:
	//	keyword ...
	//	(
	//		Name:[expr, expr, ...]
	//	)
	// It finds the corresponding name-value pair. If not exist,
	// it is an error. Otherwise, the parameter value should
	// be an array of expressions. Otherwise, it is an error.
	if(name_values.size() <= 0) return false;
	OmnString pname = name;
	AosExprObjPtr expr;
	for (itr = name_values.begin(); itr != name_values.end();itr++)
	{
		expr = *itr;
		OmnString name1 = expr->getName();
		if (name1.toLower() == pname.toLower())
		{
			AosExprObjPtr expr_value = expr->getValueAsExpr();
			if(!expr_value) return false;
			if (expr_value->getType() != AosExprType::eBrackets)
			{
				return false;
			}
			values = expr_value->getExprList();
			name_values.erase(itr);
			return true;
		}
	}
	return false;
}


bool
AosJimoParser::getParmArrayStr(
		AosRundata *rdata,
		const OmnString &parm,
		vector<AosExprObjPtr> &name_values,
		vector<OmnString> &parmArray)
{
	// This function assume
	//	Keyword ...
	//	(
	//		name:[expr, expr, ...]
	//	)
	OmnString pname = parm;
	AosExprObjPtr expr;
	for (itr = name_values.begin(); itr != name_values.end(); itr++)
	{
		expr = *itr;
		OmnString name = expr->getName();
		if (name.toLower() == pname.toLower())
		{
			AosExprObjPtr expr_value = expr->getValueAsExpr();
			if(!expr_value) return false;
			if (expr_value->getType() != AosExprType::eBrackets)
			{
				return false;
			}
			vector<AosExprObjPtr> values = expr_value->getExprList();
			aos_assert_r(values.size()>0,false);
			for(size_t  i = 0;i < values.size();i++)
			{
				OmnString value = values[i]->dumpByNoQuote();
				parmArray.push_back(value);
			}
			name_values.erase(itr);
			return true;
		}
	}
	return false;
}


bool
AosJimoParser::nextSemiColon(AosRundata *rdata)
{
	// The caller expects the next character is ';'.
	mCrtIdx = mOrigStmt.indexOfWithEscape(mCrtIdx,';');
	return true;
}
 

bool
AosJimoParser::parseOneJQLStatement(
		AosRundata *rdata,
		vector<AosJqlStatementPtr> &statements,
		const int start_idx)
{
	// This is not a generic obj. Use JQLParser to parse it. It parses
	// just one statement. To do so, it scans from the current position
	// all the way to the next ';', excluding escaped ';'.
	bool rslt;
	int end_idx = mOrigStmt.indexOfWithEscape(mCrtIdx,';');
	if(end_idx == -1)
		end_idx = mDataLen;
	OmnString stmt(&mData[start_idx],end_idx-start_idx+1);

	int rslt_value = -1;

	if (stmt.indexOf("{", 0) < 0)
	{
		gAosJQLParser.lock();
		rslt_value = AosParseJQL(stmt.getBuffer(), true);
		gAosJQLParser.unlock();	
		if (rslt_value)
		{
			mErrmsg = rdata->getJqlMsg();
			if(mErrmsg == "")
			{
				extern OmnString getErrMsg();
				mErrmsg = getErrMsg();
			}
			rdata->setJqlMsg(mErrmsg);
			return false;
		}
	}

	if(rslt_value != 0)
	{
		rslt = gAosJQLParser.parseWithRegex(stmt,rdata);
		if(!rslt)
		{
			rslt_value = AosParseJQL(stmt.getBuffer(),true);
			if(rslt_value)
			{
				AosLogError(rdata, false, "stdin:syntax_error")
					<< AosFN("Statement") << stmt << enderr;
				return false;
			}
		}
	}
	vector<AosJqlStatementPtr> stmts = gAosJQLParser.getStatements();
	if (stmts.size() == 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement") << enderr;
		return false;
	}

	OmnString errmsg = gAosJQLParser.getErrmsg();
	if (errmsg != "")
	{
		AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;
		return false;
	}
	for(size_t i = 0;i < stmts.size();i++)
	{
		statements.push_back(stmts[i]);
	}
	mCrtIdx = end_idx+1;

	return true;
}



bool
AosJimoParser::getGenericObjsLocked(
		AosRundata *rdata,
		vector<AosJimoParserObjPtr> &genobjs)
{
	// This function uses mOrigStmt to retrieve GenericObj
	// based on keywords. Starting from beginning, it gets
	// the next keyword from mOrigStmt, and then checks whether
	// the Parser Tree has matching GenericObj or Parserlet.

	AosParserletPtr parserlet = mRootParserlet;
	genobjs.clear();
	while (parserlet)
	{
		//	Retrieves the next keyword. A keyword is made of digits,
		// letters, underscores, and dashes.
		OmnString keyword = nextKeyword(rdata).toLower();
		rdata->getJqlMsg();
		if (keyword == "")
		{
			// No more keywords. Did not find generic objs for this
			// statement.
			return true;
		}

		AosParserletPtr pp = 0;
		parserlet->getParserlet(rdata, keyword, pp, genobjs);
		if (genobjs.size() > 0) return true;
		if (!pp) return true;

		parserlet = pp;
	}

	return true;
}

OmnString
AosJimoParser::nextProcdureParm(AosRundata *rdata)
{
	return nextWordPriv(rdata, mNameCharset);
}


OmnString
AosJimoParser::nextObjName(AosRundata *rdata)
{
	return nextWordPriv(rdata, mNameCharset);
}


OmnString
AosJimoParser::nextKeyword(AosRundata *rdata)
{
	return nextWordPriv(rdata, mKeywordCharset);
}

OmnString
AosJimoParser::nextExpr(AosRundata *rdata)
{
	return nextWordPriv(rdata, mExprCharset);
}


OmnString
AosJimoParser::nextWordPriv(AosRundata *rdata, const AosCharset &charset)
{
	if (mCrtIdx >= mDataLen) return "";
	AosCharset::ErrorCode errcode = AosCharset::eNoError;
	int start_idx, word_len;
	char invalid_char;
	charset.nextWord(mData, mDataLen, mCrtIdx, start_idx, word_len, invalid_char, errcode);
	rdata->getJqlMsg();
	if(errcode == AosCharset::eInvalidChar)
	{
		OmnString msg ;
		msg << "illegal character \"" << invalid_char << "\" is not allowed";
		rdata->setJqlMsg(msg);
	}
	if (word_len == 0) return "";
	return OmnString(&mData[start_idx], word_len);

}


bool
AosJimoParser::parseNameValueList(
		AosRundata *rdata,
		vector<AosExprObjPtr> &name_values)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosJimoParser::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosJimoParser::runJQL(
		AosRundata *rdata,
		const AosJimoProgObjPtr &job,
		const OmnString &verb_name)
{
	OmnNotImplementedYet;
	return false;
}
