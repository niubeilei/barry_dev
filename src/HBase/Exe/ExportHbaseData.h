//////////////////////////////////////////////////////////////////////////
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
// 2014/07/10 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HBase_ExportHbaseData
#define AOS_HBase_ExportHbaseData

#include "HBase/HBaselib/Hbase.h" 

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include "/home/andy/Andy/boost/boost_1_55_0/boost/typeof/typeof.hpp"
#include "/home/andy/Andy/boost/boost_1_55_0/boost/property_tree/ptree.hpp"  
#include "/home/andy/Andy/boost/boost_1_55_0/boost/property_tree/xml_parser.hpp"  


#include <vector>
#include <map>


using namespace std;

using boost::shared_ptr;
using namespace boost::property_tree;  

using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;

class ExportHbaseData 
{
public:
	struct Table
	{
	public:
		string			mTableName;
		string			mFileName;
		string			mInputFileName;
		string			mRowKeyPrefix;
		string			mRowKeyStop;
		vector<string>	mFields;
		int				mFileMaxSize;
		string			mRowKeyStart;
		int				mType;
		int				mRowKeyLen;
		bool			mIsNeedWrite;
		bool			mIsInit;
		Table();
		~Table();

	public:
		bool init(ptree &table);
	};

	boost::shared_ptr<HbaseClient>  mClient;
	boost::shared_ptr<TTransport> 	mTransPort;
	string			mDataDir;
	string			mFieldDelimiter;
	string			mRecordDelimiter;
	bool			mIsAllTable;
	vector<Table>	mTables;

public:
	ExportHbaseData();
	~ExportHbaseData();
	bool start();
	bool init(string config_name);
	bool conn(string addr, int prot);
	bool read_data_from_hbase();
	bool write_data_to_file(
		vector<string> fields,
		const std::vector<TRowResult> &rowResult,
		FILE *ff,
		int &offset);

	bool getLine(
			FILE *ff,
			string &buff,
			bool &finished);

	int str_spilt(
			vector<string> &stringsV,
			const char *orig_str, 
			const char *sep);

	bool createTable(ExportHbaseData::Table table);
};

#endif
