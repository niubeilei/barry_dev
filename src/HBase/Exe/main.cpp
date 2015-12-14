
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "/home/andy/AOS2/src/HBase/HBaselib/Hbase.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace apache::hadoop::hbase::thrift;

namespace {

typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::vector<ColumnDescriptor> ColVec;
typedef std::map<std::string,ColumnDescriptor> ColMap;
typedef std::vector<TCell> CellVec;
typedef std::map<std::string,TCell> CellMap;


static void
printRow(const std::vector<TRowResult> &rowResult)
{
//  for (size_t i = 0; i < rowResult.size(); i++) {
    std::cout << "row: " << rowResult[0].row << ", cols: ";
    for (CellMap::const_iterator it = rowResult[0].columns.begin();
         it != rowResult[0].columns.end(); ++it) {
      std::cout << it->first << " => " << it->second.value << "; ";
    }
    std::cout << std::endl;
//  }
}

static void
printVersions(const std::string &row, const CellVec &versions)
{
  std::cout << "row: " << row << ", values: ";
  for (CellVec::const_iterator it = versions.begin(); it != versions.end(); ++it) {
    std::cout << (*it).value << "; ";
  }
  std::cout << std::endl;
}

}

int
main(int argc, char** argv)
{
  if (argc < 3) {
    std::cerr << "Invalid arguments!\n" << "Usage: DemoClient host port" << std::endl;
    return -1;
  }
  bool isFramed = false;
  boost::shared_ptr<TTransport> socket(new TSocket(argv[1], boost::lexical_cast<int>(argv[2])));
  boost::shared_ptr<TTransport> transport;

  if (isFramed) {
    transport.reset(new TFramedTransport(socket));
  } else {
    transport.reset(new TBufferedTransport(socket));
  }
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

  const std::map<Text, Text> dummyAttributes; // see HBASE-6806 HBASE-4658
  HbaseClient client(protocol);
  try {
    transport->open();

    std::string t("zykiedb");

    //
    // Scan all tables, look for the demo table and delete it.
    //
    std::cout << "scanning tables..." << std::endl;
    StrVec tables;
    client.getTableNames(tables);
    for (StrVec::const_iterator it = tables.begin(); it != tables.end(); ++it) {
      std::cout << " found: " << *it << std::endl;
      if (t == *it) {
        if (client.isTableEnabled(*it)) {
          std::cout << " disabling table: " << *it << std::endl;
          client.disableTable(*it);
        }
        std::cout << " deleting table: " << *it << std::endl;
        client.deleteTable(*it);
      }
    }

    //
    // Create the demo table with two column families, entry: and unused:
    //
    ColVec columns;
    columns.push_back(ColumnDescriptor());
    columns.back().name = "entry:";
    columns.back().maxVersions = 20;

    std::cout << "creating table: " << t << std::endl;
    try {
      client.createTable(t, columns);
    } catch (const AlreadyExists &ae) {
      std::cerr << "WARN: " << ae.message << std::endl;
    }

    ColMap columnMap;
    client.getColumnDescriptors(columnMap, t);
    std::cout << "column families in " << t << ": " << std::endl;
    for (ColMap::const_iterator it = columnMap.begin(); it != columnMap.end(); ++it) {
      std::cout << " column: " << it->second.name << ", maxVer: " << it->second.maxVersions << std::endl;
    }

    std::vector<Mutation> mutations;
	char buf[32];
	sprintf(buf, "%05d", 10001);
	std::string row(buf);
    std::cout << "row id : " << row << std::endl;
	std::vector<TRowResult> rowResult;

    mutations.clear();
    mutations.push_back(Mutation());
    mutations.back().column = "entry:name";
    mutations.back().value = "jackie";
	client.mutateRow(t, row, mutations, dummyAttributes);
	client.getRow(rowResult, t, row, dummyAttributes);
	printRow(rowResult);

	char buf2[32];
	sprintf(buf2, "%05d", 10002);
	std::string row2(buf2);
    std::cout << "row id : " << row2 << std::endl;
	std::vector<TRowResult> rowResult2;

    mutations.clear();
    mutations.push_back(Mutation());
    mutations.back().column = "entry:name";
    mutations.back().value = "ice";
	client.mutateRow(t, row2, mutations, dummyAttributes);
	client.getRow(rowResult2, t, row2, dummyAttributes);
	printRow(rowResult2);
//	client.deleteAllRow(t, row, dummyAttributes);


  } catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
  }
}
