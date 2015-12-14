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
#ifndef AOS_JQLStatement_JqlStmtHBase_H
#define AOS_JQLStatement_JqlStmtHBase_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

#include "HBase/HBaselib/Hbase.h" 
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>


using namespace std;
using boost::shared_ptr;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;


class AosJqlStmtHBase : public AosJqlStatement
{
public:

	enum E
	{
		eInvalid,
		ePrefix,
		eStart,
		eMax
	};

	struct Table
	{
	public:
		OmnString			mTableName;
		OmnString			mFileName;
		OmnString			mRowKeyPrefix;
		OmnString			mRowKeyStop;
		vector<string>		mFields;
		map<string, bool>		mNeedGetValuesMap;
		int					mFileMaxSize;
		OmnString			mRowKeyStart;
		E					mType;
		int					mRowKeyLen;
		bool				mIsInit;
		Table();
		~Table();

	public:
		bool config(const AosXmlTagPtr &table);
		E toEnum(OmnString type)
		{
			if (type == "pf") return ePrefix;
			if (type == "start") return eStart;
			return eInvalid;
		}
		bool isValid(const E opr)           
		{                                                    
			return (opr >eInvalid && opr < eMax);
		} 
	};

	OmnString						mConfig;
	OmnString						mAddr;
	int								mPort;

	//boost::shared_ptr<HbaseClient>  mClient;
	//boost::shared_ptr<TTransport> 	mTransPort;
	OmnString						mDataDir;
	OmnString						mFieldDelimiter;
	OmnString						mRecordDelimiter;
	bool							mIsAllTable;
	vector<Table>					mTables;


public:
	AosJqlStmtHBase();
	~AosJqlStmtHBase();

	void setConfig(OmnString conf){mConfig = conf;};

	bool runHBase(const AosRundataPtr &rdata);  
	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	bool conn(OmnString addr, int prot);

	bool read_data_from_hbase();
	bool write_data_to_file(
		AosJqlStmtHBase::Table &table,
		const std::vector<TRowResult> &rowResult,
		FILE *ff,
		int &offset);

	virtual void dump();
	virtual AosJqlStatement *clone();
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);

private:

};

#endif
