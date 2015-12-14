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
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_Objid_h
#define AOS_SEUtil_Objid_h

#include "Rundata/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "ErrorMgr/ErrmsgId.h"
#include "MultiLang/LangDictMgr.h"
#include "SEUtil/Cloudid.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/MetaExtension.h"
#include "SEUtil/JqlTypes.h"
#include "SEUtil/ObjidType.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeErrors.h"
#include "SEBase/SeUtil.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>


#define AOSOBJIDBASE_ROOT		"zky_root"
#define AOSOBJIDBASE_SYSROOT	"zky_sysroot"

#define AOSOBJIDRES_HOMEVPD				"homevpd"
#define AOSOBJIDRES_USER_HOMEFOLDER		"home"
#define AOSOBJIDRES_USER_BASEFOLDER		"userbase"
#define AOSOBJIDRES_USER_ALBUM			"album"
#define AOSOBJIDRES_USER_ATCL			"article"
#define AOSOBJIDRES_USER_BLOG			"blog"
#define AOSOBJIDRES_USER_CMT			"comment"
#define AOSOBJIDRES_USER_DIARY			"diary"
#define AOSOBJIDRES_USER_PUBLISH		"publish"
#define AOSOBJIDRES_USER_MBLOG			"mblog"	//ken 01/05/2011
#define AOSOBJIDRES_USER_LOSTFOUND		"lostfound"	//ken 01/23/2011
#define AOSOBJIDRES_USER_MISC			"zky_usermisc"
#define AOSOBJIDRES_USER_CLOUDCARD		"cloudcard"
#define AOSOBJIDRES_USER_VPD			"vpd"
#define AOSOBJIDRES_USER_IMAGE			"image"
#define AOSOBJIDRES_INBOX_CTNR			"inbox"
#define AOSOBJIDRES_PUBNAMEPACE			"pubnspc"
#define AOSOBJIDPRE_DFTOBJID			"zkydoid"
#define AOSOBJIDPRE_ACCESS_RCD			"zky_oid01"
#define AOSOBJIDPRE_USER_PFOLDER		"zky_oid02"
#define AOSOBJIDPRE_CRT_HTMLID			"zky_oid03"
#define AOSOBJIDPRE_SSID				"zky_oid04"
#define AOSOBJIDPRE_USER				"zky_oid05"
#define AOSOBJIDPRE_SYSUSER_CTNR		"zky_sysuser"
#define AOSOBJIDPRE_LOG_CTNR			"zky_log"
#define AOSOBJIDPRE_LOSTaFOUND			"lost+found"
#define AOSOBJIDPRE_GUEST				"zky_guest"
#define AOSOBJIDPRE_UNKNOWN				"zky_unknown"
#define AOSOBJIDPRE_CTNRADMIN			"zky_ctnradm"
#define AOSOBJIDPRE_ROOTCTNR			"zky_rootctnr"
#define AOSOBJIDPRE_ROOTUSER			"zky_rootuser"
#define AOSOBJIDPRE_CIDCTNR				"zky_cidctnr"
#define AOSOBJIDPRE_USERDOMAINCTNR		"zky_userdomain"
#define AOSOBJIDRES_USER_DATA			"zky_userdata"
#define AOSOBJIDRES_SYSTEM_DATA			"_zt3k_sysdata"		// Chen Ding, 2013/05/13
#define AOSOBJIDRES_FUNC_JIMODOC		"_ztfuncjmoid"		// Chen Ding, 2014/04/08
#define AOSRSVEDOBJID_STAT_JOIN_OBJID	"zky_statjoin"		// Ketty, 2014/09/24


class AosObjid
{
private:
	enum
	{
		eMaxObjidLen = 256,
		eMinObjidLen = 5,
		eConnChar = '.'
	};

	AosObjid();
	~AosObjid();

	static int smMaxObjidLen;

public:
	static bool	isValidObjid(const OmnString &objid, const AosRundataPtr &rdata);

