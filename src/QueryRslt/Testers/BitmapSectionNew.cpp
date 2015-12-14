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
// 01/06/2013       Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/Testers/BitmapSectionNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "BitmapUtil/BitmapUtil.h"
#include "Rundata/Rundata.h"
#include "BitmapMgr/BitmapMgr.h"
#include <algorithm>
AosBitmapSectionNew::AosBitmapSectionNew()
{
	 
}


AosBitmapSectionNew::~AosBitmapSectionNew()
{
}

bool
AosBitmapSectionNew::config()
{
	 
	return true;
}


bool 
AosBitmapSectionNew::start()
{
	aos_assert_r(basicTest(), false);
	return true;
}


bool
AosBitmapSectionNew::basicTest()
{
	u64 try_count = 0;
	while (true)
	{
OmnScreen<< "----------------------" << try_count++ << endl;
		init();
		createBlock();
        createBitmap();
        aos_assert_r(checkSections(), false);
/*        addBit();
        aos_assert_r(checkSections(), false);
        addBit();
        aos_assert_r(checkSections(), false);
        orBlock();
        aos_assert_r(checkSections(), false);
        andBlock();
        aos_assert_r(checkSections(), false);
        continue;
 */       
        u64 loop = rand()%1000;
        for(u64 i=0; i<loop; i++)
        {
            int oper = rand()%1000;
            if(oper > 100)
            {
        	    addBit();
            }
            else if (oper > 10)
            {
        	    removeBit();
            }
            else if (oper > 2)
            {
OmnScreen<< i << "/" << loop << " orBlock" << endl;
        	    orBlock();
            }
            else if (oper > 0)
            {
OmnScreen<< i << "/" << loop << " andBlock" << endl;
        	    andBlock();
            }
            else
            {
OmnScreen<< i << "/" << loop << " checkSections" << endl;
        	    aos_assert_r(checkSections(), false);
            }
            
        }
    }
    
	return true;
}

bool
AosBitmapSectionNew::init()
{
	mRsltDocids.clear();
	mOrDocids.clear();
	mAndDocids.clear();
	return true;
}

bool
AosBitmapSectionNew::createBlock()
{
	//rslt block
	mRsltDocids.clear();
	u64 docid_num = rand()/(100*10000);
	for (u64 i=0; i<docid_num; i++)
	{
		mRsltDocids.push_back(rand()%eMaxDocid);
	}
	
	sort(mRsltDocids.begin(), mRsltDocids.end());
	mRsltDocids.erase(unique(mRsltDocids.begin(), mRsltDocids.end()), mRsltDocids.end());
	
	//and block
	mAndDocids.clear();
	u64 andblock_num = rand()/(100*10000);
	u64 per_and = rand()%100;
	for (u64 i=0; i<andblock_num; i++)
	{
		if ((per_and < 20) && (docid_num>0))
		{
			mAndDocids.push_back(mRsltDocids[rand()%docid_num]);
		}
		else
		{
			mAndDocids.push_back(rand()%eMaxDocid);
		}
		
	}
	sort(mAndDocids.begin(), mAndDocids.end());
	mAndDocids.erase(unique(mAndDocids.begin(), mAndDocids.end()), mAndDocids.end());
	
	//or block
	mOrDocids.clear();
	u64 orblock_num = rand()/(100*10000);
	u64 per_or = rand()%100;
	for (u64 i=0; i<orblock_num; i++)
	{
		if ((per_or < 20) && (docid_num>0))
		{
			mOrDocids.push_back(mRsltDocids[rand()%docid_num]);
		}
		else
		{
			mOrDocids.push_back(rand()%eMaxDocid);
		}
		
	}
	sort(mOrDocids.begin(), mOrDocids.end());
	mOrDocids.erase(unique(mOrDocids.begin(), mOrDocids.end()), mOrDocids.end());
	
	return true;
}

bool
AosBitmapSectionNew::createBitmap()
{
	mBitmap = OmnNew AosBitmap();
	u64 docid_num = mRsltDocids.size();
	for (u64 i=0; i<docid_num; i++)
	{
		mBitmap->appendDocid(mRsltDocids[i]);
	}
	
	return true;
}

bool
AosBitmapSectionNew::addBit()
{
	u64 doc_id = rand();
	if (checkAndAdd(mRsltDocids, doc_id) == false)
	{
		 mBitmap->appendDocid(doc_id);
		 return true;
	}

	return true;
}

bool
AosBitmapSectionNew::removeBit()
{
	u64 doc_id = rand();
	if (checkAndRemove(mRsltDocids, doc_id))
	{
		 mBitmap->removeDocid(doc_id);
		 return true;
	}

	return false;
}

