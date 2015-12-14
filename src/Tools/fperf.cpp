#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <sys/time.h>

int main(int argc, char* argv[]){
	char *fileName;
	FILE *p;
	int len;
	int oprLen;
	int opr;  //0: read  1: write  2: seek only
	bool randomFlag;
	int rounds;
	char *buff;
	int i;
	int tStart, tEnd;
	int num;
	struct timeval tv;
	int pos;

	if (argc < 6)
	{
		printf("usage: fperf <filename> <filesize> <-ran random/-seq sequential> <-r for read/-w for write/-s for seek> <rounds> <each read/write size>\n");
		return -1;
	}

	fileName = argv[1];
	len = atoi(argv[2]);

	if (strcmp(argv[3], "-ran") == 0)
		randomFlag = true;
	else
		randomFlag = false;

	if (strcmp(argv[4], "-r") == 0)
		opr = 0;
	else if (strcmp (argv[4], "-w") == 0)
		opr = 1;
	else
		opr = 2;

	rounds = atoi(argv[5]);
	oprLen = atoi(argv[6]);

	//rounds = len / oprLen;
	buff = (char *)malloc(oprLen);
	for (i = 0; i < oprLen;  i++)
	{
		buff[i] = '0' + i % 5;
	}

	//for read and write
	p = fopen(fileName,"r+");
	if( p == NULL){
		printf("Error on open file %s\n", fileName);
		return -1;
	}

    gettimeofday(&tv, NULL);
	tStart = tv.tv_sec;
	printf("operation rounds: %d\n", rounds);
	for (i = 0; i < rounds; i++)
	{
		if (randomFlag)
		{
			pos = random();
			pos = pos % (len - oprLen);
			fseek(p, pos, SEEK_SET);
			//for debugging
			//printf("seeking pos is: %d\n", pos);
		}

		if (opr == 0)
		{
			num = fread(buff, 1, oprLen, p);
			//for debugging
			//printf("read %d bytes\n", num);
		}
		else if (opr == 1)
		{
			num = fwrite(buff, 1, oprLen, p);
			//for debugging
			//printf("write %d bytes\n", num);
		}
		else
		{
			//do nothing for seek only
		}
		/*
		if (i == rounds - 1)
		{
			buff[oprLen - 1] = '\0';
			if (opr == 0)
				printf("read %d bytes in the last round %d: %s \n", num, rounds, buff);
			else
				printf("write %d bytes in the last round %d: %s \n", num, rounds, buff);
		}
		*/
	}

	//if (fflush(p) != 0)
	//	printf("flushing error!\n");

	free(buff);
	fclose(p);

    gettimeofday(&tv, NULL);
	tEnd = tv.tv_sec;
	printf("Taking time: %u\n", tEnd - tStart);
	return 0;
}
