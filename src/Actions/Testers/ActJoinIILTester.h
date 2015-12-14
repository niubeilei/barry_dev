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
// 05/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_ActJoinIILTester_h
#define Aos_Actions_Testers_ActJoinIILTester_h

#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


class AosActJoinIILTester : virtual public OmnTestPkg
{
private:
	enum
	{
		eKey,
		eValue,
		eMaxMatchTypes
	};
	
	enum
	{
		eU64,
		eStr,
		eMaxColumnsTypes
	};
	
	enum
	{
		eInnerJoin,
		eOutJoin,

		eMaxJoinTypes
	};

	enum
	{
		eMinColumns = 3,
		eMaxColumns = 5,
		
		eMinStrLen = 10,
		eMaxStrLen = 15,

		eMinRecordSize = 20,
		eMaxRecordSize = 30
	};
	
	struct IILInfo
	{
		OmnString		mIILName;
		int				mFieldIdx;
	};
/*	
	struct IILScannerConf
	{
		OmnString 			mIILName;
		AosIILMatchType::E	matchtype;
		OmnString			fieldsep;
		OmnString			ignore_match_error;
		int					pagesize;
		OmnString			value1;
		OmnString			opr1;
		OmnString			value2;
		OmnString			opr2;
		vector<Selector>	selectors;
	};

	struct Selector
	{
		OmnString			entry_selector;
		OmnString			value_type;
		int					field_idx;
		bool				create_flag;
		OmnString			dft_value;
	};
*/

	int				mMatchType;
	int				mJoinType;
	int				mNumColumns;
	int				mColumnTypes[eMaxColumns];
	int				mColumnLens[eMaxColumns];
	
	int				mNumRecords;
	int				mRecordLen;
	char *			mRecords[eMaxRecordSize];	 			

	u64				mDocid;
	u64				mIILIDs;

	int				mNumIILs;
	IILInfo			mIILInfo[eMaxColumns];

	AosRundataPtr	mRundata;

public:
	AosActJoinIILTester();
	~AosActJoinIILTester() {};

	virtual bool start();

private:
	bool		actionRun(const AosXmlTagPtr &xml, const AosRundataPtr &rdata);
	bool		basicTest();
	bool		torturer();
	
	bool		reset();
	bool		createTable();
	bool		createIILs();
	
	
	
/*	
	bool		splitTable();
	bool		createConfForOneIIL(const int iilidx);
	OmnString	getPrimaryKeyValueType(const int iilidx);
	OmnString	createOneScanner(const int idx);
	OmnString	createConfs();
	OmnString	getEntrySelectorType();
	OmnString	getFieldSep();
	OmnString	getIgnoreMatchError();
	int			getPageSize();
	OmnString	getValue1();
	OmnString	getValue2();
	OmnString	getOpr1();
	OmnString	getOpr2();
	u64			generateDocid(
					const AosIILValueType::E value_type, 
					OmnString &value, 
					const int col_idx, 
					const int record_idx);
	bool 		createNewIILAsFamily(const int iilidx, const int col_idx);
	bool 		createNewIILAsKey(const int iilidx, const int col_idx);
	bool 		createNewIILAsMapper(const int iilidx, const int col_idx);
	OmnString	createIILName();
	AosIILValueType::E generateValueType();
	int			pickColumn();
	*/
};
#endif
