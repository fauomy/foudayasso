/* md5.h
*/

#ifndef	_MD5_H_
#define	_MD5_H_

#define		SYS_LITTLE_ENDIAN
// #define		SYS_WINDOWS_DLL

#if		defined(__cplusplus)
#define		EXTERN_C		extern "C"
#else
#define		EXTERN_C
#endif

#if		defined(SYS_WINDOWS_DLL)
#define		DLL_EXPORT	__declspec(dllexport) 
#else
#define		DLL_EXPORT
#endif

#if     !defined(INT8)
#define         INT8    signed char
#endif

#if		!defined(UINT8)
#define         UINT8   unsigned char
#endif

#if     !defined(INT16)
#define         INT16    short
#endif

#if		!defined(UINT16)
#define         UINT16   unsigned short
#endif

#if     !defined(INT32)
#define			INT32	int
#endif

#if		!defined(UINT32)
#define         UINT32  unsigned int
#endif

#if	!defined(INT64)
#define		INT64	long long int
#endif

#if !defined(UINT64)
#define		UINT64	unsigned long long int
#endif

#if     !defined(BYTE)
#define         BYTE    UINT8
#endif

#if     !defined(BOOL)
#define		BOOL	int
#endif

#if		!defined(TRUE)
#define		TRUE	1
#endif

#if		!defined(FALSE)
#define		FALSE	0
#endif

#if     !defined(INT)
#define         INT     int
#endif

#if     !defined(UINT)
#define         UINT    unsigned int
#endif

#if     !defined(LONG)
#define         LONG    long
#endif

#if     !defined(ULONG)
#define         ULONG   unsigned long
#endif

/* Define rotate left */
#define 	INT32_ROL(w, n)		(((w) << (n)) | ((UINT32)(w) >> (32 - (n))))
#define 	INT32_ROR(w, n)		(((w) << (32 - (n))) | ((UINT32)(w) >> (n)))

/* If the ROTATE instruciton is available, reversing the byte order 
   needs only 5 instrcutions, and may finish in three cycles*/
#define	INT32_REVERSE(w)	\
((INT32_ROL((w), 24) & 0xFF00FF00u) | (INT32_ROL((w), 8) & 0xFF00FF))

/* Suppose the memory has 
  0x01, 0x02, 0x03, 0x04, ....
  and   *p == 0x01;
  After LOAD_INT32_L(w, p), w == 0x04030201;
  After LOAD_INT32_B(w, p), W == 0x01020304;
  w is 32 bits unsigned integer
*/

#define	INT64_REVERSE(w, rw)	{	\
	UINT64	t = (w);				\
	t = (t >> 32) | (t << 32);		\
	t =		((t & 0xFFFF0000FFFF0000) >> 16) | ((t & 0x0000FFFF0000FFFF) << 16);	\
	(rw) =	((t & 0xFF00FF00FF00FF00) >> 8) | ((t & 0x00FF00FF00FF00FF) << 8);		\
}


#if	defined(SYS_LITTLE_ENDIAN)

#define		LOAD_INT32_L(w, p)	((w) = *((INT32 *)(p)))
#define		LOAD_INT32_B(w, p)	{UINT32 __t = *((UINT32 *)(p)); (w) = INT32_REVERSE(__t);}
#define		SAVE_INT32_L(w, p)	(*((INT32 *)(p)) = (w))
#define		SAVE_INT32_B(w, p)	(*((INT32 *)(p)) = INT32_REVERSE(w))

#define		LOAD_INT64_B(w, p)	{ INT64_REVERSE(*((UINT64 *)(p)), w) }
#define		SAVE_INT64_B(w, p)	{ INT64_REVERSE(w, *((INT64 *)(p))) }

#else

#define		LOAD_INT32_B(w, p)	((w) = *((INT32 *)(p)))
#define		LOAD_INT32_L(w, p)	{UINT32 __t = *((INT32 *)(p)); (w) = INT32_REVERSE(__t);}
#define		SAVE_INT32_B(w, p)	(*((INT32 *)(p)) = (w))
#define		SAVE_INT32_L(w, p)	(*((INT32 *)(p)) = INT32_REVERSE(w))

#define		LOAD_INT64_B(w, p)	((w) = *((INT64 *)(p)))
#define		SAVE_INT64_B(w, p)	(*((INT64 *)(p)) = (w))

#endif

#if		defined(INLINE_NOT_SUPPORTED)
#define		inline	
#endif

#define		FUNCTION_PREFIX		EXTERN_C DLL_EXPORT

/* assuming int is of 32 bits */
typedef struct MD5_IR_tag {
    unsigned int	a, b, c, d;
    unsigned int	len_h, len_l;
    int				head;
    unsigned char	q[64];
} HashMD5;

FUNCTION_PREFIX
void 	MD5_init(HashMD5 *pMD5);
FUNCTION_PREFIX
void	MD5_update(HashMD5 *pMD5, unsigned char *p, unsigned int n);
FUNCTION_PREFIX
void	MD5_end(HashMD5 *pMD5, unsigned char *phv);

unsigned int	ComputeCanary(const BYTE *in, int nb);

#endif
