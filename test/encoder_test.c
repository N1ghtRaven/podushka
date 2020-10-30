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

Test(package_submit_pocket, generate_pdu_type)
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
    package_submit_pocket(&pocket, &pdu_pocket);

    // printf("%s", pdu_pocket.da.data);

    // dump_submit_pocket(&pdu_pocket);

    // Add test
    uint8_t pdu[SUBMIT_PDU_MAX_SIZE];
    size_t size = serialize_submit_pocket(&pdu_pocket, pdu, &size);
    // printf("PDU: %s\n Size: %d", pdu, size);

    cr_assert(true);
}

Test(package_submit_pocket, encoder_ucs2_test)
{
    uint8_t test_msg[28] = "Привет, друг!"; // 13
    uint8_t test_msg2[50];

    gsm_encode_UCS2(&test_msg, 28, &test_msg2); // Test me
    // printf(test_msg2);

    cr_assert(true);
}

Test(package_submit_pocket, encoder_gsm7bit_test)
{
    int i;
    char test_msg[] = "diafaan.com";
    uint8_t result[128];
    memset(result,0, sizeof(result));

    size_t size = gsm_encode_7bit(test_msg, 12, result);
	// printf("7bit 2:");
	// for(i=0;i<size;i++)
	// 	printf(" %02x", result[i]);
	// printf("\n");
		
	// printf("ascii 1:");
	// for(i=0;i<12;i++)
	// 	printf(" %02x", test_msg[i]);
	// printf("\n");

    cr_assert(true);
}
