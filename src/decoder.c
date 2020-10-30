#include "decoder.h"

pdu_parse_status parse_deliver_pocket(uint8_t *hex, size_t size, deliver_pdu_pocket *pocket)
{
    if (size < PDU_MIN_LEN || size > PDU_MAX_LEN)
    {
        return WRONG_POCKET_SIZE;
    }

    // Clear pocket struct
    memset(pocket, 0, sizeof(deliver_pdu_pocket));

    uint8_t buffer[size];
    uint8_t frame = 0;

    //Parse TP-SCA
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->SCA.size = (uint8_t) strtol(buffer, NULL, 16);

    if (pocket->SCA.size < SCA_MIN_LEN || pocket->SCA.size > SCA_MAX_LEN)
    {
        return WRONG_SCA_SIZE;
    }

    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->SCA.type = (uint8_t) strtol(buffer, NULL, 16);

    strncpy(pocket->SCA.data, hex + frame, (pocket->SCA.size - 1) * 2);
    frame += (pocket->SCA.size - 1) * 2;  

    //Parse TP-MTI-CO
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->PDU_TYPE = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-OA
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->OA.size = (uint8_t) strtol(buffer, NULL, 16);

    if (pocket->OA.size < OA_MIN_LEN || pocket->OA.size > OA_MAX_LEN)
    {
        return WRONG_OA_SIZE;
    }

    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->OA.type = (uint8_t) strtol(buffer, NULL, 16);

    uint8_t real_oa_size = (pocket->OA.size % 2) > 0 ? pocket->OA.size + 1 : pocket->OA.size;
    strncpy(pocket->OA.data, hex + frame, real_oa_size);
    frame += real_oa_size;

    //Parse TP-PID
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->PID = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-DCS
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->DCS = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-SCTS
    strncpy(pocket->SCTS, hex + frame, SCTS_SIZE);
    frame += SCTS_SIZE;

    //Parse TP-UDL
    strncpy(buffer, hex + frame, 2);
    frame += 2;
    pocket->UDL = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-UD
    strncpy(pocket->UD, hex + frame, pocket->UDL * 2);
    frame += pocket->UDL * 2;

    if (pocket->UDL * 2 > UD_MAX_LEN)
    {
        return WRONG_UD_SIZE;
    }

    return NO_ERROR;
}



/**
 * Извлекает из ASCII символа (шеснадцатиричная СС) числовое значение
 * @param input входной байт в виде символа
 * @return числовое значание (-1, если данные ошибочны)
 */
