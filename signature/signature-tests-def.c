
/*
 * Copyright (C) 2018 Zhiyi Zhang, Tianyuan Yu, Edward Lu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "signature-tests-def.h"

#include "ndn-lite/ndn-enums.h"

#include <stdbool.h>

char *signature_test_names[SIGNATURE_NUM_TESTS] = {
  "test_signature",
};

bool signature_test_results[SIGNATURE_NUM_TESTS];

signature_test_t signature_tests[SIGNATURE_NUM_TESTS] = {
    {
      signature_test_names,
      0,
      NDN_ECDSA_CURVE_SECP256R1,
      dummy_signature_1,
      sizeof(dummy_signature_1),
      &signature_test_results[0]
    },
};

const uint8_t dummy_signature_1[] = {
  0xB2, 0xFC, 0x62, 0x14, 0x78, 0xDC, 0x10, 0xEA,
  0x61, 0x42, 0xB9, 0x34, 0x67, 0xE6, 0xDD, 0xE3,
  0x3D, 0x35, 0xAA, 0x5B, 0xA4, 0x24, 0x6C, 0xD4,
  0xB4, 0xED, 0xD8, 0xA4, 0x59, 0xA7, 0x32, 0x12,
  0x57, 0x37, 0x90, 0x5D, 0xED, 0x37, 0xC8, 0xE8,
  0x6A, 0x81, 0xE5, 0x8F, 0xBE, 0x6B, 0xD3, 0x27,
  0x20, 0xBB, 0x16, 0x2A, 0xD3, 0x2F, 0xB5, 0x11,
  0x1B, 0xD1, 0xAF, 0x76, 0xDB, 0xAD, 0xB8, 0xCE
};
