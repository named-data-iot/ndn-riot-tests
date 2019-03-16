
/*
 * Copyright (C) 2019 Xinyu Ma
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "util-tests.h"

#include <stdio.h>

#include "util-tests-def.h"
#include "../print-helpers.h"
#include "../test-helpers.h"

#include <stdio.h>
#include "ndn-lite/util/memory-pool.h"
#include "ndn-lite/util/msg-queue.h"
#include "ndn-lite/ndn-constants.h"
#include <string.h>

static const char *_current_test_name;
static bool _all_function_calls_succeeded = true;

#define test_assert(cond) if(!(cond)) return false

void _run_util_test(util_test_t *test);

bool run_util_tests(void) {
  memset(util_test_results, 0, sizeof(bool)*UTIL_NUM_TESTS);
  for (int i = 0; i < UTIL_NUM_TESTS; i++) {
    _run_util_test(&util_tests[i]);
  }
  
  return check_all_tests_passed(util_test_results, util_test_names,
                                UTIL_NUM_TESTS);
}

bool _run_memory_pool_test(){
  uint8_t pool[NDN_MEMORY_POOL_RESERVE_SIZE(6, 3)];
  uint8_t *ptr[4];

  ndn_memory_pool_init(pool, 6, 3);
  ptr[0] = ndn_memory_pool_alloc(pool);
  test_assert(ptr[0]);
  ptr[1] = ndn_memory_pool_alloc(pool);
  test_assert(ptr[1]);
  ptr[2] = ndn_memory_pool_alloc(pool);
  test_assert(ptr[2]);
  ptr[3] = ndn_memory_pool_alloc(pool);
  test_assert(ptr[3] == NULL);
  test_assert(ndn_memory_pool_free(pool, ptr[1]) == 0);
  ptr[3] = ndn_memory_pool_alloc(pool);
  test_assert(ptr[3] && ptr[3] == ptr[1]);
  test_assert(ndn_memory_pool_free(pool, ptr[2]) == 0);
  memset(ptr[0], 0x1F, 6);
  test_assert(ndn_memory_pool_free(pool, ptr[0]) == 0);
  ptr[3][-1] = 0xFF;
  test_assert(ndn_memory_pool_free(pool, ptr[0]) == -1);
  return true;
}

typedef struct ndn_msg{
  void* obj;
  ndn_msg_callback func;
  size_t length;
  uint8_t param[];
} ndn_msg_t_back;
char buf[8192];
bool ret;
uint32_t ulret = 0;

void msgproc(void *self, size_t param_length, void *param) {
  if(strlen((char*)param) != param_length){
    ret = false;
    return;
  }
  if((bool*)self != &ret){
    ret = false;
    return;
  }
  ret = (strcmp((char*)param, "TEST 1\n") == 0);
}

void dummy_msgproc(void *self, size_t param_length, void *param){
  ret = true;
}

void add_msgproc(void *self, size_t param_length, void *param){
  ulret += *(uint32_t*)param;
  ndn_msgqueue_post(self, add_msgproc, sizeof(uint32_t), &ulret);
}

bool _run_msg_queue_test(){
  bool ret = true;
  size_t sz = sizeof(ndn_msg_t_back);

  // Empty logic
  ndn_msgqueue_init();
  ret = ndn_msgqueue_empty();
  test_assert(ret == true);
  ret = ndn_msgqueue_dispatch();
  test_assert(ret == false);
  ret = ndn_msgqueue_empty();
  test_assert(ret == true);
  
  // Post, dispatch
  strcpy(buf, "TEST 1\n");
  ret = ndn_msgqueue_post(&ret, msgproc, strlen(buf), buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_empty();
  test_assert(ret == false);
  ret = false;
  test_assert(ndn_msgqueue_dispatch() == true);
  test_assert(ret == true);
  ret = ndn_msgqueue_empty();
  test_assert(ret == true);
  
  // Rewind
  test_assert(NDN_MSGQUEUE_SIZE == 4096);
  ndn_msgqueue_init();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 1000, buf);
  test_assert(ret == false);
  ret = false;
  test_assert(ndn_msgqueue_dispatch() == true);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 1000, buf);
  test_assert(ret == true);
  ret = false;
  test_assert(ndn_msgqueue_dispatch() == true);
  test_assert(ret == true);
  ret = false;
  test_assert(ndn_msgqueue_dispatch() == true);
  test_assert(ret == true);
  ret = false;
  test_assert(ndn_msgqueue_dispatch() == false);
  test_assert(ret == false);
  
  // Not empty after post
  ndn_msgqueue_init();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000 - sz, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000 - sz, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 96 - sz, buf);
  test_assert(ret == false);
  ndn_msgqueue_dispatch();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000 - sz, buf);
  test_assert(ret == false);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 96 - sz, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2000 - sz, buf);
  test_assert(ret == false);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 96 - sz, buf);
  test_assert(ret == true);
  
  // Empty defrag
  ndn_msgqueue_init();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 1000 - sz, buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 2096 - sz, buf);
  test_assert(ret == true);
  ndn_msgqueue_dispatch();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 1500 - sz, buf);
  test_assert(ret == false);
  ndn_msgqueue_dispatch();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 4095 - sz, buf);
  test_assert(ret == true);

  // Process one round
  ndn_msgqueue_init();
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 0, NULL);
  test_assert(ret == true);
  *(uint32_t*)buf = 1;
  ret = ndn_msgqueue_post(&ret, add_msgproc, sizeof(uint32_t), buf);
  test_assert(ret == true);
  ret = ndn_msgqueue_post(&ret, dummy_msgproc, 0, NULL);
  test_assert(ret == true);
  ret = false;
  ndn_msgqueue_process();
  test_assert(ret == true);
  test_assert(ulret == 1);
  ndn_msgqueue_process();
  test_assert(ulret == 2);
  ndn_msgqueue_process();
  test_assert(ulret == 4);
  ndn_msgqueue_process();
  test_assert(ulret == 8);

  return true;
}

void _run_util_test(util_test_t *test) {
  
  _current_test_name = test->test_names[test->test_name_index];
  _all_function_calls_succeeded = true;

  _all_function_calls_succeeded = (_all_function_calls_succeeded && _run_memory_pool_test());

  if (_all_function_calls_succeeded)
  {
    *test->passed = true;
  }
  else {
    printf("In _run_util_test, something went wrong.\n");
    *test->passed = false;
  }

}
