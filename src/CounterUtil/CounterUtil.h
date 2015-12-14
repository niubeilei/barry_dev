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
//  Entry Type Format
//  	Entry Level			four bits
//  	Append Bit			one bit
//  	Statistics Types	five bits
//  	Time Types			four bits
//  	Reserved			two bits
//  	Total:	16 bits
//
//
// Modification History:
// 03/29/2012 Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterUtil_h
#define Aos_CounterUtil_CounterUtil_h

#include "alarm_c/alarm.h"
#include "CounterUtil/StatTypes.h"
#include "SEUtil/IILName.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "UtilTime/TimeGran.h"
#include "Util/StrSplit.h"

#define AOS_ROOT_COUNTER_KEY				"__zkyroot"
#define AOS_TIMED_COUNTER_PREFIX			"__zkykgr642"
#define AOS_COUNTER_FIELD_SEP              	"|$|"
#define AOS_COUNTER_SEGMENT_SEP            	"|$$|"
#define AOS_COUNTER_PART_SEP            	"|$_|"
#define AOS_COUNTER_MEM_SEP        	    	"_"
#define AOS_VAR_PREFIX						"__zcv{"
#define AOS_VAR_PREFIX_LEN					6

#define AOS_COUNTER_SEGMENT_SEP2			"\1"

class AosCounterUtil
{
public:
	enum
	{
		eMaxTransPerBuffTrans = 1000000,
		eLargeValue = 0xfffffffffffffffLL,
		eMaxCounterSegments = 500,
		eMaxPerCounterEntry = 1000000
	};

	enum CounterNameFormat 
	{
		eInvalid,
		eAll,
		eSub,
		eMax
	};

	inline static u64 convertValueToU64(const int64_t &vv)
	{
		return (u64)(vv + eLargeValue);
	}

	inline static int64_t convertValueToInt64(const u64 &vv)
	{
		return (int64_t)(vv - eLargeValue);
	}

	inline static bool isValidCounterName(const OmnString &cname)
	{
		if (cname == "")
		{
			return false;
		}

		if (cname.length() > 3) return true;
		const char *data = cname.data();
		if (data[0] == '_' && 
			data[1] == '_' && 
			data[1] == 'z' && 
			data[2] == 'k' &&
			data[3] == 'y')
		{
			return false;
		}
	
		return true;
	}

	inline static OmnString composeTimeCounterName(
			const OmnString &cname,
			const AosStatType::E statType,
			const u64 &unitime)
	{
		//Tf|$_|cname|$_|unitime
		aos_assert_r(cname != "", "");
		OmnString name;
		char *data = (char *)name.data();
		data[0] = AosTimeGran::eTimeFlag;
		data[1] = AosStatType::toChar(statType);
		name.setLength(2);
		name << AOS_COUNTER_PART_SEP << cname << AOS_COUNTER_PART_SEP 
			<< unitime;
		return name;
	}

	inline static OmnString composeTimeCounterName(
			const OmnString &cname, 
			const AosStatType::E statType,
			const u64 &unitime,
			const OmnString &member)
	{
		aos_assert_r(member != "", "");
		OmnString name;
		char *data = (char *)name.data();
		data[0] = AosTimeGran::eTimeFlag;
		data[1] = AosStatType::toChar(statType);
		name.setLength(2);
		if (cname == "") 
		{
			//Tf|$_|unitime|$_|member
			name << AOS_COUNTER_PART_SEP << unitime << AOS_COUNTER_PART_SEP << member;
		}
		else
		{
			//Tf|$_|cname|$$|unitime|$_|member
			name << AOS_COUNTER_PART_SEP << cname << AOS_COUNTER_SEGMENT_SEP 
				<< unitime << AOS_COUNTER_PART_SEP << member;
		}
		return name;
	}

	inline static OmnString composeCounterName(
			const OmnString &cname, 
			const AosStatType::E statType,
			const OmnString &member)
	{
		//Nf|$_|cname|$_|member
		OmnString name;
		char *data = (char *)name.data();
		data[0] = AosTimeGran::eNoTimeFlag;
		data[1] = AosStatType::toChar(statType);
		name.setLength(2);
		name << AOS_COUNTER_PART_SEP << cname << AOS_COUNTER_PART_SEP << member;
		return name;
	}

	inline static OmnString composeRootCounterName(const AosStatType::E statType)
	{
		//Nf|$_|__zkyroot
		OmnString name;
		char *data = (char *)name.data();
		data[0] = AosTimeGran::eNoTimeFlag;
		data[1] = AosStatType::toChar(statType);
		name.setLength(2);
		name << AOS_COUNTER_PART_SEP << AOS_ROOT_COUNTER_KEY;
		return name;
	}

