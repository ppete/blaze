/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/**********************************************************************************************/
/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman, Worcester, UK.   All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 01/08/2005

 PP: 27/06/23 Updated include files and types to match C/C++ standard type names.
*/

#ifndef _SHA1_H
#define _SHA1_H

#include <cstdlib>
#include <cstdint>
#include "brg_types.h"

#define SHA1_BLOCK_SIZE  64
#define SHA1_DIGEST_SIZE 20

#if defined(__cplusplus)
extern "C"
{
#endif

/** BEGIN: UTS RNG Harness **/

#define POS_MASK    0x7fffffff
#define HIGH_BITS   0x80000000

#define sha1_context sha1_ctx_s
typedef std::uint8_t RNG_state;
typedef std::uint32_t uint32;
//typedef char *   caddr_t;

/**********************************/
/* random number generator state  */
/**********************************/
struct state_t {
  std::uint8_t state[20];
};


/***************************************/
/* random number generator operations  */
/***************************************/
void   rng_init(RNG_state *state, int seed);
void   rng_spawn(RNG_state *mystate, RNG_state *newstate, int spawnNumber);
int    rng_rand(RNG_state *mystate);
int    rng_nextrand(RNG_state *mystate);
char * rng_showstate(RNG_state *state, char *s);
void   rng_showtype( void );

/** END: UTS RNG Harness **/
/* type to hold the SHA256 context  */

struct sha1_ctx_s
{ std::uint32_t count[2];
  std::uint32_t hash[5];
  std::uint32_t wbuf[16];
};

typedef struct sha1_ctx_s sha1_ctx;

/* Note that these prototypes are the same for both bit and */
/* byte oriented implementations. However the length fields */
/* are in bytes or bits as appropriate for the version used */
/* and bit sequences are input as arrays of bytes in which  */
/* bit sequences run from the most to the least significant */
/* end of each byte                                         */

VOID_RETURN sha1_compile(sha1_ctx ctx[1]);

VOID_RETURN sha1_begin(sha1_ctx ctx[1]);
VOID_RETURN sha1_hash(const unsigned char data[], unsigned long len, sha1_ctx ctx[1]);
VOID_RETURN sha1_end(unsigned char hval[], sha1_ctx ctx[1]);
VOID_RETURN sha1(unsigned char hval[], const unsigned char data[], unsigned long len);

#if defined(__cplusplus)
}
#endif

#endif
