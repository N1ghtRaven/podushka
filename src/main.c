#include "pdu.h"

size_t gsm_encode_UCS2(uint8_t *input, size_t size, uint8_t *output)
{
    size_t output_size = 0;

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t c = input[i];
        uint8_t c_size = 1;

        // get char size
        if (c > 128)
        {
            for (uint8_t i2 = 1; i2 <= 7; i2++)
            {
                if ( ((c << i2) & 0xFF) >> 7 == 0 )
                {
                    c_size = i2;
                }
            }
        }

        uint8_t symbol_b[c_size + 1];
        memset(symbol_b, 0, (c_size + 1) * sizeof(uint8_t));
        for (uint8_t i2 = 0; i2 < c_size; i2++)
        {
            symbol_b[i2] = input[i + i2];
        }
    
        uint8_t char_code = symbol_b[0];
        if (c_size > 1)
        {
            uint8_t actual_char = char_code & (0xFF >> (c_size + 1));
            actual_char = actual_char << (6 * (c_size - 1));

            for (uint8_t i2 = 1; i2 < c_size; i2++)
            {
                char_code = symbol_b[i2];
                if ((char_code >> 6) != 2)
                {
                    // TODO: ERROR
                }
                actual_char |= ((char_code & 0x3F) << (6 * (c_size - 1 - i)));
            }

            output[output_size++] = (char_code & 0xFF00) >> 8;
        }
        output[output_size++] = char_code & 0xFF;

        i += c_size - 1;
    }

    return output_size;
}

void main(void)
{
    int8_t test_msg[60] = "Привет"; // 13
    uint8_t test_msg2[50];

    gsm_encode_UCS2(&test_msg, 6, &test_msg2);
    printf(test_msg2);
}