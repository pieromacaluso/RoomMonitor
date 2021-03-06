/**
 * File creato per contenere le varie funzioni di validazioni degli input.
 * Valore di ritorno: 1 in caso di errore, 0 in caso di successo
 * */

#include "validation.h"
#include <limits.h>

/**
 * ID string validation. It must be a number
 * @param id    ID schedina
 * @return int  0 if validated, 1 otherwise
 */
int idValidation(char *id) {
    unsigned i;
    if (strlen(id) > 5 || strlen(id) == 0) {
        return 1;
    }
    for (i = 0; i < strlen(id); i++) {
        if (!(id[i] >= '0' && id[i] <= '9')) {
            return 1;
        }
    }
    return 0;
}

/**
 * SSID AP validation. In our case we decided to be strict and allows only alfanumerical SSID
 * @param ssid_ap   SSID string
 * @return int  0 if validated, 2 otherwise
 */
int ssidApValidation(char *ssid_ap) {
    unsigned i;

    if (strlen(ssid_ap) == 0 || strlen(ssid_ap) > 31)
        return 2;

    for (i = 0; i < strlen(ssid_ap); i++) {
        // Non possibili caratteri speciali e < >
        if (!isalnum((unsigned char) ssid_ap[i])) {
            return 2;
        }
    }
    return 0;
}

/**
 * Password validation. It must contain at least one lowercase letter, one uppercase letter and a number. Its length
 * must be between 8 and 63
 * @param pass  Password string
 * @return int  0 if validated, 3 otherwise
 */
int passwordApValidation(char *pass) {
    int i;
    int min = 0, Maius = 0, num = 0;

    if (strlen(pass) < 8 || strlen(pass) > 63)
        return 3;

    for (i = 0; i < strlen(pass); i++) {
        if (isdigit((unsigned char) pass[i]))
            num = 1;
        if (islower((unsigned char) pass[i]))
            min = 1;
        if (isupper((unsigned char) pass[i]))
            Maius = 1;
    }

    if (num == 1 && min == 1 && Maius == 1)
        return 0;
    else
        return 3;
}

/**
 * Channel validation. It must be an integer between 0 and 13
 * must be between 8 and 63
 * @param channel   Channel string
 * @return int  0 if validated, 4 otherwise
 */
int channelValidation(char *channel) {
    int ch;

    ch = strtol(channel, NULL, 10);
    if (ch == 0)
        return 4;

    if (ch > 0 && ch <= 13)      //solo canali 2.4GHz
        return 0;
    else
        return 4;
}

/**
 * Port validation. It must be an integer between 1024 and 65535.
 * @param channel   Channel string
 * @return int  0 if validated, 8 otherwise
 */
int portValidation(char *port) {
    long p;

    p = strtol(port, NULL, 10);
    if (p < 1024 || p > 65535)
        return 8;
    return 0;
}

/**
 * SSID Server validation. In this case the only limit is given by the length of the string.
 * @param ssid_ap   SSID string
 * @return int  0 if validated, 5 otherwise
 */
int ssidServerValidation(char *ssid_server) {
    if (strlen(ssid_server) == 0 || strlen(ssid_server) > 31)
        return 5;
    else
        return 0;
}

/**
 * Password Server validation. In this case the only limit is given by the length of the string.
 * @param pass   pass string
 * @return int  0 if validated, 6 otherwise
 */
int passServerValidation(char *password_server) {

    if (strlen(password_server) < 8 || strlen(password_server) > 63)
        return 6;
    else
        return 0;
}

/**
 * IP Validation. This is the most complex one.
 * @param ip_server     IP string
 * @return int      0 if validated, 6 otherwise
 */
int ipValidation(char *ip_server) {
    unsigned i;
    int numDot = 0;
    int num;
    char *numC;
    char *tempIp = calloc(strlen(ip_server) + 1, sizeof(char));
    strcpy(tempIp, ip_server);

    //0.0.0.0 -> 255.255.255.255

    if (strlen(tempIp) < 7 || strlen(tempIp) > 15)
        return 9;

    for (i = 0; i < strlen(tempIp); i++) {
        if (!((tempIp[i] >= '0' && tempIp[i] <= '9') || tempIp[i] == '.'))
            return 9;
        //Conteggio numero punti
        if (tempIp[i] == '.') {
            numDot++;
            if (numDot > 3)
                return 9;
        }
    }
    //Verifica 4 numeri tra 0 e 255
    for (i = 0; i < 4; i++) {
        numC = strtok(tempIp, ".");
        num = atoi(numC);
        if (num < 0 || num > 255)
            return 9;
    }
    free(tempIp);
    return 0;
}


