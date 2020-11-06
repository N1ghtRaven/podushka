#ifndef _ENCODER_h_
#define _ENCODER_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


#define SUBMIT_PDU_MAX_SIZE 330
#define DA_MAX_LEN 12 * 2

#ifndef UD_MAX_LEN
    #define UD_MAX_LEN 140 * 2
#endif

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


typedef struct
{
    uint8_t sca; // 0x00 default 

    uint8_t pdu_type;
    uint8_t mr; // Message Reference

    struct
    {
        uint8_t size; // number count, if count not odd +1
        uint8_t type;
        uint8_t data[DA_MAX_LEN];
    } da; // Destination Address

    uint8_t pid; // Protocol ID
    uint8_t dcs; // Data Coding Scheme 0x08 - normal mode, 0x18 - flash mode
    uint8_t vp; // Validate Period RELATIVE

    uint8_t udl;
    uint8_t ud[UD_MAX_LEN];
} submit_pdu_pocket;

/**
 * Сериализует Submit пакет в строку
 * @param pdu_pocket Submit пакет
 * @param output сериализованная строка
 * @param size размер сериализованной строки
 */
size_t serialize_submit_pocket(submit_pdu_pocket *pdu_pocket, uint8_t *output, size_t *size);

typedef enum
{
    INTERANATIONAL_TYPE = 0x91,
    NATIVE_TYPE = 0x81
} destination_num_type;

typedef enum
{
    MDCS_7_BIT = 0x00,
    MDCS_UCS2 = 0x08
} message_data_scheme;

typedef enum
{
    MINUTE,
    HOUR,
    DAY,
    WEEK
} vp_scale;

typedef struct
{
    struct
    {
        destination_num_type type;
        uint8_t size;
        uint8_t addr[DA_MAX_LEN];
    } destination;

    struct
    {
        message_data_scheme mdcs;
        uint8_t size;
        uint8_t data[UD_MAX_LEN];
    } message;

    struct
    {
        vp_scale scale;
        uint8_t value;
    } ttl; // Time to Live!
    
} submit_pocket;

typedef enum
{
    NO_ERROR = 0,
    WRONG_DATA_SCHEME = 1,
    WRONG_VP_SCALE = 2, // Warning
} pdu_package_status;


/**
 * Упаковывает сообщение в Submit PDU структуру
 * @param pocket структура содержащая сообщение
 * @param pdu_pocket Submit PDU пакет
 */
pdu_package_status package_submit_pocket(submit_pocket *pocket, submit_pdu_pocket *pdu_pocket);


#endif