	static bool decomposeObjid(const OmnString &objid, 
			 OmnString &prefix,
			 OmnString &cid)
	{
		const int len = objid.length();
		aos_assert_r(len > 0, false);	

		const char *data = objid.data();
		for (int i=len-1; i>=0; i--)
		{
			if (data[i] == eConnChar)
			{
				// Ice, 2011/01/25
				// if (i < len-1-AosCloudid::eMinCloudidLen)
				if (AosCloudid::eMinCloudidLen > len-i)
				{
					// This is not a cloudid
					prefix = objid;
					cid = "";
					return true;
				}
				prefix.assign(data, i);
				cid.assign(&data[i+1], len-i-1);
				return true;
			}
		}

		// There is no eConnChar. This is allowed.
		prefix = objid;
		cid = "";
		return true;
	}


	static OmnString getPrefix(const OmnString &objid)
	{
		const int len = objid.length();
		aos_assert_r(len > 0, "");	

		const char *data = objid.data();
		for (int i=len-1; i>=0; i--)
		{
			if (data[i] == eConnChar)
			{
				aos_assert_r(i < len-1-AosCloudid::eMinCloudidLen, "");
				return OmnString(data, i);
			}
		}

		// There is no eConnChar.
		return objid;
	}

	static OmnString getCid1(const OmnString &objid)
	{
		const int len = objid.length();
		aos_assert_r(len > 0, "");	

		const char *data = objid.data();
		for (int i=len-1; i>=0; i--)
		{
			if (data[i] == eConnChar)
			{
				aos_assert_r(i < len-1-AosCloudid::eMinCloudidLen, "");
				return &data[i+1];
			}
		}

		return "";
	}

	static OmnString compose(const OmnString &prefix, const OmnString &cid)
	{
		if (cid == "") return prefix;

		OmnString nn = prefix;
		nn << (char)eConnChar << cid;
		return nn;
	}

	static OmnString compose(const OmnString &prefix, const int seqno, const OmnString &cid)
	{
		OmnString nn = prefix;
		nn << "(" << seqno << ")";
		if (cid != "") nn << (char)eConnChar << cid;
		return nn;
	}

	static OmnString createNewObjid(
		const AosXmlTagPtr &objtemp, 
		const OmnString &dft, 
		const u64 &docid,
		bool &changed, 
		const AosRundataPtr &rdata);

	// Chen Ding, 12/23/2010, Zky2056
	static OmnString getObjidRoot(const OmnString &objid, int &num)
	{
		// If objid is in the form: xxx(nnn), where 'nnn' is a digit
		// string, it return xxx as the prefix
		// and nnn is set to 'num'. Otherwise, it returns 'objid' itself.
		num = -1;
		int startidx = objid.indexOf(0, '(', true);
		if (startidx < 0) return objid;
		int endidx = objid.indexOf(0, ')', true);
		if (endidx < 0) return objid;
		if (endidx <= startidx+1) return objid;

		// Check whether it is a digital string
		const char *data = objid.data();
		for (int i=startidx+1; i<endidx; i++)
		{
			if (data[i] < '0' || data[i] > '9') return objid;
		}

		// Found the root
		OmnString id(data, startidx);
		num = atoi(&data[startidx+1]);
		return id;
	}

	static OmnString createUrlObjid(OmnString &url)
	{
		//2011/09/13 Felicia
		/*OmnString objid = AOSZTG_URLOBJID;
		OmnString md5str = AosMD5Encrypt(url);
		objid << "_" << md5str;
		return objid;*/
		
		//clear port.
		//2010/10/29 james
		int idx1 = url.findSubString(":", 0);
		idx1 = url.findSubString(":", idx1 + 1);
		if (idx1 > 0) 
		{
			int idx2 = url.findSubString("/", idx1);
			url.remove(idx1, idx2 - idx1);
		}
		OmnString objid = AOSZTG_URLOBJID;
		objid << url;
		return objid;
	}

	static OmnString createImgObjid(
			const OmnString &objid, 
			const int seqno, 
			const OmnString &cloudid)
	{
		OmnString nn = objid;
		if (cloudid != "") 
		{
			nn << "_" << seqno << (char)eConnChar << cloudid;
		}
		return nn;
	}

