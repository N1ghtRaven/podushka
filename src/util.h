#ifndef _UTIL_h_
#define _UTIL_h_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Меняет порядок байтов в цепочке
 * @param input_chain исходная цепочка байт
 * @param len длина цепочки
 * @param output_chain цепочка байт с измененным порядком
 * @return размер измененной цепочки
 */
size_t switch_endianness(uint8_t *input_chain, size_t len, uint8_t *output_chain);

#endif