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
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/QueryRslt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Base64/Base64.h"
#include "QueryCond/QueryCond.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "XmlUtil/SeXmlParser.h"


AosQueryRslt::AosQueryRslt()
:
mCrtIdx(0),
mNumDocs(0),
mMemCap(eInitMemSize),
mDataReverse(false),
mWithValues(true),					// Chen Ding, 2014/02/15
mOperator(AosDataColOpr::eIndex),
mRcdLen(eDefaultRcdLen),
mValueType("str")
{
	mDocids = OmnNew u64[eInitMemSize];
}

/*AosQueryRslt::AosQueryRslt(const i64 &memsize)
:
mCrtIdx(0),
mNumDocs(0),
mDataReverse(false),
mWithValues(false)
{
	if(memsize > 0)
	{
		mMemCap = memsize;
		mDocids = OmnNew u64[memsize];
	}else
	{
		mMemCap = eInitMemSize;
		mDocids = OmnNew u64[eInitMemSize];
	}
}
*/

AosQueryRslt::~AosQueryRslt()
{
	if(mDocids)
	{
		delete [] mDocids;
		mDocids = 0;
	}
}


bool 
AosQueryRslt::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (!mDocids) return false;
	for (i64 i=0; i<mNumDocs; i++)
	{
		if (mDocids[i] == docid) return true;
	}
	return false;
}


AosQueryRsltObjPtr
AosQueryRslt::getQueryRslt()
{
	return OmnNew AosQueryRslt();
}


void
AosQueryRslt::returnQueryRslt(const AosQueryRsltObjPtr &rslt)
{
}


void
AosQueryRslt::setNoData()
{
	mNumDocs = 0;
}

u64
AosQueryRslt::removeOldData()
{
	if(!mCrtIdx)return 0;
	u64 num = mCrtIdx;
	bool rslt = removeOldData(num);
	aos_assert_r(rslt ,0);
	mCrtIdx = 0;
	return num;
}

bool
AosQueryRslt::removeOldData(const i64 &pos)
{
	aos_assert_r(pos >= 0 && mNumDocs>=pos, false);

	if(pos == 0)return true;

	if(pos == mNumDocs)
	{
		if(mValues)
		{
			mValues->clear();
		}
		mNumDocs = 0;
		mCrtIdx = 0;
		return true;
	}
	
	if(mValues)
	{
		mValues->removeNumEntries(pos);
	}

	mNumDocs -= pos;

	memmove(mDocids,&(mDocids[pos]),mNumDocs*sizeof(u64));
	
	// if it is reverse, no need to remove docs, just set the size.

	mCrtIdx = 0;

	return true;
}

void
AosQueryRslt::clear()
{
	mCrtIdx = 0;
	mNumDocs = 0;
	mDataReverse = false;

//	mWithValues = false;			// Chen Ding, 08/12/2011
	mValues = 0;

	if(mMemCap != eInitMemSize)
	{
		if(mDocids)
		{
			delete []mDocids;
		}
		mDocids = OmnNew u64[eInitMemSize];
		mMemCap = eInitMemSize;
	}
}

bool 			
AosQueryRslt::appendDocid(const u64 docid)
{
	if(mNumDocs >= mMemCap)
	{
		expandMem();
	}

	mDocids[mNumDocs++] = docid;
	return true;
}


bool 	
AosQueryRslt::exchangeContent(
		const AosQueryRsltObjPtr &obj1,
		const AosQueryRsltObjPtr &obj2)
{
	// Chen Ding, 11/22/2012
	AosQueryRslt *rslt1 = (AosQueryRslt*)obj1.getPtr();
	AosQueryRslt *rslt2 = (AosQueryRslt*)obj2.getPtr();

	u64* tmpdids = rslt1->mDocids;
	rslt1->mDocids = rslt2->mDocids;
	rslt2->mDocids = tmpdids;
	
	
	i64 tmpnum = rslt1->mNumDocs;
	rslt1->mNumDocs = rslt2->mNumDocs;
	rslt2->mNumDocs = tmpnum;
	
	i64 tmpcap = rslt1->mMemCap;
	rslt1->mMemCap = rslt2->mMemCap;
	rslt2->mMemCap = tmpcap;
	return true;
}


bool
AosQueryRslt::hasMoreDocid()
{
	return mCrtIdx < mNumDocs;
	return true;
}


bool 			
AosQueryRslt::mergeFrom(const AosQueryRsltObjPtr &rhs_rslt)
{
	aos_assert_r(rhs_rslt,false);
	return mergeFrom(rhs_rslt,0,rhs_rslt->getNumDocs());
}


