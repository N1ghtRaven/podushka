#include <criterion/criterion.h>
#include <locale.h>
#include <stdio.h>
#include "../src/encoder.h"

void dump_submit_pocket(submit_pdu_pocket *pocket)
{
    printf(":: Service Center Address\n");
    printf("SCA - %02x\n", pocket->sca);

    printf("PDU Type - %02x\n\n", pocket->pdu_type);
    printf("MR - %02x\n\n", pocket->mr);

    printf(":: Destination Address\n");
    printf("DA Size - %02x\n", pocket->da.size);
    printf("DA Type - %02x\n", pocket->da.type);
    printf("DA Data - %s\n\n", pocket->da.data);
    
    printf("PID - %02x\n", pocket->pid);
    printf("DCS - %02x\n", pocket->dcs);
    printf("VP - %02x\n\n", pocket->vp);
    
    printf(":: User Data\n");
    printf("UDL - %02x\n", pocket->udl);
    printf("UD - %s\n", pocket->ud);
}

Test(switch_endianness, valid)
{
    const char* valid = "000102030405060708090A0B0C0D0E0F";
    char* raw = "00102030405060708090A0B0C0D0E0F0";

    size_t raw_size = 8;

    char result[raw_size];
    switch_endianness(raw, raw_size, result);
    
    cr_assert(!strncmp(valid, result, raw_size), "Except %s, but recieve %s",valid,result);
}

Test(switch_endianness, wrong_size)
{
    char* raw = "0";
    size_t raw_size = 1;

    char result[raw_size];
    size_t new_size = switch_endianness(raw, raw_size, result);
    
    cr_assert(!new_size, "Except %d, but recieve %s",0,new_size);
}

Test(package_submit_pocket, generate_7bit_pdu_type)
{
    submit_pocket pocket = {0};
    pocket.destination.type = INTERANATIONAL_TYPE;
    pocket.destination.size = 11;
    strncpy(pocket.destination.addr, "79147851125", pocket.destination.size);

    pocket.message.mdcs = MDCS_7_BIT;
    pocket.message.size = 15;
    strncpy(pocket.message.data, "Hello, friend!", pocket.message.size);

    pocket.ttl.scale = DAY;
    pocket.ttl.value = 1;

    submit_pdu_pocket pdu_pocket = {0};
    uint8_t st = package_submit_pocket(&pocket, &pdu_pocket);

    if (st != NO_ERROR)
    {
        dump_submit_pocket(&pdu_pocket);
        cr_assert(false, "Expect %d, but recieve %d", NO_ERROR, st);
    }

    uint8_t pdu[SUBMIT_PDU_MAX_SIZE];
    size_t size = serialize_submit_pocket(&pdu_pocket, pdu, &size);
    // printf(":: 7bit GSM\nPDU: %s\nSize: %d\n", pdu, size);

    cr_assert(true);
}

Test(package_submit_pocket, generate_ucs2_pdu_type)
{
    submit_pocket pocket = {0};
    pocket.destination.type = INTERANATIONAL_TYPE;
    pocket.destination.size = 11;
    strncpy(pocket.destination.addr, "79147851125", pocket.destination.size);

    pocket.message.mdcs = MDCS_UCS2;
    pocket.message.size = 24;
    strncpy(pocket.message.data, "Привет, друг!", pocket.message.size);

    pocket.ttl.scale = DAY;
    pocket.ttl.value = 1;

    submit_pdu_pocket pdu_pocket = {0};
    uint8_t st = package_submit_pocket(&pocket, &pdu_pocket);

    if (st != NO_ERROR)
    {
        dump_submit_pocket(&pdu_pocket);
        cr_assert(false, "Expect %d, but recieve %d", NO_ERROR, st);
    }

    // dump_submit_pocket(&pdu_pocket);
    uint8_t pdu[SUBMIT_PDU_MAX_SIZE];
    size_t size = serialize_submit_pocket(&pdu_pocket, pdu, &size);
    // printf(":: USC2\nPDU: %s\nSize: %d\n", pdu, size);

    cr_assert(true);
}

Test(package_submit_pocket, encoder_ucs2_test)
{
    uint8_t input_msg[28] = "Привет, друг!";
    uint8_t valid_msg[53] = "041f04400438043204350442002c002004340440044304330021";
    
    uint8_t output_msg[100];
    size_t output_size = gsm_encode_UCS2(&input_msg, 28, &output_msg);
    if (output_size == 0)
    {
        cr_assert(false, "Some error");
    }

    if (output_size != 53)
    {
        cr_assert(false, "Wrong size. Expect %d, but recieve %d", 53, output_size);
    }

    cr_assert(!strncmp(valid_msg, output_msg, output_size));
}

Test(package_submit_pocket, encoder_gsm7bit_test)
{
    char input_msg[] = "Hello, friend!";
    uint8_t valid_msg[] = "c8329bfd6681ccf274d94d0e01";
                                                  
    uint8_t output_msg[128];
    memset(output_msg, 0, sizeof(output_msg));

    size_t output_size = gsm_encode_7bit(input_msg, 15, output_msg);
    if (output_size != 27)
    {
        printf("%s\n", output_msg);
        cr_assert(false, "Wrong size. Expect %d, but recieve %d", 27, output_size);
    }

    cr_assert(!strncmp(valid_msg, output_msg, output_size), "Expect %s, but recieve %s", valid_msg, output_msg);
}

Test(package_submit_pocket, encoder_gsm7bit_test2)
{
    char input_msg[] = "Goodbye, friend!";
    uint8_t valid_msg[] = "c7f79b2cce975920b33c5d769343";
                                                  
    uint8_t output_msg[128];
    memset(output_msg, 0, sizeof(output_msg));

    size_t output_size = gsm_encode_7bit(input_msg, 17, output_msg);
    if (output_size != 30)
    {
        printf("%s\n", output_msg);
        cr_assert(false, "Wrong size. Expect %d, but recieve %d", 30, output_size);
    }

    cr_assert(!strncmp(valid_msg, output_msg, output_size), "Expect %s, but recieve %s", valid_msg, output_msg);
}
