////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/CompareFun.h"

#include "XmlUtil/XmlTag.h"

//int (*AosCompareFun::mCmpFuncs[15])(const char *lhs, const char* rhs, const int cmp_pos);

inline int _cstr_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	return strcmp(lhs + cmp_pos, rhs + cmp_pos);
}

/*
inline int _num_str_cmp(AosCompareFun::CompareData* cmp)
{
	if (cmp->cmp_len == -1)
	{
		int lLen = strlen(cmp->lhs + cmp->cmp_pos);
		int rLen = strlen(cmp->rhs + cmp->cmp_pos);
		if(lLen < rLen)
			return -1;
		else if(lLen > rLen)
			return 1;
		return strcmp(cmp->lhs + cmp->cmp_pos, cmp->rhs + cmp->cmp_pos);
	}
	return strncmp(cmp->lhs + cmp->cmp_pos, cmp->rhs + cmp->cmp_pos, cmp->cmp_len);
}
*/

inline int _buff_str_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	u32 llen = *(u32*)(lhs - sizeof(u32) + cmp_pos);
	u32 rlen = *(u32*)(rhs - sizeof(u32) + cmp_pos);
	if (llen == rlen)
	{
		return strncmp(lhs, rhs, llen);
	} 
	else
	{
		int len = llen > rlen ? rlen : llen;
		int rslt = strncmp(lhs, rhs, len);
		if (rslt == 0)
		{
			if (llen > rlen) return 1;
			else return -1;
		}
		return rslt;
	}
}

inline int _u64_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	u64 l = *(u64*)(lhs + cmp_pos);
	u64 r = *(u64*)(rhs + cmp_pos);
	if (l == r) return 0;
	else if (l > r) return 1;
	else return -1;
}

inline int _u32_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	u32 l = *(u32*)(lhs + cmp_pos);
	u32 r = *(u32*)(rhs + cmp_pos);
	if (l == r) return 0;
	else if (l > r) return 1;
	else return -1;
}

inline int _i64_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	i64 l = *(i64*)(lhs + cmp_pos);
	i64 r = *(i64*)(rhs + cmp_pos);
	if (l == r) return 0;
	else if (l > r) return 1;
	else return -1;
}

inline int _i32_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{
	i32 l = *(i32*)(lhs + cmp_pos);
	i32 r = *(i32*)(rhs + cmp_pos);
	if (l == r) return 0;
	else if (l > r) return 1;
	else return -1;
}

inline int _double_cmp(const char *lhs, const char* rhs, const int cmp_pos)
{

	double l = *(double*)(lhs + cmp_pos);
	double r = *(double*)(rhs + cmp_pos);
	if (l == r) return 0;
	else if (l > r) return 1;
	else return -1;
}


AosCompareFunPtr
AosCompareFun::getCompareFunc(const AosXmlTagPtr &config)
{
	// The config should be in the form:
	// 	<compfunc 
	// 		cmpfun_type="xxx"
	// 		cmpfun_size="xxx"/>
	aos_assert_r(config, NULL);
	OmnString typestr = config->getAttrStr("cmpfun_type");
	FunType type = getFunType(typestr);
	aos_assert_r(type != eInvalid, NULL);

	int size = config->getAttrInt("cmpfun_size", 0);
	//bool numalphabetic = config->getAttrBool("cmpfun_alphabetic", false);
	bool numalpha = config->getAttrBool("cmpfun_alphabetic", false);
	bool reserve = config->getAttrBool("cmpfun_reserve", false);
	switch (type)
	{
	//case eU64U64IIL1:
	//	 return OmnNew AosFunU64U641(reserve);

//	case eU64U64IIL2:
//		 return OmnNew AosFunU64U642(reserve);

//	case eStrU64IIL1:
//		 aos_assert_r(size != 0, NULL);
//		 return OmnNew AosFunStrU641(size, reserve,  numalphabetic);

//	case eStrU64IIL2:
//		 aos_assert_r(size != 0, NULL);
//		 return OmnNew AosFunStrU642(size, reserve, numalphabetic);

	case eCustom:
		 //aos_assert_r(size != 0, NULL);
		 return OmnNew AosFunCustom(size, reserve, config);

	case eVarStr:		// Levi, 2014/08/21
		 aos_assert_r(size != 0, NULL);
		 return OmnNew AosFunVarStr(size, reserve, numalpha);
		 
	default:
		 break;
	}

	OmnAlarm << "Unrecognized func type: " << type << enderr;
	return 0;
}