bool
AosBitmapSectionNew::orBlock()
{
	AosBitmapObjPtr bb = OmnNew AosBitmap();
	u64 size = mOrDocids.size();
{
	OmnScreen << "mOrDocids.size : " << mOrDocids.size() << endl;

	for(u64 i=0; i<mOrDocids.size(); i++)
	{
//		std::cout << mOrDocids[i] << " ";
	}
	OmnScreen << endl << endl;

    OmnScreen << "mRsltDocids.size : " << mRsltDocids.size() << endl;
	for(u64 i=0; i<mRsltDocids.size(); i++)
	{
//		std::cout << mRsltDocids[i] << " ";
	}
	OmnScreen << endl << endl;
}
	for(u64 i=0; i<size; i++)
	{
		bb->appendDocid(mOrDocids[i]);
		checkAndAdd(mRsltDocids, mOrDocids[i]);
	}

{
	OmnScreen << "mRsltDocids.size : " << mRsltDocids.size() << endl;
	for(u64 i=0; i< mRsltDocids.size(); i++)
	{
//		std::cout<< mRsltDocids[i] << " ";
	}
	std::cout << std::endl;
}
	AosBitmapObjPtr tempBitmap = AosBitmap::countOr(mBitmap, bb);
{
	OmnScreen << "tempbitmap size: " << tempBitmap->getDocidCount() << endl;
	tempBitmap->reset();
	vector<u64> docids;
	tempBitmap->getDocids(docids);
	OmnScreen << "tempbitmap docid vector size " << docids.size() << endl;
	for(u64 i=0; i<docids.size(); i++)
	{
//		std::cout << docids[i] << " ";
	}
	OmnScreen << endl << endl;
}
    AosBitmap::exchangeContent(tempBitmap, mBitmap);
    AosBitmapMgr::getSelf()->returnBitmap(tempBitmap);

	return true;
}

bool
AosBitmapSectionNew::andBlock()
{
	AosBitmapObjPtr bb = OmnNew AosBitmap();
	u64 size = mAndDocids.size();
	for(u64 i=0; i<size; i++)
	{
		bb->appendDocid(mAndDocids[i]);
	}
	
	AosBitmapObjPtr tempBitmap = AosBitmap::countAnd(mBitmap, bb);
    AosBitmap::exchangeContent(tempBitmap, mBitmap);
    AosBitmapMgr::getSelf()->returnBitmap(tempBitmap);
    
    //
    vector_and(mRsltDocids, mAndDocids);
    
	return true;
}


bool
AosBitmapSectionNew::checkAndAdd(vector<u64> &docid_vector, const u64 &docid)
{
	vector<u64>::iterator itr = docid_vector.begin();
	vector<u64>::iterator itr_end = docid_vector.end();
	if (itr == itr_end)
	{
		docid_vector.insert(itr, docid);
		return false;
	}
	
	while (itr != itr_end)
	{
		if (*itr == docid)
		{
			//OmnScreen << "equal *itr" << *itr << endl;
			return true;
		}
		
		if (*itr > docid)
		{
			docid_vector.insert(itr, docid);
			return false;
		}
		itr++;
	}
	docid_vector.insert(itr_end, docid);

	return false;
}

bool
AosBitmapSectionNew::checkAndRemove(vector<u64> &docid_vector, const u64 &docid)
{
	vector<u64>::iterator itr = docid_vector.begin();
	vector<u64>::iterator itr_end = docid_vector.end();
	while(itr != itr_end)
	{
		if(*itr == docid)
		{
			docid_vector.erase(itr);
			return true;
		}
		itr++;
	}
	
	return false;
}
  
bool
AosBitmapSectionNew::vector_and(vector<u64> &a, const vector<u64> &b)
{
	vector<u64> result;
	vector<u64>::iterator itr_a      =  a.begin();
	vector<u64>::iterator itrA_end   =  a.end();
	vector<u64>::const_iterator itr_b      =  b.begin();
	vector<u64>::const_iterator itrB_end   =  b.end();
	
	while((itr_a != itrA_end) && (itr_b != itrB_end))
	{
		if(*itr_a == *itr_b)
		{
			result.push_back(*itr_a);
			itr_a++;
			itr_b++;
		}
		else if(*itr_a < *itr_b)
		{
			itr_a++;
		}
		else
		{
			itr_b++;
		}
	}
	
	a.clear();
	
	u64 size = result.size();
	for(u64 i=0; i<size; i++)
	{
		a.push_back(result[i]);
	}
	
	return true;
}


bool
AosBitmapSectionNew::checkSections()
{
	bitmaps.clear();
	
	AosRundataPtr rdata;
	aos_assert_r(mBitmap->getSections(bitmaps, rdata), false);
	
	//check rslt
	u64 rslt_size = 0;
	mapitr_t itr = bitmaps.begin();
    while(itr != bitmaps.end())
    {
    	rslt_size += itr->second->getDocidCount();
    	itr++;
    }
    
    aos_assert_r(rslt_size==mBitmap->getDocidCount(), false);
    aos_assert_r(rslt_size==mRsltDocids.size(), false);
   
   
    mBitmap->reset();
    u64 doc_id;
    u64 section_id;
    for(u64 index=0; index<rslt_size; index++)
    {
    	doc_id = mRsltDocids[index];
    	
    	section_id = AosBitmapUtil::getSectionId(doc_id);
    	
    	//check section
    	itr = bitmaps.find(section_id);
    	aos_assert_r(itr != bitmaps.end(), false);
    	
    	//check doc_id exist
    	aos_assert_r(itr->second->checkDoc(doc_id), false);
    	
    	//check doc_id not exist
    	u64 num = 0;
    	for(itr=bitmaps.begin(); itr!=bitmaps.end(); itr++)
    	{
    		if (itr->first == section_id) 
    		{
    			num++;
    			aos_assert_r((num==1), false);
    			continue;
    		}
    		
    		aos_assert_r(((itr->second->checkDoc(doc_id)) == false), false);
    	}
    }
     
	return true;
}
/*
void
AosBitmapSectionNew::debug_show(const<u64> &vect, const char *msg)
{
	OmnScreen << "---------------" << msg << endl;
	for(u64 i=0; i<vect.size(); i++)
	{
		OmnScreen << "---------------" << msg << endl;		
	}
}
*/