	inline static OmnString composeRootTimeCounterName(
				const AosStatType::E statType,
				const u64 &unitime)
	{
		//Tf|$_|__zkyroot|$_|unitime
		OmnString name;
		char *data = (char *)name.data();
		data[0] = AosTimeGran::eTimeFlag;
		data[1] = AosStatType::toChar(statType);
		name.setLength(2);
		name << AOS_COUNTER_PART_SEP << AOS_ROOT_COUNTER_KEY 
			<< AOS_COUNTER_PART_SEP << unitime;
		return name;
	}

	inline static OmnString composeCounterKey(const OmnString &cname, const AosStatType::E type)
	{
		// Counter Key is in the form:
		// 	Nf|$_|cname
		// where 'T' is the time flag and 'S' is the statistics flag.
		OmnString key;
		key << (char)AosTimeGran::eNoTimeFlag
			<< AosStatType::toChar(type) << AOS_COUNTER_PART_SEP << cname;
		return key;
	}

	inline static OmnString composeTimeCounterKey(const OmnString &cname, const AosStatType::E type)
	{
		// Counter Key is in the form:
		// 	Tf|$_|cname
		// where 'T' is the time flag and 'S' is the statistics flag.
		OmnString key;
		key << (char)AosTimeGran::eTimeFlag
			<< AosStatType::toChar(type) << AOS_COUNTER_PART_SEP << cname;
		return key;
	}
	
	inline static OmnString composeTimeCounterKey(
			const OmnString &cname, 
			const AosStatType::E type, 
			const u64 &unitime)
	{
		// Counter Key is in the form:
		// 	TS|$_|cname|$$|unitime
		// where 'T' is the time flag and 'S' is the statistics flag.
		OmnString key;
		key << (char)AosTimeGran::eTimeFlag
			<< AosStatType::toChar(type) << AOS_COUNTER_PART_SEP << cname 
			<< AOS_COUNTER_SEGMENT_SEP << unitime;
		return key;
	}

	inline static OmnString composeTimeCounterKey(
			const OmnString &cname, 
			const AosStatType::E type, 
			const OmnString &member)
	{
		// Counter Key is in the form:
		// 	Tf|$_|cname|$_|member
		// 	member == ""  result: Tf|$_|cname|$_|
		// where 'T' is the time flag and 'S' is the statistics flag.
		OmnString key;
		key << (char)AosTimeGran::eTimeFlag
			<< AosStatType::toChar(type) << AOS_COUNTER_PART_SEP << cname 
			<< AOS_COUNTER_PART_SEP << member;
		return key;
	}

	inline static OmnString composeTimeCounterNamePart(const OmnString &cname, const u64 &unitime)
	{
		//Tf|$_|cname|$_|unitime
		aos_assert_r(cname != "", "");
		OmnString name;
		name << cname << AOS_COUNTER_PART_SEP << unitime;
		return name;
	}

	inline static OmnString composeTimeContainerPart(const OmnString &cname, const u64 &unitime)
	{
		//Tf|$_|cname|$$|unitime
		aos_assert_r(cname != "", "");
		OmnString name;
		name << cname << AOS_COUNTER_SEGMENT_SEP << unitime;
		return name;
	}

