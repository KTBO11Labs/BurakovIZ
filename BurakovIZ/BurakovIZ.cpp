#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

struct Client {
    char full_name[200];    // фамилию, имя, отчество клиента
    char phone_number[100]; // номер телефона
    char start_date[10];    // дата заключения договора
    char end_date[10];      // дата окончания договора
    float debt;             // размер задолженности
    float credit;           // допустимый кредит
};

struct Service {
    char type_name[100];    // название услуги
    int code;               // код услуги
    float tariff;           // тарифф
    char unit[10];          // единица измерения
};

struct Usage {
    char phone_number[100]; // номер телефона
    int code;               // код услуги
    char date[100];         // дата
    char time[100];         // время
    char volume[100];       // обьем использования
};

int get_month(char* date) {
    int month = 0;
    int reader = 0;
    for (int i = 0; i < 6; i++) {
        if (reader) {
            if (date[i] == '.') {
                break;
            }
            month = month * 10 + static_cast<int>(date[i]) - 48;
        }
        if (date[i] == '.') {
            reader = 1;
        }
    }

    return month;
}

int main() {
    setlocale(LC_ALL, "Rus");

    struct Client clients[1000];
    struct Service services[1000];
    struct Usage usages[1000];

    FILE* Param_file = fopen("Param.ini", "r");
    if (Param_file == NULL) {
        printf("Не удается открыть файл Param.ini\n");
    }
    FILE* Report_file = fopen("Report.txt", "w");
    FILE* Clients_file = fopen("Clients.txt", "r");
    if (Clients_file == NULL) {
        printf("Не удается открыть файл Clients.txt\n");
    }
    FILE* Service_file = fopen("Service.txt", "r");
    if (Service_file == NULL) {
        printf("Не удается открыть файл Service.txt\n");
    }
    FILE* Usage_file = fopen("Usage.txt", "r");
    if (Usage_file == NULL) {
        printf("Не удается открыть файл Usage.txt\n");
    }

    char smonth[10];
    fgets(smonth, 2, Param_file);
    int month = atoi(smonth);

    int i = 0;
    char str[1000];
    while (fgets(str, 1000, Clients_file)) {       // Чтение из файла Clients.txt

        char* buffer = strtok(str, ",");
        strcpy(clients[i].full_name, buffer);

        buffer = strtok(NULL, ",");
        strcpy(clients[i].phone_number, buffer);

        buffer = strtok(NULL, ",");
        strcpy(clients[i].start_date, buffer);

        buffer = strtok(NULL, ",");
        strcpy(clients[i].end_date, buffer);

        buffer = strtok(NULL, ",");
        clients[i].debt = atof(buffer);

        buffer = strtok(NULL, ",");
        clients[i].credit = atof(buffer);

        i++;
    }

    i = 0;
    while (fgets(str, 1000, Service_file)) {       // Чтение из файла Service.txt

        char* buffer = strtok(str, ",");
        strcpy(services[i].type_name, buffer);

        buffer = strtok(NULL, ",");
        services[i].code = atoi(buffer);

        buffer = strtok(NULL, ",");
        services[i].tariff = atof(buffer);

        buffer = strtok(NULL, ",");
        strcpy(services[i].unit, buffer);

        i++;
    }

    i = 0;
    while (fgets(str, 1000, Usage_file)) {       // Чтение из файла Service.txt

        char* buffer = strtok(str, ",");
        strcpy(usages[i].phone_number, buffer);

        buffer = strtok(NULL, ",");
        usages[i].code = atoi(buffer);

        buffer = strtok(NULL, " ");
        strcpy(usages[i].date, buffer);

        buffer = strtok(NULL, ",");
        strcpy(usages[i].time, buffer);

        buffer = strtok(NULL, ",");
        strcpy(usages[i].volume, buffer);

        i++;
    }

    int client_usages[10]; // коды услуг которые использовал конкретный пользователь
    int icu = 0;    // итератор массива кодов
    int ind = 0;    // совпадения услуг
    int u = 1;      // проверка на вывод ФИО и \n после
    int clk = 0;     // количество найденных клиентов
    for (int i = 0; i < 10; i++) {
        u = 1;
        for (int j = 0; j < 10; j++) {
            if (strcmp(clients[i].phone_number, usages[j].phone_number) == 0) {
                if (get_month(usages[j].date) == month) {
                    if (u == 1) {
                        u = 2;
                        fprintf(Report_file, "%s : ", clients[i].full_name);
                        clk++;
                        icu++;
                        client_usages[0] = usages[j].code;
                    } 
                    else {
                        ind = 0;
                        for (int k = 0; k < icu; k++) {
                            if (usages[j].code == client_usages[k]) {
                                ind++;
                            }
                        }
                        if (ind == 0) {
                            client_usages[icu] = usages[j].code;
                            icu++;
                        }
                    }
                }
            }
        }
        for (int z = 0; z < icu; z++) {
            for (int x = 0; x < 10; x++) {
                if (services[x].code == client_usages[z]) {
                    fprintf(Report_file, "%s", services[x].type_name);
                    if (z + 1 != icu) {             // постановка запятой кроме последней
                        fprintf(Report_file, ", ");
                    }
                }
            }
        }

        if (u == 2) {
            fprintf(Report_file, "\n");
        }
        
        icu = 0;
    }
    if (clk == 0) {
        fprintf(Report_file, "Не найдено ни одного использования услуг в %d месяце", month);
    }

    fclose(Report_file);
    fclose(Param_file);
    fclose(Clients_file);
    fclose(Service_file);
    fclose(Usage_file);

    return 0;
}