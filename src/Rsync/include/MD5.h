#ifndef MD5_H
#define MD5_H

typedef unsigned int UINT32;
class MD5
{
    public:
        typedef struct
        {
            UINT32 state[4];   /* state (ABCD) */
            UINT32 count[2];   /* number of bits, modulo 2^64 (lsb first) */
            unsigned char buffer[64];   /* input buffer */
        } MD5_CTX;

        static unsigned char PADDING[64];
    public:
        MD5();
        virtual ~MD5();
        static void MD5Transform (UINT32 a[4], unsigned char b[64]);
        static void MD5Init(MD5_CTX *context);
        static void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
        static void MD5Final(unsigned char digest[16], MD5_CTX * context);
        static void Encode(unsigned char *output, UINT32 *input, unsigned int len);
        static void Decode(UINT32 *output, unsigned char *input, unsigned int len);
        static void TransTo32ByteMd5(char *pTextStr, char *pOutStr);
    protected:
    private:
};

#endif // MD5_H