AosFunCustom::AosFunCustom(int _size, bool reserve, const AosXmlTagPtr &config):
AosCompareFun(_size, reserve)
{
	aos_assert(config);
	int cmp_pos, cmp_len;
	OmnString typestr;
	DataType data_type, field_type;

	mRecordType = AosDataRecordType::toEnum(config->getAttrStr("record_type"));

	memset(mCmpFuncs, 0, 15*sizeof(void*));
	mCmpFuncs[(int)eStr] = &_cstr_cmp;
//	mCmpFuncs[(int)eNumStr] = &_num_str_cmp;
	mCmpFuncs[(int)eBuffStr] = &_buff_str_cmp;
	mCmpFuncs[(int)eI32] = &_i32_cmp;
	mCmpFuncs[(int)eI64] = &_i64_cmp;
	mCmpFuncs[(int)eU32] = &_u32_cmp;
	mCmpFuncs[(int)eU64] = &_u64_cmp;
	mCmpFuncs[(int)eDouble] = &_double_cmp;
	mCmpFuncs[(int)eNumber] = &_double_cmp;


	AosXmlTagPtr datafields = config->getFirstChild("datafields");
	if (datafields)
	{
		AosXmlTagPtr field = datafields->getFirstChild();
		OmnString type_str;
		while(field)
		{
			type_str = field->getAttrStr("type");
			AosDataFieldType::E type = AosDataFieldType::toEnum(type_str);
			aos_assert(AosDataFieldType::isValid(type));
			mFieldsType.push_back(type);
			field = datafields->getNextChild();
		}
	}
	//mRcdFdsNum = mFieldsType.size();
mFieldsType.push_back(AosDataFieldType::eStr);
mFieldsType.push_back(AosDataFieldType::eBinU64);
mRcdFdsNum = config->getAttrInt("record_fields_num", 0);	//for test barry
	if (mRcdFdsNum != 0)
	{
		size = sizeof(int) + sizeof(i64) + sizeof(u16) * mRcdFdsNum;
	}

	int pre_pos = 0;
	int pre_len = 0;
	bool is_reserve = false;
	AosXmlTagPtr cmp_fields = config->getFirstChild("cmp_fields");
	aos_assert(cmp_fields);

	AosXmlTagPtr field = cmp_fields->getFirstChild("field");
	//no cmpfield
	//aos_assert(field);

	memset(mFields, 0, 50*sizeof(CmpFieldInfo));
	mFieldSize = 0;
	while(field)
	{
		cmp_pos = field->getAttrInt("cmp_pos", -1);
		aos_assert(cmp_pos != -1);

		typestr = field->getAttrStr("cmp_datatype", "");
		data_type = getDataType(typestr);
		aos_assert(data_type != eInvalidDataType);

		cmp_len = field->getAttrInt("cmp_size", -1);
		typestr = field->getAttrStr("field_type", "");
		field_type = getDataType(typestr);
		//this assert should not be commented out, but there are way too much hard coded code, White, 2015-12-10 09:46:28
		//aos_assert(field_type != eInvalidDataType);		//Andy and White, 2015-12-09 15:04:17

		//aos_assert(pre_pos + pre_len <= cmp_pos);

		pre_pos = cmp_pos;
		pre_len = cmp_len;

		is_reserve = field->getAttrBool("cmp_reserve", false);

		CmpFieldInfo cmp_info;
		cmp_info.mCmpPos = cmp_pos;
		cmp_info.mDataType = data_type;
		cmp_info.mCmpLen = cmp_len;
		cmp_info.mFieldType = field_type;
		cmp_info.mIsReserve = is_reserve;

		mFields[mFieldSize] = cmp_info;
		mFieldSize++;
		field = cmp_fields->getNextChild();
	}

	AosXmlTagPtr agrs = config->getFirstChild(AOSTAG_AGGREGATIONS);
	if(agrs)
	{
		AosXmlTagPtr agr = agrs->getFirstChild(AOSTAG_AGGREGATION);
		while (agr)
		{
			AosAgr agr_data;
			agr_data.mAgrPos = agr->getAttrU32("agr_pos", 0);
			agr_data.mAgrType = getDataType(agr->getAttrStr("agr_type"));
			aos_assert(agr_data.mAgrType != eInvalidDataType);

			typestr = agr->getAttrStr("field_type");
			agr_data.mAgrFieldType = getDataType(typestr);
			//barry 2015/12/17
			if (agr_data.mAgrFieldType == eAgrStr)
				mHasAgrStr = true;
			agr_data.mAgrFun = AosDataColOpr::toEnum(agr->getAttrStr("agr_fun"));
			mAosAgrs.push_back(agr_data);
			agr = agrs->getNextChild();
		}
	}
	aos_assert(pre_pos + pre_len <= size);
}

