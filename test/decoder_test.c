#include <criterion/criterion.h>
#include <locale.h>
#include <stdio.h>
#include "../src/decoder.h"
// #include "../src/util.h"

deliver_pdu_pocket pocket;
deliver_pocket dec_pocket;


void dump_deviler_pocket(void)
{
    printf(":: Service Center Address\n");
    printf("SCA Size - %d\n", pocket.SCA.size);
    printf("SCA Type - %d\n", pocket.SCA.type);
    printf("SCA Data - %s\n\n", pocket.SCA.data);

    printf("MTI_CO - %d\n\n", pocket.PDU_TYPE);
    
    printf(":: Originator Address\n");
    printf("OA Size - %d\n", pocket.OA.size);
    printf("OA Type - %d\n", pocket.OA.type);
    printf("OA Data - %s\n\n", pocket.OA.data);
    
    printf("PID - %d\n", pocket.PID);
    printf("DCS - %d\n", pocket.DCS);
    printf("SCTS - %s\n\n", pocket.SCTS);
    
    printf(":: User Data\n");
    printf("UDL - %d\n", pocket.UDL);
    printf("UD - %s\n", pocket.UD);
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

Test(switch_endianness, valid)
{
    char* valid = "000102030405060708090A0B0C0D0E0F";
    char* raw = "00102030405060708090A0B0C0D0E0F0";
    size_t raw_size = 33;

    char result[raw_size];
    switch_endianness(raw, raw_size, result);
    
    cr_assert(!strncmp(valid, result, raw_size), "Except %s, but recieve %s", valid, result);
}

Test(switch_endianness, wrong_size)
{
    char* raw = "0";
    size_t raw_size = 1;

    char* result;
    size_t new_size = switch_endianness(raw, raw_size, result);
    
    cr_assert(!new_size, "Except %d, but recieve %s",0,new_size);
}

Test(num_from_ascii, valid)
{
    uint8_t validate[22] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15,
        10, 11, 12, 13, 14, 15
    };
    uint8_t *input = "0123456789abcdefABCDEF";

    for (int8_t i = 0; i < 22; i++)
    {
        int8_t data = num_from_ascii(*(input + i));

        if (data == -1)
        {
            cr_assert(false, "#%d: Receive wrong data", i);    
        }

        if (data != validate[i])
        {
            cr_assert(false, "#%d: Expected %d, but receive %d", i, validate[i], data);
        }
    }
    
    cr_assert(true);
}

Test(num_from_ascii, wrong_data)
{
    const uint8_t* input = "G";
    int8_t data = num_from_ascii(*input);
    cr_assert(data == -1);
}

Test(gsm_decode_7bit, valid)
{
    const uint8_t *sample = "E474D81C0EBB5DE3771B";
    const uint8_t *valid = "diafaan.com";

    uint8_t result[21] = {0};
    gsm_decode_7bit(sample, 21, result);

    cr_assert(!strncmp(valid, result, 12), "Except %s, but recieve %s",valid,result);
}

// TODO: More Tests 
Test(decode_pdu_pocket, valid_UCS2)
{
    char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
    size_t size = 275;
    
    pdu_parse_status pst = parse_deliver_pocket(hex_pocket, size, &pocket);
    pdu_decode_status dst = decode_pdu_pocket(&pocket, &dec_pocket);

    cr_assert(!dst, "%d", dst);
}

Test(decode_pdu_pocket, valid_7bit)
{
    setlocale(LC_ALL, "");

    char* hex_pocket = "0791448720003023240DD0E474D81C0EBB010000111011315214000BE474D81C0EBB5DE3771B";
    size_t size = 76;
    
    pdu_parse_status pst = parse_deliver_pocket(hex_pocket, size, &pocket);
    pdu_decode_status dst = decode_pdu_pocket(&pocket, &dec_pocket);

    cr_assert(!dst, "%d", dst);
}
