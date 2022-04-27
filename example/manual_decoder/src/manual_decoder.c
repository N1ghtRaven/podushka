#include <locale.h>
#include <stdio.h>

#include "../../../src/decoder.h"

void dump_pdu(deliver_pdu_pocket *pocket)
{
    printf(":: Service Center Address\n");
    printf("SCA Size - %d\n", pocket->SCA.size);
    printf("SCA Type - %02x\n", pocket->SCA.type);
    printf("SCA Data - %s\n\n", pocket->SCA.data);

    printf("MTI_CO - %02x\n\n", pocket->PDU_TYPE);
    
    printf(":: Originator Address\n");
    printf("OA Size - %d\n", pocket->OA.size);
    printf("OA Type - %02x\n", pocket->OA.type);
    printf("OA Data - %s\n\n", pocket->OA.data);
    
    printf("PID - %02x\n", pocket->PID);
    printf("DCS - %02x\n", pocket->DCS);
    printf("SCTS - %02x\n\n", pocket->SCTS);
    
    printf(":: User Data\n");
    printf("UDL - %02x\n", pocket->UDL);
    printf("UD - %s\n", pocket->UD);
}

char *settime(struct tm *u)
{
    char s[40];
    char *tmp;
    for (int i = 0; i<40; i++) s[i] = 0;
    int length = strftime(s, 40, "%d.%m.%Y %H:%M:%S", u);
    tmp = (char*)malloc(sizeof(s));
    strcpy(tmp, s);
    return(tmp);
}

void dump_sms(deliver_pocket *pocket)
{
    printf("От кого: %s\n", pocket->sender.data);
    printf("Сообщение: %s\n", pocket->message.data);

    const time_t timer = pocket->time.timestamp;
    struct tm *time = localtime(&timer);
    printf("Дата: %s\n", settime(time));
}

void main()
{
    setlocale(LC_ALL, "");

    printf("Insert here PDU pocket: ");

    uint8_t hex_pocket[PDU_MAX_LEN + 1] = {0};
    size_t size = 0;

    fgets(hex_pocket, PDU_MAX_LEN + 1, stdin);
    for (uint16_t i = 0; i < PDU_MAX_LEN; i++)
    {
        if (hex_pocket[i] == '\0')
        {
            size = i;
            break;
        }
    }
    
    deliver_pdu_pocket pocket = {0};
    pdu_parse_status pst = parse_deliver_pocket(hex_pocket, size, &pocket);

    if (pst != NO_ERROR)
    {
        printf("PDU parse status: %d\n", pst);
    }

    printf("\n:: PDU POCKET\n");
    dump_pdu(&pocket);

    deliver_pocket dec_pocket = {0};
    pdu_decode_status dst = decode_pdu_pocket(&pocket, &dec_pocket);
    
    if (dst != NO_ERROR)
    {
        printf("PDU decode status: %d\n", pst);
    }

    printf("\n:: SMS POCKET\n");
    dump_sms(&dec_pocket);
}