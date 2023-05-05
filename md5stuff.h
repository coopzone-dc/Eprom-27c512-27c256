#ifndef _MD5STUFF_H
#define _MD5STUFF_H

/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security,
 * Inc. MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Written by Solar Designer <solar at openwall.com> in 2001, and placed
 * in the public domain.  There's absolutely no warranty.
 *
 * This differs from Colin Plumb's older public domain implementation in
 * that no 32-bit integer data type is required, there's no compile-time
 * endianness configuration, and the function prototypes match OpenSSL's.
 * The primary goals are portability and ease of use.
 *
 * This implementation is meant to be fast, but not as fast as possible.
 * Some known optimizations are not included to reduce source code size
 * and avoid compile-time configuration.
 */

/*
 * Updated by Scott MacVicar for arduino
 * <scott@macvicar.net>
 */

#include <string.h>

typedef unsigned long MD5_u32plus;

typedef struct {
  MD5_u32plus lo, hi;
  MD5_u32plus a, b, c, d;
  unsigned char buffer[64];
  MD5_u32plus block[16];
} MD5_CTX;

void make_digest(char *md5str, const unsigned char *digest, int len);
#define F(x, y, z)      ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)      ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)      ((x) ^ (y) ^ (z))
#define I(x, y, z)      ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
  (a) += f((b), (c), (d)) + (x) + (t); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
  (a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
# define SET(n) \
  (*(MD5_u32plus *)&ptr[(n) * 4])
# define GET(n) \
  SET(n)
#else
# define SET(n) \
  (ctx->block[(n)] = \
  (MD5_u32plus)ptr[(n) * 4] | \
  ((MD5_u32plus)ptr[(n) * 4 + 1] << 8) | \
  ((MD5_u32plus)ptr[(n) * 4 + 2] << 16) | \
  ((MD5_u32plus)ptr[(n) * 4 + 3] << 24))
# define GET(n) \
  (ctx->block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There are no alignment requirements.
 */
static const void *body(void *ctxBuf, const void *data, size_t size);

void MD5Init(void *ctxBuf);

void MD5Update(void *ctxBuf, const void *data, size_t size);

void MD5Final(unsigned char *result, void *ctxBuf);

// result must point to a 33 byte string
void do_md5(char *arg, int len,char *result);



#endif