bool 			
AosQueryRslt::mergeFrom(
		const AosQueryRsltObjPtr &rhs_rslt,
		const u64 &startidx, 
		const u64 &len)
{
	// This function merges 'len' number of entries from 'rhs_rslt', 
	// from the start position 'startidx'.
	if(!rhs_rslt || rhs_rslt->getNumDocs() == 0)
	{
		return true;
	}
	if(mValues)
	{
		aos_assert_r(rhs_rslt->isWithValues(),false);
	}
	if(mValues)mValues->setNumEntries(mNumDocs);
	setDataReverse(rhs_rslt->isDataReverse());
	u64 rhs_total = rhs_rslt->getNumDocs();
	if(rhs_total == 0) return true;
	AosBuffArrayPtr rhs_values = rhs_rslt->getValues();

	i64 num_entries = rhs_values->getNumEntries();
	aos_assert_r(num_entries >= 0 && (u64)num_entries == rhs_total, false);

	u64 rhs_move = len;
	if(len == 0)rhs_move = rhs_total - startidx;
	if(rhs_move > rhs_total - startidx)
	{
		rhs_move = rhs_total - startidx;
	}
	expandMem(mNumDocs + rhs_move);

	if(rhs_move > 0)
	{
		memcpy(&(mDocids[mNumDocs]), &(rhs_rslt->getDocidsPtr()[startidx]), sizeof(u64) * rhs_move);
	}
	
	// Chen Ding, 2014/02/06
	// mNumDocs += rhs_total;
	mNumDocs += rhs_move;

	if(!mValues)
	{
		aos_assert_r(mRcdLen > 0,false);
		AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
		comp_func->setReverse(mDataReverse);
		mValues = OmnNew AosBuffArray(
		        comp_func, false, false, eDefaultRcdSize*mRcdLen);
		mValues->setAutoExtend(true,10);
	}
	// AosBuffArrayPtr rhs_values = rhs_rslt->getValues();
	aos_assert_r(rhs_values, false);
	mValues->mergeFrom(rhs_values,startidx,len);

//	rhs_rslt->setNumDocs(0);

	return true;
}

	
u64  			
AosQueryRslt::nextDocid(bool &finished)
{
	// Chen Ding, 03/27/2012
	finished = false;
	if(mCrtIdx < 0 || mCrtIdx >= mNumDocs)
	{
		finished = true;
		return 0;
	}
	
	return mDocids[mCrtIdx++];
}


i64
AosQueryRslt::getNumLeft()
{
	return mNumDocs - mCrtIdx;
}


bool 			
AosQueryRslt::moveTo(const i64 &pos, bool &finished, const AosRundataPtr &rdata)
{
	finished = false;
	
	if(pos >= mNumDocs)
	{
		finished = true;
		mCrtIdx = mNumDocs - 1;
		return true;
	}
	
	mCrtIdx = pos;

	return true;
}


bool 			
AosQueryRslt::filterByBitmap(const AosBitmapObjPtr &bitmap)
{
	aos_assert_r(bitmap,false);

	i64 crt_idx = 0;
	for (i64 i = 0;i < mNumDocs;i++)
	{
		if(bitmap->checkDoc(mDocids[i]))
		{
			mDocids[crt_idx] = mDocids[i];
			crt_idx ++;			
		}		
	}
	mNumDocs = crt_idx;
	return true;
}


bool 			
AosQueryRslt::backOffStep(const i64 &step)
{
    aos_assert_r(mCrtIdx - step >= 0, false);
    mCrtIdx -= step ;
	return true;
}


OmnString	
AosQueryRslt::getLastValue()const
{
	return mValues->getEntry(mNumDocs-1);
}


bool 			
AosQueryRslt::nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata)
{
	// Chen Ding, 03/27/2012
	finished = false;

	if(mCrtIdx < 0 || mCrtIdx >= mNumDocs)
	{
		docid = 0;
		finished = true;
		return true;
	}
	finished = false;

	docid = mDocids[mCrtIdx++];
	return true;
}


bool 			
AosQueryRslt::nextDocidValue(u64 &docid, OmnString &value, const AosRundataPtr &rdata)
{
	bool finished = false;
	aos_assert_r(mValues, false);
	i64 idx = mCrtIdx;
	docid = nextDocid(finished);
	value = "";
	if (finished) return true;
	//value.assign(mValues->getRecord(idx),mValues->getRecordLen());
	value.assign(mValues->getEntry(idx), strlen(mValues->getEntry(idx)));
	return true;
}


bool 			
AosQueryRslt::nextDocidValue(
		u64 &docid, 
		OmnString &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// Chen Ding, 2014/02/06
//	aos_assert_rr(mCrtIdx >= 0, rdata, false);

	aos_assert_r(mValues, false);
	i64 idx = mCrtIdx;
	docid = nextDocid(finished);
	if(finished) return true;
	//value.assign(mValues->getRecord(idx),mValues->getRecordLen());
	value.assign(mValues->getEntry(idx), strlen(mValues->getEntry(idx)));
	return true;
}


bool 			
AosQueryRslt::nextDocidValue(
		u64 &docid, 
		u64 &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mValues, false);
	i64 idx = mCrtIdx;
	docid = nextDocid(finished);
	value = 0;
	if (finished) return true;
	value = *((u64*)(mValues->getEntry(idx)));
	return true;
}

bool
AosQueryRslt::nextDocidValue(
		u64 &docid, 
		d64 &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mValues, false);
	i64 idx = mCrtIdx;
	docid = nextDocid(finished);
	value = 0;
	if (finished) return true;
	value = *((d64*)(mValues->getEntry(idx)));
	return true;
}

bool
AosQueryRslt::nextDocidValue(
		u64 &docid, 
		i64 &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mValues, false);
	i64 idx = mCrtIdx;
	docid = nextDocid(finished);
	value = 0;
	if (finished) return true;
	value = *((i64*)(mValues->getEntry(idx)));
	return true;
}

bool 			
AosQueryRslt::nextU64DocidValue(u64 &docid, 
								u64 &value, 
								bool &finished, 
								const AosRundataPtr &rdata)
{
	return nextDocidValue(docid,value,finished, rdata);
}


/*bool 			
AosQueryRslt::nextValue(OmnString &value,const AosRundataPtr &rdata)
{
	aos_assert_r(mValueBuff,false);
	value = mValueBuff->getOmnStr("");
	return true;
}

bool 			
AosQueryRslt::nextU64Value(u64 &value,const AosRundataPtr &rdata)
{
	aos_assert_r(mValueBuff,false);
	value = mValueBuff->getU64(0);
	return true;
}

*/

bool 			
AosQueryRslt::appendBlock(
		const u64* docs, 
		const i64 &num_docs)
{
	aos_assert_r(docs && num_docs >= 0,false);
	if(num_docs == 0)
	{
		return true;
	}
	expandMemPriv(mNumDocs+num_docs);
	memcpy(&(mDocids[mNumDocs]),docs,num_docs*sizeof(u64));
	mNumDocs += num_docs;
	return true;
}


