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
// 2014/07/10 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "HBase/Exe/ExportHbaseData.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <iostream>

#include <boost/foreach.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace std;

typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::map<std::string,TCell> CellMap;
typedef std::vector<ColumnDescriptor> ColVec;

ExportHbaseData::ExportHbaseData()
{
	mIsAllTable = false;
}

ExportHbaseData::~ExportHbaseData()
{
}

bool
ExportHbaseData::init(string conf_name)
{
	ptree conf;
	read_xml(conf_name, conf);

	string addr = conf.get<string>("config.hbase_addr");
	int port = conf.get<int>("config.hbase_port");
	mDataDir = conf.get<string>("config.data_dir");
	mFieldDelimiter = conf.get<string>("config.field_delimiter");
	mRecordDelimiter = conf.get<string>("config.record_delimiter");
	mIsAllTable = conf.get<bool>("config.read_all");

	BOOST_AUTO(tables, conf.get_child("config.hbase"));
	for (BOOST_AUTO(pos, tables.begin()); pos != tables.end(); ++pos) 
	{
		Table table_s;
		ptree t = pos->second;
		table_s.init(t);
		mTables.push_back(table_s);
	}  
	bool rslt = conn(addr, port);
	return rslt;
}


bool
ExportHbaseData::conn(string addr, int port)
{
	bool isFramed = false;
	boost::shared_ptr<TTransport> socket(new TSocket(addr.data(), boost::lexical_cast<int>(port)));

	if (isFramed) {
		mTransPort.reset(new TFramedTransport(socket));
	} else {
		mTransPort.reset(new TBufferedTransport(socket));
	}

	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(mTransPort));
	mClient = boost::make_shared<HbaseClient>(protocol);
	try {
		mTransPort->open();
		std::cout << "scanning tables..." << std::endl;
		StrVec tables;
		mClient->getTableNames(tables);
		for (StrVec::const_iterator it = tables.begin(); it != tables.end(); ++it)
		{
			std::cout << " found: " << *it << std::endl;
		}
	} catch (const TException &tx) {
		std::cerr << "ERROR: " << tx.what() << std::endl;
		return false;
	}
	return true;
}

bool 
ExportHbaseData::start()
{
	read_data_from_hbase();
	return true;
}


bool
ExportHbaseData::read_data_from_hbase()
{
	const std::map<Text, Text> dummyAttributes;
	for (int32_t i = 0; i < mTables.size(); i++)
	{
		string filename = "";
		int offset = 0;
		filename = mDataDir + mTables[i].mFileName; 
		FILE *ff = fopen64(filename.data(), "w+b");
		if (!ff)
		{
			cerr << "ERROR : file is null.";
		}
		string row_perfix = mTables[i].mRowKeyPrefix;
		string row_key = mTables[i].mRowKeyStart;
		int row_key_int  = atoi(row_key.data());
		size_t len = mTables[i].mRowKeyLen;
		std::vector<std::string> rows;
		std::vector<TRowResult> rowResult;
		if ( mTables[i].mType > 0)
		{
			ScannerID  scannId;
			StrMap attributes;
			if (mTables[i].mType == 1)
				scannId = mClient->scannerOpenWithStop(mTables[i].mTableName, mTables[i].mRowKeyStart , "", mTables[i].mFields, attributes);
			else
				scannId = mClient->scannerOpenWithPrefix(mTables[i].mTableName, mTables[i].mRowKeyPrefix, mTables[i].mFields, attributes);

			while(1 && scannId)
			{
				mClient->scannerGetList(rowResult, scannId, 10000);
				write_data_to_file(mTables[i].mFields, rowResult, ff, offset);
				if (rowResult.empty()) 
				{
					mClient->scannerClose(scannId);	
					break;
				}
				rowResult.clear();
			}
		}
		fclose(ff);
	}
	return true;
}


bool
ExportHbaseData::createTable(ExportHbaseData::Table table)
{	
	std::cout << "scanning tables..." << std::endl;
	StrVec tables;
	mClient->getTableNames(tables);
	for (StrVec::const_iterator it = tables.begin(); it != tables.end(); ++it) 
	{
		std::cout << " found: " << *it << std::endl;
		if (table.mTableName == *it) {

			if (mClient->isTableEnabled(*it)) {                        
				std::cout << " disabling table: " << *it << std::endl; 
				mClient->disableTable(*it);                              
			}                                                        
			std::cout << " deleting table: " << *it << std::endl;    
			mClient->deleteTable(*it);                                 
		}
	}
	ColVec columns;

	for (int32_t i = 0; i < table.mFields.size(); i++)
	{
		columns.push_back(ColumnDescriptor());
		columns.back().name = table.mFields[i];
		columns.back().maxVersions = 50;
	}

	std::cout << "creating table: " << table.mTableName << std::endl;
	try {
		mClient->createTable(table.mTableName, columns);
	} catch (const AlreadyExists &ae) {
		std::cerr << "WARN: " << ae.message << std::endl;
	}
	return true;
}