#if 0
int
AosFunCustom::startCompare(
		const int cmp_len)
{
	int rslt = 0;
	switch(cmp_type)
	{
	case eStr:
			if (cmp_len == -1)
		 		rslt = strcmp(lhs+cmp_pos, rhs+cmp_pos);
			else
				rslt = strncmp(lhs+cmp_pos, rhs+cmp_pos, cmp_len);
		 break;

	case eNumStr:
		 {
			if (cmp_len == -1)
			{
				 int lLen = strlen(lhs+cmp_pos);
				 int rLen = strlen(rhs+cmp_pos);
				 if(lLen < rLen)
				 {
					 rslt = -1;
					 return rslt;
				 }
				 else if(lLen > rLen)
				 {
					 rslt = 1;
					 return rslt;
				 }
				 rslt = strcmp(lhs+cmp_pos, rhs+cmp_pos);
			}
			else
			{
				rslt = strncmp(lhs+cmp_pos, rhs+cmp_pos, cmp_len);
			}
		 }
		 break;

	case eBuffStr:
			if (cmp_len == -1)
			{
				u32 llen = *(u32*)(lhs-sizeof(u32)+cmp_pos);
				u32 rlen = *(u32*)(rhs-sizeof(u32)+cmp_pos);

				int len = llen > rlen ? rlen : llen;
				rslt = strncmp(lhs, rhs, len);
				if (rslt == 0 && llen != rlen)
				{
					if (llen > rlen) return 1;
					else return -1;
				}
			}
			else
				rslt = strncmp(lhs+cmp_pos, rhs+cmp_pos, cmp_len);
		 break;

	case eU64:
		 {
			 u64 l = *(u64*)(lhs+cmp_pos);
			 u64 r = *(u64*)(rhs+cmp_pos);
			 if (l==r) rslt = 0;
			 else if(l<r) rslt = -1;
			 else rslt = 1;
		 }
		 break;

	case eU32:
		 {
			 u32 l = *(u32*)(lhs+cmp_pos);
			 u32 r = *(u32*)(rhs+cmp_pos);
			 if (l==r) rslt = 0;
			 else if(l<r) rslt = -1;
			 else rslt = 1;
		 }
		 break;

	case eI64:
		 {
			 int64_t l = *(int64_t*)(lhs+cmp_pos);
			 int64_t r = *(int64_t*)(rhs+cmp_pos);
			 if (l==r) rslt = 0;
			 else if(l<r) rslt = -1;
			 else rslt = 1;
		 }
		 break;

	case eI32:
		 {
			 int l = *(int*)(lhs+cmp_pos);
			 int r = *(int*)(rhs+cmp_pos);
			 if (l==r) rslt = 0;
			 else if(l<r) rslt = -1;
			 else rslt = 1;
		 }
		 break;

	case eNumber: // Add by Young, 2014/12/19
	case eDouble:
		 {
			 double l = *(double*)(lhs+cmp_pos);
			 double r = *(double*)(rhs+cmp_pos);
			 if (l==r) rslt = 0;
			 else if(l<r) rslt = -1;
			 else rslt = 1;
		 }
		 break;

	default:

		OmnAlarm << "Unrecognized data type: " << cmp_type<< enderr;
		 break;
	}
	return rslt;
}