bool 			
AosQueryRslt::appendBlockRev(
		const u64* docs, 
		const i64 &num_docs)
{
	aos_assert_r(docs && num_docs >= 0,false);
	if(num_docs == 0)
	{
		return true;
	}
	expandMemPriv(mNumDocs+num_docs);
	
	i64 cur1 = num_docs-1;
	i64 cur2 = mNumDocs;
	for(;cur1 >= 0;cur1--)
	{
		mDocids[cur2++] = docs[cur1];
	}
//	memcpy(&(mDocids[mNumDocs]),docs,num_docs*sizeof(u64));
	mNumDocs += num_docs;
	return true;
}

//bool 			
//AosQueryRslt::appendBlock(
//		const u64* docs, 
//		const i64 &num_docs, 
//		const i64 &iilidx)
//{
//}



bool			
AosQueryRslt::serializeToXml(AosXmlTagPtr &xml)
{
	//the formate of xml is
	//<query_rslt isWithValues="true|false" isCompression="true|false">
	//	<docids><![CDATA[...]]></docids>
	//	<values><![CDATA[...]]></values>
	//</query_rslt>

	OmnString str;
	str << "<query_rslt ";
	if(mDataReverse) str << "dataReverse=\"" << mDataReverse << "\" ";
	if(mWithValues) str << "isWithValues=\"" << mWithValues << "\" ";
	if(mRcdLen) str << "rcdLen=\"" << mRcdLen << "\" ";
	if(mNumDocs) str << "numDocs=\"" << mNumDocs << "\" "; 
	str << "operator=\"" << AosDataColOpr::toStr(mOperator) << "\" ";
	str << " >";

	if(mNumDocs)
	{
		str << "<docids><![BDATA[";
		str << mNumDocs * sizeof(u64) << ":";
		str.append((char*)mDocids, mNumDocs * sizeof(u64));
		/*
		int len1 = str.length();
		int len2 = mNumDocs * sizeof(u64);
		int len3 = len1 + len2*4/3 + 100;
		str.setSize(len3);
		int len4 = EncodeBase64((u8*)mDocids, (u8*)str.getBuffer()+len1, len2,99999999);
		str.setLength(len1 + len4);
		//for(i64 i=0; i<mNumDocs; i++)
		//{
		//	str << mDocids[i];
		//	if(i != mNumDocs-1) str << ",";
		//}
		*/
		str << "]]></docids>";
	}	
	if(mWithValues && mNumDocs)
	{
		AosXmlTagPtr values_xml;
		
		
		mValues->serializeToXml(values_xml);
		str << values_xml->toString();
//		str << "<values><![BDATA[" << mValueBuff->dataLen() << ":";
//		str.append(mValueBuff->data(), mValueBuff->dataLen());
//		str << "]]></values>";
		/*
		OmnString v(mValueBuff->data(), mValueBuff->dataLen());
		OmnString md5 = AosMD5Encrypt(v);
		str << "<values MD5=\"" << md5 << "\"><![CDATA[" << v << "]]></values>";
		*/
	}
	str	<< "</query_rslt>";
		
	AosXmlParser parser;
	xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
	
}


bool			
AosQueryRslt::serializeFromXml(const AosXmlTagPtr &xml)
{
	bool rslt = false;
	clear();

	mCrtIdx = xml->getAttrInt("crtIdx", 0);

	mDataReverse = xml->getAttrBool("dataReverse", false);
	mNumDocs = xml->getAttrInt("numDocs", 0);
	mOperator = AosDataColOpr::toEnum(xml->getAttrStr("operator"));
	
	if(mNumDocs)
	{
	 	expandMemPriv(mNumDocs+10);	
		AosBuffPtr buff = xml->getNodeTextBinaryUnCopy("docids" AosMemoryCheckerArgs);
		aos_assert_r(buff, true);
		memcpy(mDocids, buff->data(), buff->dataLen());

		/*
	 	OmnString docidStr = xml->getNodeText("docids");
		int strlen = docidStr.length();
		if(strlen < (int)(mNumDocs*sizeof(u64)*4/3) + 50)
		{
			int datalen = DecodeBase64((u8*)docidStr.getBuffer(), (u8*)mDocids, strlen);
			if(datalen != (mNumDocs*sizeof(u64)))
			{
				OmnAlarm << "data error" << enderr;
			}
		}
		*/

		// OmnStrParser1 parser(docidStr, ",", false, false);
		// OmnString val;
		// bool rslt;
		// while ((val = parser.nextWord()) != "")
		//  {
		//	rslt = appendDocid(atoll(val.data()));
		//	aos_assert_r(rslt, false);
		//}
	}

	mWithValues = xml->getAttrBool("isWithValues", false);
	mRcdLen = xml->getAttrInt("rcdLen",0);
	if (mWithValues)
	{
		aos_assert_r(mRcdLen > 0,false);
		AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
		comp_func->setReverse(mDataReverse);
		mValues = OmnNew AosBuffArray(
		        comp_func, false, false, eDefaultRcdSize*mRcdLen);
		mValues->setAutoExtend(true,10);
		if(mNumDocs > 0)
		{
			AosXmlTagPtr values_xml = xml->getFirstChild("buff_array");
			rslt = mValues->serializeFromXml(values_xml);
			aos_assert_r(rslt,false);		
		}
/*			AosBuffPtr buff = xml->getNodeTextBinaryUnCopy("values" AosMemoryCheckerArgs);
			//mValueBuff = OmnNew AosBuff(buff->dataLen()+10, eBuffIncSize AosMemoryCheckerArgs);
			//felicia, 2012/09/26
			mValueBuff = OmnNew AosBuff(buff->dataLen()+10 AosMemoryCheckerArgs);
			aos_assert_r(mValueBuff, false);
			memcpy(mValueBuff->data(), buff->data(), buff->dataLen());
			mValueBuff->setDataLen(buff->dataLen());
*/
			/*
			OmnString valueStr = xml->getNodeText("values");
			OmnString md5_1 = AosMD5Encrypt(valueStr);
	
			AosXmlTagPtr values = xml->getFirstChild("values");
			aos_assert_r(values, false);
			OmnString md5_2 = values->getAttrStr("MD5", "");
			if(md5_1 != md5_2)
			{
				OmnAlarm << "md5 error" << enderr;
				return false;
			}
			mValueBuff = OmnNew AosBuff(valueStr.length(), eBuffIncSize AosMemoryCheckerArgs);
		yy	memcpy(mValueBuff->data(), valueStr.data(), valueStr.length());
//OmnScreen << "md5_1:" << md5_1 << ",crtIdx:" << mValueBuff->getCrtIdx() << endl;
//OmnScreen << "firstValueLen:" << *(int *)mValueBuff->data() << ",first value:" << mValueBuff->getOmnStr("") << endl;
		}
			*/
		else
		{
			//mValueBuff = OmnNew AosBuff(eBuffInitSize, eBuffIncSize AosMemoryCheckerArgs);
			//felicia, 2012/09/26
//			mValueBuff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
			AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
			comp_func->setReverse(mDataReverse);
			mValues = OmnNew AosBuffArray(
			        comp_func, false, false, eDefaultRcdSize*mRcdLen);
			mValues->setAutoExtend(true,10);
		}
	}
	reset();
	return true;	
}

