#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <functional>
#include <list>

#ifndef WITHOUT_GC
  #define GC_THREADS 1
  #define GC_DEBUG 1
  #include <gc/gc.h>

  #include "gc-cxx11/gc_cxx11.hpp" // use GC allocator modified to work with C++11
#endif /* WITHOUT_GC */

#include "stack.hpp"


namespace lf = lockfree;
namespace fg = locking;

//
// choose manager

#if defined TEST_NO_MANAGER

template <class T>
using default_alloc = lf::just_alloc<T>;

#elif defined TEST_GC_MANAGER

template <class T>
using default_alloc = lf::gc_manager<T, gc_allocator_cxx11>;

#elif defined TEST_EPOCH_MANAGER

template <class T>
using default_alloc = lf::epoch_manager<T>;

#elif defined TEST_PUB_SCAN_MANAGER

template <class T>
using default_alloc = lf::pub_scan_manager<T>;

#else /* undefined MANAGER */

  #error "preprocessor define for memory manager is needed (TEST_JUST_ALLOC, TEST_GC_MANAGER, TEST_EPOCH_MANAGER, TEST_PUB_SCAN_MANAGER)"

#endif /* TEST_NO_MANAGER */


//
// choose stack implementation

#if defined TEST_LOCKFREE_STACK

template <class T>
using stack = lf::stack<T, default_alloc<T> >;

#else

 #error "preprocessor define for container class is needed (TEST_LOCKING_SKIPLIST, TEST_LOCKFREE_SKIPLIST, HTM_ENABLED)"

#endif

typedef stack<int> container_type;

static inline
void fail()
{
  throw std::logic_error("error");
}

template <class T>
static inline
bool success(const std::pair<T, bool>& res)
{
  return res.second;
}


void producer(container_type& stack, int count)
{
  gc_cxx_thread_context gc_guard;

  for (int i = count; i > 0; --i)
    stack.push(i);
}

void consumer(container_type& stack, int count)
{
  gc_cxx_thread_context gc_guard;

  while (count)
  {
    auto result = stack.pop();

    if (result.second) --count;
  }
}

typedef std::chrono::time_point<std::chrono::system_clock> time_point;
static time_point            starttime;

void parallel_test(const size_t cntoper, const size_t cntthreads)
{
  std::cout << std::endl;

  container_type          stack;
  std::list<std::thread>  exp_threads;

  //~ waiting_threads = cntthreads;

  starttime = std::chrono::system_clock::now();

  // spawn
  for (size_t i = 0; i < cntthreads; ++i)
  {
    exp_threads.emplace_back( (i % 2) ? consumer : producer,
                              std::ref(stack),
                              cntoper / cntthreads
                            );
  }

  // join
  for (std::thread& thr : exp_threads) thr.join();

  time_point     endtime = std::chrono::system_clock::now();
  int            elapsedtime = std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count();

  std::cout << "elapsed time = " << elapsedtime << "ms" << std::endl;

  if (((cntthreads % 2) == 0) == success(stack.pop()))
  {
    std::cerr << "expected " << ((cntthreads % 2)? "a non-" : "an ") << "empty stack"
              << std::endl;
    fail();
  }

  std::cerr << elapsedtime << std::endl;
}


template <class T>
static inline
size_t asNum(const T& t)
{
  size_t            res = 0;
  std::stringstream str;

  str << t;
  str >> res;

  return res;
}


int main(int argc, char** args)
{
#ifndef WITHOUT_GC
  GC_INIT();
  GC_allow_register_threads();
  // GC_find_leak = 1;
#endif /* WITHOUT_GC */

  size_t pnoiter = 40000000;
  size_t num_threads = 6;
  size_t num_runs = 1;

  if (argc > 1) pnoiter = asNum(*(args+1));
  if (argc > 2) num_threads = asNum(*(args+2));
  if (argc > 3) num_runs = asNum(*(args+3));

  // sequential_test(pnoiter);

  try
  {
    std::cout << "*** stack test " << pnoiter << "<ops  threads>" << num_threads << std::endl;
    std::cout << typeid(container_type).name() << std::endl;

    for (size_t i = 0; i < num_runs; ++i)
    {
      std::cout << "***** ***** ***** restart ***** " << i << std::endl;
      parallel_test(pnoiter, num_threads);
    }

    //~ std::cerr <<"Average time: "<<total_time/num_runs<<std::endl;
    std::cout << std::endl;
  }
  catch(...)
  {
    std::cout << "Error caught..." << std::endl;
  }

  return 0;
}