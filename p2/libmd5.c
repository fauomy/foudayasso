/*  md5.c

*/

#include	<stdio.h>
#include	<string.h>
#include	"canary.h"

/***********************************************************************************/
static const INT32	A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0x10325476;

#define		MD5_NB_BLOCK	64

#define		F(X,Y,Z)	(((X) & ((Y) ^ (Z))) ^ (Z))
#define		G(X,Y,Z)	F(Z,X,Y)
#define		H(X,Y,Z)	((X) ^ (Y) ^ (Z))
#define		I(X,Y,Z)	((Y) ^ ((X) | (~(Z))))

#define		CSL(X, n)	INT32_ROL(X, n)

#define		FF(a, b, c, d, M, s, t) \
    (a) += F((b), (c), (d)) + (M) + (t),\
    (a)  = CSL((a), (s)) + (b)

#define		GG(a, b, c, d, M, s, t) \
    (a) += G((b), (c), (d)) + (M) + (t),\
    (a)  = CSL((a), (s)) + (b)

#define		HH(a, b, c, d, M, s, t) \
    (a) += H((b), (c), (d)) + (M) + (t),\
    (a)  = CSL((a), (s)) + (b)

#define		II(a, b, c, d, M, s, t) \
    (a) += I((b), (c), (d)) + (M) + (t),\
    (a)  = CSL((a), (s)) + (b)


#define	  T10		0XD76AA478
#define	  T11		0XE8C7B756
#define	  T12		0X242070DB
#define	  T13		0XC1BDCEEE
#define	  T14		0XF57C0FAF
#define	  T15		0X4787C62A
#define	  T16		0XA8304613
#define	  T17		0XFD469501
#define	  T18		0X698098D8
#define	  T19		0X8B44F7AF
#define	  T1A		0XFFFF5BB1
#define	  T1B		0X895CD7BE
#define	  T1C		0X6B901122
#define	  T1D		0XFD987193
#define	  T1E		0XA679438E
#define	  T1F		0X49B40821
#define	  T20		0XF61E2562
#define	  T21		0XC040B340
#define	  T22		0X265E5A51
#define	  T23		0XE9B6C7AA
#define	  T24		0XD62F105D
#define	  T25		0X02441453
#define	  T26		0XD8A1E681
#define	  T27		0XE7D3FBC8
#define	  T28		0X21E1CDE6
#define	  T29		0XC33707D6
#define	  T2A		0XF4D50D87
#define	  T2B		0X455A14ED
#define	  T2C		0XA9E3E905
#define	  T2D		0XFCEFA3F8
#define	  T2E		0X676F02D9
#define	  T2F		0X8D2A4C8A
#define	  T30		0XFFFA3942
#define	  T31		0X8771F681
#define	  T32		0X6D9D6122
#define	  T33		0XFDE5380C
#define	  T34		0XA4BEEA44
#define	  T35		0X4BDECFA9
#define	  T36		0XF6BB4B60
#define	  T37		0XBEBFBC70
#define	  T38		0X289B7EC6
#define	  T39		0XEAA127FA
#define	  T3A		0XD4EF3085
#define	  T3B		0X04881D05
#define	  T3C		0XD9D4D039
#define	  T3D		0XE6DB99E5
#define	  T3E		0X1FA27CF8
#define	  T3F		0XC4AC5665
#define	  T40		0XF4292244
#define	  T41		0X432AFF97
#define	  T42		0XAB9423A7
#define	  T43		0XFC93A039
#define	  T44		0X655B59C3
#define	  T45		0X8F0CCC92
#define	  T46		0XFFEFF47D
#define	  T47		0X85845DD1
#define	  T48		0X6FA87E4F
#define	  T49		0XFE2CE6E0
#define	  T4A		0XA3014314
#define	  T4B		0X4E0811A1
#define	  T4C		0XF7537E82
#define	  T4D		0XBD3AF235
#define	  T4E		0X2AD7D2BB
#define	  T4F		0XEB86D391

FUNCTION_PREFIX
void 	MD5_init(HashMD5 *pH)
{
    pH->len_h  = pH->len_l = 0;
    pH->head = 0;
    pH->a = A;
    pH->b = B;
    pH->c = C;
    pH->d = D;
}

