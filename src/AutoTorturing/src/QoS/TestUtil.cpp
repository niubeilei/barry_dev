#include "TestUtil.h"

#include "Alarm/Alarm.h"
#include "Util/String.cpp"

OmnString aos_test_util_proto_int2str(const int p)
{
	if (p == 17) return "udp";
	if (p == 6)  return "tcp";

	OmnAlarm << "Unrecognized protocol: " << p << enderr;
	return "";
}

