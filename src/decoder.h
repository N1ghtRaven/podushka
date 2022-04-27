#ifndef _DECODER_h_
#define _DECODER_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"


// Service Center Address
#define SCA_MIN_LEN 1 * 2
#define SCA_MAX_LEN 12 * 2

// Originator Address
#define OA_MIN_LEN 1 * 2
#define OA_MAX_LEN 12 * 2

// PDU fields with fixed offset
#define PDU_TYPE_LEN 1 * 2
#define PID_LEN 1 * 2
#define DCS_LEN 1 * 2
#define UDL_LEN 1 * 2

// User Data
#ifndef UD_MAX_LEN
    #define UD_MAX_LEN 140 * 2
#endif

#define PDU_MIN_LEN SCA_MIN_LEN + OA_MIN_LEN + PDU_TYPE_LEN + PID_LEN + DCS_LEN + UDL_LEN
#define PDU_MAX_LEN SCA_MAX_LEN + OA_MAX_LEN + PDU_TYPE_LEN + PID_LEN + DCS_LEN + UDL_LEN + UD_MAX_LEN

#define OA_LITTLE_ENDIAN_NUMBER 0x81
#define OA_7_BIT 0xD0
#define DCS_7_BIT 0x00
#define DCS_UCS2 0x08
#define SCTS_SIZE 14

typedef enum
{
    NO_ERROR = 0,
    WRONG_POCKET_SIZE = 1,
    WRONG_SCA_SIZE = 2,
    WRONG_OA_SIZE = 3,
    WRONG_UD_SIZE = 4
} pdu_parse_status;

typedef struct
{
    struct
    {
        uint8_t size;
        uint8_t type;
        uint8_t data[SCA_MAX_LEN];
    } SCA;
    
    uint8_t PDU_TYPE;

    struct
    {
        uint8_t size;
        uint8_t type;
        uint8_t data[OA_MAX_LEN];
    } OA;
    
    uint8_t PID;
    uint8_t DCS;
    uint8_t SCTS[SCTS_SIZE];

    uint8_t UDL;
    uint8_t UD[UD_MAX_LEN];
} deliver_pdu_pocket;

/** 
 * Преобразует строку в структуру
 * @param hex строка содержащая PDU пакет 
 * @param size размер строки
 * @param pocket PDU пакет
 * @return результат преобразования
 */
pdu_parse_status parse_deliver_pocket(uint8_t *hex, size_t size, deliver_pdu_pocket *pocket);


typedef enum
{
    _NO_ERROR = 0,
    WRONG_OA_TYPE = 1,
    WRONG_TIMESTAMP = 2
} pdu_decode_status;

typedef struct
{
    struct {
        uint8_t data[OA_MAX_LEN];
        uint8_t size;
    } sender;

    struct {
        uint8_t data[UD_MAX_LEN];
        uint8_t size;
    } message;

    struct {
        time_t timestamp;
        int8_t timezone;
    } time;

} deliver_pocket;


/**
 * TODO покрыть тестами
 * Декодирует PDU пакет в человеко-читаемый вид
 * @param pdu_pocket PDU пакет
 * @param pocket результат декодировния
 * @return статус декодировния
 */
pdu_decode_status decode_pdu_pocket(deliver_pdu_pocket *pdu_pocket, deliver_pocket *pocket);


#endif