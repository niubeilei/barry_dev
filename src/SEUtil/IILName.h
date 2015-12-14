////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/22/2010	Created by Chen Ding
// 2015/06/07   Moved from IILUtil
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_IILName_h
#define AOS_SEUtil_IILName_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "SEUtil/IILTags.h"
#include "SEUtil/DocZTGs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Cloudid.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/DocZTGs.h"
#include "SEUtil/MetaExtension.h"
#include "SEUtil/SeErrors.h"
#include "SEUtil/MetaExtension.h"
#include "Util/String.h"
#include "Debug/Debug.h"


class AosIILName
{
public:
	enum
	{
		eSep = '_',
		eMaxWordIdx_AttrLen = 30,
		eMaxNumFields = 200
	};

private:
	AosIILName();
	~AosIILName();

public:
	static OmnString
	composeCtnrMemberListing(const OmnString &ctnr_objid, const OmnString &attrname)
	{
		// There are several ways of listing members for a container:
		// 1. Listing members by objid: 
		// 			iilname = AOSIILTAG_CONTAINER + "_" + ctnr_objid 
		// 2. Listing members by attribute name: 
		// 			iilname = AOSIILTAG_CONTAINER + "_" + ctnr_objid + "_" + attrname
		if (ctnr_objid == "")
		{
			if (attrname == "")
			{
				OmnAlarm << "container and attribute name are both empty!" << enderr;
				return "";
			}

			// Chen Ding, 07/15/2012
			// return composeObjidListingName("");
			return composeAttrIILName(attrname);
		}

		if (attrname == "")
		{
			return composeCtnrMemberObjidIILName(ctnr_objid);
		}

		return composeContainerAttrIILName(ctnr_objid, attrname);
		// OmnString iilname = AOSIILTAG_CONTAINER;
		// iilname << (char)eSep << ctnr_objid << (char)eSep << attrname;
		// return iilname;
	}

	static OmnString composeContainerAttrWordIILName(
			const OmnString &ctnr_objid, 
			const OmnString &aname,
			const OmnString &word)
	{
		if (ctnr_objid == "")
			return composeAttrWordIILName(aname, word);

		if (aname == "")
		{
			return composeContainerWordIILName(ctnr_objid, word);
		}
		OmnString iilname = AOSZTG_CTNR_ATTRWORD_IIL;
		iilname << (char)eSep << aname << (char)eSep <<  word << (char)eSep << ctnr_objid;
		return iilname;
	}

	static OmnString composeAttrWordIILName(
			const OmnString &aname,
			const OmnString &word)
	{
		if (aname == "") return word;
		OmnString iilname = AOSZTG_ATTRWORD_INDEX;
		iilname << (char)eSep << aname << (char)eSep <<word;
		return iilname;
	}

	// static OmnString composeCmtIILName()
	// {
	// 	OmnString iilname = AOSZTG_CMT_IIL;
	// 	return iilname;
	// }

	static OmnString composeHookIILName(const int hook)
	{
		OmnString iilname = AOSZTG_HOOK;
		iilname << (char)eSep << hook;
		return iilname;
	}

	static OmnString composeObjidListingName(const OmnString &siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
	 		return AOSZTG_SITEOBJID;
		}

