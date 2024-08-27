/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "fragment.h"
#include <stdlib.h>
#include <string.h>

#include <list>
#include <vector>
#include <CMutex.h>
#include <thread>
#include <iostream>
#include <memory>

namespace ufmt::EVB {
bool debug=false;
bool threadsafe=true;   // By default threadsafe.
/**
 * The following data structures are used to re-use both fragment headers
 * (easy) and their bodies.  This is done because profiling showed that
 * 
 *  almost 50% of event builder time was spent in dynamic memory allocation
 *  Here's the idea:
 *    Allocation of fragment headers is done by first checking to see if the
 *    fragmentHeaderPool list has a header.  If so, the front of that list is
 *    given to the caller.  If not, a new one is allocated with malloc.
 *    When freed, a fragment header goes unconditionally back to the back of the
 *    fragmentHeaderPool.
 *
 *    Fragment bodies are a bit trickier as they are of variable size.  We therefore
 *    have several fragment body pools.  Each contains zero or more fixed length
 *    power of two fragments (e.g.  2, 4, 8, 16, 32... bytes).  The most significant
 *    set bit (+1) of the size of the requested block is used to select a pool.
 *    After that the algorithm is the same as for fragment header... but
 *    fixed sized blocks appropriate to the pool are used.
 *
 *    Similarly on free, the size of the block is used to determine the
 *    pool into which the block is returned.  The set of pools itself is
 *    resized as needed to ensure we have sufficient pools to satsify the
 *    memory request so far.
 */ 
  
// The pool below is for fragment headers:

typedef std::vector<pFragment> FragmentHeaderPool;
typedef std::vector<void*>     FragmentBodyPool;

FragmentHeaderPool fragmentHeaderPool;

// The vector below contains the fragment body pools. The size of fragments
// from each pool is 2x the size of the prior pool.

std::vector<FragmentBodyPool> fragmentBodyPools;

CMutex poolProtector;             // So we're threadsafe.

/**
 *  resetFragmentPool
 *     For testing purposes - frees all elements of the header and body pools:
 */
void
resetFragmentPool()
{
  while(!fragmentHeaderPool.empty()) {
    free(fragmentHeaderPool.back());
    fragmentHeaderPool.pop_back();
  }
  while(!fragmentBodyPools.empty()) {
    FragmentBodyPool& pool(fragmentBodyPools.back());
    while(!pool.empty()) {
      free(pool.back());
      pool.pop_back();
    }
    fragmentBodyPools.pop_back();
  }
}

/**
 * getPoolNumber
 *    Return the index of the pool in fragmentBodyPools that will have
 *    the smallest storage region at least as big as the size passed in.
 *
 * @param size   - Number of bytes needed.
 * @return unsigned - index into fragmentBodyPools
 * @note there's no assurance the indicated element exists.  Callers should use
 *       getPool to obtain a reference to the indexed pool as that will enlarge
 *       the vecstor of pools if needed.
 *       
 */
static unsigned
getPoolNumber(unsigned size)
{
  // Yeah, this is O(n) for number of bits but it's not dependent on the
  // size of unsigned.  so we don't  use one of the O(log n) confusing algorithms.
  
  unsigned n = 0;               
  while (size) {
    size = size >> 1;
    n++;                   // This gives one bigger.
  }
  
  return n;
}
/**
 * getPool
 *    Given a pool number returns a reference to the pool -- even if we
 *    have to enlarge the fragmentBodyPools vector to do that.
 *
 * @param unsigned poolNo - pool number
 * @return FragmentBodyPool& - reference to the appropriate pool number.
 */
static FragmentBodyPool&
getPool(unsigned poolNo)
{
    
   static std::vector<void*> emptyList;           // Used to expand the pools. 

    while (poolNo >= fragmentBodyPools.size()) {
      
      fragmentBodyPools.push_back(emptyList);
    }
    try {
      return fragmentBodyPools.at(poolNo);
    }
    catch (...) {
      std::cerr << "getPool - no fragmentBody Pool for " << poolNo << std::endl;
      std::cerr.flush();
      throw;               // Likely crashes us.
    }
}
/**
 * poolSize
 *   Returns the size to use when allocating elements for this pool.
 *
 * @param poolNo - Pool number.
 * @return unsigned - number of bytes of blocks in each pool.
 */
static unsigned
poolSize(unsigned poolNo)
{
  return (1 << poolNo);
}
/**
 * getFragmentDescription [static] - get a new fragment header
 *
 * @return pFragment
 */
static pFragment
getFragmentDescription()
{
  pFragment result(0);
  if (!fragmentHeaderPool.empty()) {
    result = fragmentHeaderPool.back();
    fragmentHeaderPool.pop_back();
  } else {
    result = static_cast<pFragment>(malloc(sizeof(Fragment)));
  }
  return result;
}

/**
 * freeFragmentDescription
 *    Releses a fragment description by putting it on the back of the fragmentHeaderPool
 */
static void
freeFragmentHeader(pFragment pHeader)
{
  fragmentHeaderPool.push_back(pHeader);
}

/**
 * getFragmentBody
 *    Return a pointer to storage that's at least large enough to hold the
 *    specified body size.
 * @param bytes - number of bytes required.
 * @return void* - note that the storage pointed to is _at least_ bytes bit.
 *                 could be larger.
 */
static void*
getFragmentBody(size_t bytes)
{
  void* result(nullptr);
  
  int poolNo = getPoolNumber(bytes);
  FragmentBodyPool& pool(getPool(poolNo));          // Makes more pools if needed.
  
  if (pool.empty()) {
    if (debug) std::cerr << std::this_thread::get_id() << " pool " << poolNo << " empty\n";
    result = malloc(poolSize(poolNo));
  } else {
    if (debug) std::cerr << std::this_thread::get_id() << " Satsified body from pool\n";
    result = pool.back();
    pool.pop_back();
  }
  if (debug) std::cerr.flush();
  return result;
}

/**
 * freeFragmentBody
 *    Given a fragment header returns the fragment body to the appropriate
 *    pool.
 *
 * @param pFrag  - Fragment header pointer.
 */
static void
freeFragmentBody(pFragment pFrag)
{
  unsigned poolNo = getPoolNumber(pFrag->s_header.s_size);
  FragmentBodyPool& pool(getPool(poolNo));
  pool.push_back(pFrag->s_pBody);
}
/**
 * Free a fragment.  The assumption is that  both the header and the body 
 * are dynamically allocated.
 *
 * @param p - pointer to the fragment.
 */

extern "C" {
void freeFragment(pFragment p) 
{
  std::unique_ptr<CriticalSection> l;
  if (threadsafe) l.reset(new CriticalSection(poolProtector));
  freeFragmentBody(p);
  p->s_pBody = 0;
  freeFragmentHeader(p);

}
}
/**
 * Create a new dynamically allocated fragment from an existing
 * header that has been filled in .k
 * - Ther etruned fragment can be freed via freeFragment.
 * - We need the s_size field filled in because that's what determines
 *  the full size of the storage.
 *
 * @para pHeader - Pointer to the fragment header.
 *
 *  TODO: malloc failure handling.
 */
extern "C" {
pFragment allocateFragment(const FragmentHeader* pHeader)
{
  std::unique_ptr<CriticalSection> l;
  if (threadsafe) l.reset(new CriticalSection(poolProtector));
  pFragment p = getFragmentDescription();
  memcpy(&(p->s_header), pHeader, sizeof(FragmentHeader));

  p->s_pBody = getFragmentBody(pHeader->s_size);

  return p;
}
}
/**
 * Create a new dynamically allocated fragment from 
 * the parameters that go in the header. The body is
 * allocated but nothing put in it.
 *
 * @param timestamp - The fragment timestamp.
 * @param sourceId  - The fragment source id.
 * @param size      - The size of the fragment body.
 */
extern "C" {
pFragment newFragment(uint64_t timestamp, uint32_t sourceId, uint32_t size)
{
  FragmentHeader h = {timestamp, sourceId, size, BARRIER_NOTBARRIER};
  return allocateFragment(&h);
}
}
/**
 * Determine the total number of bytes needed to flatten a fragment chain out into
 * memory (e.g. a message payload).
 *
 * @param p - pointer to the fragment chain.
 */
extern"C" {
size_t
fragmentChainLength(pFragmentChain p)
{
  size_t result = 0;
  while (p) {
    result += sizeof(FragmentHeader) + p->s_pFragment->s_header.s_size;
    p = p->s_pNext;
  }

  return result;
}
}
}