	/*
	 * This function is replaced with attachCidAsNeeded(...)
	static bool addCloudid(OmnString &objid, const OmnString &cid)
	{
		// This function adds the cloudid 'cid' as needed, which means
		// that if the objid already ends with 'cid', it does nothing.
		// Otherwise, it appends 'cid' to 'objid'.
		if (cid == "") return true;
		OmnString prefix, cc;
		bool rslt = decomposeObjid(objid, prefix, cc);
		aos_assert_r(rslt, false);
		if (cc == cid) return true;
		objid << (char)eConnChar << cid;
		return true;
	}
	*/

	static OmnString composeCidCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_CIDCTNR;
		}

		OmnString objid = AOSOBJIDPRE_CIDCTNR;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeLostFoundCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSZTG_LOSTFOUND_LOGIN;
		}

		OmnString objid = AOSZTG_LOSTFOUND_LOGIN;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeLoginLogCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSZTG_LOGCTNR_LOGIN;
		}

		OmnString objid = AOSZTG_LOGCTNR_LOGIN;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeInvReadLogCtnr(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSZTG_LOGCTNR_INVREAD;
		}

		OmnString objid = AOSZTG_LOGCTNR_INVREAD;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeSuperUserObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_ROOTUSER;
		}

		OmnString objid = AOSOBJIDPRE_ROOTUSER;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeLogCtnr(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_LOG_CTNR;
		}

		OmnString objid = AOSOBJIDPRE_LOG_CTNR;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeVoteAttr(const OmnString &votetype)
	{
		OmnString aname = AOSZTG_VOTE;
		aname << ":" << votetype;
		return aname;
	}

	static OmnString composeUserHomeCtnrObjid(const OmnString &cid)
	{
		OmnString objid = AOSCTNR_USERHOME;
		objid << (char)eConnChar << cid;
		return objid;
	}

	static OmnString composePrivImageCtnrObjid(const OmnString &cid)
	{
		OmnString objid = AOSOBJIDRES_USER_IMAGE;
		objid << (char)eConnChar << cid;
		return objid;
	}

	static OmnString getSysUserCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_SYSUSER_CTNR;
		}

		OmnString objid = AOSOBJIDPRE_SYSUSER_CTNR;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString getRootCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_ROOTCTNR;
		}

		aos_assert_r(siteid != 0, "");
		OmnString objid = AOSOBJIDPRE_ROOTCTNR;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeRootAcctObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDBASE_ROOT;
		}

		aos_assert_r(siteid != 2, "");
		OmnString objid = AOSOBJIDBASE_ROOT;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeSysRootAcctObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDBASE_SYSROOT;
		}

		aos_assert_r(siteid != 0,"");
		OmnString objid = AOSOBJIDBASE_SYSROOT;
		objid << "_" << siteid;
		return objid;
	}

	static bool isRootObjid(const u32 siteid, const OmnString &objid)
	{
		OmnString dd = composeRootAcctObjid(siteid);
		return (dd == objid);
	}

	static OmnString composeSysCtnrObjid(const OmnString &otype, const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			OmnString objid = AOSZTG_SYSCTNR_OBJID_PREFIX;
			objid << "_" << otype;
			return objid;
		}

		OmnString objid = AOSZTG_SYSCTNR_OBJID_PREFIX;
		objid << "_" << siteid << "_" << otype;
		return objid;
	}

	static OmnString composeUserCtnrObjid(const OmnString &otype, const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			OmnString objid = AOSZTG_USER_OBJID_PREFIX;
			objid << "_" << otype;
			return objid;
		}

		OmnString objid = AOSZTG_USER_OBJID_PREFIX;
		objid << "_" << siteid << "_" << otype;
		return objid;
	}

	static OmnString composeGuestObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_GUEST;
		}

		OmnString objid = AOSOBJIDPRE_GUEST;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeUserArcdHomeContainerObjid(const OmnString &cid)
	{
		// All user's access records are put under the container:
		// 	home.cid
		// 		AOSCTNR_ACCESSRECORD + '.' + cid
		aos_assert_r(cid != "", "");
		OmnString ctnr_objid = AOSCTNR_ACCESSRECORD;
		ctnr_objid << (char)eConnChar << cid;
		return ctnr_objid;
	}

	static bool procObjid(OmnString &objid, const OmnString &cid, OmnString &errmsg);

	static OmnString composeUserInboxCtnrObjid(const OmnString &cid)
	{
		aos_assert_r(cid != "", "");
		OmnString objid = AOSOBJIDRES_INBOX_CTNR;
		objid << (char)eConnChar << cid;
		return objid;
	}

	static OmnString composePubNamespaceObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDRES_PUBNAMEPACE;
		}

		aos_assert_r(siteid != 0, "");
		OmnString objid = AOSOBJIDRES_PUBNAMEPACE;
		objid << (char)eConnChar << siteid;
		return objid;
	}

	/*
	static bool
	normalizeObjid(OmnString &objid)
	{
		static char lsCharMap[] = 
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 3, 0, 1, 1, 1, 0, 0, 
			1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 0 
		};

		// Objids cannot contain white spaces. This function
		// removes the white spaces as needed. If objid contains 
		// no white spaces, it returns false. Otherwise, it
		// returns true.
		char *data = (char *)objid.data();
		int len = objid.length();
		aos_assert_r(len > 0, false);

		// Since most objids do not contain white spaces, 
		// we will do two rounds. The first round just check
		// whether it contains white spaces.
		bool found = false;
		u8 c;
		int idx = 0;
		for (idx=0; idx<len; idx++)
		{
			c = (u8)data[idx];
			if (c < 127 && !lsCharMap[c])
			{
				found = true;
				break;
			}
		}
		if (!found) return false;

		// It does contain white spaces. 
		// int idx = 0;
		// for (int i=0; i<len; i++)
		// {
		// 	c = data[i];
		// 	if (c < 127 && !lsCharMap[c])
		// 	{
		// 		// Skip it
		// 		continue;
		// 	}
		// 	data[idx++] = data[i];
		// }
		data[idx] = 0;
		objid.setLength(idx);
		return true;
	}
	*/

	static bool checkObjid(
					const OmnString &objid, 
					AosObjidType::E &objid_type,
					const AosRundataPtr &rdata);

	static bool isUserHomeCtnrObjid(const OmnString &objid)
	{
		OmnString prefix, cid;
		decomposeObjid(objid, prefix, cid);
		return (cid != "" && prefix == AOSTAG_CTNR_HOME);
	}

	static bool isMetaObjid(const OmnString &objid, AosMetaExtension::E &type)
	{
		// Meta objids are in the form:
		// 		<objid-prefix>.$xxx
		// This function checks whether the objid is in this form. If yes, 
		// whether the meta extension is recognized. If yes, it returns
		// true. Otherwise, it returns false.
		int len = objid.length();
		aos_assert_r(len > 0, false);
		const char *data = objid.data();

		int idx = len-1;
		while (idx > 0 && data[idx] != AosMetaExtension::eIdChar) idx--;

		if (idx <= 0 || idx == len-1 || data[idx-1] != AosMetaExtension::eSepChar) return false;
		type = AosMetaExtension::toEnum(&data[idx+2]);
		if (!(type > AosMetaExtension::eInvalid && type < AosMetaExtension::eMax))
		{
			return false;
		}
		return true;
	}

	static bool separateObjidTmpl(const OmnString &objid, 
			OmnString &real_objid, 
			OmnString &template_objid, 
			const OmnString &cid, 
			OmnString &errmsg);

	static OmnString composeCtnrAdminObjid(const OmnString &docid)
	{
		OmnString id = AOSOBJIDPRE_CTNRADMIN;
		id << "_" << docid;
		return id;
	}

	static OmnString composeUserAcctObjid(const OmnString &cid)
	{
		aos_assert_r(cid != "", AOSTAG_USERPREFIX);
		OmnString nn = AOSTAG_USERPREFIX;
		nn << (char)eConnChar << cid;
		return nn;
	}

	static OmnString composeUserDomainCtnrObjid(const u32 siteid)
	{
		if (OmnApp::getVersion() == AOSAPP_VERSION_0_11)
		{
			return AOSOBJIDPRE_USERDOMAINCTNR;
		}

		aos_assert_r(siteid !=0, "");
		OmnString objid = AOSOBJIDPRE_USERDOMAINCTNR;
		objid << "_" << siteid;
		return objid;
	}

	static OmnString composeIdGenCtnrObjid()
	{
		return AOSTAG_CTNR_IDGENS;
	}

	static OmnString composeTempObjid(
			const OmnString &objid,
			const u32 seqno,
			const u64 &offset)
	{
		OmnString dd = AOSZTG_TEMPOBJ;
		dd << "_"  << seqno << "_" << offset;
		return dd;
	}

	// Chen Ding, 03/14/2012, OBJID_CHANGE
	// static OmnString getDftObjid(
	//	const u64 &docid, 
	//	const bool is_public,
	//	const OmnString &cid,
	//	const AosRundataPtr &rdata)
	//{
	//	// One wants to create an object but did not specify the objid. 
	//	// This function will create the default objid.
	//	OmnString prefix = AOSOBJIDPRE_DFTOBJID;
	//	prefix << docid;
	//	OmnString objid;
	//	if (is_public) 
	//	{
	//		objid = prefix;
	//	}
	//	else
	//	{
	//		objid = AosObjid::compose(prefix, cid);
	//	}
	//	return objid;
	//}
	static inline OmnString getDftObjid(const u64 &docid, const AosRundataPtr &rdata)
	{
		// One wants to create an object but did not specify the objid. 
		// This function will create the default objid.
		OmnString objid = AOSOBJIDPRE_DFTOBJID;
		char buff[16];
		sprintf(buff, "%010llu", (unsigned long long)docid);
		//objid << docid;
		objid << buff;

		return objid;
	}

	// Chen Ding, 12/23/2010, Zky2056
	static OmnString createDftObjid(const u32 siteid, const u64 &docid)
	{
		OmnString objid = AOSDFT_OBJID_PREFIX;
		objid << docid;
		return objid;
	}

	static OmnString composeSystemDocObjid(const u64 &docid)
	{
		OmnString objid = AOSZTG_SYSTEM_DOC;
		objid << "_" << docid;
		return objid;
	}

	// Chen Ding, 12/24/2011
	static OmnString composeUserDomainOprArdObjid(const OmnString &docid)
	{
		OmnString objid = AOSRSVEDOBJID_DOMAIN_OPRARD;
		objid << "_" << docid;
		return objid;
	}

	// Chen Ding, 12/24/2011
	inline static OmnString composeUserOprArdObjid(const u64 &userid)
	{
		OmnString objid = AOSRSVEDOBJID_USER_OPRARD;
		objid << "_" << userid;
		return objid;
	}

	// Chen Ding, 12/24/2011
	inline static OmnString composeUserOprArdObjid(const OmnString &userid)
	{
		OmnString objid = AOSRSVEDOBJID_USER_OPRARD;
		objid << "_" << userid;
		return objid;
	}

	// Chen Ding, 12/24/2011
	static OmnString composeOprDocCtnrObjid(const OmnString &domain_docid)
	{
		OmnString objid = AOSRSVEDOBJID_OPRDOC_CTNR;
		objid << "_" << domain_docid;
		return objid;
	}

	// Chen Ding, 12/26/2011
	static OmnString composeDefaultUserOprArdObjid(const OmnString &domain_docid)
	{
		OmnString objid = AOSRSVEDOBJID_DFT_USER_OPRDOC;
		objid << "_" << domain_docid;
		return objid;
	}

	inline static bool attachCidAsNeeded(OmnString &objid, const OmnString &cid)
	{
		aos_assert_r(objid.length() > 0, false);
		aos_assert_r(cid.length() > 0, false);
		OmnString prefix, cc;
		decomposeObjid(objid, prefix, cc);
		if (cc == cid) return true;
		objid = compose(objid, cid);
		return true;
	}

	inline static OmnString composeAlarmLogCtnrObjid()
	{
		return AOSRSVEDOBJID_ALARM_LOG;
	}

	inline static OmnString composeStmcInstDocObjid(const u64 &stmc_doc_docid)
	{
		OmnString objid = AOSRSVEDOBJID_STMC_INSTDOC;
		objid << "_" << stmc_doc_docid;
		return objid;
	}

	inline static OmnString composeDictCtnrObjid(const AosLocale::E locale)
	{
		OmnString objid = AOSRSVEDOBJID_DICTCTNR;
		objid << "_" << locale;
		return objid;
	}

	// Chen Ding, 2013/11/14
	inline static OmnString composeUserJobContainerObjid(const u64 userid)
	{
		aos_assert_r(userid > 0, "");
		OmnString objid = AOSRSVEDOBJID_USERJOBCONTAINER;
		objid << "_" << userid;
		return objid;
	}

	inline static bool doesCidMatch(const OmnString &objid, const OmnString &cid)
	{
		// This function checks whether 'objid' ends with the same cloudid
		// as 'cid'. If yes, it returns true. Otherwise, it returns false.
		int objid_len = objid.length();
		if (objid_len <= 0) return false;

		int cid_len = cid.length();
		if (cid_len <= 0) return false;

		const char *data = objid.data();
		int idx = objid_len-1;
		while (idx > 0 && data[idx] != eConnChar) idx--;

		if (idx <= 0) return false;
		return strncmp(&data[idx+1], cid.data(), cid_len) == 0;
	}

	// JACKIE
	inline static OmnString composeDictEntryObjid(
			const AosLocale::E locale, 
			const OmnString &msg_id)
	{
		OmnString objid = AOSRSVEDOBJID_DICTENTRY_OBJID;
		objid << "_" << locale << "_" << msg_id;
		return objid;
	}
	static bool addNumToObjid(OmnString &objid, const AosRundataPtr &rdata);

	// Chen Ding, 2013/12/07
	inline static OmnString composeSchemaObjid(const OmnString &schema_type)
	{
		OmnString objid = AOSRSVEDOBJID_SCHEMA_OBJID;
		objid << "_" << schema_type;
		return objid;
	}

	// Chen Ding, 2014/01/30
	inline static OmnString composeSysDefValJimoObjid(const OmnString &val_name)
	{
		OmnString objid = AOSRSVEDOBJID_SYSDEFVAL_JIMO;
		objid << "_" << val_name;
		return objid;
	}

	// Chen Ding, 2014/01/30
	inline static OmnString composeQueryFuncJimoObjid(const OmnString &func_name)
	{
		OmnString objid = AOSRSVEDOBJID_QUERYFUNC_JIMO;
		objid << "_" << func_name;
		return objid;
	}

	// Chen Ding, 2014/01/30
	inline static OmnString composeQueryVarJimoObjid(const OmnString &var_name)
	{
		OmnString objid = AOSRSVEDOBJID_QUERYVAR_JIMO;
		objid << "_" << var_name;
		return objid;
	}

	// Chen Ding, 2014/01/30
	inline static OmnString composeJimoDocObjid(const OmnString &jimo_classname)
	{
		OmnString objid = AOSRSVEDOBJID_JIMODOC_OBJID;
		objid << "_" << jimo_classname;
		return objid;
	}

	// Ketty, 2014/08/26
	inline static OmnString composeAggrFuncJimoObjid(const OmnString &func_name)
	{
		OmnString objid = AOSRSVEDOBJID_AGGRFUNC_OBJID;
		objid << "_" << func_name;
		return objid;
	}

	// Ketty, 2014/09/23
	inline static OmnString composeJoinStatDocObjid(vector<OmnString> &table_names)
	{
		if(table_names.size() == 0 || table_names.size() == 1)	return "";
		
		sort(table_names.begin(), table_names.end());

		OmnString objid = AOSRSVEDOBJID_STAT_JOIN_OBJID;
		for(u32 i=0; i<table_names.size(); i++)
		{
			objid << "_" << table_names[i];
		}
		return objid;
	}

	static OmnString getObjidByJQLDocName(const JQLTypes::DocType type, const OmnString name);
	static OmnString getObjidPrefixByJQLDocType(const JQLTypes::DocType type);

private:
	static bool init();
};
#endif

