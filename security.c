#include "banking.h"

/* SHA-256 round constants */
static const unsigned int _kv[64] =
{
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/* Bitwise rotation/logic macros */
#define _RT(x,n) (((x)>>(n))|((x)<<(32-(n))))
#define _SR(x,n) ((x)>>(n))
#define _CH(x,y,z) (((x)&(y))^(~(x)&(z)))
#define _MJ(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
#define _S0(x) (_RT(x,2) ^_RT(x,13)^_RT(x,22))
#define _S1(x) (_RT(x,6) ^_RT(x,11)^_RT(x,25))
#define _G0(x) (_RT(x,7) ^_RT(x,18)^_SR(x,3))
#define _G1(x) (_RT(x,17)^_RT(x,19)^_SR(x,10))

/* SHA-256 core transform */
static void _sha256_core(const unsigned char *msg, size_t len,unsigned char *digest)
{
    /* Initial hash values */
    unsigned int h0=0x6a09e667, h1=0xbb67ae85, h2=0x3c6ef372, h3=0xa54ff53a,h4=0x510e527f, h5=0x9b05688c, h6=0x1f83d9ab, h7=0x5be0cd19;

    /* Pad message */
    size_t bit_len = len * 8;
    size_t pad_len = len + 1;
    while (pad_len % 64 != 56) pad_len++;
    pad_len += 8;

    unsigned char *m = (unsigned char *)calloc(pad_len, 1);
    if (!m)
    {
        return;
    }
    memcpy(m, msg, len);
    m[len] = 0x80;

    /* Append length big-endian */
    for (int i = 7; i >= 0; i--)
        m[pad_len - 8 + (7 - i)] = (unsigned char)((bit_len >> (i*8)) & 0xFF);

    /* Process 512-bit chunks */
    for (size_t chunk = 0; chunk < pad_len; chunk += 64)
        {
        unsigned int w[64];
        for (int i = 0; i < 16; i++)
            w[i] = ((unsigned int)m[chunk+i*4]   <<24) |((unsigned int)m[chunk+i*4+1] <<16) |((unsigned int)m[chunk+i*4+2] << 8) |((unsigned int)m[chunk+i*4+3]      );
        /* Message schedule */
        for (int i = 16; i < 64; i++)
            w[i] = _G1(w[i-2]) + w[i-7] + _G0(w[i-15]) + w[i-16];

        unsigned int a=h0,b=h1,c=h2,d=h3,e=h4,f=h5,g=h6,hh=h7;
        /* Compression rounds */
        for (int i = 0; i < 64; i++)
        {
            unsigned int t1 = hh + _S1(e) + _CH(e,f,g) + _kv[i] + w[i];
            unsigned int t2 = _S0(a) + _MJ(a,b,c);
            hh=g; g=f; f=e; e=d+t1;
            d=c;  c=b; b=a; a=t1+t2;
        }
        h0+=a; h1+=b; h2+=c; h3+=d;
        h4+=e; h5+=f; h6+=g; h7+=hh;
    }
    free(m);

    /* Pack digest */
    unsigned int words[8] = {h0,h1,h2,h3,h4,h5,h6,h7};
    for (int i = 0; i < 8; i++)
    {
        digest[i*4  ] = (words[i]>>24)&0xFF;
        digest[i*4+1] = (words[i]>>16)&0xFF;
        digest[i*4+2] = (words[i]>> 8)&0xFF;
        digest[i*4+3] = (words[i]    )&0xFF;
    }
}

/* Hash string to hex */
void bx_digest_hex(const char *src, char *hex_out)
{
    unsigned char digest[32];
    _sha256_core((const unsigned char *)src, strlen(src), digest);
    for (int i = 0; i < 32; i++)
        sprintf(hex_out + i*2, "%02x", digest[i]);
    hex_out[64] = '\0';
}

#define _CF "credentials.dat"

/* Save hashed credentials */
int bx_store_cred(const char *uid, const char *pwd, int acc_no)
{
    char uh[65], ph[65];
    bx_digest_hex(uid, uh);
    bx_digest_hex(pwd, ph);

    FILE *fp = fopen(_CF, "a");
    if (!fp)
    {
        printf("\n[ERROR] Cannot write to credentials file!\n");
        return 0;
    }
    fprintf(fp, "%d|%s|%s\n", acc_no, uh, ph);
    fclose(fp);
    return 1;
}

/* Verify login credentials */
int bx_verify_cred(const char *uid, const char *pwd, int *acc_out)
{
    char uh[65], ph[65];
    bx_digest_hex(uid, uh);
    bx_digest_hex(pwd, ph);

    FILE *fp = fopen(_CF, "r");
    if (!fp)
    {
        return 0;
    }

    char line[200], su[65], sp[65];
    int  acc;
    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line,"\r\n")] = '\0';
        if (sscanf(line, "%d|%64[^|]|%64s", &acc, su, sp) != 3)
        {
             continue;
        }
        if (strcmp(uh, su)==0 && strcmp(ph, sp)==0)
        {
            *acc_out = acc;
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Check if user ID exists */
int bx_uid_taken(const char *uid)
{
    char uh[65];
    bx_digest_hex(uid, uh);

    FILE *fp = fopen(_CF, "r");
    if (!fp) return 0;

    char line[200], su[65], sp[65];
    int  acc;
    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line,"\r\n")] = '\0';
        if (sscanf(line, "%d|%64[^|]|%64s", &acc, su, sp) != 3)
        {
            continue;
        }
        if (strcmp(uh, su)==0)
            {
                 fclose(fp);
                 return 1;
        }
    }
    fclose(fp);
    return 0;
}
