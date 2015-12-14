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
//
// Modification History:
// 07/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEBase/SecUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Objid.h"
#include "Base64/Base64.h"
#include "XmlUtil/XmlTag.h"
#include <openssl/sha.h>


OmnString 
AosSecUtil::signValue(const OmnString &value)
{
	if (value == "") return "";
	const char *data = value.data();
	int size = value.length();
	return signValue(data, size);
}


OmnString 
AosSecUtil::signValue(const char *data, const int &size)
{
	// This function signs the value 'value' with the system secret key. 
	// In the current implementations, the system secret key is hard coded.
	// In the future, it should be generated dynamically.
	aos_assert_r(data, "");
	aos_assert_r(size > 0, "");

	static const int lsSh1Length = 20;
	static const int lsBase64Sh1Length = 30;
	unsigned char hash[lsSh1Length + 10];
	SHA_CTX s;

	SHA1_Init(&s);
	SHA1_Update(&s, data, size);
	SHA1_Final(hash, &s);

	char sh1_Base64[lsBase64Sh1Length + 10];
	int len = EncodeBase64((unsigned  char*)hash, (unsigned char *)sh1_Base64, 
			lsSh1Length, lsBase64Sh1Length);
	aos_assert_r(len <lsBase64Sh1Length, "");
	// sh1_Base64[lsSh1Length] = 0;
	sh1_Base64[len] = 0;
	OmnString id;
	id.assign(sh1_Base64, len);
	// Why do we need to do this? Chen Ding, 11/12/2011
	// AosObjid::normalizeObjid(id);
	return id;
};


