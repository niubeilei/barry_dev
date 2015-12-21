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
//
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtHBase.h"

#include "SEInterfaces/ExprObj.h"

#include "Util/File.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"

#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>

/*
#include <boost/foreach.hpp>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace std;

typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::map<std::string,TCell> CellMap;
typedef std::vector<ColumnDescriptor> ColVec;
*/

AosJqlStmtHBase::AosJqlStmtHBase()
{
	mPort = 0;
	mIsAllTable = false;
}

AosJqlStmtHBase::~AosJqlStmtHBase()
{
}


bool
AosJqlStmtHBase::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOp == JQLTypes::eRun) return runHBase(rdata);
	AosSetEntityError(rdata, "JQL_HBase_run_err", "JQL HBase", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtHBase::runHBase(const AosRundataPtr &rdata)
{
	bool rslt = false;
	OmnString str;
	OmnFile file(mConfig, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.getLength() > 0)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	rslt = file.readToString(str);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr conf = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	if (!conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	rslt = config(rdata, conf);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	rslt = conn(mAddr, mPort);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	rslt = read_data_from_hbase();
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	return true;
}

bool
AosJqlStmtHBase::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	bool rslt = false;
	mAddr = conf->getAttrStr("hbase_addr");
	if (mAddr == "")
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mPort = conf->getAttrInt("hbase_port", 0);
	if (!mPort )
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr node = conf->getFirstChild("data_dir");
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mDataDir = node->getNodeText();
	if (mDataDir == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	node = conf->getFirstChild("field_delimiter");
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mFieldDelimiter = node->getNodeText();
	if (mFieldDelimiter == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	node = conf->getFirstChild("record_delimiter");
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mRecordDelimiter = node->getNodeText();
	if (mRecordDelimiter == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	node = conf->getFirstChild("read_all");

	if (node) mIsAllTable = node->getNodeText() == "true" ? true : false;

	AosXmlTagPtr tables = conf->getFirstChild("tables");
	if (!tables)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr table = tables->getFirstChild(true);
	while (table)
	{
		Table table_s;
		rslt = table_s.config(table);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		mTables.push_back(table_s);
		table = tables->getNextChild();
	}

	return true;
}

bool
AosJqlStmtHBase::conn(OmnString addr, int port)
{
	/*
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
	*/
	return true;
}


bool
AosJqlStmtHBase::read_data_from_hbase()
{
	/*
	const std::map<Text, Text> dummyAttributes;
	for (u32 i = 0; i < mTables.size(); i++)
	{
		int file_num = 0;
		OmnString filename = "";
		int offset = 0;
		filename << mDataDir <<  mTables[i].mFileName << "_" << file_num; 
		FILE *ff = fopen64(filename.data(), "w+b");
		if (!ff) {
			cerr << "ERROR : file is null.";
		}
		OmnString row_perfix = mTables[i].mRowKeyPrefix;
		OmnString row_key = mTables[i].mRowKeyStart;
		std::vector<OmnString> rows;
		std::vector<TRowResult> rowResult;
		if ( mTables[i].mType > 0)
		{
			ScannerID  scannId;
			StrMap attributes;
			if (mTables[i].mType == eStart)
				scannId = mClient->scannerOpenWithStop(mTables[i].mTableName.data(), 
						mTables[i].mRowKeyStart.data() , "", mTables[i].mFields, attributes);
			else
				scannId = mClient->scannerOpenWithPrefix(mTables[i].mTableName,
						mTables[i].mRowKeyPrefix, mTables[i].mFields, attributes);

			while(1 && scannId)
			{
//cout << "=================================== read " << OmnGetTime() << endl;
				mClient->scannerGetList(rowResult, scannId, 10000);
//cout << "=================================== read end" << OmnGetTime() << endl;
				write_data_to_file(mTables[i], rowResult, ff, offset);
//cout << "=================================== write end" << OmnGetTime() << endl;
				if (rowResult.empty()) 
				{
					mClient->scannerClose(scannId);	
					break;
				}
				rowResult.clear();

				if (offset > mTables[i].mFileMaxSize)
				{
					fclose(ff);
					file_num++;
					filename = "";
					filename << mDataDir <<  mTables[i].mFileName << "_" << file_num; 
					ff = fopen64(filename.data(), "w+b");
					if (!ff) {
						cerr << "ERROR : file is null.";
					}
					offset = 0;
				}
			}
		}
		fclose(ff);
	}
	*/
	return true;
}


/*
bool
AosJqlStmtHBase::write_data_to_file(
		AosJqlStmtHBase::Table &table,
		const std::vector<TRowResult> &rowResult,
		FILE *ff,
		int &offset)
{

	vector<string> fields = table.mFields;
	if (!ff) {
		cerr << "ERROR : fiel is null." << endl;
		return false;
	}
	OmnString buff = "";
	CellMap::const_iterator it;
	OmnString field = "";
	for (size_t i = 0; i < rowResult.size(); i++)
	{
		buff << rowResult[i].row << mFieldDelimiter;
		for (u32 j = 0; j < fields.size(); j++)
		{
			field = fields[j];
			for (it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
			{
				if (field != it->first)
				{
					continue;
				}
				if (table.mNeedGetValuesMap.find(field) == table.mNeedGetValuesMap.end())
				{
					if(j != 0) buff << mFieldDelimiter;
				}
				else
				{
					if (j == 0)
					{
						buff << it->second.value;
					}
					else
					{
						buff << mFieldDelimiter << it->second.value;
					}
				}
			}

		}
		buff << '\n';
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

	return true;
}
*/


AosJqlStmtHBase::Table::Table()
{
	mFileMaxSize = 0;
	mRowKeyLen = 0;
	mIsInit = false;
}


AosJqlStmtHBase::Table::~Table()
{
}


bool
AosJqlStmtHBase::Table::config(const AosXmlTagPtr &table)
{
	mTableName = table->getAttrStr("name");

	AosXmlTagPtr file_node = table->getFirstChild("file");
	if (!file_node)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mFileName = file_node->getAttrStr("file_name");
	mFileMaxSize = file_node->getAttrInt("file_size", 0);

	AosXmlTagPtr row_key = table->getFirstChild("row_key");
	if (!row_key)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mRowKeyPrefix = row_key->getAttrStr("prefix");
	mRowKeyStart = row_key->getAttrStr("start");
	mRowKeyLen = row_key->getAttrInt("len", 0);
	OmnString type = row_key->getAttrStr("type");
	mType = toEnum(type);
	if (!isValid(mType))
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr fields = table->getFirstChild("fields");
	if (!fields)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr field = fields->getFirstChild("name");
	OmnString name;
	bool need_getvalue;
	while (field)
	{
		need_getvalue = field->getAttrBool("need_get", false);
		name = field->getNodeText();
		if (need_getvalue) mNeedGetValuesMap[name] = true; 
		if (name == "")
		{
			//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		mFields.push_back(name);
		field = fields->getNextChild();
	}

	mIsInit = true;
	return true;
}

AosJqlStatement *
AosJqlStmtHBase::clone()
{
	return OmnNew AosJqlStmtHBase(*this);
}


void 
AosJqlStmtHBase::dump()
{
}