int8_t num_from_ascii(char input)
{
    if ( input - '0' >= 0 && '9' - input >= 0 )
    {
        return input - '0';
    } else if ( input - 'a' >= 0 && 'f' - input >= 0 )
    {
        return input - 'a' + 10;
    } else if ( input - 'A' >= 0 && 'F' - input >= 0 )
    {
        return input - 'A' + 10;
    }
    else
    {
        return -1; // Wrong data 
    }
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

/**
 * Декодер GSM 7-ми битной строки
 * @param input входная закодированная строка
 * @param size размер входной строки
 * @param output раскодированная строка
 * @return размер выходной строки
 */
uint8_t gsm_decode_7bit(uint8_t *input, size_t size, uint8_t *output)
{
    uint8_t output_size = 0;

    uint16_t bit_buffer = 0;
    uint8_t bit_buffer_size = 0;
    while ( *input != '\0' ) 
    {
        // Convert char to num
        int16_t num = ( num_from_ascii(*(input++)) << 4 ) | num_from_ascii(*(input++));

        bit_buffer = (uint8_t) num << bit_buffer_size | bit_buffer;
        bit_buffer_size += 8;
        while ( bit_buffer_size >= 7)
        {
            output[output_size++] = ( bit_buffer & 0x7F );
            bit_buffer = bit_buffer >> 7;
            bit_buffer_size -= 7;
        }
    }
    
    return output_size;
}

/**
 * Декодер UCS2 строки
 * @param input входная строка
 * @param size размер входной строки
 * @param output раскодированная строка
 * @return размер выходной строки
 */
size_t gsm_decode_UCS2(uint8_t *input, size_t size, uint8_t *output)
{
    uint8_t c[5] = "";
    for (uint16_t i = 0; i < size - 3; i += 4)
    {
        // Получаем UNICODE-код символа из HEX представления
        uint64_t code = ( ((uint16_t) num_to_ascii(input[i])) << 12) +    
                        ( ((uint16_t) num_to_ascii(input[i + 1])) << 8) +
                        ( ((uint16_t) num_to_ascii(input[i + 2])) << 4) +
                        ( (uint16_t) num_to_ascii(input[i + 3]) );
        
        // В соответствии с количеством байт формируем символ
        // Не забываем про завершающий ноль
        if (code <= 0x7F) 
        {                               
            c[0] = (char) code;                              
            c[1] = 0;                 
        }
        else if (code <= 0x7FF)
        {
            c[0] = (char) (0xC0 | (code >> 6));
            c[1] = (char) (0x80 | (code & 0x3F));
            c[2] = 0;
        }
        else if (code <= 0xFFFF)
        {
            c[0] = (char) (0xE0 | (code >> 12));
            c[1] = (char) (0x80 | ((code >> 6) & 0x3F));
            c[2] = (char) (0x80 | (code & 0x3F));
            c[3] = 0;
        }
        else if (code <= 0x1FFFFF)
        {
            c[0] = (char) (0xE0 | (code >> 18));
            c[1] = (char) (0xE0 | ((code >> 12) & 0x3F));
            c[2] = (char) (0x80 | ((code >> 6) & 0x3F));
            c[3] = (char) (0x80 | (code & 0x3F));
            c[4] = 0;
        }
        strcat(output, c);             
    }

    return size;  
}


pdu_decode_status decode_pdu_pocket(deliver_pdu_pocket *pdu_pocket, deliver_pocket *pocket)
{
    // Clear pocket struct
    memset(pocket, 0, sizeof(deliver_pocket));

    // Decode Sender
    size_t buffer_size = PDU_MAX_LEN;
    uint8_t buffer[buffer_size];

    switch (pdu_pocket->OA.type)
    {
        case OA_7_BIT:
            buffer_size = gsm_decode_7bit(pdu_pocket->OA.data, pdu_pocket->OA.size, buffer);
            break;
        case OA_LITTLE_ENDIAN_NUMBER: //TODO: Testing
            buffer_size = pdu_pocket->OA.size;
            switch_endianness(pdu_pocket->OA.data, pdu_pocket->OA.size, buffer);
            break;
        default:
            return WRONG_OA_TYPE;    
    }

    pocket->sender.size = buffer_size;
    strncpy(pocket->sender.data, buffer, buffer_size);

    // Decode Message
    buffer_size = UD_MAX_LEN;
    memset(buffer, '\0', buffer_size * sizeof(uint8_t));

    switch (pdu_pocket->DCS)
    {
        case DCS_7_BIT:
            buffer_size = gsm_decode_7bit(pdu_pocket->UD, pdu_pocket->UDL, buffer);
            break;
        case DCS_UCS2:
            buffer_size = gsm_decode_UCS2(pdu_pocket->UD, (pdu_pocket->UDL * 2) + 1, buffer);
            break;
    }

    strncpy(pocket->message.data, buffer, buffer_size);
    pocket->message.size = buffer_size;

    // Decode Timestamp
    struct tm timestamp = {};
    char tm_buffer[2];
    uint8_t tm_frame = 0;
    
    buffer_size = SCTS_SIZE;
    switch_endianness(pdu_pocket->SCTS, SCTS_SIZE, buffer); 

    // Year
    strncpy(tm_buffer, buffer, 2);
    timestamp.tm_year = strtol(tm_buffer, NULL, 10) + 100;

    // Month
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    timestamp.tm_mon = strtol(tm_buffer, NULL, 10) - 1;
    
    // Day
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    timestamp.tm_mday = strtol(tm_buffer, NULL, 10);

    // Hour
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    timestamp.tm_hour = strtol(tm_buffer, NULL, 10);

    // Minute
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    timestamp.tm_min = strtol(tm_buffer, NULL, 10);

    // Sec
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    timestamp.tm_sec = strtol(tm_buffer, NULL, 10);
    
    // TimeZone
    strncpy(tm_buffer, buffer + (tm_frame += 2), 2);
    pocket->time.timezone = (int8_t) strtol(tm_buffer, NULL, 10) / 4;
    pocket->time.timestamp = mktime(&timestamp);
   
    if (pocket->time.timestamp == -1)
    {
        return WRONG_TIMESTAMP;
    }

    return _NO_ERROR;
}
