#include "util.h"

size_t switch_endianness(uint8_t *input_chain, size_t len, uint8_t *output_chain)
{
    if (len < 2)
    {
        return 0;
    }

    memset(output_chain, len, sizeof(uint8_t));
    for (size_t i = 0; i < len; i += 2)
    {
        output_chain[i] = input_chain[i + 1];
        output_chain[i + 1] = input_chain[i];
    }

    return len;
}
