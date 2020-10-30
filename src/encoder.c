#include "encoder.h"


size_t serialize_submit_pocket(submit_pdu_pocket *pdu_pocket, uint8_t *output, size_t *size)
{
    memset(output, 0, sizeof(uint8_t) * SUBMIT_PDU_MAX_SIZE);
    return sprintf(
            output,
            "%02x%02x%02x%02x%02x%s%02x%02x%02x%02x%s\0",
            pdu_pocket->sca,
            pdu_pocket->pdu_type,
            pdu_pocket->mr,
            pdu_pocket->da.size,
            pdu_pocket->da.type,
            pdu_pocket->da.data,
            pdu_pocket->pid,
            pdu_pocket->dcs,
            pdu_pocket->vp,
            pdu_pocket->udl,
            pdu_pocket->ud
        );
}

/**
 * https://github.com/vbs100/gsm7bit/blob/master/gsm7bit.c
 * https://github.com/RiccardoSottini/7bit_encoding
 * TODO Покрыть тестами
 * Энкодер GSM 7-ми битной строки
 * @param input входная ascii строка
 * @param size размер ascii строки
 * @param output gsm7bit строка
 * @return размер gsm7bit строки
 */
uint8_t gsm_encode_7bit(uint8_t *input, size_t size, uint8_t *output)
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
            //TODO: Some error
            return 0;
        }

        result |= ((input[i] & 0x3F) << (6 * (size - 1 - i)));
    }

    return result;
}


/**
 * https://github.com/smoothwind/ucs2-utf8
 * TODO: Покрыть тестами
 * Кодировщик UCS2 -> UTF8 строки
 * @param input входная UCS2 строка
 * @param size размер входной строки
 * @param output UTF8 строка
 * @return размер выходной строки
 */
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
            output_size += sprintf(c, "%02x%02x", (code & 0xFF00) >> 8, code & 0xFF);
            strcat(output, c);
        }
        k += c_size - 1;
    }
    
    return ++output_size;
}


pdu_package_status package_submit_pocket(submit_pocket *pocket, submit_pdu_pocket *pdu_pocket)
{
    // Clear pocket struct
    memset(pdu_pocket, 0, sizeof(submit_pdu_pocket));

    pdu_pocket->sca = 0x00; // get from SIM
    pdu_pocket->pdu_type = DEFAULT_PDU_TYPE;
    pdu_pocket->mr = 0x00;

    // Destination address
    pdu_pocket->da.size = pocket->destination.size;
    pdu_pocket->da.type = pocket->destination.type; 
    
    uint8_t buffer_size = pocket->destination.size;
    uint8_t buffer[DA_MAX_LEN] = {0};
    strncpy(buffer, pocket->destination.addr, buffer_size);

    // is odd
    if (buffer_size % 2)
    {
        buffer[buffer_size++] = 'F';
    }

    switch_endianness(buffer, buffer_size, pdu_pocket->da.data); //?&

    pdu_pocket->pid = 0x00;
    pdu_pocket->dcs = pocket->message.mdcs;
    
    switch (pdu_pocket->dcs)
    {
        case MDCS_7_BIT:
            pdu_pocket->udl = gsm_encode_7bit(pocket->message.data, pocket->message.size, pdu_pocket->ud);
            break;
        case MDCS_UCS2:
            pdu_pocket->udl = gsm_encode_UCS2(pocket->message.data, pocket->message.size, pdu_pocket->ud);
            break;
        default:
            return WRONG_DATA_SCHEME;              
    }

    memset(&pdu_pocket->vp, 0, sizeof(uint8_t) * 7); // TODO: Extract from pdu type
    switch (pocket->ttl.scale)
    {
        case MINUTE:
            // MAX 12 hour min 5 min
            pdu_pocket->vp = (pocket->ttl.value / 5) - 1;
            break;
        case HOUR:
            // MIN 12 hour max 24
            pdu_pocket->vp = ((pocket->ttl.value * 60 - 720) / 30) + 143;
            break;
        case DAY:
            // Min 1 day max 30
            pdu_pocket->vp = pocket->ttl.value + 166;
            break;
        case WEEK:
            // Min 1 week max 63
            pdu_pocket->vp = pocket->ttl.value + 192;
            break;
        default:
            // 5 min
            pdu_pocket->vp = 0;
            return WRONG_VP_SCALE; 
    }

    return NO_ERROR; 
}