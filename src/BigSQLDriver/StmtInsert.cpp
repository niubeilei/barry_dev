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
// Below is from http://dev.mysql.com/doc/refman/5.6/en/insert.html
// INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE]
//     [INTO] tbl_name
//     [PARTITION (partition_name,...)] 
//     [(col_name,...)]
//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
//     [ ON DUPLICATE KEY UPDATE
//          col_name=expr
//          [, col_name=expr] ... ]
//
// Modification History:
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigSQLDriver/StmtInsert.h"

#include "BigSQLDriver/BigSQLParser.h"
#include "BigSQLDriver/StmtNames.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;

AosSqlStmtInsert::AosSqlStmtInsert(const bool flag)
:
AosSqlStmtParser(AOSSQLSTMTNAME_INSERT, AosSqlStmtType::eInsert, flag)
{
}


AosSqlStmtInsert::~AosSqlStmtInsert()
{
}


bool
AosSqlStmtInsert::parse(
	const OmnString &statement, 
	OmnStrParser1 &parser,
	OmnString &stmt,
	const AosBigSQLParser *bigsql,
	const AosRundataPtr &rdata)
{
	// INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE]
	//     [INTO] tbl_name
	//     [PARTITION (partition_name,...)] 
	//     [(col_name,...)]
	//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
	//     [ ON DUPLICATE KEY UPDATE
	//          col_name=expr
	//          [, col_name=expr] ... ]
	
	stmt = "<insert stmt_id=\"insert\" ";
	// AosXmlTagPtr stmt = AosXmlParser::parse(str AosMemoryCheckerArgs);

	bool low_priority_found = false;
	bool high_priority_found = false;
	bool delayed_found = false;
	bool ignore_found = false;

	bool is_tablename_case_sensitive = bigsql->isTableNameCaseSensitive();

	// The following loop scans for the keywords: 
	// 	LOW_PRIORITY, 
	// 	HIGH_PRIORITY,
	// 	DELAYED,
	// 	IGNORED, 
	// 	INTO
	// It breaks out as soon as it encounters a word that is not one of these
	// keywords. There shall be no more than five keywords. Keywords are
	// case insensitive.
	OmnString tablename;
	bool finished = false;
	while (!finished)
	{
		tablename = parser.nextWord("");
		if (tablename.length() <= 0)
		{
			rdata->setErrmsg(AOSDICTERM("invalid_statement_001", rdata));
			return false;
		}

		switch (tablename[0])
		{
		case 'l':
		case 'L':
			 if (!is_tablename_case_sensitive)
			 {
				 tablename.toLower();
				 if (tablename == "low_priority")
				 {
					 if (low_priority_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_low_priority", rdata));
						 return false;
					 }
					 low_priority_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
					 continue;
				 }
			 }
			 else
			 {
			 	 OmnString ww = tablename;
			 	 ww.toLower();
			 	 if (ww == "low_priority")
			 	 {
					 if (low_priority_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_low_priority", rdata));
						 return false;
					 }
					 low_priority_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
				 	 continue;
			 	 }
			 }

			 // The 'tablename' is not one of the keyword. It should be the table name.
			 finished = true;
			 break;

		case 'd':
		case 'D':
			 if (!is_tablename_case_sensitive)
			 {
				 tablename.toLower();
				 if (tablename == "delayed")
				 {
					 if (delayed_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_delayed", rdata));
						 return false;
					 }
					 delayed_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
					 continue;
				 }
			 }
			 else
			 {
			 	 OmnString ww = tablename;
			 	 ww.toLower();
			 	 if (ww == "delayed")
			 	 {
					 if (delayed_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_delayed", rdata));
						 return false;
					 }
					 delayed_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
				 	 continue;
			 	 }
			 }

			 // The 'tablename' is not one of the keyword. It should be the table name.
			 finished = true;
			 break;

		case 'h':
		case 'H':
			 if (!is_tablename_case_sensitive)
			 {
				 tablename.toLower();
				 if (tablename == "high_priority")
				 {
					 if (high_priority_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_high_priority", rdata));
						 return false;
					 }
					 high_priority_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
					 continue;
				 }
			 }
			 else
			 {
			 	 OmnString ww = tablename;
			 	 ww.toLower();
			 	 if (ww == "high_priority")
			 	 {
					 if (high_priority_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_high_priority", rdata));
						 return false;
					 }
					 high_priority_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
				 	 continue;
			 	 }
			 }

			 // The 'tablename' is not one of the keyword. It should be the table name.
			 finished = true;
			 break;

		case 'i':
		case 'I':
			 if (!is_tablename_case_sensitive)
			 {
				 tablename.toLower();
				 if (tablename == "ignore")
				 {
					 if (ignore_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_ignore", rdata));
						 return false;
					 }
					 ignore_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
					 continue;
				 }
			 }
			 else
			 {
			 	 OmnString ww = tablename;
			 	 ww.toLower();
			 	 if (ww == "ignore")
			 	 {
					 if (ignore_found)
					 {
						 rdata->setErrmsg(AOSDICTERM("dup_ignore", rdata));
						 return false;
					 }
					 ignore_found = true;
					 stmt << AOSTAG_LOW_PRIORITY << "=\"true\" ";
				 	 continue;
			 	 }
			 }

			 // The 'tablename' is not one of the keyword. It should be the table name.
			 finished = true;
			 break;

		default:
			 // The 'tablename' is not one of the keyword. It should be the table name.
			 finished = true;
			 break;
		}
	}

	if (tablename.length() <= 0)
	{
		rdata->setErrmsg(AOSDICTERM("missing_table_name", rdata));
		return false;
	}

	if (!bigsql->isValidTablename(tablename, rdata))
	{
		return false;
	}

	stmt << AOSTAG_TABLENAME << "=\"" << tablename << "\" ";

	// Next step, process the PARTITION
	//     [PARTITION (partition_name,...)] 
	//     [(col_name,...)]
	//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
	//     [ ON DUPLICATE KEY UPDATE
	//          col_name=expr
	//          [, col_name=expr] ... ]
	OmnString word = parser.nextWord("");
	if (word.length() <= 0)
	{
		// This is to create an empty doc. 
		stmt << "/>";
		return true;
	}

	if (word[0] == 'P' || word[0] == 'p')
	{
		word.toLower();
		if (word == "partition")
		{
			// Partitions are not supported in JimoDB since tables are 
			// automatically partitioned. Partition will be ignored.
			parser.skipParenthesis('\'', rdata);
			word = parser.nextWord("");
			if (word.length() <= 0)
			{
				// It is to create an empty doc. 
				stmt << "/>";
				return true;
			}
		}
	}

	// Next step, process (col_name, col_name, ...)
	//     [(col_name,...)]
	//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
	//     [ ON DUPLICATE KEY UPDATE
	//          col_name=expr
	//          [, col_name=expr] ... ]
	vector<OmnString> colnames;
	if (word[0] == '(')
	{
		parser.getValueList(colnames, ",", ')', eMaxFields, rdata);
	}

	// Next process Values
	//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
	//     [ ON DUPLICATE KEY UPDATE
	//          col_name=expr
	//          [, col_name=expr] ... ]
	word = parser.nextWord("");
	word.toLower();
	if (word[0] != 'v') 
	{
		rdata->setErrmsg(AOSDICTERM("expect_value_after_tablename", rdata));
		return 0;
	}

	// Next process ({expr | DEFAULT), (expr | DEFAULT), ...), (...), (...), ...
	//     {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
	//     [ ON DUPLICATE KEY UPDATE
	//          col_name=expr
	//          [, col_name=expr] ... ]
	word = parser.nextWord("");
	stmt << ">";
	int count = 0;
	while (word[0] == '(')
	{
		count++;
		if (!parseOneValueSet(parser, colnames, stmt, rdata)) return false;
		word = parser.nextWord("");
	}

	if (count <= 0)
	{
		// (value, value, ...) is mandatory.
		rdata->setErrmsg(AOSDICTERM("missing_values", rdata));
		return false;
	}
		
	// Next process [ON DUPLICATE ...]
	word = parser.nextWord("");
	if (word.length() <= 0)
	{
		// This is the end of the statement.
		stmt << "</insert>";
		return true;
	}

	OmnNotImplementedYet;
	return false;
}


bool
AosSqlStmtInsert::parseOneValueSet(
		OmnStrParser1 &parser,
		const vector<OmnString> &colnames,
		OmnString &stmt,
		const AosRundataPtr &rdata)
{
	// This function parses one set of values. The input is in
	// the form:
	//     ({expr | DEFAULT}, {expr | DEFAULT}, ..., {expr | DEFAULT})
	// If 'colnames' is not empty, the number of columns should match
	// the number of values. Otherwise, it is an error.
	// 'parser' points right after '('. 
	OmnNotImplementedYet;
	return false;
}

