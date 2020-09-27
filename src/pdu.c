#include "pdu.h"

#include <stdlib.h>
#include <string.h>

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
 */
void switch_endianness(uint8_t *input_chain, size_t len, uint8_t *output_chain)
{
    for (size_t i = 0; i < len; i += 2)
    {
        output_chain[i] = input_chain[i + 1];
        output_chain[i + 1] = input_chain[i];
    }
}

pdu_decode_status decode_pdu_pocket(deliver_pdu_pocket *pdu_pocket, deliver_pocket *pocket)
{
    // Clear pocket struct
    memset(pocket, 0, sizeof(deliver_pdu_pocket));

    // Decode Sender
    size_t buffer_size = OA_MAX_LEN;
    uint8_t buffer[buffer_size];

    switch (pdu_pocket->TP_OA.type)
    {
        case OA_7_BIT:
            //buffer_size = decode7bit(pdu_pocket.TP_OA.data, pdu_pocket.TP_OA.size, &buffer);
            break;
        case OA_LITTLE_ENDIAN_NUMBER:
            buffer_size = pdu_pocket->TP_OA.size;
            switch_endianness(pdu_pocket->TP_OA.data, pdu_pocket->TP_OA.size, &buffer);
            break;
        default:
            return 1; // ERROR    
    }

    pocket->sender.size = buffer_size;
    strncpy(pocket->sender.data, buffer, buffer_size);


    return 0;
}

#ifndef UNIT_TEST
deliver_pdu_pocket pocket;

void main(void)
{
    char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
    size_t size = 275;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);
}
#endif