bool 
AosQueryRslt::serializeFromBuff(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosQueryRslt::serializeToBuff(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 11/28/2012
bool
AosQueryRslt::initQueryRslt()
{
	AosQueryRsltObjPtr rr = OmnNew AosQueryRslt();
	AosQueryRsltObj::setQueryRslt(rr);
	return true;
}


//Ice
bool 			
AosQueryRslt::merge(const AosQueryRsltObjPtr &rhs_rslt)
{
	
	aos_assert_r(rhs_rslt && isDataReverse() == rhs_rslt->isDataReverse(), false);

	AosBuffArrayPtr lhs_values = getValues();
	AosBuffArrayPtr rhs_values = rhs_rslt->getValues();

	int rcd_len = lhs_values->getRecordLen();
	aos_assert_r(rcd_len == rhs_values->getRecordLen(), false);

	aos_assert_r((lhs_values && rhs_values) || (!lhs_values && !rhs_values), false);

	if(!lhs_values && !rhs_values)
	{
		mergeFrom(rhs_rslt);
		return true;
	}

	i64 lhs_total = mNumDocs;
	i64 rhs_total = rhs_rslt->getNumDocs();

	if (rhs_total <= 0) return true;

//	u64 lhs_values_len = lhs_value_buff->dataLen();
//	u64 rhs_values_len = rhs_value_buff->dataLen();

	u64 * lhs_docids = getDocidsPtr();
	u64 * rhs_docids = rhs_rslt->getDocidsPtr();

	i64 total_rcds = lhs_total + rhs_total;

	u64 * new_block = OmnNew u64[total_rcds];
	AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
	comp_func->setReverse(mDataReverse);
	AosBuffArrayPtr new_buff = OmnNew AosBuffArray(
	        comp_func, false, false, eDefaultRcdSize*mRcdLen);
	new_buff->setAutoExtend(true,10);
	if (lhs_total <= 0)
	{
		new_buff->mergeFrom(rhs_values,0);

		mValues = new_buff;
		memcpy(new_block, rhs_docids, rhs_total * sizeof(u64));

		if(mDocids)
		{
			delete [] mDocids;
		}
		mDocids = new_block;
		mNumDocs = total_rcds; 
		return true;
	}

	char* lhs_value;
	char* rhs_value;
	i64 lhs_crt = 0;
	i64 rhs_crt = 0;

	// Now it is ready to merge the two buffs. Both are not empty.
	i64 num_rcds = 0;
	int addtype = 2;
	while (num_rcds < total_rcds)
	{
		// Check whether lhs has run out. If yes, copy the remaining of rhs.
		if (lhs_total == lhs_crt && rhs_total == rhs_crt) 
		{
			break;
		}

		if (lhs_total == lhs_crt)
		{
			if (addtype == 1 || addtype == 2)
			{
				rhs_value = rhs_values->getEntry(rhs_crt);
			}

			new_buff->mergeFrom(rhs_values,rhs_crt);
			mValues = new_buff;
			mValues->setReverse(mDataReverse);

			memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, (rhs_total - rhs_crt) * sizeof(u64));
			if(mDocids)
			{
				delete []mDocids;
			}
			mDocids = new_block;
			mNumDocs = total_rcds; 
			return true;
		}

		// Check whether rhs has run out. If yes, copy the remaining of lhs.
		if (rhs_total == rhs_crt)
		{
			if (addtype == 1 || addtype == 2)
			{
				lhs_value = lhs_values->getEntry(lhs_crt);
			}

			new_buff->mergeFrom(lhs_values,lhs_crt);
			mValues = new_buff;
			mValues->setReverse(mDataReverse);

			memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, (lhs_total - lhs_crt) * sizeof(u64));
			if(mDocids)
			{
				delete []mDocids;
			}
			mDocids = new_block;
			mNumDocs = total_rcds; 
			return true;
		}

		if(addtype == 0)
		{
			lhs_value = lhs_values->getEntry(lhs_crt);
		}
		else if(addtype == 1)
		{
			rhs_value = rhs_values->getEntry(rhs_crt);
		}
		else
		{
			lhs_value = lhs_values->getEntry(lhs_crt);
			rhs_value = rhs_values->getEntry(rhs_crt);
		}


		int rr = strncmp(lhs_value, rhs_value, rcd_len);
		if(!mDataReverse)
		{
			if (rr < 0)
			{
				// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
				lhs_crt++;
				aos_assert_r(lhs_crt <= lhs_total, false);
				num_rcds++;
				addtype = 0;
				continue;
			}
			else if (rr > 0)
			{
				// lhs_crt > rhs_crt. Need to put rhs_crt to new_buff and 
				// advance rhs_crt.
				new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
				rhs_crt ++;
				aos_assert_r(rhs_crt <= rhs_total, false);
				num_rcds++;
				addtype = 1;
				continue;
			}
		}
		else
		{
			if (rr < 0)
			{
				// lhs_crt > rhs_crt. Need to put rhs_crt to new_buff and 
				// advance rhs_crt.
				new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
				rhs_crt ++;
				aos_assert_r(rhs_crt <= rhs_total, 0);
				num_rcds++;
				addtype = 1;
				continue;
			}
			else if (rr > 0)
			{
				// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
				lhs_crt++;
				aos_assert_r(lhs_crt <= lhs_total, 0);
				num_rcds++;
				addtype = 0;
				continue;
			}
		}
		
		// felicia,2013/03/10
		// lhs_value == rhs_value
		// but docid is not equal
		// must sort the docid
		u64 rhs_docid;
		u64 lhs_docid;
		switch (mOperator)
		{
		case AosDataColOpr::eIndex:
			 if (*(lhs_docids + lhs_crt) < *(rhs_docids + rhs_crt))
			 {
				new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
					lhs_crt++;
					aos_assert_r(lhs_crt <= lhs_total, false);
					num_rcds++;
					addtype = 0;
					break;
			 }
			 else if(*(lhs_docids + lhs_crt) > *(rhs_docids + rhs_crt))
			 {
				new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
				rhs_crt++;
				aos_assert_r(rhs_crt <= rhs_total, false);
				num_rcds++;
				addtype = 1;
				break;
			 }
			 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
			 lhs_crt++;
			 aos_assert_r(lhs_crt <= lhs_total, false);
			 num_rcds++;

			 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
			 memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
			 rhs_crt++;
			 aos_assert_r(rhs_crt <= rhs_total, false);
			 num_rcds++;
			 addtype = 2;
			 break;

		case AosDataColOpr::eNormal:
			 // To be done
			 // the two array has merged
			 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 lhs_docid = *(rhs_docids + rhs_crt) + *(lhs_docids + lhs_crt);
			 new_block[num_rcds] = lhs_docid;				
			 num_rcds++;
			 rhs_crt++;
			 lhs_crt++;
			 addtype = 2;
			 break;

		case AosDataColOpr::eNoUpdate:
			 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
			 num_rcds++;
			 lhs_crt++;
			 rhs_crt++;
			 addtype=2;
			 break;

		case AosDataColOpr::eSetValue:
			 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
			 memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
			 num_rcds++;
			 lhs_crt++;
			 rhs_crt++;
			 addtype=2;
			 break;

		case AosDataColOpr::eMaxValue:
			 rhs_docid = *(rhs_docids + rhs_crt);
			 lhs_docid = *(lhs_docids + lhs_crt);
			 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 if (lhs_docid >= rhs_docid)
			 {
				memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
			 }
			 else
			 {
				memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
			 }
			 num_rcds++;
			 lhs_crt++;
			 rhs_crt++;
			 addtype = 2;
			 break;
			
		case AosDataColOpr::eMinValue:
			 rhs_docid = *(rhs_docids + rhs_crt);
			 lhs_docid = *(lhs_docids + lhs_crt);
			 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 if (lhs_docid >= rhs_docid)
			 {
				memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, sizeof(u64));
			 }
			 else
			 {
				memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, sizeof(u64));
			 }
			 num_rcds++;
			 lhs_crt++;
			 rhs_crt++;
			 addtype = 2;
			 break;
			
		default:
			 OmnAlarm << "UNrecognized opr: " << mOperator << enderr;
			 return false;
		}
	}
	if(mDocids)
	{
		delete []mDocids;
	}

	mDocids = new_block;
	mValues = new_buff;
	mValues->setReverse(mDataReverse);
	mNumDocs = total_rcds; 
	return true;
}


