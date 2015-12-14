////////////////////////////////////////////////////////////////////////////
//
// 03/11/2011: Created by Tracy 
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/ClientPlatform.h"


AosClientPlatform::AosClientPlatform()
:
mOsName(""),
mBrowserType(""),
mBrowserVersion(""),
mTablet(false)
{
}


AosClientPlatform::AosClientPlatform(
		const OmnString &userAgent,
		const OmnString &platform
		)
:
mOsName(""),
mBrowserType(""),
mBrowserVersion(""),
mTablet(false)
{
	bool rlst = init(userAgent, platform);
	aos_assert(rlst);
}


bool
AosClientPlatform::init(
		const OmnString &userAgent,
		const OmnString &platform)
{
	if (userAgent == "") return true;
	if (userAgent.indexOf("Android", 0) >= 0 
			|| userAgent.indexOf("Tablet PC", 0) >= 0)
	{
		mTablet = true;
	}
	mOsName = platform;
	return true;
}
