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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgProc_GareaMsgProc_h
#define AOS_MsgProc_GareaMsgProc_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MsgProc/MsgProc.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocClientObj.h"
#include <vector>
#include <map>
using namespace std;


#define AOSSOAPDOC_GAREADOC			"garea_doc_0"

#define AOSTAG_BODY					"body"
#define AOSTAG_COMMAND				"command"
#define AOSTAG_PAGE					"page"
#define AOSTAG_DEVICEID				"deviceId"
#define AOSTAG_REQUEST				"request"
#define AOSTAG_UPDATETIME			"updateTime"
#define AOSTAG_PATIENT_ATTR			"zky_patient_attr"
#define AOSTAG_PATIENT_VALUE		"zky_patient_value"
#define AOSTAG_DOCTOR_ATTR			"zky_doctor_attr"
#define AOSTAG_DOCTOR_VALUE			"zky_doctor_value"
#define AOSTAG_SYNCINTERVAL			"zky_syncinterval" 
#define AOSTAG_ANDROID_PARA			"zky_android_para"
#define AOSTAG_ERRORCODE			"zky_errorcode" 
#define AOSTAG_DEVICE_CTNR			"zky_device_ctnr"
#define AOSTAG_PATIENT_CTNR			"zky_patient_ctnr" 
#define AOSTAG_DEVIECE_ATTR			"zky_device_attr" 
#define AOSTAG_DOCTOR_ID			"zky_doctor_id" 


class AosGareaMsgProc : public AosMsgProc
{
	OmnDefineRCObject;

	struct ShengliInfo
	{
		vector<int>	HR;
		vector<int>	SPO2;
		vector<int>	PR;
		vector<int>	NIBP_SYS;
		vector<int>	NIBP_DIA;
		vector<int>	NIBP_MAP;
		vector<float>	GLU;
		vector<int> RES1;
		vector<int> RES2;
		vector<int> RES3;
		OmnString   RES4;
	};

	struct UploadInfo
	{
		OmnString user_cloudid;
		OmnString doctor_id;
		u64       doctor_docid;
		OmnString data;
		OmnString card_num;
		OmnString name;
		OmnString id_number;
		OmnString sex;		
		
		UploadInfo()
		{
		}

		UploadInfo(const OmnString &u, 
				const OmnString &d,
				const u64 &dt_did,
				const OmnString &dt,
				const OmnString c_n,
				const OmnString nm,
				const OmnString idnum,
				const OmnString s)
		:
		user_cloudid(u),
		doctor_id(d),
		doctor_docid(dt_did),
		data(dt),
		card_num(c_n),
		name(nm),
		id_number(idnum),
		sex(s)	
		{
		}
		
		UploadInfo &operator = (const UploadInfo &rhs)
		{
			if (this == &rhs) return *this;
			user_cloudid = rhs.user_cloudid;
			doctor_id = rhs.doctor_id;
			doctor_docid = rhs.doctor_docid;
			data = rhs.data;
			card_num  = rhs.card_num  ;
			name      = rhs.name      ;
			id_number = rhs.id_number ;
			sex	      = rhs.sex	      ;
			return *this;
		}
	};

	typedef map<OmnString, UploadInfo> AosUpdHtryDataSMap;
	typedef map<OmnString, UploadInfo>::iterator AosUpdHtryDataSMapItr;
	typedef map<OmnString, UploadInfo> AosUpdRtDataSMap;
	typedef map<OmnString, UploadInfo>::iterator AosUpdRtDataSMapItr;

private:
	
	enum
	{
		eXueYangDataSize = 60,
		eShengliDataSize = 40,
		eStatusDataSize = 24,
		eEthernetDataSize = 32,
		eWifiDataSize = 104,
		eMaxConds = 5,
		eMaxDataLen = 4000000,		// 4M
		eCardioElectSize = 8500,
		eInterval = 1000
	};