// Ken
bool
AosQueryRslt::merge(
		const AosQueryRsltObjPtr &rhs,
		const bool merge_all)
{
	aos_assert_r(rhs, false);
	aos_assert_r(mDataReverse == rhs->isDataReverse(), false);
	aos_assert_r(mOperator == rhs->getOperator(), false);
	aos_assert_r(mWithValues && rhs->isWithValues(), false);

	i64 lhs_total = mNumDocs;
	i64 rhs_total = rhs->getNumDocs();
	if (rhs_total <= 0) return true;

	AosBuffArrayPtr rhs_values = rhs->getValues();

	aos_assert_r(rhs_values, false);

	if (mNumDocs <= 0)
	{
		bool rslt = appendBlock(rhs->getDocidsPtr(), rhs_total);
		aos_assert_r(rslt, false);
			
		AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
		comp_func->setReverse(mDataReverse);
		mValues = OmnNew AosBuffArray(
		        comp_func, false, false, eDefaultRcdSize*mRcdLen);
		mValues->setAutoExtend(true,10);
		mValues->setReverse(mDataReverse);
		mValues->mergeFrom(rhs_values,0);
		return true;
	}

	aos_assert_r(mValues, false);
	AosBuffArrayPtr lhs_values = mValues;

	int rcd_len = lhs_values->getRecordLen();
	aos_assert_r(rcd_len == rhs_values->getRecordLen(), false);

	u64 * lhs_docids = mDocids;
	u64 * rhs_docids = rhs->getDocidsPtr();

	i64 total_rcds = lhs_total + rhs_total;

	AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
	comp_func->setReverse(mDataReverse);
	AosBuffArrayPtr new_buff = OmnNew AosBuffArray(
	        comp_func, false, false, eDefaultRcdSize*mRcdLen);
	new_buff->setAutoExtend(true,10);
	u64 * new_block = OmnNew u64[total_rcds];
	
	i64 lhs_crt = 0;
	i64 rhs_crt = 0;
	char* lhs_value = lhs_values->getEntry(lhs_crt);
	char* rhs_value = rhs_values->getEntry(rhs_crt);
	// Now it is ready to merge the two buffs. Both are not empty.
	i64 num_rcds = 0;
	while (num_rcds < total_rcds)
	{
		// Check whether lhs has run out. If yes, copy the remaining of rhs.
		if (lhs_total == lhs_crt && rhs_total == rhs_crt)
		{
			break;
		}

		if (lhs_total == lhs_crt)
		{
			if (!merge_all) break;

			new_buff->mergeFrom(rhs_values,rhs_crt);

			memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, (rhs_total - rhs_crt) * sizeof(u64));
			num_rcds += (rhs_total - rhs_crt);
			break;
		}

		// Check whether rhs has run out. If yes, copy the remaining of lhs.
		if (rhs_total == rhs_crt)
		{
			if (!merge_all) break;

			new_buff->mergeFrom(lhs_values,lhs_crt);
				
			memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, (lhs_total - lhs_crt) * sizeof(u64));
			num_rcds += (lhs_total - lhs_crt);
			break;
		}
		
		int rr = strncmp(lhs_value, rhs_value,rcd_len);
		if (rr == 0)
		{
			if (lhs_docids[lhs_crt] == rhs_docids[rhs_crt])
			{
				rr = 0;
			}
			else
			{
				rr = lhs_docids[lhs_crt] < rhs_docids[rhs_crt] ? -1 : 1; 
			}
		}
	
		if (rr != 0)
		{
			if ((!mDataReverse && rr < 0) || (mDataReverse && rr > 0))
			{
				// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				new_block[num_rcds] = lhs_docids[lhs_crt];
				num_rcds++;

				lhs_crt++;
				if(lhs_crt >= lhs_total)
				{
					lhs_value = 0;
				}
				else
				{
					lhs_value = lhs_values->getEntry(lhs_crt);
				}
			}
			else if ((!mDataReverse && rr > 0) || (mDataReverse && rr < 0))
			{
				// lhs_crt > rhs_crt. Need to put rhs_crt to new_buff and 
				// advance rhs_crt.
				new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				new_block[num_rcds] = rhs_docids[rhs_crt];
				num_rcds++;

				rhs_crt++;
				if (rhs_crt >= rhs_total)
				{
					rhs_value = 0;
				}
				else
				{
					rhs_value = rhs_values->getEntry(rhs_crt);
				}
			}
		}
		else
		{
			switch (mOperator)
			{
			case AosDataColOpr::eIndex:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt];
				 num_rcds++;
				 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				 new_block[num_rcds] = rhs_docids[rhs_crt];
				 break;
			
			case AosDataColOpr::eNormal:
			 	 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 	 new_block[num_rcds] = lhs_docids[lhs_crt] + rhs_docids[rhs_crt];
			 	 break;

			case AosDataColOpr::eNoUpdate:
			 	 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt];
			 	 break;

			case AosDataColOpr::eSetValue:
			 	 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
			 	 new_block[num_rcds] = rhs_docids[rhs_crt];
			 	 break;

			case AosDataColOpr::eMaxValue:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt] > rhs_docids[rhs_crt] ? lhs_docids[lhs_crt] : rhs_docids[rhs_crt];
			 	 break;
			
			case AosDataColOpr::eMinValue:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt] < rhs_docids[rhs_crt] ? lhs_docids[lhs_crt] : rhs_docids[rhs_crt];
				 break;
			default:
				 OmnAlarm << "UNrecognized opr: " << mOperator << enderr;
				 return 0;
			}
			
			num_rcds++;
			lhs_crt++;
			if(lhs_crt >= lhs_total)
			{
				lhs_value = 0;
			}
			else
			{
				lhs_value = lhs_values->getEntry(lhs_crt);
			}

			rhs_crt++;
			if(rhs_crt >= rhs_total)
			{
				rhs_value = 0;
			}
			else
			{
				rhs_value = rhs_values->getEntry(rhs_crt);
			}
		}
	}
		
	mNumDocs = num_rcds; 
	mDocids = new_block;
	mValues = new_buff;
	mValues->setReverse(mDataReverse);
	return true;
}

