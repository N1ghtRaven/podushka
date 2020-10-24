#include "pdu.h"


uint16_t size_from_char(uint8_t b)
{
    if (b < 128)
    {
        return 1;
    }

    for (uint8_t i = 1; i <= 7; i++)
    {
        if (((b << i) & 0xFF) >> 7 == 0)
        {
            return i;
        }
    }

    return 1;
}

uint16_t ascii_to_num(uint8_t *input, uint16_t size)
{
    if (size == 1)
    {
        return input[0]; 
    } 

    uint16_t result = (input[0] & (0xFF >> (size + 1))) << (6 * (size - 1)); 
    for (uint16_t i = 1; i < size; i++)
    {
        if ((input[i] >> 6) != 2)
        {
            // Some error
            return 0;
        }

        result |= ((input[i] & 0x3F) << (6 * (size - 1 - i)));
    }

    return result;
}


size_t gsm_encode_UCS2(char * input, size_t size, uint8_t *output)
{
    memset(output, '\0', sizeof(uint8_t) * (size * 2));

    size_t output_size = 0;
    char c[4] = {0};
    for (uint8_t k = 0; k < size; k++)
    {
        uint16_t c_size = size_from_char(input[k]);
        uint8_t ascii[c_size + 1];
        for (uint8_t i = 0; i < c_size; i++)
        {
            ascii[i] = input[k + i];
        }
        ascii[c_size] = '\0';

        uint16_t code = ascii_to_num(ascii, c_size);
        if (code > 0)
        {
            sprintf(c, "%02x%02x", (code & 0xFF00) >> 8, code & 0xFF);
            strcat(output, c);
            output_size += 4;
        }
        k += c_size - 1;
    }
    
    return ++output_size;
}


/**
 * Переводит число в символ таблицы ASCII
 * @param num число
 * @return символ таблицы ASCII
 */
uint8_t num_to_ascii(uint8_t num)
{
    if ((num >= 0x30) && (num <= 0x39)) 
    {
        return num - 0x30;
    }
    else if ((num >= 'A') && (num <= 'F'))
    {
        return num - 'A' + 10;
    }   
    else
    {
        return 0;
    }
}

// https://github.com/chenxh89/gsm7bit/blob/master/gsm7bit.c
size_t gsm_encode_7bit(uint8_t *input, size_t size, uint8_t *output)
{
    uint8_t buffer[(size * 7) / 8];
    for(uint8_t i = 0; i < size * 7; i++)
	{
		uint8_t b = (input[i / 7] & (1 << i % 7)) == (1 << i % 7);
        buffer[i / 8] |= (b << i % 8);
	}

    char c[2] = {0};
    for(uint8_t i = 0; i < (size * 7) / 8; i++)
    {
        sprintf(c, "%02x", buffer[i]);
        strcat(output, c);
    }

    return ((size * 7) / 8);
}

void main(void)
{
    int i;
    char test_msg[] = "diafaan.com";
    uint8_t result[128];
    memset(result,0, sizeof(result));

    size_t size = gsm_encode_7bit(test_msg, 12, result);
	// printf("7bit 2:");
	// for(i=0;i<size;i++)
	// printf(" %02x", result[i]);
    printf("%s", result);
	// printf("\n");
		
	// printf("ascii 1:");
	// for(i=0;i<12;i++)
	// 	printf(" %02x", test_msg[i]);
	// printf("\n");
}

// void main(void)
// {
//     uint8_t test[213] = "041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438"; 

//     char  test_msg[13] = "Привет"; // 13
//     uint8_t test_msg2[500];

//     // gsm_decode_UCS2(test, 213, test_msg2);
//     // gsm_encode_UCS2(&test_msg, 13, test_msg2);
//     gsm_encode_UCS2(test_msg, 13, test_msg2);
//     printf(test_msg2);
// }