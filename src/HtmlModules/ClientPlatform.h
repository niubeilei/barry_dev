////////////////////////////////////////////////////////////////////////////
//
// Modification History:
// 07/20/2010: Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_ClientPlatform_h
#define Aos_HtmlModules_ClientPlatform_h

#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "alarm_c/alarm.h"

class AosClientPlatform
{
public:
	AosClientPlatform();
	AosClientPlatform(
			const OmnString &userAgent, 
			const OmnString &platform);
	~AosClientPlatform() {}

	bool init(const OmnString &userAgent,
			const OmnString &platform);

	bool isTablet() const { return mTablet;};
	
private:

	OmnString mOsName;
	OmnString mBrowserType;
	OmnString mBrowserVersion;
	bool	  mTablet;
};

#endif