	inline static OmnString getTimePart(const OmnString cname)
	{
		//xxxx|$$|xxxx|$$|xxxx|$_|20120514
		vector<OmnString> timestr;
		bool finished = false;
		//AosStrSplit::splitStr(cname.data(), AOS_COUNTER_PART_SEP, timestr, 3, finished);
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_PART_SEP, timestr, 3, finished);
		aos_assert_r(timestr.size() == 3, "");
		return timestr[2]; 
	}

	inline static OmnString getTimePart1(const OmnString cname)
	{
		//xxxx|$$|xxxx|$$|20120514|$_|member
		OmnString container = getContainerPart(cname); 
		aos_assert_r(container != "", "");
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(container.data(), AOS_COUNTER_SEGMENT_SEP, str, 100, finished);
		aos_assert_r(str.size() > 0, "");	
		int idx = str.size()-1;
		return str[idx];
	}

	inline static OmnString getContainerPart(const OmnString cname)
	{
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_PART_SEP, str, 100, finished);
		if (str.size() == 1) return cname;
		if (str.size() == 2) return str[0];
		aos_assert_r(str.size() > 2, "");
		return str[1]; 
	}

	inline static OmnString getCounterNamePart(const OmnString &cname)
	{
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_PART_SEP, str, 100, finished);
		if (str.size() == 2) return str[1];
		aos_assert_r(str.size() >= 3, "");
		OmnString ss;
		ss << str[1] << AOS_COUNTER_PART_SEP << str[2];
		return ss;
	}

	inline static OmnString getCounterNamePart1(const OmnString &cname)
	{
		//Nf|$$_|xxxx|$$|xxxx|$$|20120514|$_|member
		//result: xxxx|$$|xxxx|$$|member 
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_PART_SEP, str, 100, finished);
		aos_assert_r(str.size() > 2, "");
		OmnString type = str[0];
		OmnString member = str[2];
		OmnString container = str[1];
		str.clear();
		AosStrSplit::splitStrBySubstr(container.data(), AOS_COUNTER_SEGMENT_SEP, str, 100, finished);
		aos_assert_r(str.size() > 0, "");
		OmnString ss;
		for (u32 i = 0; i< str.size() -1; i++)
		{
			if (ss == "") 
			{
				ss  << type << AOS_COUNTER_PART_SEP << str[i];
			}
			else
			{
				ss << AOS_COUNTER_SEGMENT_SEP << str[i];
			}
		}
		if (member != "") ss << AOS_COUNTER_PART_SEP << member;
		return ss;
	}
	
	inline static bool isValidTimeCounterName(const OmnString &cname)
	{
		if (cname.data()[0] != AosTimeGran::eTimeFlag) return false;
		return true;
	}

	inline static bool isValidNoTimeCounterName(const OmnString &cname)
	{
		if (cname.data()[0] != AosTimeGran::eNoTimeFlag) return false;
		return true;
	}

	inline static OmnString composeCounterIILName(const OmnString &cname)
	{
		//_zt1i_counter_id
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP, str, 100, finished);
		aos_assert_r(str.size() > 0, "");
		OmnString counter_id = str[0];
		return  AosIILName::composeCounterIILName(counter_id);
	}

	inline static OmnString getMemberPart(const OmnString cname)
	{
		//1. Nf|$_|xxx|$$|xxx|$$|xxx|$_|member
		//2. xxx|$$|xxx|$$|xxx|$_|member
		//result: member
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_PART_SEP, str, 100, finished);
		if (str.size() == 2) return str[1];
		if (str.size()< 3) return "";
		return str[2]; 
	}

	inline static OmnString getCounterName(const OmnString cname)
	{
		//Nf|$$|xxx|$$|xx|$$|xxx|$_|member
		//result: xxx|$$|xxx|$$|member
		if (cname == "") return "";
		OmnString container = getContainerPart(cname);
		if (container == "") return "";

		OmnString member = getMemberPart(cname);
		if (member == "") return container;
		
		OmnString str = container;
		str << AOS_COUNTER_SEGMENT_SEP << member;
		return str;
	}


	inline static bool splitCounterName(const OmnString &cname,  vector<OmnString> &str)
	{
		if (cname == "") return false;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP, str, 100, finished);
		return true;
	}