int
AosFunCustom::compare(
		const char* lhs,
		const char* rhs,
		const int cmp_pos,
		const DataType &cmp_type,
		const int cmp_len,
		DataType &field_type,
		const bool isReserve)
{
	int rslt = 0;
	switch(cmp_type)
	{
	case eStr:
//	case eNumStr:
	case eU64:
	case eU32:
	case eI64:
	case eI32:
	case eNumber: 
	case eDouble:
		 rslt = startCompare(lhs, rhs, cmp_pos, cmp_type, cmp_len);
		 break;
	case eRecord:
		 {
			 const char* lrecord = (const char*)(*(i64*)(lhs+sizeof(int))) + (*(int*)lhs);
			 const char* rrecord = (const char*)(*(i64*)(rhs+sizeof(int))) + (*(int*)rhs);
			 i64 field_offset = sizeof(int) + sizeof(i64) + sizeof(u16)*cmp_pos;
			 int loffset = *((u16*)(lhs+field_offset));
			 int roffset = *((u16*)(rhs+field_offset));
		 	 rslt = startCompare(lrecord+loffset+sizeof(u32), rrecord+roffset+sizeof(u32), 0, field_type, cmp_len);
		 }
		 break;

	default:

		OmnAlarm << "Unrecognized data type: " << cmp_type << enderr;
		 break;
	}
	if (isReserve) return rslt * (-1);
	return rslt;
}
#endif


int 
AosFunCustom::cmpPriv(const char* lhs, const char* rhs)
{
	int rslt = 0;
	if (mRecordType != AosDataRecordType::eBuff)
	{
		for(u32 i=0; i < mFieldSize; i++)
		{
			rslt = (*mCmpFuncs[mFields[i].mFieldType])(lhs, rhs, mFields[i].mCmpPos);
			if (rslt != 0) break;
		}
	}
	else
	{
		i64 field_offset;
		int loffset, roffset;
		i8 l_null, r_null;
		const char* lrecord = (const char*)(*(i64*)(lhs+sizeof(int))) + (*(int*)lhs);
		const char* rrecord = (const char*)(*(i64*)(rhs+sizeof(int))) + (*(int*)rhs);

		for(u32 i=0; i < mFieldSize; i++)
		{
			field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * mFields[i].mCmpPos;
			loffset = *((u16*)(lhs+field_offset))+sizeof(u32);
			roffset = *((u16*)(rhs+field_offset))+sizeof(u32);
			if (mFields[i].mFieldType == AosCompareFun::eBuffStr)
			{
				l_null = *((i8*)(lrecord + loffset - 5));
				r_null = *((i8*)(rrecord + roffset - 5));
			}
			else
			{
				l_null = *((i8*)(lrecord + loffset - 1));
				r_null = *((i8*)(rrecord + roffset - 1));
			}

			if (l_null == 1)
			{
				if (r_null == 1)
					rslt = 0;
				else
					rslt = -1;
			}
			else
			{
				if (r_null == 0) {
					rslt = (*mCmpFuncs[mFields[i].mFieldType])(lrecord+loffset, rrecord+roffset, 0);
				}
				else 
					rslt = 1;
			}


			if (rslt != 0) break;
		}
	}
	if (rslt < 0) rslt = -1;
	if (mReverse) return rslt * (-1);
	return rslt;
}