bool
ExportHbaseData::write_data_to_file(
		vector<string> fields,
		const std::vector<TRowResult> &rowResult,
		FILE *ff,
		int &offset)
{
	if (!ff) 
	{
		cerr << "ERROR : fiel is null." << endl;
		return false;
	}
	std::string buff = "";
	CellMap::const_iterator it;
	std::string field = "";
	for (size_t i = 0; i < rowResult.size(); i++)
	{
		buff += rowResult[i].row + mFieldDelimiter;
		for (int j = 0; j < fields.size(); j++)
		{
			field = fields[j];
			for (it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
			{
				if (field != it->first)
					continue;

				if (j == 0)
					buff += it->second.value;
				else
					buff += mFieldDelimiter + it->second.value;
			}
		}
		buff += '\n';
	}
	if (offset != ftell(ff))
	{
		int rslt = fseeko64(ff, offset, SEEK_SET);
		if (rslt == -1)          
		{
			std::cout << "ERROR: " << std::endl;
		}
	}
	int nn = fwrite(buff.data(), 1, buff.length(), ff);
	if (nn != buff.length())
	{
		std::cout << "ERROR: " << std::endl;
	}
	offset +=  buff.length();
}


int
ExportHbaseData::str_spilt(
		vector<string> &stringsV,
		const char *orig_str, 
		const char *sep) 
{
	if (!orig_str || strlen(orig_str) == 0) 
	{
		return 0;
	}
	if (!sep) return -1;

	int idx = 0;
	int start_idx = 0;
	int strl = strlen(orig_str);
	char c;
	char ch = sep[0];
	int sep_len = strlen(sep);
	int entries = 0;
	while ((c = orig_str[idx]))
	{
		if (c == ch && strncmp(&orig_str[idx], sep, sep_len) == 0)
		{
			int len = idx - start_idx; 
			stringsV.push_back(string(&orig_str[start_idx], len));
			if (start_idx >= strl) return entries;
			++entries;
			idx += sep_len;
			start_idx = idx;
			if (entries >= 5000) return entries;
			continue;
		}
		idx++;
	}

	int len = idx - start_idx;
	stringsV.push_back(string(&orig_str[start_idx], len));
	return ++entries;
}


bool
ExportHbaseData::getLine(
		FILE *ff,
		string &buff,
		bool &finished)
{
	if (!ff) {
		std::cerr << "ERROR : File is null." << endl;
		return false;
	}

	finished = false;
	buff = "";
	int c;

	while (1)
	{
		c = fgetc(ff);
		if (c == EOF)
		{
			finished = true;
			return true; 
		}

		if (c == '\n')
		{
			buff += c;
		}
	}
	return true;
}

ExportHbaseData::Table::Table()
{
	mFileMaxSize = 0;
	mRowKeyLen = 0;
	mIsNeedWrite = false;
	mIsInit = false;
}


ExportHbaseData::Table::~Table()
{
}


bool
ExportHbaseData::Table::init(ptree &table)
{
	write_xml(cout, table);
	BOOST_AUTO(fields, table.get_child("table.fields"));
	mTableName = table.get<string>("table.name");
	mInputFileName = table.get<string>("table.input_file");
	mIsNeedWrite = table.get<bool>("table.write");

	mFileName = table.get<string>("table.file.<xmlattr>.file_name");
	mFileMaxSize = table.get<int>("table.file.<xmlattr>.file_size");

	mRowKeyPrefix = table.get<string>("table.row_key.<xmlattr>.prefix");
	mRowKeyStart = table.get<string>("table.row_key.<xmlattr>.start");
	mRowKeyLen = table.get<int>("table.row_key.<xmlattr>.len");
	mType = table.get<int>("table.row_key.<xmlattr>.type");


	for (BOOST_AUTO(pos, fields.begin()); pos != fields.end(); ++pos) 
	{
		mFields.push_back(pos->second.data());
	}
	mIsInit = true;
	return true;
}