/*

#include "stdio.h"
#include <stdint.h>
#include "string.h"
#include "stdlib.h"

#define BUFFER_SIZE 0xFF
#define BYTE_COUNT 2
#define TP_SCTS_SIZE 14

struct
{
    enum 
    { 
        OA_7_BIT = 0xD0, OA_LITTLE_ENDIAN_NUMBER = 0x91, 
        DCS_7_BIT = 0x00, DCS_UCS2 = 0x08
    };

    struct
    {
        uint8_t size;
        uint8_t type;
        uint8_t data[BUFFER_SIZE];
    } TP_SCA;
    
    uint8_t TP_MTI_CO;

    struct
    {
        uint8_t size;
        uint8_t type;
        uint8_t data[BUFFER_SIZE];
    } TP_OA;
    
    uint8_t TP_PID;
    uint8_t TP_DCS;
    uint8_t TP_SCTS[BUFFER_SIZE];

    uint8_t TP_UDL;
    uint8_t TP_UD[BUFFER_SIZE];
} pdu_pocket;

struct
{
    struct {
        uint8_t data[BUFFER_SIZE];
        uint8_t size;
    } from;

    struct {
        uint8_t data[BUFFER_SIZE];
        uint8_t size;
    } message;

    struct 
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        uint8_t timezone;
    } timestamp;
} sms;

void parsePdu(uint8_t *hex, size_t size)
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t frame = 0;

    //Parse TP-SCA
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_SCA.size = (uint8_t) strtol(buffer, NULL, 16);

    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_SCA.type = (uint8_t) strtol(buffer, NULL, 16);

    strncpy(pdu_pocket.TP_SCA.data, hex + frame, (pdu_pocket.TP_SCA.size - 1) * 2);
    frame += (pdu_pocket.TP_SCA.size - 1) * 2;  

    //Parse TP-MTI-CO
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_MTI_CO = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-OA
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_OA.size = (uint8_t) strtol(buffer, NULL, 16);

    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_OA.type = (uint8_t) strtol(buffer, NULL, 16);

    uint8_t real_oa_size = (pdu_pocket.TP_OA.size % 2) > 0 ? pdu_pocket.TP_OA.size + 1 : pdu_pocket.TP_OA.size;
    strncpy(pdu_pocket.TP_OA.data, hex + frame, real_oa_size);
    frame += real_oa_size;

    //Parse TP-PID
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_PID = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-DCS
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_DCS = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-SCTS
    strncpy(pdu_pocket.TP_SCTS, hex + frame, TP_SCTS_SIZE);
    frame += TP_SCTS_SIZE;

    //Parse TP-UDL
    strncpy(buffer, hex + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    pdu_pocket.TP_UDL = (uint8_t) strtol(buffer, NULL, 16);

    //Parse TP-UD
    strncpy(pdu_pocket.TP_UD, hex + frame, pdu_pocket.TP_UDL * 2);
    frame += pdu_pocket.TP_UDL * 2;
}



void extractData (char input, uint8_t *output)
{
    if ( input - '0' >= 0 && '9' - input >= 0 ) {
        * output = input - '0';
    } else if ( input - 'a' >= 0 && 'f' - input >= 0 ) {
        * output = input - 'a' + 10;
    } else if ( input - 'A' >= 0 && 'F' - input >= 0 ) {
        * output = input - 'A' + 10;
    }
}

size_t decode7bit(uint8_t *hex, size_t len, uint8_t *res)
{
    uint16_t data = 0;
    uint8_t data_length = 0;

    size_t res_size = -1;
    while ( *hex != '\0' ) 
    {
        uint8_t new_data, d;

        extractData(*hex, &d);
        new_data = d << 4;

        extractData(*(hex + 1 ), &d);
        new_data = new_data | d;

        hex += 2;

        data = new_data << data_length | data;
        data_length += 8;

        while ( data_length >= 7 ) {
            res_size++;
            res[res_size] = (data & 0x7f);
            
            data = data >> 7;
            data_length -= 7;
        }
    }
    return res_size + 1;
    //strncpy(sms.from.data, res, index + 1);
    // out = res;
    // out_size = index + 1;
    //sms.from.size = index + 1;
}



void switchEndian(uint8_t *little_endian_chain, size_t len, uint8_t *res)
{
    for (size_t i = 0; i < len; i += 2)
    {
        res[i] = little_endian_chain[i + 1];
        res[i + 1] = little_endian_chain[i];
    }
}


uint8_t hexToChar(uint8_t c) {
  if      ((c >= 0x30) && (c <= 0x39)) return (c - 0x30);
  else if ((c >= 'A') && (c <= 'F'))   return (c - 'A' + 10);
  else                                 return (0);
}

size_t decodeUCS2(uint8_t *s, size_t size, uint8_t *out) {
  uint8_t res[BUFFER_SIZE];                         // Функция декодирования UCS2 строки
  unsigned char c[5] = "";                            // Массив для хранения результата
  for (uint16_t i = 0; i < (size * 2) - 3; i += 4) {       // Перебираем по 4 символа кодировки
    unsigned long code = (((uint16_t)hexToChar(s[i])) << 12) +    // Получаем UNICODE-код символа из HEX представления
                         (((uint16_t)hexToChar(s[i + 1])) << 8) +
                         (((uint16_t)hexToChar(s[i + 2])) << 4) +
                         ((uint16_t)hexToChar(s[i + 3]));
    if (code <= 0x7F) {                               // Теперь в соответствии с количеством байт формируем символ
      c[0] = (char)code;                              
      c[1] = 0;                                       // Не забываем про завершающий ноль
    } else if (code <= 0x7FF) {
      c[0] = (char)(0xC0 | (code >> 6));
      c[1] = (char)(0x80 | (code & 0x3F));
      c[2] = 0;
    } else if (code <= 0xFFFF) {
      c[0] = (char)(0xE0 | (code >> 12));
      c[1] = (char)(0x80 | ((code >> 6) & 0x3F));
      c[2] = (char)(0x80 | (code & 0x3F));
      c[3] = 0;
    } else if (code <= 0x1FFFFF) {
      c[0] = (char)(0xE0 | (code >> 18));
      c[1] = (char)(0xE0 | ((code >> 12) & 0x3F));
      c[2] = (char)(0x80 | ((code >> 6) & 0x3F));
      c[3] = (char)(0x80 | (code & 0x3F));
      c[4] = 0;
    }
    strcat(out, c); // Добавляем полученный символ к результату                 
  }
  return size;  
}

void decodePdu()
{
    //Decode Sender
    uint8_t buffer[BUFFER_SIZE];
    size_t buffer_size = BUFFER_SIZE;
    switch (pdu_pocket.TP_OA.type)
    {
        case OA_7_BIT:
            buffer_size = decode7bit(pdu_pocket.TP_OA.data, pdu_pocket.TP_OA.size, &buffer);
            break;
        case OA_LITTLE_ENDIAN_NUMBER:
            buffer_size = pdu_pocket.TP_OA.size;
            switchEndian(pdu_pocket.TP_OA.data, pdu_pocket.TP_OA.size, &buffer);
            break;
    }
    strncpy(sms.from.data, buffer, buffer_size);
    sms.from.size = buffer_size;

    //Decode Messgae
    memset(buffer, '\0', BUFFER_SIZE);
    buffer_size = BUFFER_SIZE;
    switch (pdu_pocket.TP_DCS)
    {
        case DCS_7_BIT:
            buffer_size = decode7bit(pdu_pocket.TP_UD, pdu_pocket.TP_UDL, &buffer);
            break;
        case DCS_UCS2:
            buffer_size = decodeUCS2(pdu_pocket.TP_UD, (pdu_pocket.TP_UDL * 2) + 1, &buffer);
            break;
    }
    strncpy(sms.message.data, buffer, buffer_size);
    sms.message.size = buffer_size;

    //Decode timestamp
    memset(buffer, '\0', BUFFER_SIZE);
    buffer_size = BUFFER_SIZE;
    uint8_t timestamp_buffer[BUFFER_SIZE], frame;
    switchEndian(pdu_pocket.TP_SCTS, TP_SCTS_SIZE, &buffer);

    strncpy(timestamp_buffer, buffer, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.year = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.month = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.day = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.hour = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.min = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.sec = (uint8_t) strtol(timestamp_buffer, NULL, 10);

    strncpy(timestamp_buffer, buffer + frame, BYTE_COUNT);
    frame += BYTE_COUNT;
    sms.timestamp.timezone = (uint8_t) strtol(timestamp_buffer, NULL, 10);
}

int div_up(int x, int y)
{
    return (x - 1) / y;
}

void main()
{
    div_t qq = div(387, 16);
    int q = div_up(387, 16);
    printf("%d.%d",qq.quot, qq.rem);
}

// void main(void)
// {
//     uint8_t *hex = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
//     size_t size = 275;
    
//     parsePdu(hex, size);
//     //parsePdu("07919761989901F0040B919701119905F80000211062320150610CC8329BFD065DDF72363904", 77);
//     //parsePdu("07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438", 275);
    
//     printf("TP-SCA size - %d\n", pdu_pocket.TP_SCA.size);
//     printf("TP-SCA type - %d\n", pdu_pocket.TP_SCA.type);
//     printf("TP-SCA data - %s\n", pdu_pocket.TP_SCA.data);
//     printf("TP-MTI-CO - %d\n", pdu_pocket.TP_MTI_CO);
//     printf("TP-OA size - %d\n", pdu_pocket.TP_OA.size);
//     printf("TP-OA type - %d\n", pdu_pocket.TP_OA.type);
//     printf("TP-OA data - %s\n", pdu_pocket.TP_OA.data);
//     printf("TP-PID - %d\n", pdu_pocket.TP_PID);
//     printf("TP-DCS - %d\n", pdu_pocket.TP_DCS);
//     printf("TP-SCTS data - %s\n", pdu_pocket.TP_SCTS);
//     printf("TP-UDL - %d\n", pdu_pocket.TP_UDL);
//     printf("TP-UD - %s\n\n", pdu_pocket.TP_UD);

//     decodePdu();
//     printf("From - %s\n", sms.from.data);
//     printf("Message - %s\n", sms.message.data);
//     printf("Timestamp - %d/%d/%d %d:%d:%d %d\n", sms.timestamp.year, sms.timestamp.month, sms.timestamp.day, sms.timestamp.hour, sms.timestamp.min, sms.timestamp.sec, sms.timestamp.timezone);
// }  


*/

