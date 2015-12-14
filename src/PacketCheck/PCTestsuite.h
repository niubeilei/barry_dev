#ifndef _Aos_PCTESTSUITE
#define _Aos_PCTESTSUITE

#include "PCTestcase.h"

class AosPCTestsuite
{
public:
	AosPCTestsuite(std::string configFileName);
	~AosPCTestsuite();
	int init();

public:
	AosPCTestcasePtrArray mPCTestcases;

private:
	std::string mConfigFileName;
	AosPCTestcaseConfig mPCTestcaseConfig;

};

#endif