static void	MD5_main(HashMD5 *pH, BYTE *in, int  nb)
{	
    UINT32	a, b, c, d;
    UINT32	a1, b1, c1, d1;
    UINT32	M[16];
    UINT32	t;

	a = pH->a;
	b = pH->b;
	c = pH->c;
	d = pH->d;

	while (nb -- )
    {
		a1 = a; b1 = b; c1 = c; d1 = d;

		LOAD_INT32_L(t, in); M[ 0] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 1] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 2] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 3] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 4] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 5] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 6] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 7] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 8] = t; in += 4;
		LOAD_INT32_L(t, in); M[ 9] = t; in += 4;
		LOAD_INT32_L(t, in); M[10] = t; in += 4;
		LOAD_INT32_L(t, in); M[11] = t; in += 4;
		LOAD_INT32_L(t, in); M[12] = t; in += 4;
		LOAD_INT32_L(t, in); M[13] = t; in += 4;
		LOAD_INT32_L(t, in); M[14] = t; in += 4;
		LOAD_INT32_L(t, in); M[15] = t; in += 4;

		/* Round 1 */

		FF(a, b, c, d, M[ 0],  7, T10);
		FF(d, a, b, c, M[ 1], 12, T11);
		FF(c, d, a, b, M[ 2], 17, T12);
		FF(b, c, d, a, M[ 3], 22, T13);
	    
		FF(a, b, c, d, M[ 4],  7, T14);
		FF(d, a, b, c, M[ 5], 12, T15);
		FF(c, d, a, b, M[ 6], 17, T16);
		FF(b, c, d, a, M[ 7], 22, T17);
	 
		FF(a, b, c, d, M[ 8],  7, T18);
		FF(d, a, b, c, M[ 9], 12, T19);
		FF(c, d, a, b, M[10], 17, T1A);
		FF(b, c, d, a, M[11], 22, T1B);
	    
		FF(a, b, c, d, M[12],  7, T1C);
		FF(d, a, b, c, M[13], 12, T1D);
		FF(c, d, a, b, M[14], 17, T1E);
		FF(b, c, d, a, M[15], 22, T1F);

		/* Round 2 */

		GG(a, b, c, d, M[ 1],  5, T20);
		GG(d, a, b, c, M[ 6],  9, T21);
		GG(c, d, a, b, M[11], 14, T22);
		GG(b, c, d, a, M[ 0], 20, T23);
	    
		GG(a, b, c, d, M[ 5],  5, T24);
		GG(d, a, b, c, M[10],  9, T25);
		GG(c, d, a, b, M[15], 14, T26);
		GG(b, c, d, a, M[ 4], 20, T27);
	 
		GG(a, b, c, d, M[ 9],  5, T28);
		GG(d, a, b, c, M[14],  9, T29);
		GG(c, d, a, b, M[ 3], 14, T2A);
		GG(b, c, d, a, M[ 8], 20, T2B);
	    
		GG(a, b, c, d, M[13],  5, T2C);
		GG(d, a, b, c, M[ 2],  9, T2D);
		GG(c, d, a, b, M[ 7], 14, T2E);
		GG(b, c, d, a, M[12], 20, T2F);

		/* Round 3 */

		HH(a, b, c, d, M[ 5],  4, T30);
		HH(d, a, b, c, M[ 8], 11, T31);
		HH(c, d, a, b, M[11], 16, T32);
		HH(b, c, d, a, M[14], 23, T33);
	    
		HH(a, b, c, d, M[ 1],  4, T34);
		HH(d, a, b, c, M[ 4], 11, T35);
		HH(c, d, a, b, M[ 7], 16, T36);
		HH(b, c, d, a, M[10], 23, T37);
	 
		HH(a, b, c, d, M[13],  4, T38);
		HH(d, a, b, c, M[ 0], 11, T39);
		HH(c, d, a, b, M[ 3], 16, T3A);
		HH(b, c, d, a, M[ 6], 23, T3B);
	    
		HH(a, b, c, d, M[ 9],  4, T3C);
		HH(d, a, b, c, M[12], 11, T3D);
		HH(c, d, a, b, M[15], 16, T3E);
		HH(b, c, d, a, M[ 2], 23, T3F);

		/* Round 4 */

		II(a, b, c, d, M[ 0],  6, T40);
		II(d, a, b, c, M[ 7], 10, T41);
		II(c, d, a, b, M[14], 15, T42);
		II(b, c, d, a, M[ 5], 21, T43);
	    
		II(a, b, c, d, M[12],  6, T44);
		II(d, a, b, c, M[ 3], 10, T45);
		II(c, d, a, b, M[10], 15, T46);
		II(b, c, d, a, M[ 1], 21, T47);
	 
		II(a, b, c, d, M[ 8],  6, T48);
		II(d, a, b, c, M[15], 10, T49);
		II(c, d, a, b, M[ 6], 15, T4A);
		II(b, c, d, a, M[13], 21, T4B);
	    
		II(a, b, c, d, M[ 4],  6, T4C);
		II(d, a, b, c, M[11], 10, T4D);
		II(c, d, a, b, M[ 2], 15, T4E);
		II(b, c, d, a, M[ 9], 21, T4F);

		a += a1;
		b += b1;
		c += c1;
		d += d1;
    }

	pH->a = a;
    pH->b = b;
    pH->c = c;
    pH->d = d;
}