void
AosQueryRslt::toString(const AosRundataPtr &rdata)
{
	reset();
	bool finished;
	u64 docid;
	OmnString value;
	OmnScreen << "Dump QueryRslt: " << endl;
	if(mNumDocs == 0) return;
	while (1)
	{
		if (mWithValues)
		{
			nextDocidValue(docid, value, finished, rdata);
			cout << "[" << value << ", " << docid << "]" << endl;
		}
		else
		{
			docid = nextDocid(finished);
			cout << "[" << docid << "]" << endl;
		}

		if (finished) return;
	}
}


//shawn
bool
AosQueryRslt::merge(
		const AosQueryRsltObjPtr &rhs,
		const bool merge_all_left,
		const bool merge_all_right,
		bool &cut_left,
		bool &cut_right,
		OmnString &edge_value,
		u64 &edge_u64value,
		u64 &edge_docid)
{
	cut_left = false;
	cut_right = false;
	edge_value = "";
	edge_u64value = 0;
	edge_docid = 0;
	OmnString edge_value2;
	
	aos_assert_r(rhs, false);
	aos_assert_r(mDataReverse == rhs->isDataReverse(), false);
	aos_assert_r(mOperator == rhs->getOperator(), false);
	aos_assert_r(mWithValues && rhs->isWithValues(), false);

	i64 lhs_total = mNumDocs;
	i64 rhs_total = rhs->getNumDocs();
	if (rhs_total <= 0) return true;

	AosBuffArrayPtr rhs_values = rhs->getValueBuff();
	aos_assert_r(rhs_values, false);

	if (mNumDocs <= 0)
	{
		bool rslt = appendBlock(rhs->getDocidsPtr(), rhs_total);
		aos_assert_r(rslt, false);
			
		AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
		comp_func->setReverse(mDataReverse);
		mValues = OmnNew AosBuffArray(
		        comp_func, false, false, eDefaultRcdSize*mRcdLen);
		mValues->setAutoExtend(true,10);
		mValues->setReverse(mDataReverse);
		mValues->mergeFrom(rhs_values,0);
		return true;
	}

	aos_assert_r(mValues, false);
	AosBuffArrayPtr lhs_values = mValues;

	int rcd_len = lhs_values->getRecordLen();
	aos_assert_r(rcd_len == rhs_values->getRecordLen(), false);

	u64 * lhs_docids = mDocids;
	u64 * rhs_docids = rhs->getDocidsPtr();

	i64 total_rcds = lhs_total + rhs_total;

	AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
	comp_func->setReverse(mDataReverse);
	AosBuffArrayPtr new_buff = OmnNew AosBuffArray(
	        comp_func, false, false, eDefaultRcdSize*mRcdLen);
	new_buff->setAutoExtend(true,10);

	u64 * new_block = OmnNew u64[total_rcds];
	
	i64 lhs_crt = 0;
	i64 rhs_crt = 0;
	char* lhs_value = lhs_values->getEntry(lhs_crt);
	char* rhs_value = rhs_values->getEntry(rhs_crt);

	// Now it is ready to merge the two buffs. Both are not empty.
	i64 num_rcds = 0;
	while (num_rcds < total_rcds)
	{
		// Check whether lhs has run out. If yes, copy the remaining of rhs.
		if (lhs_total == lhs_crt && rhs_total == rhs_crt)
		{
			break;
		}

		if (lhs_total == lhs_crt)
		{
			if (!merge_all_right)
			{
				if(rhs_total != rhs_crt)
				{
					edge_value = edge_value2;
					edge_u64value = 0;
					edge_docid = lhs_docids[lhs_total-1];
					cut_right = true;
				}
				break;
			}
			
			new_buff->mergeFrom(rhs_values,rhs_crt);

			memcpy(&new_block[num_rcds], rhs_docids + rhs_crt, (rhs_total - rhs_crt) * sizeof(u64));
			num_rcds += (rhs_total - rhs_crt);
			break;
		}

		// Check whether rhs has run out. If yes, copy the remaining of lhs.
		if (rhs_total == rhs_crt)
		{
			if (!merge_all_left)
			{
				if(lhs_total != lhs_crt)
				{
					edge_value = edge_value2;
					edge_u64value = 0;
					edge_docid = rhs_docids[lhs_total-1];
					cut_right = true;
				}
				break;
			}
			new_buff->mergeFrom(lhs_values,lhs_crt);

			memcpy(&new_block[num_rcds], lhs_docids + lhs_crt, (lhs_total - lhs_crt) * sizeof(u64));
			num_rcds += (lhs_total - lhs_crt);
			break;
		}
		
		int rr = strncmp(lhs_value, rhs_value, rcd_len);
		if (rr == 0)
		{
			if (lhs_docids[lhs_crt] == rhs_docids[rhs_crt])
			{
				rr = 0;
			}
			else
			{
				rr = lhs_docids[lhs_crt] < rhs_docids[rhs_crt] ? -1 : 1; 
			}
		}
	
		if (rr != 0)
		{
			if ((!mDataReverse && rr < 0) || (mDataReverse && rr > 0))
			{
				// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				new_block[num_rcds] = lhs_docids[lhs_crt];
				num_rcds++;

				lhs_crt++;
				if(lhs_crt >= lhs_total)
				{
					edge_value2 = lhs_value;
					lhs_value = 0;
				}
				else
				{
					lhs_value = lhs_values->getEntry(lhs_crt);
				}
			}
			else if ((!mDataReverse && rr > 0) || (mDataReverse && rr < 0))
			{
				// lhs_crt > rhs_crt. Need to put rhs_crt to new_buff and 
				// advance rhs_crt.
				new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				new_block[num_rcds] = rhs_docids[rhs_crt];
				num_rcds++;

				rhs_crt++;
				if (rhs_crt >= rhs_total)
				{
					edge_value2 = rhs_value;
					rhs_value = 0;
				}
				else
				{
					rhs_value = rhs_values->getEntry(rhs_crt);
				}
			}
		}
		else
		{
			switch (mOperator)
			{
			case AosDataColOpr::eIndex:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt];
				 num_rcds++;
				 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
				 new_block[num_rcds] = rhs_docids[rhs_crt];
				 break;
			
			case AosDataColOpr::eNormal:
			 	 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
			 	 new_block[num_rcds] = lhs_docids[lhs_crt] + rhs_docids[rhs_crt];
			 	 break;

			case AosDataColOpr::eNoUpdate:
			 	 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt];
			 	 break;

			case AosDataColOpr::eSetValue:
			 	 new_buff->appendEntry(rhs_value, strlen(rhs_value),0);
			 	 new_block[num_rcds] = rhs_docids[rhs_crt];
			 	 break;

			case AosDataColOpr::eMaxValue:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt] > rhs_docids[rhs_crt] ? lhs_docids[lhs_crt] : rhs_docids[rhs_crt];
			 	 break;
			
			case AosDataColOpr::eMinValue:
				 new_buff->appendEntry(lhs_value, strlen(lhs_value),0);
				 new_block[num_rcds] = lhs_docids[lhs_crt] < rhs_docids[rhs_crt] ? lhs_docids[lhs_crt] : rhs_docids[rhs_crt];
				 break;
			default:
				 OmnAlarm << "UNrecognized opr: " << mOperator << enderr;
				 return 0;
			}
			
			num_rcds++;
			lhs_crt++;
			if(lhs_crt >= lhs_total)
			{
				edge_value2 = lhs_value;
				lhs_value = 0;
			}
			else
			{
				lhs_value = lhs_values->getEntry(lhs_crt);
			}

			rhs_crt++;
			if(rhs_crt >= rhs_total)
			{
				edge_value2 = rhs_value;
				rhs_value = 0;
			}
			else
			{
				rhs_value = rhs_values->getEntry(rhs_crt);
			}
		}
	}
		
	mNumDocs = num_rcds; 
	mDocids = new_block;
	mValues = new_buff;
	mValues->setReverse(mDataReverse);

	return true;
}

