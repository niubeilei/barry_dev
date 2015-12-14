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
// 07/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_DataCol_h
#define Aos_HtmlModules_DataCol_h

#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/Ptrs.h"
#include "HtmlServer/HtmlReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "alarm_c/alarm.h"

#define AOSDCL_STR				"str_datacol"
#define AOSDCL_DB				"db_datacol"

class AosDataCol : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum DataColType
	{
		eDclType_Invalid = 0,
		eDclType_Str,
		eDclType_Db,
		eDclType_Max
	};

public:
	static AosDataColPtr    smDcls[eDclType_Max];

private:
	static DataColType
	getDataColType(OmnString dclType)
	{
		const char *data = dclType.data();
		const int len = dclType.length();
		aos_assert_r(len >= 5, eDclType_Invalid);
			 
		switch (data[0])
		{
		case 's':
			 aos_assert_r(len >= 6, eDclType_Invalid);
			 if (dclType== AOSDCL_STR) return eDclType_Str;
			 break;

		case 'd':
			 aos_assert_r(len >= 6, eDclType_Invalid);
			 if (dclType== AOSDCL_DB) return eDclType_Db;
			 break;

		default:
			 return eDclType_Invalid;
		}
		OmnAlarm << "Invalid Datacol type: " << dclType << enderr;
		return eDclType_Invalid;
	}

public:
	AosDataCol(){}
	~AosDataCol(){}

	virtual OmnString getJsonConfig( const AosXmlTagPtr &vpd){return "";};


	virtual AosXmlTagPtr retrieveData(
					const AosHtmlReqProcPtr &htmlPtr,
					const AosXmlTagPtr &vpd,        //where the vpd from?
					const AosXmlTagPtr &obj,
					const OmnString &tagname){return 0;}

	virtual bool retrieveContainers(
					const AosHtmlReqProcPtr &htmlPtr,
			 		AosXmlTagPtr &vpd,
			 		const AosXmlTagPtr &obj,
			 		const OmnString &parentid,
					const OmnString &tagname,
			 		AosHtmlCode &code) {return true;}
	
	static AosDataColPtr getInstance(const AosXmlTagPtr &vpd);

	// Chen Ding, 11/26/2011
	static AosXmlTagPtr retrieveDataStatic(
					const AosHtmlReqProcPtr &htmlPtr,
					const AosXmlTagPtr &vpd,
					const AosXmlTagPtr &obj,
					const OmnString &tagname);
					
};

#endif
