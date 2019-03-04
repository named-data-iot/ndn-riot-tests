
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
#include "ndn-lite/ndn-constants.h"
#include <string.h>

static const char *_current_test_name;
static bool _all_function_calls_succeeded = true;

void _run_util_test(util_test_t *test);

bool run_util_tests(void) {
  memset(util_test_results, 0, sizeof(bool)*UTIL_NUM_TESTS);
  for (int i = 0; i < UTIL_NUM_TESTS; i++) {
    _run_util_test(&util_tests[i]);
  }
  
  return check_all_tests_passed(util_test_results, util_test_names,
                                UTIL_NUM_TESTS);
}

void _run_util_test(util_test_t *test) {
  
  _current_test_name = test->test_names[test->test_name_index];
  _all_function_calls_succeeded = true;

  uint8_t pool[NDN_MEMORY_POOL_RESERVE_SIZE(6, 3)];
  uint8_t *ptr[4];

  ndn_memory_pool_init(pool, 6, 3);
label_try:
  ptr[0] = ndn_memory_pool_alloc(pool);
  if(!ptr[0]){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  ptr[1] = ndn_memory_pool_alloc(pool);
  if(!ptr[0]){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  ptr[2] = ndn_memory_pool_alloc(pool);
  if(!ptr[0]){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  ptr[3] = ndn_memory_pool_alloc(pool);
  if(ptr[3] != NULL){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  if(ndn_memory_pool_free(pool, ptr[1]) != 0){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  ptr[3] = ndn_memory_pool_alloc(pool);
  if(!ptr[3] || ptr[3] != ptr[1]){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  if(ndn_memory_pool_free(pool, ptr[2]) != 0){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  memset(ptr[0], 0x1F, 6);
  if(ndn_memory_pool_free(pool, ptr[0]) != 0){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  ptr[3][-1] = 0xFF;
  if(ndn_memory_pool_free(pool, ptr[0]) != -1){
    _all_function_calls_succeeded = false;
    goto label_finally;
  }
  
label_finally:
  if (_all_function_calls_succeeded)
  {
    *test->passed = true;
  }
  else {
    printf("In _run_util_test, something went wrong.\n");
    *test->passed = false;
  }
  
}