#ifdef UNIT_TEST
#include <criterion/criterion.h>
#include <stdio.h>

deliver_pdu_pocket pocket;

void setup(void)
{
// pdu p;
//     pocket = p;
}

void dump_pocket(void)
{
    printf(":: Service Center Address\n");
    printf("SCA Size - %d\n", pocket.TP_SCA.size);
    printf("SCA Type - %d\n", pocket.TP_SCA.type);
    printf("SCA Data - %s\n\n", pocket.TP_SCA.data);

    printf("MTI_CO - %d\n\n", pocket.TP_MTI_CO);
    
    printf(":: Originator Address\n");
    printf("OA Size - %d\n", pocket.TP_OA.size);
    printf("OA Type - %d\n", pocket.TP_OA.type);
    printf("OA Data - %s\n\n", pocket.TP_OA.data);
    
    printf("PID - %d\n", pocket.TP_PID);
    printf("DCS - %d\n", pocket.TP_DCS);
    printf("SCTS - %s\n\n", pocket.TP_SCTS);
    
    printf(":: User Data\n");
    printf("UDL - %d\n", pocket.TP_UDL);
    printf("UD - %s\n", pocket.TP_UD);
}

Test(deliver_pdu_parser, valid_string) //, .fini = dump_pocket
{
    char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
    size_t size = 275;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);

    cr_assert(NO_ERROR == st, "Expect %d, but recieve %d", NO_ERROR, st);
}

