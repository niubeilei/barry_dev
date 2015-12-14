#ifndef AOS_SEUtil_RandXmlObj_h
#define AOS_SEUtil_RandXmlObj_h

#include "Random/Ptrs.h"
#include "Random/RandomRule.h"
#include "SEUtil/Ptrs.h"
#include <map>

class AosRandXmlObj
{
protected:
	AosRandomRulePtr mRule;
	std::map<OmnString, AosRandomRulePtr> 	mChildRules;
	
public:
	virtual OmnString getXmlStr(
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata) = 0;
};

#endif