bool
AosCompareFun::mergeData(char *v1, char *v2, char *data, int &len)
{
	if (strncmp("manuf_name", v1,10) ==0 )
	{
		aos_assert_r(strncmp("manuf_name", v2,10) == 0, false);
	}
	bool rslt;
	for (u32 i = 0 ; i < mAosAgrs.size(); i++)
	{
		aos_assert_r(size > (i64)mAosAgrs[i].mAgrPos, false);
		switch(mAosAgrs[i].mAgrType)
		{
			case eU64:
				rslt = agrU64(v1, v2, mAosAgrs[i].mAgrFun, mAosAgrs[i].mAgrPos);
				aos_assert_r(rslt, false);
				break;
			case eI64:
				rslt = agrInt64(v1, v2, mAosAgrs[i].mAgrFun, mAosAgrs[i].mAgrPos);
				break;
			case eNumber:	// Add by Young, 2014/12/19
			case eDouble:
				rslt = agrDouble(v1, v2, mAosAgrs[i].mAgrFun, mAosAgrs[i].mAgrPos);
				break;
			case eStr:
			{
				if (AosDataColOpr::toStr(mAosAgrs[i].mAgrFun) == "set")
				{
					return true;
				}
				OmnAlarm << "DataType Invalid: " << mAosAgrs[i].mAgrType << enderr;
				break;
			}
			case eRecord:
			{
				char* v1_rcd = (char*)(*(i64*)(v1+sizeof(int))) + (*(int*)v1);
				char* v2_rcd = (char*)(*(i64*)(v2+sizeof(int))) + (*(int*)v2);

				i64 field_offset = sizeof(int) + sizeof(i64) + sizeof(u16)*mAosAgrs[i].mAgrPos;
				int v1_offset = *((u16*)(v1+field_offset));
				int v2_offset = *((u16*)(v2+field_offset));

				char* v1_r = v1_rcd+v1_offset+sizeof(u32);
				char* v2_r = v2_rcd+v2_offset+sizeof(u32);
				switch(mAosAgrs[i].mAgrFieldType)
				{
				case eU64:
					rslt = agrU64(v1_r, v2_r, mAosAgrs[i].mAgrFun, 0);
					aos_assert_r(rslt, false);
					break;
				case eI64:
					rslt = agrInt64(v1_r, v2_r, mAosAgrs[i].mAgrFun, 0);
					aos_assert_r(rslt, false);
					break;
				case eNumber:
				case eDouble:
					rslt = agrDouble(v1_r, v2_r, mAosAgrs[i].mAgrFun, 0);
					aos_assert_r(rslt, false);
					break;
				case eBuffStr:
				case eStr:
				{
					if (AosDataColOpr::toStr(mAosAgrs[i].mAgrFun) == "set")
					{
						return true;
					}
					OmnAlarm << "DataType Invalid: " << mAosAgrs[i].mAgrFieldType << enderr;
					break;
				}
				case eAgrStr:
					rslt = agrStr(v1_rcd, v2_rcd, v1_offset+sizeof(u32), v2_offset+sizeof(u32), data, len);
					aos_assert_r(rslt, false);
					break;
				default:
					OmnAlarm << "DataType Invalid: " << mAosAgrs[i].mAgrFieldType << enderr;
					break;
				}
				break;
			}
			default:
				OmnAlarm << "DataType Invalid: " << mAosAgrs[i].mAgrType << enderr;
				break;
		}
	}
	return true;
}