bool
AosQueryRslt::appendEntry(
		const u64 docid, 
		const OmnString &value)
{
	appendDocid(docid);
	appendStrValue(value.data());
	return true;
}

void 	
AosQueryRslt::setDataReverse(const bool reverse)
{
	if(mDataReverse == reverse)return;
	mDataReverse = reverse;
	if(!mValues)return;
	mValues->setReverse(reverse);
}


bool		
AosQueryRslt::getEntriesBeforeValue(
		const AosBuffArrayPtr &buff_array,
		const OmnString last_value,
		const bool &reverse)
{
	// This function retrieves all the values from 'buff_array'
	// that is no less than 'last_value'. 
	if(mNumDocs == 0) return true;

	aos_assert_r(mValues,false);
	if(mValues)mValues->setNumEntries(mNumDocs);
	i64 bound = mNumDocs;
	if(last_value != "")
	{
		bound = mValues->findBoundary(0, last_value.data(), reverse, true);
	}

	// 2014/01/28, Chen Ding, 
	// This does not look correct. 'buff_array' is the one to collect
	// the contents. It can be empty when it comes in.
	// aos_assert_r(bound >= 0 && bound < buff_array->getNumEntries(),false);
	aos_assert_r(bound >= 0, false);
	aos_assert_r(bound <= mNumDocs, false);
	for (i64 pos = 0;pos < bound;pos++)
	{
		buff_array->appendEntry(mValues->getEntry(pos),strlen(mValues->getEntry(pos)), mDocids[pos], 0);
	}

	// shrink
	i64 num_remove = bound;	
	mCrtIdx = 0;
	aos_assert_r(num_remove <= mNumDocs, false);
	mNumDocs -= num_remove;

	if(mNumDocs)
	{
		// memmove(mDocids, mDocids + num_remove*sizeof(u64), mNumDocs*sizeof(u64));
		memmove(mDocids, &mDocids[num_remove], mNumDocs*sizeof(u64));
	}
	
	if(mValues)
	{
		mValues->setNumEntries(mNumDocs + num_remove);
		mValues->shrinkFromHead(num_remove);
	}
	return true;
}


