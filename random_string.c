#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

void get_rand_str(char dst[],int num)
{
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i,str_len = 0;
    char random_char[10] = {0};
    str_len = strlen(str);
    srand((unsigned int)time((time_t *)NULL));
    for(i = 1; i <= num; i++){
        sprintf(random_char,"%c",str[(rand()%str_len)]);//rand()%str_len
        strcat(dst, random_char);
    }
}

int main()
{
    char postgres_passwd[140]={0};//receive 140 string array
    char mqtt_broker_passwd[100]={0};//receive 140 string array

    get_rand_str(postgres_passwd, 10);
    printf("POSTGRES_PASSWORD=%s\n", postgres_passwd); //output 

    sleep(1);
    get_rand_str(mqtt_broker_passwd, 10);
    printf("MQTT_PASSWORD=%s\n", mqtt_broker_passwd); //output 

    return 0;
}
