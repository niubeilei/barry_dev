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
// 07/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_Objname_h
#define AOS_SEUtil_Objname_h

#include "Util/String.h"

#define AOSOBJNAME_INBOX_CTNR			"inboxctnr"
#define AOSOBJNAME_SYSROOT				"sysroot"
#define AOSOBJNAME_ROOT					"root"
#define AOSOBJNAME_SYSTEM				"sys"
#define AOSOBJNAME_ACCESS_RECORD		"AccessRecord"
#define AOSOBJNAME_ACCESS_RECORD_CTNR	"AccessRecords"
#define AOSOBJNAME_GUEST				"Guest"
#define AOSOBJNAME_UNKNOWN				"Unknown"
#define AOSOBJNAME_CONTAINER			"Container"
#define AOSOBJNAME_PREFIX				"zkyname"

#define AOSOBJNAME_PUBNAMESPACE_CTNR	"public_name"
#define AOSOBJNAME_INVALID_READ_CTNR	"invalid_reads"
#define AOSOBJNAME_CID_CTNR				"cloudids"
#define AOSOBJNAME_LOGINLOG_CTNR		"login_log"
#define AOSOBJNAME_PRE_CTNR				"ctnr"
#define AOSOBJNAME_MBLOG_CTNR 			"Mblogs"
#define AOSOBJNAME_LOSTFOUND_CTNR		"LostFound"
#define AOSOBJNAME_BLOG_CTNR			"blogs"
#define AOSOBJNAME_COMMENT_CTNR			"comments"
#define AOSOBJNAME_ARTICLE_CTNR			"articles"
#define AOSOBJNAME_PUBLISH_CTNR			"publishes"
#define AOSOBJNAME_ALBUM_CTNR			"albums"
#define AOSOBJNAME_IMAGE_CTNR			"images"
#define AOSOBJNAME_DIARY_CTNR			"diaries"
#define AOSOBJNAME_CLOUDCARD_CTNR		"ccards"
#define AOSOBJNAME_MISC_CTNR			"misc"
#define AOSOBJNAME_VPD_CTNR				"vpds"
#define AOSOBJNAME_TEMPOBJ_CONTAINER	"sys_tmpobj_ctnr"
#define AOSOBJNAME_SYSMONITOR_CONTAINER	"zky_sysmonitor"
#define AOSOBJNAME_USERDATA				"userdata"
#define AOSOBJNAME_DICT_PARENT			"Dictionary Parent Container"
#define AOSOBJNAME_DICTCTNR				"Dictionary"
#define AOSOBJNAME_SYSLOG_CONTAINER		"System Log"


class AosObjname
{
public:
	static OmnString composeCtnrAttrName(const OmnString &otype)
	{
		// In userdoc, different types of docs may be put in different 
		// containers, such as all blogs are put under the blog 
		// container. This function composes the name of the 
		// attribute for the object type 'otype'. 
		OmnString aname = AOSANAME_PREF_USER_CTNR;
		aname << "_" << otype;
		return aname;
	}

	static OmnString composeUserCtnrName(const OmnString &otype)
	{
		OmnString name = AOSOBJNAME_PRE_CTNR;
		name << "_" << otype;
		return name;
	}

	static OmnString composePublicNameSpace()
	{
		return AOSOBJNAME_PUBNAMESPACE_CTNR;
	}

	static OmnString composeInvalidReadLogName()
	{
		return AOSOBJNAME_INVALID_READ_CTNR;
	}

	static OmnString composeCidCtnrName()
	{
		return AOSOBJNAME_CID_CTNR;
	}

	static OmnString composeLoginLogCtnrName()
	{
		return AOSOBJNAME_LOGINLOG_CTNR;
	}

	static OmnString composeMblogCtnrName() 
	{
		return AOSOBJNAME_MBLOG_CTNR;
	}
	
	static OmnString composeLostFoundCtnrName() 
	{
		return AOSOBJNAME_LOSTFOUND_CTNR;
	}
	
	static OmnString composeBlogCtnrName() 
	{
		return AOSOBJNAME_BLOG_CTNR;
	}
	
	static OmnString composeAlbumCtnrName() 
	{
		return AOSOBJNAME_ALBUM_CTNR;
	}
	
	static OmnString composeArticleCtnrName() 
	{
		return AOSOBJNAME_ARTICLE_CTNR;
	}
	
	static OmnString composeCommentCtnrName() 
	{
		return AOSOBJNAME_COMMENT_CTNR;
	}
	
	static OmnString composePublishCtnrName() 
	{
		return AOSOBJNAME_PUBLISH_CTNR;
	}

	static OmnString composeImageCtnrName() 
	{
		return AOSOBJNAME_IMAGE_CTNR;
	}

	static OmnString composeDiaryCtnrName() 
	{
		return AOSOBJNAME_DIARY_CTNR;
	}

	static OmnString composeMiscCtnrName()
	{
		return AOSOBJNAME_MISC_CTNR;
	}

	static OmnString composeCloudCardCtnrName()
	{
		return AOSOBJNAME_CLOUDCARD_CTNR;
	}

	static OmnString composeVpdCtnrName()
	{
		return AOSOBJNAME_VPD_CTNR;
	}
};
#endif

