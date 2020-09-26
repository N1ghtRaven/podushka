#ifdef UNIT_TEST
#include <criterion/criterion.h>
#include <stdio.h>
#endif

#include "pdu.h"

#include <stdlib.h>
#include <string.h>

pdu_parse_status parse_deliver_pocket(uint8_t *hex, size_t size, deliver_pdu_pocket *pocket)
{
    if (size < PDU_MIN_LEN || size > PDU_MAX_LEN)
    {
        return WRONG_POCKET_SIZE;
    }

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

    if (pocket->TP_UDL * 2 < UD_MIN_LEN || pocket->TP_UDL * 2 > UD_MAX_LEN)
    {
        return WRONG_UD_SIZE;
    }

    return NO_ERROR;
}

deliver_pdu_pocket pocket;
// void main(void)
// {
//     char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
//     size_t size = 275;
    
//     pdu_parse_status st = parse_pdu_pocket(hex_pocket, size, &pocket);

// }

#ifdef UNIT_TEST




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

Test(deliver_pdu_parser, valid_string) //, .init = setup, .fini = dump_pocket
{
    char* hex_pocket = "07919761980614F82414D0D9B09B5CC637DFEE721E0008022070817432216A041F04300440043E043B044C003A0020003100380035003900200028043D0438043A043E043C04430020043D043500200433043E0432043E04400438044204350029000A0414043E044104420443043F0020043A00200438043D0444043E0440043C0430044604380438";
    size_t size = 275;
    
    pdu_parse_status st = parse_deliver_pocket(hex_pocket, size, &pocket);

    cr_assert(!st);
}


// TODO: More tests
Test(deliver_pdu_parser, junk, .init = setup, .fini = dump_pocket)
{
    // Generate 
    srand(time(0));
    size_t junk_size = rand() % 255;      // Returns a pseudo-random integer between 0 and RAND_MAX.
    uint8_t junk[junk_size];
    for (uint32_t i = 0; i < junk_size; i++)
    {
        junk[i] = rand() % 255;
    }

    pdu_parse_status st = parse_deliver_pocket(junk, junk_size, &pocket);
    printf("%d\n",st);
    cr_assert(st);
}

#endif