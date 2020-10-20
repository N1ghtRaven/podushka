#include "pdu.h"

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
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_SCA.size = (uint8_t) strtol(buffer, NULL, 16);

    if (pocket->TP_SCA.size < SCA_MIN_LEN || pocket->TP_SCA.size > SCA_MAX_LEN)
    {
        return WRONG_SCA_SIZE;
    }

    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_SCA.type = (uint8_t) strtol(buffer, NULL, 16);

    strncpy(pocket->TP_SCA.data, hex + frame, (pocket->TP_SCA.size - 1) * 2);
    frame += (pocket->TP_SCA.size - 1) * 2;  

    //Parse TP-MTI-CO
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_MTI_CO = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-OA
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_OA.size = (uint8_t) strtol(buffer, NULL, 16);

    if (pocket->TP_OA.size < OA_MIN_LEN || pocket->TP_OA.size > OA_MAX_LEN)
    {
        return WRONG_OA_SIZE;
    }

    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_OA.type = (uint8_t) strtol(buffer, NULL, 16);

    uint8_t real_oa_size = (pocket->TP_OA.size % 2) > 0 ? pocket->TP_OA.size + 1 : pocket->TP_OA.size;
    strncpy(pocket->TP_OA.data, hex + frame, real_oa_size);
    frame += real_oa_size;

    //Parse TP-PID
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_PID = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-DCS
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_DCS = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-SCTS
    strncpy(pocket->TP_SCTS, hex + frame, TP_SCTS_SIZE);
    frame += TP_SCTS_SIZE;

    //Parse TP-UDL
    strncpy(buffer, hex + frame, PDU_FRAME_STEP);
    frame += PDU_FRAME_STEP;
    pocket->TP_UDL = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-UD
    strncpy(pocket->TP_UD, hex + frame, pocket->TP_UDL * 2);
    frame += pocket->TP_UDL * 2;

    if (pocket->TP_UDL * 2 > UD_MAX_LEN)
    {
        return WRONG_UD_SIZE;
    }

    return NO_ERROR;
}



/**
 * TODO: Покрыть тестами
 * Меняет порядок байтов в цепочке
 * @param input_chain исходная цепочка байт
 * @param len длина цепочки
 * @param output_chain цепочка байт с измененным порядком
 * @return размер измененной цепочки
 */
size_t switch_endianness(uint8_t *input_chain, size_t len, uint8_t *output_chain)
{
    if (len < 2)
    {
        return 0;
    }

    memset(output_chain, len, sizeof(uint8_t));

    // is odd 
    if (!(len % 2))
    {
        len -=1;
    }

    for (size_t i = 0; i < len; i += 2)
    {
        output_chain[i] = input_chain[i + 1];
        output_chain[i + 1] = input_chain[i];
    }

    return len;
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
 * TODO Покрыть тестами
 * Декодер GSM 7-ми битной строки
 * @param input входная закодированная строка
 * @param size размер входной строки
 * @param output раскодированная строка
 * @return размер выходной строки
 */
size_t gsm_decode_7bit(uint8_t *input, size_t size, uint8_t *output)
{
    size_t output_size = 0;

    uint16_t bit_buffer = 0;
    uint8_t bit_buffer_size = 0;
    while ( *input != '\0' ) 
    {
        // Convert char to num
        int16_t num = ( num_from_ascii(*(input++)) << 4 ) | num_from_ascii(*(input++));
        if (num < 0)
        {
            return -1; // TODO: drop some error
        }

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

    switch (pdu_pocket->TP_OA.type)
    {
        case OA_7_BIT:
            buffer_size = gsm_decode_7bit(pdu_pocket->TP_OA.data, pdu_pocket->TP_OA.size, &buffer);
            break;
        case OA_LITTLE_ENDIAN_NUMBER: //TODO: Testing
            buffer_size = pdu_pocket->TP_OA.size;
            switch_endianness(pdu_pocket->TP_OA.data, pdu_pocket->TP_OA.size, &buffer);
            break;
        default:
            // Why drop???
            return WRONG_OA_TYPE;    
    }

    pocket->sender.size = buffer_size;
    strncpy(pocket->sender.data, buffer, buffer_size);

    // Decode Message
    buffer_size = UD_MAX_LEN;
    memset(buffer, '\0', buffer_size * sizeof(uint8_t));

    switch (pdu_pocket->TP_DCS)
    {
        case DCS_7_BIT:
            buffer_size = gsm_decode_7bit(pdu_pocket->TP_UD, pdu_pocket->TP_UDL, &buffer);
            break;
        case DCS_UCS2:
            buffer_size = gsm_decode_UCS2(pdu_pocket->TP_UD, (pdu_pocket->TP_UDL * 2) + 1, &buffer);
            break;
    }

    strncpy(pocket->message.data, buffer, buffer_size);
    pocket->message.size = buffer_size;

    // Decode Timestamp
    struct tm timestamp = {};
    char tm_buffer[PDU_FRAME_STEP];
    uint8_t tm_frame = 0;
    
    buffer_size = TP_SCTS_SIZE;
    switch_endianness(pdu_pocket->TP_SCTS, TP_SCTS_SIZE, &buffer); 

    // Year
    strncpy(tm_buffer, buffer, PDU_FRAME_STEP);
    timestamp.tm_year = strtol(tm_buffer, NULL, 10) + 100;

    // Month
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    timestamp.tm_mon = strtol(tm_buffer, NULL, 10) - 1;
    
    // Day
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    timestamp.tm_mday = strtol(tm_buffer, NULL, 10);

    // Hour
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    timestamp.tm_hour = strtol(tm_buffer, NULL, 10);

    // Minute
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    timestamp.tm_min = strtol(tm_buffer, NULL, 10);

    // Sec
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    timestamp.tm_sec = strtol(tm_buffer, NULL, 10);
    
    // TimeZone
    strncpy(tm_buffer, buffer + (tm_frame += PDU_FRAME_STEP), PDU_FRAME_STEP);
    pocket->time.timezone = (int8_t) strtol(tm_buffer, NULL, 10) / 4;
    pocket->time.timestamp = mktime(&timestamp);
   
    if (pocket->time.timestamp == -1)
    {
        return WRONG_TIMESTAMP;
    }

    return _NO_ERROR;
}
