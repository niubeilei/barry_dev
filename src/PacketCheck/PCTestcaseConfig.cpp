#include "PCTestcaseConfig.h"
#include <stdio.h>

#define MAXMESLEN 1024000 
#define SEPERATOR_BINARY ' '

bool byteStringToStream(std::string byteString, char *byteStream, unsigned int &len)
{
	unsigned int byteStringLen = byteString.length();
	if (len <  byteStringLen / 3)
	{
		len = byteStringLen/3+1;
		return false;
	}

	std::string tmp;
	unsigned int tmpChar;
	int pos = byteString.find(SEPERATOR_BINARY);
	len = 0;
	while (pos != std::string::npos)
	{
		tmp = byteString.substr(0, pos);
		sscanf(tmp.c_str(), "%x", &tmpChar);	
		byteStream[len++] = tmpChar;
	
		byteString = byteString.substr(pos+1);
		pos = byteString.find(SEPERATOR_BINARY);
	}
	if (byteString.length() == 2)
	{
		sscanf(byteString.c_str(), "%x", &tmpChar);	
		byteStream[len++] = tmpChar;
	}

	return true;
}

bool byteStreamToString(char* byteStream, unsigned int len, std::string &byteString)
{
	unsigned int i, j;
	char tmp[3]={0};

	for (i = 0, j = 0; i < len; i++)
	{
		sprintf(tmp, "%x", byteStream[i]);
		byteString += tmp;
		byteString += SEPERATOR_BINARY;
	}
	return true;
}

TCData::TCData()
{
    mId = -1;
	mSendContent = NULL;
	mRecvContent = NULL;
}

TCData::TCData(const TCData &tcData)
{
	if (this != &tcData)
	{
		mType = tcData.mType;
		mId = tcData.mId;

		mSendLength = tcData.mSendLength;
		mSendContent = new char[tcData.mSendLength];
		memcpy(mSendContent, tcData.mSendContent, mSendLength);

		mRecvLength = tcData.mRecvLength;
		mRecvContent = new char[tcData.mRecvLength];
		memcpy(mRecvContent, tcData.mRecvContent, mRecvLength);
	}
}

TCData::~TCData()
{
	if (mSendContent != NULL)
		delete [] mSendContent;
	if (mRecvContent != NULL)
		delete [] mRecvContent;
}

int TCData::getType()
{
	return mType;
}

std::string TCData::getTypeString()
{
	std::string typeStr;
	switch (mType)
	{
		case eTCDATA_HTTP:
			typeStr = "HTTP";
			break;
		case eTCDATA_AM:
			typeStr = "AM";
			break;
		default:
			break;
	}
	return typeStr;
}

void TCData::setType(int type)
{
	mType = type;
	return;
}

int TCData::getID()
{
	return mId;
}

void TCData::setID(int id)
{
	mId = id;
}

void TCData::setRecvBuf(const char *buf)
{
	if (buf == NULL)
		return;
	memset(mRecvContent, 0, mRecvLength);
	memcpy(mRecvContent, buf, mRecvLength);
}

void TCData::getRecvBuf(char* &buf)
{
	if (mRecvContent == NULL)
		return;

	buf = mRecvContent;
}

void TCData::setSendBuf(const char *buf)
{
	if (buf == NULL)
		return;

	memset(mSendContent, 0, mSendLength);
	memcpy(mSendContent, buf, mSendLength);
}

void TCData::getSendBuf(char* &buf)
{
	if (mSendContent == NULL)
		return;

	buf = mSendContent;
}

void TCData::setSendLength(int length)
{
	mSendLength = length;
	if (mSendContent != NULL)
	{
		delete [] mSendContent;
	}

	mSendContent = new char[mSendLength];

}

int TCData::getSendLength()
{
	return mSendLength;
}

void TCData::setRecvLength(int length)
{
	mRecvLength = length;

	if (mRecvContent != NULL)
	{
		delete [] mRecvContent;
	}

	mRecvContent = new char[mRecvLength];
}

int TCData::getRecvLength()
{
	return mRecvLength;
}

TCData& TCData::operator = (const TCData& tcData)
{
	mType = tcData.mType;
	mId = tcData.mId;

	mSendLength = tcData.mSendLength;
	mSendContent = new char[tcData.mSendLength];
	strncpy(mSendContent, tcData.mSendContent, mSendLength);

	mRecvLength = tcData.mRecvLength;
	mRecvContent = new char[tcData.mRecvLength];
	strncpy(mRecvContent, tcData.mRecvContent, mRecvLength);

	return *this;
}

int TCData::getContentType()
{
	int contentType;
	switch (mType)	
	{
		case eTCDATA_HTTP:
			contentType = eTCDATA_TEXT;
			break;
		case eTCDATA_AM:
			contentType = eTCDATA_BINARY;
			break;
		default:
			contentType = eTCDATA_OTHER;
			break;
	}
	return contentType;
}

AosPCTestcaseConfig::AosPCTestcaseConfig()
{
}

AosPCTestcaseConfig::AosPCTestcaseConfig(const char* configFileName):
	mConfigFileName(configFileName)
{
}

AosPCTestcaseConfig::~AosPCTestcaseConfig()
{
	TCDataPtrVector::iterator iter;
	for (iter=mTCDataPtrVector.begin(); iter<mTCDataPtrVector.end(); iter++)
	{
		TCData* tcData = *iter;
		mTCDataPtrVector.erase(iter);
		delete tcData;
	}
}

void AosPCTestcaseConfig::setConfigFileName(const char* configFileName)
{
	mConfigFileName = configFileName;
}