	enum AosDataType
	{
		eXindian		= 1,
		eXueya			= 2,
		eXuetang		= 3,
		eXueyang		= 4
	};

	enum AosCmd
	{
		eInValid,

		eLogin 			  = 1,
		eSyncPtntInfo	  = 2,
		eSetAdroidPara	  = 3,
		eUnDefined		  = 4,
		eUpdHisData		  = 5,
		eUpdRtimeData  	  = 6,
		eSyncDctrInfo	  = 7,
		
		eMax
	};

	struct AosSoapRequest
	{
		AosCmd			mCmd;
		int 			mPage;
		int 			mErrCode;
		OmnString		mVersion;
		OmnString		mDvcId;
		OmnString 		mErrmsg;

		AosSoapRequest(){};
		~AosSoapRequest(){};
		void setCmd(const AosCmd &cmd)
		{
			mCmd = cmd;
		}
		void setVersion(const OmnString &ver)
		{
			mVersion = ver;
		}
		void setDvcId(const OmnString &vcid)
		{
			mDvcId = vcid;
		}
		void setErrCode(const int &errcode)
		{
			mErrCode = errcode;
		}
		void setErrmsg(const OmnString &errmsg)
		{
			mErrmsg = errmsg;
		}
		void setPage(const int &pnum)
		{
			mPage = pnum;
		}
		AosCmd getCmd() const
		{
			aos_assert_r(mCmd>eInValid 
					&& mCmd < eMax 
					&& mCmd != eUnDefined, 
					eInValid);
			return mCmd;
		}
		OmnString getVersion() const
		{
			return mVersion;
		}
		OmnString getDvcId() const
		{
			return mDvcId;
		}
		int getErrCode() const
		{
			return mErrCode;
		}
		OmnString  getErrmsg() const
		{
			return mErrmsg;
		}
		int getPage() const
		{
			return mPage;
		}

		AosCmd toCmd(const OmnString &cmd)
		{
			if (cmd == "1") return eLogin;
			if (cmd == "2") return eSyncPtntInfo;
			if (cmd == "3") return eSetAdroidPara;
			if (cmd == "5") return eUpdHisData;
			if (cmd == "6") return eUpdRtimeData; 
			if (cmd == "7") return eSyncDctrInfo;
			else
			{
				OmnAlarm << "Invalid command" << enderr;
				return eInValid;
			}
			OmnShouldNeverComeHere;
			return eInValid;
		}
	};


private:
	u64								mMaxDataSize;
	int								mPageSize;
	OmnString						mDeviceCtnr;
	OmnString						mPatientCtnr;
	AosXmlTagPtr 					mDoc;
	AosUpdHtryDataSMap				mUpdHtryDataSMap;
	AosUpdRtDataSMap				mUpdRtDataSMap;
	AosDocClientObjPtr 				mDocClientObj;

	int								mMax_xindian;
	int								mMin_xindian;
	int								mMax_xueyangwave;
	int								mMin_xueyangwave;
	int								mMax_spo2;
	int								mMin_spo2;
	float							mMax_glu;
	float							mMin_glu;
	int								mMax_nibp_sys;
	int								mMin_nibp_sys;
	int								mMax_nibp_dia;
	int								mMin_nibp_dia;
	int								mMax_nibp_map;
	int								mMin_nibp_map;

public:
	AosGareaMsgProc(const bool flag);
	~AosGareaMsgProc();

	virtual bool msgReceived(const OmnString &appid, 
							 const AosXmlTagPtr &msg, 
							 const AosRundataPtr &rdata);
	bool				init();
	bool				initParam();
private:
	bool				login(
						AosSoapRequest &req,			
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);

	bool				syncPatientInfo(
						AosSoapRequest &req,
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);

	bool				setAndroidPara(
						AosSoapRequest &req,
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);

	bool				uploadHistoryData(
						AosSoapRequest &req,
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);

	bool				uploadRtimeData(
						AosSoapRequest &req,
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);