#define	    BUFSIZE	16
unsigned int	ComputeCanary(const BYTE *in, int nb)
{
    HashMD5	md5;
    BYTE	buf[MD5_NB_BLOCK];
    static  char    *iv = "CyberSEED2014";
    char    *p;
    int	    i, j;
    unsigned rv;

    // initialize the buffer with iv;
    for (i = 0, p = iv; i < MD5_NB_BLOCK; i ++) {
	buf[i] = *p ++;
	if (! *p) {
	    p = iv;
	}
    }

    // xor it with incoming bytes; only half block
    for (i = 0, j = 0; (i < (MD5_NB_BLOCK >> 1)) && in[j]; i ++, j++) {
	buf[i] ^= in[j];
    }
    
    MD5_init(&md5);
    MD5_main(&md5, buf, 1);

    // output canary
    rv = md5.d ^ md5.c;
    if ((rv & 0xFFFF) != 0x1234) {
	return (rv << 8);
    }
    else
	return rv;
}

FUNCTION_PREFIX
void	MD5_update(HashMD5 *pH, unsigned char *p, unsigned int n)
{
    unsigned int		head;

	pH->len_l += n;
	if (pH->len_l < n)
		pH->len_h ++;

	head = pH->head;

	if (head + n < 64)
	{ /* q is not full */
		memcpy(pH->q + head, p, n);
		pH->head += n;
		return;
	}
	
	if (head)
	{ /* if something is in the queue, fill the queue first */
		unsigned int i;
		i = 64 - head;
		n -= i;
		memcpy(pH->q + head, p, i);
		p += i;
		MD5_main(pH, pH->q, 1);
		pH->head = 0;
    }

    if (n >= 64)
		MD5_main(pH, p, n >> 6);

	head = n & 0x3F;
	if (head)
	{
		memcpy(pH->q, p + (n - head), head);
		pH->head = head;
	}
}

FUNCTION_PREFIX
void  MD5_end(HashMD5 *pH, unsigned char *phv)
{
    static unsigned char padding[128] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int		head = pH->head;
    int		ppos;

	ppos =  (head >= 56) ? 120 - head: 56 - head;

	{
		UINT32	h, l;

		l = pH->len_l << 3;
		h = (pH->len_h << 3) | (pH->len_l >> 29);
		SAVE_INT32_L(l, padding + ppos);
		SAVE_INT32_L(h, padding + (ppos + 4));
	}

    MD5_update(pH, padding, ppos+8);

	/* restore padding */
	*(UINT32 *)(padding + ppos) = 0;
	*(UINT32 *)(padding + (ppos+4)) = 0;

	{
		UINT32	a, b, c, d;
		BYTE	*q = phv;

		a = pH->a;
		b = pH->b;
		c = pH->c;
		d = pH->d;

		memset((BYTE *)pH, 0, sizeof(*pH));

		SAVE_INT32_L(a, q);
        SAVE_INT32_L(b, q + 4);
		SAVE_INT32_L(c, q + 8);
		SAVE_INT32_L(d, q + 12);
		/* return q; */
	}
}


void	HASH_mem2hex(unsigned char *pd, unsigned char *ps, int n)
{
	static char D[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int	 v, i;
	/* char	*oldpd = pd; */

	i = n;
	while (i --)
    {
		v = *ps ++;
		*pd ++ = D[ v >> 4];
		*pd ++ = D[ v & 0xF];
    }
    *pd = 0;
}
