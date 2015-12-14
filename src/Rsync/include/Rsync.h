#ifndef RSYNC_H
#define RSYNC_H

#include "Checksum.h"
#include <sys/types.h>
#include <algorithm>
#include <vector>
using namespace std;
typedef unsigned int uint32;
typedef struct SUM_BUF
{
    off_t offset;			/* offset in file of this chunk */
    int len;			    /* length of chunk of file */
    int i;			        /* index of this chunk */
    uint32 sum1;	        /* simple checksum */
    char sum2[33];	        /* md5 checksum  */
}Sum_Buf;

typedef struct SUM_STRUCT
{
    off_t flength;		/* total file length */
    int count;			/* how many chunks */
    int remainder;		/* flength % block_length */
    int n;			/* block_length */
    Sum_Buf *sums;		/* points to info for each chunk */
}Sum_Struct;

struct Result//save report result
{
    int flag;// if flag == 1 save blockid, flag == 0 save different string blcok;
    char *str;
    int blockid;
};

class Match;

class Rsync
{
    public:
        int blocksize;
        Match *match;
        vector<Result> result;
        Sum_Struct *s;
        char* reference_buf;
        char* target_buf;

        Rsync(uint32 BlockSize);
        virtual ~Rsync();
        Sum_Struct* generate_sums(char *buf, off_t filelen);//divide file2 and calclulate the block checksum
        bool Compare(char* filename1, char* filename2);
        bool AddDifStr(char *srcbuf,int len);//save the different value
        bool ReadFile(char *filename , char * &buf, off_t &len);
        bool PrintResult();//print the final result
    protected:
    private:
};

#endif // RSYNC_H