//////////////////////////////NEW/////////////////////////////////////////////////
	inline static OmnString composeTerm2(
			const u16 &entry_type,
			const OmnString &cname,
			const u64 &unitime)
	{
		return composeTerm2(entry_type, "", cname, unitime);
	}

	inline static OmnString composeTerm2(
			const u16 entry_type,
			const OmnString &str_header,
			const OmnString &cname, 
			const u64 &unitime)
	{
		//composeTimeCounterKey2
		// Counter Key is in the form:
		// 	EntryType: level(4bits) + Append Bit(1bits) + StatType(5bits)
		// 				+ time Type(4bits) + reserved(2bits)
		// 	EntryType.cname.unitime
		// where 'T' is the time flag and 'S' is the statistics flag.
		// timeType is "mnt", append bit(1bits) set 0;
		// append bit(1bits) definition:
		// example: cname 							append bit
		//			timeterm							0
		//			timeterm.term0						0
		//			timeterm.term1						0
		//
		//			term0.timeterm						1
		//			term0.term1.timeterm				1
		// 			term0.timeterm.term1				1 
		// 			term0.time1.timeterm.term2			1
		// 			term0.time1.timeterm.term2.term3	1
		//compose key
		OmnString key((char *)&entry_type, sizeof(u16));

		// compose str_header
		if (str_header != "") 
		{
			key << str_header << AOS_COUNTER_SEGMENT_SEP2;
		}

		// compse time and cname
		int append_bit = (entry_type & 0x800) >> 11;
		if (unitime && cname != "") 
		{
			if (append_bit == 0)
			{
				key << unitime << AOS_COUNTER_SEGMENT_SEP2 << cname;
			}
			else
			{
				key << cname << AOS_COUNTER_SEGMENT_SEP2 << unitime;
			}
		}
		else
		{
			if (cname != "") key << cname; 
			if (unitime) key << unitime;
		}
//		if (append_bit == 0)
//		{
//			// timeterm.term0
//			if (unitime && cname != "")
//			{
//				key << unitime << AOS_COUNTER_SEGMENT_SEP2 << cname;
//			}
//			else
//			{
//				if (cname != "") key << cname; 
//				if (unitime) key << unitime;
//			}
//		}
//		else
//		{
//			//term0.timeterm
//			if (cname != "") key << cname;
//			if (unitime) key << AOS_COUNTER_SEGMENT_SEP2 << unitime;
//		}
		return key;
	}

	inline static OmnString modifyStatType(const OmnString &name, const AosStatType::E stat_type)
	{
		aos_assert_r(name.length() >= 2, "");
		u16 entry_type = *(u16 *)name.data();
		aos_assert_r(entry_type, "");

		entry_type = entry_type | ((u16)stat_type << 6); 
		OmnString key((char *)&entry_type, sizeof(u16));
		if(name.length() <= 2) return key;
		key << getAllTerm2(name); 
		return key;
	}

	inline static OmnString composeStrTerm2(const OmnString &cname, const OmnString &str)
	{
		aos_assert_r(cname != "", "");
		OmnString name;
		name << cname << AOS_COUNTER_SEGMENT_SEP2 << str;
		return name;
	}

	inline static u16 composeTimeEntryType(
			const int level,
			int append_bit,
			const AosStatType::E stat_type,
			const AosTimeGran::E time_gran)
	{
		// Append Bit. There is a bit to indicate whether time value is appended (1) or prepended.
		// 	EntryType: level(4bits) + Append Bit(1bits) + StatType(5bits)
		// 				+ time Type(4bits) + reserved(2bits)
		u16 entry_type = ((u16)level << 12)+
			((u16)append_bit << 11)+
			((u16)stat_type << 6)+
			((u16)time_gran << 2);
		return entry_type;
	}

	inline static u16 modifyEntryType(
			const u16 &entry_type,
			const int level,
			int append_bit,
			const AosTimeGran::E time_gran)
	{
		u16 entrytype = entry_type;
		entrytype = entrytype | ((u16)level << 12); 
		entrytype = entrytype | ((u16)append_bit << 11); 
		entrytype = entrytype | ((u16)time_gran << 2);
		return entrytype;
	}


	inline static OmnString getAllTerm2(const OmnString &cname)
	{
		// getCounterNamePart2
		// cname : EntryTypeTerm0.Term1.Term2
		// result: Term0.Term1.Term2
		aos_assert_r(cname.length() > 2, "");
		OmnString ss(&cname.data()[2], cname.length()-2);
		return ss;
	}


	inline static OmnString getTerm2(const OmnString &cname, const int index) 
	{
		// cname: Term0.Term1.Term2
		// result:TermIndex
		if (cname == "" || index < 0) return "";
		bool finished = false;
		vector<OmnString> str;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
		aos_assert_r((u32)index < str.size(), "");
		return str[index];
	}

	inline static bool splitTerm2(const OmnString &cname,  vector<OmnString> &str)
	{
		//splitCounterName2
		if (cname == "") return false;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
		return true;
	}

	inline static OmnString decomposeReplaceTerm(
			const OmnString &cname, 
			const int replace_index,
			const OmnString &kk)
	{
		//1. entryTypexxx.xxx.member
		if (replace_index < 0) return cname;
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
		if ((u32)replace_index >= str.size()|| str.size() == 0) return cname;

		OmnString ss;
		for (u32 i = 0; i < str.size(); i++)
		{
			if (ss != "") ss << AOS_COUNTER_SEGMENT_SEP2;

			if (i == (u32) replace_index) 
			{
				ss << kk;
			}
			else
			{
				ss << str[i];
			}
		}
		if (replace_index != 0) return ss;
		
		OmnString new_str(cname.data(), 2);
		new_str << ss;
		return new_str;
	}


	inline static OmnString decomposeRemoveTerm(const OmnString &cname, const int remove_index, bool is_entryType = true)
	{
		//1. entryTypexxx.xxx.member
		if (remove_index < 0) return cname;
		vector<OmnString> str;
		bool finished = false;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
		if ((u32)remove_index >= str.size()|| str.size() == 0) return cname;

		OmnString ss;
		for (u32 i = 0; i < str.size(); i++)
		{
			if (i == (u32) remove_index) continue;

			if (ss != "") ss << AOS_COUNTER_SEGMENT_SEP2;
			ss << str[i];
		}
		if (remove_index != 0) return ss;
		if (!is_entryType) return ss;
		
		OmnString new_str(cname.data(), 2);
		new_str << ss;
		return new_str;
	}


	inline static OmnString decomposeRemoveTimeTerm(const OmnString &cname)
	{
		//1. entryType.xxx.xxx.member
		//result: member
		u16 entry_type = *(u16 *)cname.data();
		aos_assert_r(entry_type, "");
		
		int append_bit = (entry_type & 0x800) >> 11;
		vector<OmnString> str;
		bool finished = false;
		u32 timeterm_idx = 0;
		switch (append_bit)
		{
		case 0:
			 // prepended  format: EntryTypeTerm0.Term1.Term2.TermTime.Term4
			 AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
			 if(str.size() <= 1) return cname;

			 aos_assert_r(str.size() >= 1, "");
			 timeterm_idx = str.size() - 2;
			 break;
		case 1:
			 // appended format: EntryTypeTerm0.term1.Term3.Term4.TermTime
			 AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);
			 if(str.size() <= 1) return cname;

			 aos_assert_r(str.size() >= 1, "");
			 timeterm_idx = str.size() - 1;
			 break;
		default:
			 OmnAlarm << "Missing !"<< enderr;
			 return "";
		}

		aos_assert_r(str.size() > 0, "");
		OmnString ss = str[0];
		for (u32 i = 1; i < str.size(); i++)
		{
			if (i != timeterm_idx) 
			{
				ss << AOS_COUNTER_SEGMENT_SEP2 << str[i];
			}
		}
		return ss;
	}

	inline static  AosCounterUtil::CounterNameFormat convertCnameFormat(const OmnString &formatstr)
	{
		// term0.term1.time.term2
		// term0.time.term1
		// time.term0
		// time
		if (formatstr == "") return AosCounterUtil::eSub;
		vector<OmnString> str;
		int n = AosStrSplit::splitStrBySubstr(formatstr.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100);
		aos_assert_r(n > 0 && (u32)n == str.size(),  AosCounterUtil::eMax);
		if (n >= 2 && str[n-2] == "time")
		{
			return AosCounterUtil::eAll;
		}
		return AosCounterUtil::eSub;
	}


	inline static OmnString  decomposeCname(
			const OmnString &cname, 
			OmnString &fieldstr,
			int &stat_field_idx) 
	{
		// data format: cn.js.15492.sz
		//input format: cn.js.sz.15492   
		//			 		part	  fieldstr   stat_idx
		//output format: cn.js.15492     sz       3 
		OmnString part = "";
		vector<OmnString> str;
		int n = AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100);
		aos_assert_r(n >= 2 && (u32)n == str.size(), "");
		int timeidx = n - 2;

		stat_field_idx = n - 1; 
		fieldstr = str[timeidx];
		for (u32 i = 0; i < str.size(); i++)
		{
			if (i == (u32)timeidx) continue; 
			if (part!= "") part << AOS_COUNTER_SEGMENT_SEP2;
			part << str[i];
		}

		if (timeidx == 0)
		{
			//input: cn.15492
			//output: 15942.cn
			u16 entry_type = *(u16 *)cname.data();
			aos_assert_r(entry_type, "");

			OmnString kk((char *)&entry_type, sizeof(u16));
			kk << part;
			part = kk;
			aos_assert_r(fieldstr.length() > 2, "");
			OmnString k1(&fieldstr.data()[2], fieldstr.length()-2);
			fieldstr = k1;
		}
		return part;
	}

	inline static OmnString composeCounterIILName(const OmnString &cname, OmnString &name)
	{
		//_zt1i_counter_id
		vector<OmnString> str;
		AosStrSplit::splitStrBySubstr(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100);
		aos_assert_r(str.size() > 0, "");
		OmnString counter_id = str[0];
		for (u32 i = 1; i < str.size(); i++)
		{
			if (name != "") name << AOS_COUNTER_SEGMENT_SEP2;
			name << str[i];
		}
		return  AosIILName::composeCounterIILName(counter_id);
	}


/////////////////////////////////////////////////////////////////////////////////////

};
#endif