	bool				syncDoctorInfo(
						AosSoapRequest &req,
						const AosXmlTagPtr &body, 
						const AosRundataPtr &rdata);
	
	
	OmnString			getTime();

	bool				parseCmd(
						AosSoapRequest &req, 
						const AosXmlTagPtr &header, 
						const AosRundataPtr &rdata);

	u32					timeStr2Int(const OmnString &timestr);

	OmnString			parseQueryRslt(
		 	            AosSoapRequest &req,
						const OmnString &tagname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata);

	bool				sendResponse(
				        AosSoapRequest &req,
				        const OmnString &response,
				        const int &interval,
				        const AosRundataPtr &rdata);
	
	bool 				uploadCardioElect(
						AosSoapRequest &req, 
						OmnString &ch1,
						OmnString &ch2,
						OmnString &ch3,
						OmnString &ch4,
						OmnString &ch5,
						OmnString &ch6,
						OmnString &ch7,
						OmnString &ch8,
						OmnString &flags,
						int &idx, 
						const u8 *data, 
						const int data_len);

	bool				uploadXueYang(
						OmnString &xindian,
						int &idx, 
						const u8 *data, 
						const int data_len);
	
	bool				uploadShengLi(
						OmnString &xindian,
						int &idx, 
						const u8 *data, 
						const int data_len);
	
	bool				procStatus(
						int &idx, 
						const char *data,
						const int datalen);

	bool				procNetworkConfig(
						int &idx, 
						const char *data,
						const int datalen);
	
	bool				procWiFi(
						int &idx, 
						const char *data,
						const int datalen);
	
	OmnString			getSsid();

	
	AosXmlTagPtr		readSmartDoc();
	bool 				uploadShengli(
						ShengliInfo &shengli, 
						int &idx, 
						const u8 *data_mem, 
						const int data_len);
	AosXmlTagPtr 		getUserDoc(
						const OmnString &user_cloudid, 
						const OmnString &account_ctnr, 
						const AosRundataPtr &rdata);
	bool 				createDocForXindian(
						AosSoapRequest &req,
						const OmnString &user_cloudid,
						const OmnString &doctor_id,
						const OmnString &card_num,
						const OmnString &name,
						const OmnString &id_number,
						const OmnString &sex,
						const OmnString &ch1,
						const OmnString &ch2,
						const OmnString &ch3,
						const OmnString &ch4,
						const OmnString &ch5,
						const OmnString &ch6,
						const OmnString &ch7,
						const OmnString &ch8,
						const OmnString &flags,
						const AosRundataPtr &rdata);
	bool 				createDocForXueyangShengli(
						AosSoapRequest &req,
						const OmnString &user_cloudid,
						const OmnString &doctor_id,
						const OmnString &card_num,
						const OmnString &name,
						const OmnString &id_number,
						const OmnString &sex,
						const OmnString &xueyang,
						const ShengliInfo &shengli,
						const AosRundataPtr &rdata);

	bool 				createDocForXueya(
						AosSoapRequest &req,
						const OmnString &user_cloudid,
						const OmnString &doctor_id,
						const OmnString &card_num,
						const OmnString &name,
						const OmnString &id_number,
						const OmnString &sex,
						const ShengliInfo &shengli,
						const AosRundataPtr &rdata);

	bool 				createDocForXuetang(
						AosSoapRequest &req,
						const OmnString &user_cloudid,
						const OmnString &doctor_id,
						const OmnString &card_num,
						const OmnString &name,
						const OmnString &id_number,
						const OmnString &sex,
						const ShengliInfo &shengli,
						const AosRundataPtr &rdata);

	bool 				createDocForXueyang(
						AosSoapRequest &req,
						const OmnString &user_cloudid,
						const OmnString &doctor_id,
						const OmnString &card_num,
						const OmnString &name,
						const OmnString &id_number,
						const OmnString &sex,
						const OmnString &xueyang,
						const ShengliInfo &shengli,
						const AosRundataPtr &rdata);
};
#endif

