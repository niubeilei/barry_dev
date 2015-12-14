#include "AosConf/DataFieldStr.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataSchema.h"
#include "AosConf/DataSplit.h"
#include "AosConf/DataConnector.h"
#include "AosConf/DataScanner.h"
#include "AosConf/DataSet.h"
#include "AosConf/JobTask.h"
#include "AosConf/Job.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataProc.h"
#include "AosConf/DataProcField.h"
#include "AosConf/DataProcCreateDatasetDoc.h"
#include "AosConf/Stat/StatConf.h"
#include "XmlUtil/XmlTag.h"
#include "Alarm/Alarm.h"

#include <string>
using namespace AosConf;
using namespace std;

int main(int argc, char** argv) 
{
	//aos_global_data_init();
	OmnApp::appStart(argc, argv);
	OmnApp theApp(argc, argv);
	AosXmlTagPtr statdoc = OmnApp::readConfig("stat_sdd_example_0.xml"); 
	AosXmlTagPtr inputds = OmnApp::readConfig("inputds.xml");
	StatConf stat1(inputds, statdoc);
	//stat1.createConfig1();
	OmnString objid, ctnr_objid;
	string job_str = stat1.createConfig(objid, ctnr_objid, 0);
	cout << job_str << endl;
}