void	
AosQueryRslt::expandMemPriv(const i64 &size)
{
	if(mMemCap < size)
	{
		u64* new_block = OmnNew u64[size];
		memcpy(new_block,mDocids,mMemCap*sizeof(u64));
		if(mDocids)
		{
			delete []mDocids;
		}
		mDocids = new_block;
		mMemCap = size;
	}
}

AosBuffArrayPtr 
AosQueryRslt::getValues()const
{
	if(mValues)mValues->setNumEntries(mNumDocs);
	return mValues;
}


bool 
AosQueryRslt::checkAllNum()
{
	if(!mNumDocs) return true;
	aos_assert_r(mValues,false);
	
	OmnString cur_value;
	u64 u64_v;
	for (i64 i = 0;i < mNumDocs;i++)
	{
		cur_value = mValues->getEntry(mNumDocs-1);
		u64_v = AosStr2U64(cur_value);
		aos_assert_r(u64_v > 0, false);
	}
	return true;
}


AosCompareFunPtr
AosQueryRslt::createCompareFunc(const OmnString &type, const int &len)
{
	OmnString str;                                                                                            
	str << "<CompareFun cmpfun_type=\"custom\" cmpfun_size=\"" << len << "\">"
		<<  "<cmp_fields>"
		<<      "<field cmp_size=\"-1\" cmp_datatype=\"" << type << "\" cmp_pos=\"0\" />"
		<<      "<field cmp_size=\"-1\" cmp_pos=\"" << len - sizeof(u64) << "\" cmp_datatype=\"u64\" />"
		<<  "</cmp_fields>"
		<< "</CompareFun>";
	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	return AosCompareFun::getCompareFunc(cmp_tag);
}


bool	
AosQueryRslt::fillValueWithDocid()
{
	aos_assert_r(mWithValues,false);
	mValues = 0;
	for(i64 i = 0;i < mNumDocs; i++)
	{
		OmnString valuestr;
		valuestr << mDocids[i];
		appendStrValue(valuestr.data());
	}
	return true;
}

