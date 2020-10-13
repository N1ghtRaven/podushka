#ifndef _PDU_h_
#define _PDU_h_

#include <stdint.h>
#include <stddef.h>
#include <time.h>

// Service Center Address
#define SCA_MIN_LEN 1
#define SCA_MAX_LEN 12 * 2

// Originator Address
#define OA_MIN_LEN 2
#define OA_MAX_LEN 12 * 2

#define PDU_TYPE_LEN 1
#define PID_LEN 1
#define DCS_LEN 1

// User Data
#define UDL 1
#define UD_MIN_LEN 0
#define UD_MAX_LEN 140 * 2

#define PDU_MIN_LEN SCA_MIN_LEN + OA_MIN_LEN + PDU_TYPE_LEN + PID_LEN + DCS_LEN + UDL + UD_MIN_LEN
#define PDU_MAX_LEN SCA_MAX_LEN + OA_MAX_LEN + PDU_TYPE_LEN + PID_LEN + DCS_LEN + UDL + UD_MAX_LEN

#define PDU_FRAME_STEP 2

#define OA_LITTLE_ENDIAN_NUMBER 0x91
#define OA_7_BIT 0xD0
#define DCS_7_BIT 0x00
#define DCS_UCS2 0x08
#define TP_SCTS_SIZE 14

#define DEFAULT_REPLY_PATH 0b0 << 7
#define DEFAULT_UDHI 0b0 << 6
#define DEFAULT_SRR 0b0 << 5

#define MISS_VP 0b00
#define RESERV_VP 0b01
#define RELATIVE_VP 0b10
#define ABSOLUTE_VP 0b11

#define DEFAULT_VPF RELATIVE_VP << 3
#define DEFAULT_RD 0b0 << 2
#define DEFAULT_MTI 0b01

#define DEFAULT_PDU_TYPE DEFAULT_REPLY_PATH | DEFAULT_UDHI | DEFAULT_SRR | DEFAULT_VPF | DEFAULT_RD | DEFAULT_MTI

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
    } TP_SCA;
    
    uint8_t TP_MTI_CO;

    struct
    {
        uint8_t size;
        uint8_t type;
        uint8_t data[OA_MAX_LEN];
    } TP_OA;
    
    uint8_t TP_PID;
    uint8_t TP_DCS;
    uint8_t TP_SCTS[TP_SCTS_SIZE];

    uint8_t TP_UDL;
    uint8_t TP_UD[UD_MAX_LEN];
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


typedef enum
{
    __NO_ERROR = 0,
    // WRONG_POCKET_SIZE = 1,
    // WRONG_SCA_SIZE = 2,
    // WRONG_OA_SIZE = 3,
    // WRONG_UD_SIZE = 4
} pdu_serialize_status;


typedef struct
{
    uint8_t sca; // 0x00 default 

    uint8_t pdu_type;
    uint8_t mr; // Message Reference

    struct
    {
        uint8_t size; // number count, if count not odd +1
        uint8_t type;
        uint8_t data[OA_MAX_LEN];
    } da; // Destination Address

    uint8_t pid; // Protocol ID
    uint8_t dcs; // Data Coding Scheme
    uint8_t vp[7]; // Validate Period

    uint8_t udl;
    uint8_t ud[UD_MAX_LEN];
} submit_pdu_pocket;

/**
 * Сериализует Submit пакет в строку
 * @param pdu_pocket Submit пакет
 * @param output сериализованная строка
 * @param size размер сериализованной строки
 */
pdu_serialize_status serialize_submit_pocket(submit_pdu_pocket *pdu_pocket, uint8_t *output, size_t *size);


typedef struct
{
    struct
    {
        /* data */
        uint8_t size;
        uint8_t addr[OA_MAX_LEN];
    } dest_addr;

    struct
    {
        /* data */
    } message;

    uint8_t ttl; // Time to Live
    
} submit_pocket;

typedef enum
{
    ___NO_ERROR = 0,
    // WRONG_POCKET_SIZE = 1,
    // WRONG_SCA_SIZE = 2,
    // WRONG_OA_SIZE = 3,
    // WRONG_UD_SIZE = 4
} pdu_package_status;


/**
 * Упаковывает сообщение в Submit PDU структуру
 * @param pocket структура содержащая сообщение
 * @param pdu_pocket Submit PDU пакет
 */
pdu_package_status package_submit_pocket(submit_pocket *pocket, submit_pdu_pocket *pdu_pocket);


#endif