		OmnString iilname = AOSZTG_SITEOBJID;
		iilname << siteid;
		return iilname;
	}

	static OmnString composeObjidListingName(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
	 		return AOSZTG_SITEOBJID;
		}

		OmnString iilname = AOSZTG_SITEOBJID;
		iilname << siteid;
		return iilname;
	}

	static OmnString composeCloudidListingName(const OmnString &siteid)
	{
		OmnString iilname = AOSZTG_CLOUDID;
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			iilname << (char)eSep << AosGetAttrPostfix(eAosAttrType_NumAlpha);
			return iilname; 
		}

		iilname << siteid;
		return iilname;
	}

	static OmnString composeCloudidListingName(const u32 siteid)
	{
		OmnString iilname = AOSZTG_CLOUDID;
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			iilname << (char)eSep << AosGetAttrPostfix(eAosAttrType_NumAlpha);
			return iilname; 
		}

		iilname << siteid;
		return iilname;
	}

	static OmnString
	composeContainerAttrIILName(const OmnString &ctnr_objid, const OmnString &aname)
	{
		if (aname == "")
		{
			return composeCtnrMemberObjidIILName(ctnr_objid);
		}
		if (ctnr_objid == "")
		{
			return composeAttrIILName(aname);
		}
		OmnString iilname = AOSZTG_CONTAINER;
		iilname << "_" << ctnr_objid << "_" << aname;
		return iilname;
	}
	
	static OmnString composeCtnrMemberCtimeIILName(const OmnString &objid)
	{
		aos_assert_r(objid != "", "");
		OmnString iilname = AOSZTG_CTNRCTIME;
		iilname << "_" << objid;
		return iilname;
	}

	static OmnString composeCtnrMemberObjidIILName(const OmnString &objid)
	{
		// It is the container's default member listing.
		aos_assert_r(objid != "", "");

		// if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		// {
		// 	OmnString iilname = AOSIILTAG_CONTAINER;
		// 	iilname << (char)eSep << objid;
		// 	return iilname;
		// }
			
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_10)
		{
			OmnString iilname = AOSZTG_CONTAINER;
			iilname << "100:" << objid;
			return iilname;
		}

		OmnString iilname = AOSZTG_CTNROBJID;
		iilname << (char)eSep << objid;
		return iilname;
	}

	static OmnString composeDescendantIILName()
	{
		OmnString iilname = AOSZTG_DESCENDANT;
		return iilname;
	}

	static OmnString composeAncestorIILName()
	{
		OmnString iilname = AOSZTG_ANCESTOR;
		return iilname;
	}

	/*
	static OmnString
	composeTagIILName(const OmnString &tag)
	{
		// Old format: AOSIILTAG_TAG + tag
		// New Format: AOSIILTAG_TAG + "_" + tag
		aos_assert_r(tag != "", "");
		OmnString iilname = AOSIILTAG_TAG;
		iilname << tag;
		return iilname;
	}
	*/
	
	// Ketty 2011/08/16
	/*
	static OmnString
	composeContainerTagIILName(const u64 &ctnr_docid, const OmnString &tag)
	{
		aos_assert_r(tag != "", "");
		OmnString iilname = AOSIILTAG_TAG;
		iilname << "_" << ctnr_docid << "_" << tag;
		return iilname;
	}
	*/

	static OmnString
	composeContainerWordIILName(const OmnString &ctnr_objid, const OmnString &keyword)
	{
		aos_assert_r(keyword != "", "");

		if (ctnr_objid== "")
			return keyword;
		OmnString iilname = AOSZTG_KEYWORD;
		iilname << "_" << ctnr_objid << "_" << keyword;
		return iilname;
	}

	static OmnString composeVersionIILName(const OmnString &pctr_objid)
	{
		aos_assert_r(pctr_objid != "", "");
		OmnString iilname = AOSIILTAG_VERSION;
		iilname << (char)eSep << pctr_objid << AosGetAttrPostfix(eAosAttrType_NumAlpha);
		return iilname;
	}

	static OmnString composeAttrIILName(const OmnString &aname)
	{
		OmnString iilname = AOSZTG_ANAME;
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			iilname << (char)eSep << aname;
			return iilname;
		}

		iilname << aname;
		return iilname;
	}

	static OmnString composeDeletedObjidIILName()
	{
		return AOSZTG_DELETEDDOC;
	}

	static OmnString composeLogMgrSeqnoIILName(const u32 moduleid)
	{
		OmnString iilname = AOSIILTAG_LOGMGR_SEQNOIIL;
		iilname << (char)eSep << moduleid;
		return iilname;
	}

	static inline OmnString composeVersionCountIILName(const OmnString &pctr_objid)
	{
		aos_assert_r(pctr_objid != "", "");
		OmnString iilname = AOSZTG_VERSIONCOUNT_IILNAME;
		iilname << (char)eSep << pctr_objid;
		return iilname;
	}

	static inline OmnString composeLogListingIILName(
				const OmnString &siteid, 
				const OmnString &pctr_objid,
				const OmnString &logname)
	{
		aos_assert_r(pctr_objid != "", "");
		aos_assert_r(logname != "", "");
		aos_assert_r(siteid != "", "");
		
		OmnString iilname = AOSZTG_LOGLISTING_IILNAME;
		iilname << (char)eSep << siteid
			<< (char)eSep << pctr_objid
			<< (char)eSep << logname;
		return iilname;
	}
	
	static inline OmnString composeLogListingIILName(
				const u32 siteid, 
				const OmnString &pctr_objid,
				const OmnString &logname)
	{
		aos_assert_r(pctr_objid != "", "");
		aos_assert_r(logname != "", "");
		aos_assert_r(siteid != 0, "");
		
		OmnString iilname = AOSZTG_LOGLISTING_IILNAME;
		iilname << (char)eSep << siteid
			<< (char)eSep << pctr_objid
			<< (char)eSep << logname;
		return iilname;
	}

	static inline OmnString composeLogGlobalIILName(const OmnString &gbl_str)
	{
		OmnString iilname = AOSZTG_LOGGLOBALIILNAME;
		iilname << (char)eSep << gbl_str;
		return iilname;
	}
	
	//ken 2011/12/05
	static OmnString
	composeInlineSearchContainerAttrIILName(const OmnString &ctnr_objid, const OmnString &aname)
	{
		aos_assert_r(ctnr_objid!="" && aname!= "", "");
		OmnString iilname = AOSZTG_INLINESEARCH;
		iilname << "_" << ctnr_objid << "_" << aname;
		return iilname;
	}

	static OmnString
	composeSobjIdMgrIILName(const OmnString &name)
	{
		aos_assert_r(name != "", "");
		OmnString nn = AOSZTG_SOBJ_ID_MGR;
		nn << "_" << name;
		return nn;
	}

	//ken 2012/05/09
	static OmnString
	composeSuperIILCtnrAttrIILName(const OmnString &ctnr_objid, const OmnString &aname)
	{
		aos_assert_r(ctnr_objid!="" && aname!= "", "");
		OmnString iilname = AOSZTG_SUPERIIL;
		iilname << "_" << ctnr_objid << "_" << aname;
		return iilname;
	}
	
	static OmnString
	composeSuperIILAttrIILName(const OmnString &aname)
	{
		aos_assert_r(aname!= "", "");
		OmnString iilname = AOSZTG_SUPERIIL;
		iilname << "_" << aname;
		return iilname;
	}
	
	static OmnString
	composeCounterIILName(const OmnString &id)
	{
		aos_assert_r(id != "", "");
		OmnString iilname = AOSZTG_COUNTER;
		iilname << (char)eSep << id ;
		return iilname;
	}

	static OmnString
	composeVirtualFileSizeIILName()
	{
		OmnString iilname = AOSZTG_VIRTUALFILESIZE;
		iilname << "_" << "virtualfilesize";
		return iilname;
	}

	static OmnString
	composeVirtualFileSeqnoIILName()
	{
		OmnString iilname = AOSZTG_VIRTUALFILESIZE;
		iilname << "_" << "virtualsegmentnum";
		return iilname;
	}

	// Chen Ding, 01/06/2013
	static OmnString composeAppIdGenIILName()
	{
		return AOSZTG_APP_IDGENS;
	}

	// Chen Ding, 01/06/2013
	static OmnString composeSysIdGenIILName()
	{
		return AOSZTG_SYS_IDGENS;
	}

	static OmnString composeParallelIILName(const OmnString &iilname)
	{
		int vid = AosGetCubeId(iilname);

		int i = 0;
		int new_vid;
		OmnString name;
		while (1)
		{
			name = iilname;
			name << "_deltaiil_zky_" << i++;
			new_vid = AosGetCubeId(name);
			if (vid == new_vid) break;
			if (i >= 1000)
			{
				OmnAlarm << "error" << enderr;
			}
		}

		return name;
	}

	static OmnString composeCubeIILName(const int cubid, const OmnString &iilname)
	{
		int i = 0;
		int new_cubid;
		OmnString name;
		while (1)
		{
			name = iilname;
			name << "__" << i++;
			new_cubid = AosGetCubeId(name);
			if (new_cubid == cubid) break;
			if (i >= 1000)
			{
				OmnAlarm << "error" << enderr;
			}
		}
		return name;
	}

	static OmnString composeSQLTablenameIILName()
	{
		return AOSZTG_JQL_TABLENAME_IIL;
	}

	static OmnString composeIndexMgrIILName()
	{
		return AOSZTG_INDEXMGR_IIL;
	}

	static OmnString composeDatasetIILName()
	{
		return AOSZTG_DATASET_IIL;
	}

	static OmnString composeSchemaIILName()
	{
		return AOSZTG_SCHEMA_IIL;
	}

	static OmnString composeIILNameIndexIILName()
	{
		return AOSZTG_IILNAME_INDEX;
	}
	
	
	//ketty 2014/02/17
	static OmnString
	composeStatKeyIILName(const OmnString &stat_objid, const OmnString &stat_identify_key, const OmnString &key)
	{
		//OmnString iilname = AOSZTG_STATIIL;
		OmnString iilname = "_zt44";
		iilname << "_" << stat_objid << "_" << stat_identify_key  << "_key_" << key;
		return iilname;
	}
	
	//ketty 2014/02/17
	static OmnString
	composeStatKeyIILName(const OmnString &stat_name, const OmnString &key)
	{
		//OmnString iilname = AOSZTG_STATIIL;
		OmnString iilname = "_zt44";
		iilname << "_" << stat_name << "_key_" << key;
		return iilname;
	}
	
	//static OmnString
	//composeInternalStatIILName(const OmnString &stat_doc_objid)
	//{
	//	//OmnString iilname = AOSZTG_STATIIL;
	//	OmnString iilname = "_zt44";
	//	iilname << "_" << stat_doc_objid << "_internal_stat";
	//	return iilname;
	//}
	
	static OmnString
	composeTableAttrWordIILName(const OmnString &table_name, const OmnString &attrname, const OmnString &word)
	{
		//OmnString iilname = AOSZTG_STATIIL;
		OmnString iilname = "_zt44";
		iilname << "_" << table_name << "_" << attrname << "_" << word;
		return iilname;
	}

	// Chen Ding, 2015/01/15
	 static OmnString
	 composeAutoIncIILKey(const OmnString &table_name, const OmnString &field_name)
	 {
		 OmnString key = AOSZTG_AUTO_INCR_IIL;
		 key << "_" << table_name << "_" << field_name;
		 return key;
	 }

	/*static OmnString
	composeAutoIncIILName(const u64 container_docid)
	{
		OmnString iilname = AOSZTG_AUTO_INCR_IIL;
		iilname << "_" << container_docid % 1234;
		return iilname;
	}*/
	static OmnString
	composeAutoIncIILName(const OmnString &key) 
	{
	    u32 hash_key = AosGetHashKey(key);
		OmnString iilname = AOSZTG_AUTO_INCR_IIL;
		iilname << "_" << (hash_key % 1234);
		return iilname;
	}

	// Chen Ding, 2015/09/13, JIMODB-758
	static OmnString 
	composeValueMapIILName(const OmnString &tablename, const OmnString &map_name)
	{
		OmnString nn = AOSZTG_VALUE_MAP_IILNAME;
		nn << "_" << tablename << "_" << map_name;
		return nn;
	}

	// Chen Ding, 2015/09/13, JIMODB-759
	static OmnString 
	composeCompoundIILName(const vector<OmnString> &fieldnames)
	{
		// A compound index is in the following format:
		// 		AOSZTG_COMPOUND_IILNAME + "_" + <fieldname> ...
		// field names are sorted in the ascending order.
		aos_assert_r(fieldnames.size() > 0 && fieldnames.size() < eMaxNumFields, "");
		OmnString nn = AOSZTG_COMPOUND_IILNAME;

		// Use an array of flags to mark whether a field has been consumed,
		// a performance tuner only.
		char flags[eMaxNumFields];
		memset(flags, 0, eMaxNumFields);

		u32 total = 0;
		u32 size = fieldnames.size();
		while (total < size)
		{
			for (u32 i=0; i<size; i++) 
			{
				if (!flags[i])
				{
					if (i == size-1)
					{
						// It is the last one
						nn << "_" << fieldnames[i];
						return nn;
					}

					int lhs_idx = i;
					for (u32 j=i+1; j<size; j++)
					{
						if (flags[j])
						{
							int r = strcmp(fieldnames[lhs_idx].data(), fieldnames[j].data());
							if (r > 0) // fieldnames[lhs_idx] > fieldnames[j]
							{
								lhs_idx = j;
							}
						}
					}
					flags[lhs_idx] = 1;
					nn << "_" << fieldnames[lhs_idx];
					total++;
				}
			}
		}

		return nn;
	}
};
#endif