bool
AosCompareFun::agrU64(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos)
{
	switch (agr_fun)
	{
		case AosDataColOpr::eIndex:
			break;
		case AosDataColOpr::eNormal:
			*(u64 *)&v1[agr_pos] += *(u64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eNoUpdate:
			break;
		case AosDataColOpr::eSetValue:
			*(u64 *)&v1[agr_pos] = *(u64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMaxValue:
			if (*(u64 *)&v1[agr_pos] < *(u64 *)&v2[agr_pos])
				*(u64 *)&v1[agr_pos] = *(u64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMinValue:
			if (*(u64 *)&v1[agr_pos] > *(u64 *)&v2[agr_pos])
				*(u64 *)&v1[agr_pos] = *(u64 *)&v2[agr_pos];
			break;
		default:
			OmnAlarm << "AosDataColOpr Invalid: " << agr_fun << enderr;
			break;
	}
	return true;
}


bool
AosCompareFun::agrInt64(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos)
{
	switch (agr_fun)
	{
		case AosDataColOpr::eIndex:
			break;
		case AosDataColOpr::eNormal:
			*(i64 *)&v1[agr_pos] += *(i64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eNoUpdate:
			break;
		case AosDataColOpr::eSetValue:
			*(i64 *)&v1[agr_pos] = *(i64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMaxValue:
			if (*(i64 *)&v1[agr_pos] < *(i64 *)&v2[agr_pos])
				*(i64 *)&v1[agr_pos] = *(i64 *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMinValue:
			if (*(i64 *)&v1[agr_pos] > *(i64 *)&v2[agr_pos])
				*(i64 *)&v1[agr_pos] = *(i64 *)&v2[agr_pos];
			break;
		default:
			OmnAlarm << "AosDataColOpr Invalid: " << agr_fun << enderr;
			break;
	}
	return true;
}


bool
AosCompareFun::agrDouble(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos)
{
	switch (agr_fun)
	{
		case AosDataColOpr::eIndex:
			break;
		case AosDataColOpr::eNormal:
			*(double *)&v1[agr_pos] += *(double *)&v2[agr_pos];
			break;
		case AosDataColOpr::eNoUpdate:
			break;
		case AosDataColOpr::eSetValue:
			*(double *)&v1[agr_pos] = *(double *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMaxValue:
			if (*(double *)&v1[agr_pos] < *(double *)&v2[agr_pos])
				*(double *)&v1[agr_pos] = *(double *)&v2[agr_pos];
			break;
		case AosDataColOpr::eMinValue:
			if (*(double *)&v1[agr_pos] > *(double *)&v2[agr_pos])
				*(double *)&v1[agr_pos] = *(double *)&v2[agr_pos];
			break;
		default:
			OmnAlarm << "AosDataColOpr Invalid: " << agr_fun << enderr;
			break;
	}
	return true;
}


bool
AosCompareFun::agrStr(char *r1, 
				char *r2, 
				const int f1_offset, 
				const int f2_offset, 
				char *data, 
				int &len)
{
	int offset = f1_offset-sizeof(u32)-sizeof(i8);
	memcpy(data, r1, offset);
	data[offset] = 0; 
	offset += sizeof(i8);

	u32 f1_len = *((u32*)(r1+f1_offset-sizeof(u32)));
	u32 f2_len = *((u32*)(r2+f2_offset-sizeof(u32)));

	*(u32*)(data+offset) = f1_len+f2_len+sizeof(char);
	offset += sizeof(u32);
	if (f1_len != 0)
	{
		memcpy(data+offset, r1+f1_offset, f1_len);
		offset += f1_len;
	}
	data[offset] = char(0x01);
	offset += sizeof(char);

	if (f2_len !=0 )
	{
		memcpy(data+offset, r2+f2_offset, f2_len);
		offset += f2_len;
	}

	int decode_len = *(int*)r1;
	bool rslt = AosBuff::decodeRecordBuffLength(decode_len);
	aos_assert_r(rslt, false);

	int remaining_len = decode_len + sizeof(u32) - f1_offset - f1_len;
	memcpy(data+offset, r1+f1_offset+f1_len, remaining_len);
	len = offset + remaining_len - sizeof(u32);
	int encode_len = len;
	rslt = AosBuff::encodeRecordBuffLength(encode_len);
	aos_assert_r(rslt, false);
	*(u32*)data = encode_len;
	return true;
}


int 
AosFunVarStr::cmpPriv(const char* lhs, const char* rhs)
{
	int pos1 = *(int*)lhs;
	int pos2 = *(int*)rhs;
	char *body1 = (char*)(*(i64*)(lhs+sizeof(int)));
	char *body2 = (char*)(*(i64*)(rhs+sizeof(int)));
	int len1 = *(int*)&body1[pos1];
	int len2 = *(int*)&body2[pos2];
	char *str1 = &body1[pos1 + sizeof(int)];
	char *str2 = &body2[pos2 + sizeof(int)];

	return cmpPriv(str1, len1, str2, len2);
}


int 
AosFunVarStr::cmpPriv(
		const char* lhs, const int llen,
		const char* rhs, const int rlen)
{
	if (mNumAlphabetic)
	{
		if (llen < rlen)
		{
			return -1;
		}
		else if (llen > rlen)
		{
			return 1;
		}
		else
		{
			return strncmp(lhs, rhs, llen);
		}
	}

	int len = llen > rlen ? rlen : llen;
	int rsltInt = strncmp(lhs, rhs, len);
	if (rsltInt == 0 && llen != rlen)
	{
		if (llen > rlen) return 1;
		else return -1;
	}
	return rsltInt;
}

