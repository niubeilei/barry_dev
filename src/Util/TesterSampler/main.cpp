#include "Util/File.h"
#include "Util/BuffArray.h"
#define RECORDNUMBER			60000000
#define DATAFILENAME			"data"
#define NUMBERBUCKET			100
#define NUMBERRCDPERBUCKET		1000000

static clock_t			sgStart = clock();

struct Record
{
	char value[8];
	u64 key;
};

bool
generateData()
{
	return true;
}

bool
createSamplerArray(
		const AosBuffArrayPtr &orig_array,
		AosBuffArrayPtr &result_array)
{
	u32 mNum = NUMBERBUCKET;/////////////////////////
	u32 mInterval = NUMBERRCDPERBUCKET;/////////////////////////
	u32 orig_num = orig_array->getNumEntries();
	int rcd_len = orig_array->getRecordLen();
	result_array = orig_array->clone();
	if (mNum != 0)
	{
		u32 interval = orig_num / mNum;
		for(u32 i=0; i<mNum; i++)
		{
			u64 rcd_idx = i*interval;
			if (rcd_idx > orig_num)
			{
				break;
			}
			char *rcddata = orig_array->getRecord(rcd_idx, rcd_len);
			result_array->addValue(rcddata, rcd_len, 0);
		}

	}
	else
	{
		u32 rslt_num = orig_num / mInterval;
		for(u32 i=0; i<rslt_num; i++)
		{
			u64 rcd_idx = i*mInterval;
			char *rcddata = orig_array->getRecord(rcd_idx, rcd_len);
			result_array->addValue(rcddata, rcd_len, 0);
		}
	}
	result_array->sort();
	return true;
}

bool
distributeData(
		vector<AosBuffArrayPtr>& buckets,
		const AosBuffArrayPtr& orig_array,
		const AosBuffArrayPtr& result_array)
{
	u32 num = result_array->getNumEntries();

	////////print
	int rlen = result_array->getRecordLen();
	for(u32 i=0; i<num; i++)
	{
		char* r = result_array->getRecord(i, rlen);
std::cout << "section: " << r << std::endl;
	}
	////////////////
	
	u32 num_section = num + 1;
	for(u32 i=0; i<num_section; i++)
	{
		AosBuffArrayPtr b = orig_array->clone();
		buckets.push_back(b);
	}
	u64 num_records = orig_array->getNumEntries();
	int rcd_len = orig_array->getRecordLen();
	u64 idx = 0;
	while(idx < num_records)
	{
		char* rcd = orig_array->getRecord(idx, rcd_len);
		int i = result_array->findBoundary(rcd);
if (idx % 1000000 == 0)
{
std::cout << "rcd: " <<  rcd 
		  << " section idx:" << i << std::endl;
}
		if (i<0)
		{
			i = num;
		}
		AosBuffArrayPtr b = buckets.at(i);
		b->addValue(rcd, rcd_len, 0);
		idx++;
	}
	
	return true;
}

char sg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u',
	'v', 'w', 'x', 'y', 'z'};

void randstr(char *buff, int maxlen)
{
	int len = rand()%maxlen +1;
	for (int i=0; i<len; i++)
	{
		buff[i] = sg[rand()%26];
	}
	buff[len] = 0;
}

bool
test()
{
	std::cout << "start to test...." << std::endl;
	AosCompareFunPtr cmp = OmnNew AosFunStrU642(32);
	AosBuffArrayPtr array = OmnNew AosBuffArray(cmp, false, false);
	for(u64 i=0; i<RECORDNUMBER; i++)
	{
		char buff[24];
		randstr(buff, 23);
		array->addValue(buff, 0, NULL);
		aos_assert_r(buff[0] != 0, false);
	}

	// clock_t begin = clock();
	// array->sort();
	// clock_t end = clock();
	// std::cout << (end-begin)/CLOCKS_PER_SEC << "s" << std::endl;

	// return true;



	std::cout << "start to sampler data takes" << endl;
	AosBuffArrayPtr result_array;
	clock_t begin = clock();
	createSamplerArray(array, result_array);
	clock_t end =  clock();
	std::cout << "sampler data takes: " << std::endl;
	std::cout << "begin: " << begin << std::endl;
	std::cout << "end: " << end << std::endl;
	std::cout << (end-begin)/CLOCKS_PER_SEC << "s" << std::endl;


	std::cout << "start to distribute data...." << std::endl;
	vector<AosBuffArrayPtr> buckets;
	begin = clock();
	distributeData(buckets, array, result_array);
	end = clock();
	std::cout << "distribute data takes: " << std::endl;
	std::cout << "begin: " << begin << std::endl;
	std::cout << "end: " << end << std::endl;
	std::cout << (end-begin)/CLOCKS_PER_SEC << "s" << std::endl;
	return true;
}

int main(int argc, char** argv)
{
	switch(argv[1][0])
	{
	case 'g':
		aos_assert_r(generateData(), 0);
		break;
	case 't':
		aos_assert_r(test(), 0);
	}
	return 0;
}
