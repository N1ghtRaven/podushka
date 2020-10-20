#include "pdu.h"

#include <stdlib.h>
#include <stdio.h>

pdu_serialize_status serialize_submit_pocket(submit_pdu_pocket *pdu_pocket, uint8_t *output, size_t *size)
{
    
}

/**
 * TODO Покрыть тестами
 * Энкодер GSM 7-ми битной строки
 * @param input входная строка
 * @param size размер входной строки
 * @param output закодированная строка
 * @return размер выходной строки
 */
// size_t gsm_encode_7bit(uint8_t *input, size_t size, uint8_t *output)
// {
//     return 0;
// }



/**
 * Энкодер UCS2 строки
 * @param input входная строка
 * @param size размер входной строки
 * @param output закодированная строка
 * @return размер выходной строки
 */
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
        for (uint8_t i2 = 0; i2 < c_size; i++)
        {
            symbol_b[i2] = input[i + i2];
        }
    
        uint64_t char_code = symbol_b[0];
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
        }
    
        strcat(output, (char_code & 0xFF00) >> 8);
        strcat(output, char_code & 0xFF);
        output_size += 2;

        i += c_size - 1;
    }

    return output_size;
}


pdu_package_status package_submit_pocket(submit_pocket *pocket, submit_pdu_pocket *pdu_pocket)
{
    // // Clear pocket struct
    // memset(pdu_pocket, 0, sizeof(submit_pdu_pocket));

    // pdu_pocket->sca = 0x00; // If set 0x00, sca get from SIM
    // pdu_pocket->pdu_type = DEFAULT_PDU_TYPE;
    // pdu_pocket->mr = 0x00;

    // // Destination address
    // pdu_pocket->da.size = pocket->dest_addr.size;
    // pdu_pocket->da.type = 0x91; // International; 0x81 - Native 
    
    // uint8_t buffer_size = pocket->dest_addr.size;
    // uint8_t buffer[OA_MAX_LEN] = {0};
    // strncpy(buffer, pocket->dest_addr.addr, buffer_size);

    // // is odd
    // if (buffer_size % 2)
    // {
    //     buffer[buffer_size++] = 'F';
    // }

    // switch_endianness(buffer, buffer_size, pdu_pocket->da.data);

    // pdu_pocket->pid = 0x00;
    // pdu_pocket->dcs = 0x08; // TODO: Implement me

    // memset(pdu_pocket->vp, 0, sizeof(uint8_t) * 7); // TODO: Implement me

    
}