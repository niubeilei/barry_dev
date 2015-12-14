#include "PCTestsuite.h"
#include "HttpPCTestcase.h"
#include "AmPCTestcase.h"

AosPCTestsuite::AosPCTestsuite(std::string configFileName)
:
mConfigFileName(configFileName)
{
}

AosPCTestsuite::~AosPCTestsuite()
{
}

int AosPCTestsuite::init()
{
	mPCTestcaseConfig.setConfigFileName(mConfigFileName.c_str());
	
	if(mPCTestcaseConfig.readFile() < 0)
	{
		cout << "read config file failed" << endl;
		return -1;
	}

	int tcNum = mPCTestcaseConfig.mTCDataPtrVector.size();
	int i;
	AosPCTestcase* tc;
	for (i=0; i<tcNum; i++)
	{
		int type = mPCTestcaseConfig.mTCDataPtrVector[i]->getType();
		switch (type)
		{
			case eTCDATA_HTTP:
				tc = new AosHttpPCTestcase();
				break;
			case eTCDATA_AM:
				tc = new AosAmPCTestcase();
				break;
			default:
				tc = NULL;
				break;
		}

		if (tc != NULL)
		{
			tc->setTCData(mPCTestcaseConfig.mTCDataPtrVector[i]);
			mPCTestcases.push_back(tc);
		}
	}
	
	return 0;
}
