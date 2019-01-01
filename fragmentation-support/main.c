/*
 * Copyright (C) 2018 Zhiyi Zhang, Tianyuan Yu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include "ndn-lite/encode/fragmentation-support.h"
#include "shell.h"
#include "msg.h"

static uint8_t payload[] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

int main(void)
{
  // tests start

  ndn_fragmenter_t fragmenter;
  ndn_fragmenter_init(&fragmenter, payload, sizeof(payload), 16, 123);
  printf("total frag pkt num: %d\n", fragmenter.total_frag_num);

  uint8_t original[200];
  ndn_frag_assembler_t assembler;
  ndn_frag_assembler_init(&assembler, original, 200);

  uint8_t frag[16] = {0};
  while (fragmenter.counter < fragmenter.total_frag_num) {
    ndn_fragmenter_fragment(&fragmenter, frag);
    printf("fragmented pkt: \n");
    for (int i = 0; i < 16; i++) {
      printf("%d ", frag[i]);
    }
    printf("\n");

    ndn_frag_assembler_assemble_frag(&assembler, frag, 16);
  }

  printf("after assembling pkt: \n");
  printf("is finished?: %d\n", assembler.is_finished);
  printf("offset?: %d\n", assembler.offset);
  for (uint32_t i = 0; i < assembler.offset; i++) {
    printf("%d ", original[i]);
  }
  printf("\n");

  return 0;
}