int AosPCTestcaseConfig::readFile()
{
	int type, id, sendLength, recvLength, len;
	FILE *mConfigFile;
	char* sendBuf, *recvBuf;
	TCData *tcData = NULL;

	if (mConfigFileName.empty())
	{
		return -1;
	}
	mConfigFile = fopen(mConfigFileName.c_str(), "r");
	if (mConfigFile == NULL)
		return -1;

	mTCDataPtrVector.clear();
	while (!feof(mConfigFile)) 
	{
		tcData = new TCData();
		len = fread(&type, 1, sizeof(int), mConfigFile);
		if (len != sizeof(int) || feof(mConfigFile))
			break;
		tcData->setType(type);

		len = fread(&id, 1, sizeof(int), mConfigFile);
		if (len != sizeof(int) || feof(mConfigFile))
			break;
		tcData->setID(id);

		len = fread(&sendLength, 1, sizeof(int), mConfigFile);
		if (len != sizeof(int) || feof(mConfigFile))
			break;
		if (sendLength > MAXMESLEN)
		{
			std::cout << "May be there some error occur." << std::endl;
			return -1;
		}
		tcData->setSendLength(sendLength);

		tcData->getSendBuf(sendBuf);
		len = fread(sendBuf, 1, sendLength, mConfigFile);
		if (len != sendLength || feof(mConfigFile))
			break;

		len = fread(&recvLength, 1, sizeof(int), mConfigFile);
		if (len != sizeof(int) || feof(mConfigFile))
			break;
		if (recvLength > MAXMESLEN)
		{
			std::cout << "May be there some error occur." << std::endl;
			return -1;
		}
		tcData->setRecvLength(recvLength);

		tcData->getRecvBuf(recvBuf);
		len = fread(recvBuf, 1, recvLength, mConfigFile);	
		if (len != recvLength)
			break;

		mTCDataPtrVector.push_back(tcData);
	}

	fclose(mConfigFile);

	return 0;
}

int AosPCTestcaseConfig::saveFile()
{
	int type, id, sendLength, recvLength, len, i;
	FILE *mConfigFile;
	TCData *tcData;
	char *sendBuf, *recvBuf;

	mConfigFile = fopen(mConfigFileName.c_str(), "w");
	if (mConfigFile == NULL)
		return -1;

	int tcNum = mTCDataPtrVector.size();
	for (i = 0; i < tcNum; i++) 
	{
		tcData = mTCDataPtrVector[i];
		type = tcData->getType();
		len = fwrite(&type, 1, sizeof(type), mConfigFile);
		if (len != sizeof(type))
			break;

		id = tcData->getID();
		len = fwrite(&id, 1, sizeof(id), mConfigFile);
		if (len != sizeof(int))
			break;

		sendLength = tcData->getSendLength();
		if (sendLength > MAXMESLEN)
		{
			std::cout << "May be there are some error occur." << std::endl;
			return -1;
		}
		len = fwrite(&sendLength, 1, sizeof(sendLength), mConfigFile);
		if (len != sizeof(int))
			break;

		tcData->getSendBuf(sendBuf);
		len = fwrite(sendBuf, 1, sendLength, mConfigFile);
		if (len != sendLength)
			break;

		recvLength = tcData->getRecvLength();
		if (recvLength > MAXMESLEN)
		{
			std::cout << "May be there are some error occur." << std::endl;
			return -1;
		}
		len = fwrite(&recvLength, 1, sizeof(recvLength), mConfigFile);
		if (len != sizeof(int))
			break;

		tcData->getRecvBuf(recvBuf);
		len = fwrite(recvBuf, 1, recvLength, mConfigFile);
		if (len != recvLength)
			break;
	}

	fclose(mConfigFile);

	if (i < tcNum)
		return -1;

	return 0;
}

void AosPCTestcaseConfig::getTCData(int tcID, TCData * &tcData)
{
    tcData = NULL;
    int tcNum = mTCDataPtrVector.size();
	for (int i = 0; i < tcNum; i++) 
	{
		if (mTCDataPtrVector[i]->getID() == tcID) 
		{
			tcData = mTCDataPtrVector[i];
			return;
		}
	}	
}

void AosPCTestcaseConfig::getTCData(int type, TCDataPtrVector tcDataVector)
{
	int tcNum = mTCDataPtrVector.size();
	for (int i = 0; i < tcNum; i++) 
	{
		if (mTCDataPtrVector[i]->getType() == type)
		{
			tcDataVector.push_back(mTCDataPtrVector[i]);
		}
	}
}

void AosPCTestcaseConfig::addTCData(const TCData& tcData)
{
	TCData* tc = new TCData(tcData);
	time_t now;
	time(&now);
	tc->setID((int)now);
	mTCDataPtrVector.push_back(tc);
}

void AosPCTestcaseConfig::deleteTCData(int tcID)
{
	/*	int tcNum = mTCDataPtrVector.size();
		for (int i = 0; i < tcNum; i++) 
		{
		TCData* tcData = mTCDataPtrVector[i];
		if (tcData->getID() == tcID) 
		{
		TCDataPtrVector::iterator iter = std::find(mTCDataPtrVector.begin(), mTCDataPtrVector.begin(), tcData);
		if (iter != mTCDataPtrVector.end())
		mTCDataPtrVector.erase(iter);
		break;
		}
		}*/
	TCDataPtrVector::iterator iter;
	for (iter=mTCDataPtrVector.begin(); iter<mTCDataPtrVector.end(); iter++)
	{
		TCData* tcData = *iter;
		if ((*iter)->getID() == tcID) 
		{
			mTCDataPtrVector.erase(iter);
			delete tcData;
			break;
		}
	}
}

int AosPCTestcaseConfig::getVectorSize()
{
	return mTCDataPtrVector.size();
}