Test(deliver_pdu_parser, big_pocket_size)
{
    size_t junk_size = PDU_MAX_LEN + 1;
    uint8_t junk[junk_size];

    srand(time(0));
    for (uint32_t i = 0; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    cr_assert(WRONG_POCKET_SIZE == st, "Expect %d, but recieve %d", WRONG_POCKET_SIZE, st);
}

Test(deliver_pdu_parser, little_pocket_size)
{
    size_t junk_size = PDU_MIN_LEN - 1;
    uint8_t junk[junk_size];

    srand(time(0));
    for (uint32_t i = 0; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    cr_assert(WRONG_POCKET_SIZE == st, "Expect %d, but recieve %d", WRONG_POCKET_SIZE, st);
}

Test(deliver_pdu_parser, big_sca_size)
{
    // Generate 
    srand(time(0));
    size_t junk_size = rand() % PDU_MAX_LEN - 1;
    uint8_t junk[junk_size];

    junk[0] = SCA_MAX_LEN + 1;
    for (uint32_t i = 1; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    cr_assert(WRONG_SCA_SIZE == st, "Expect %d, but recieve %d", WRONG_SCA_SIZE, st);
}

Test(deliver_pdu_parser, little_sca_size)
{
    // Generate 
    srand(time(0));
    size_t junk_size = rand() % PDU_MAX_LEN - 1;
    uint8_t junk[junk_size];

    junk[0] = SCA_MIN_LEN - 1;
    for (uint32_t i = 1; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    cr_assert(WRONG_SCA_SIZE == st, "Expect %d, but recieve %d", WRONG_SCA_SIZE, st);
}

Test(deliver_pdu_parser, big_oa_size)
{
    // "07919761980614F82" VALID
    char* hex_pocket = "07919761980614F82FF9846516165465465464654";
    size_t size = 42;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);
    

    cr_assert(WRONG_OA_SIZE == st, "Expect %d, but recieve %d", WRONG_OA_SIZE, st);
}

Test(deliver_pdu_parser, little_oa_size)
{
    // "07919761980614F82" VALID
    char* hex_pocket = "07919761980614F82019846516165465465464654";
    size_t size = 42;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);
    

    cr_assert(WRONG_OA_SIZE == st, "Expect %d, but recieve %d", WRONG_OA_SIZE, st);
}

Test(deliver_pdu_parser, ud_size)
{
    // "07919761980614F82414D0D9B09B5CC637DFEE721E00080220708174322" VALID
    char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E000802207081743228D8459646498455654";
    size_t size = 77;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);
    cr_assert(WRONG_UD_SIZE == st, "Expect %d, but recieve %d", WRONG_UD_SIZE, st);
}


Test(deliver_pdu_parser, junk) // .init = setup, .fini = dump_pocket
{
    // Generate 
    srand(time(0));
    size_t junk_size = rand() % 255;
    uint8_t junk[junk_size];
    for (uint32_t i = 0; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    // "Expect %d, but recieve %d", WRONG_UD_SIZE, st printf("%d\n",st);
    cr_assert(st);
}

#endif