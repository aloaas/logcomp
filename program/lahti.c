/*Fail:LAHTI.c*/
/*20.05.2001 kell 13.25*/
/*See asendab valemis "funktsioonmuutujad" .*/
/*Programmi sisendtekstis eeldame,et parameetrid on ühekohalised stringid ehk char-tüüpi.*/
/*Rajasid ja tingimuste LOEtelu eraldab semikoolon.*/
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define nimetusi 92
#define arg_vaartustus_pikkus 20
char *puhver;/*siin hoitakse tex-i kujulist algteksti*/
char **valem, *fail1 = "katse2.txt", *fail2 = "saadu2.txt", mark;
int peavalem;/*Mis järjekorranumbri all on massiivis **valem esindatud peavalem*/
int valemitearv = 0;/*Palju on algtekstis valemeid*/
int suurus[50];/*Valemite piirarv*/
/*Valemeid on "valemitearv" tükki.Masiivis suurus on seega
tähtsad suurus[0],...,suurus[valemitearv-1] */
int P_length;/*kompileerimiseks!!*/
FILE *nimi1 = NULL, *Logi = NULL;/*kompileerimiseks*/
FILE *nimi2 = NULL;

/*prototüübid*/
char *ASENDA(char *sona, char *mis, char *millega);
char *ASENDA_ESIMENE(char *sona, char *mis, char *millega);


char *PARAMETRISEERI(char n, char *vaartus, char *valem);

void VOTAMALU(void);

void LEIAPEAVALEM(void);

void LEIAPIKKUSED(void);

char *JARRAY(char *pealkiri);

void KORRIGEERI_soned(void);

char *EEMALDA_OVERLINE(char *sona);

char *VISKA_LOPUST_ALATES(char *sona, char *visatav);

void LAHTI(int a, int b, char **d, int paramvaartustusi);

void LOE(void);

void PRINDI_VALEMID(void);

char* KONTROLLI_PIIRITLEJA_OLEMASOLU(int i, int variandid_suurus, char variandid[], char* kontrollitav);

char* LEIA_ULEMINE_PIIRITLEJA(char* piiritleja_identifikaator, char* koopia);
char* LEIA_ALUMINE_PIIRITLEJA(char* piiritleja_identifikaator, char* koopia);


//------ ALO AASM�E LOODUD----------//
//leitakse forall esimesest elemendist alumine piiritleja
char* LEIA_ALUMINE_PIIRITLEJA(char* piiritleja_identifikaator, char* koopia) {
	char *esimene_osadena;  //viimane parameeter piiritleja j�rgi poolitatud

	esimene_osadena = strtok(koopia, piiritleja_identifikaator);//see on esimene piiritleja osa
	
	char* piir_koos;
	int piiri_pikkus = strlen(esimene_osadena) + strlen(piiritleja_identifikaator) + 1;
	piir_koos = malloc(piiri_pikkus);
	piir_koos[piiri_pikkus-1] = '\0';
	strcpy(piir_koos, esimene_osadena);
	strcat(piir_koos, piiritleja_identifikaator);
	if(strcmp(piiritleja_identifikaator, "\\") == 0) {
		char* piiritleja_osa;
		piiritleja_osa = malloc(5);
		piiritleja_osa[3] = '\0';
		strcpy(piiritleja_osa, "leq");
		piiri_pikkus = strlen(esimene_osadena) + strlen(piiritleja_identifikaator) +  strlen(piiritleja_osa) + 1;
		piir_koos = realloc(piir_koos, piiri_pikkus);
		piir_koos[piiri_pikkus-1] = '\0';
		strcat(piir_koos, piiritleja_osa);
	}

	return piir_koos;
}



//leitakse forall viimasest elemendist (eraldatuna komadega) �lemine piiritleja
char* LEIA_ULEMINE_PIIRITLEJA(char* piiritleja_identifikaator, char* koopia) {
	char* esimene;
	char* teine;
	char* piiritleja;
	char *viimane_osadena;  //viimane parameeter piiritleja j�rgi poolitatud
	piiritleja = malloc(2);
	piiritleja[0] = piiritleja_identifikaator[0];
	piiritleja[1] = '\0';
	viimane_osadena = strtok(koopia, piiritleja_identifikaator); //parameeter ise
	viimane_osadena = strtok(NULL, piiritleja_identifikaator); //see on esimene piiritleja osa
	
	//k�ik s�mbolid kuni esimese piiritleja_identifikaator esinemiseni l�hevad viimase parameetri alla
	while (viimane_osadena != NULL){
		
		esimene = malloc(strlen(piiritleja));
		strcpy(esimene, piiritleja);
		teine = malloc(strlen(viimane_osadena));
		strcpy(teine, viimane_osadena);
		piiritleja = (char *) malloc(3 + strlen(esimene)+ strlen(teine) +1 );
		if (strcmp(esimene, piiritleja_identifikaator)==0){
			strcpy(piiritleja, esimene);
			strcat(piiritleja, teine);
		} else {
		strcpy(piiritleja, esimene);
		strcat(piiritleja, piiritleja_identifikaator);
		strcat(piiritleja, teine);
		}

		viimane_osadena = strtok(NULL, piiritleja_identifikaator);
	}
	return piiritleja;
}



/*------------------------------------------------------------------------*/
int LEIA_LOPU_SULG(char *valem, int i, char sulg) {
/*oleme '{'-l.Tagastada temale vastava '}' "kaugus"
s.t. sulgude sisse jääva valemi pikkus*/
/*vôi oleme ':'-l ja tagastada temale vastava ')' "kaugus"*/
    int sulge = 1;/**/
    while ((sulge != 0) && (valem[i] != '\0')) {
        if (valem[i] == '\0') {
            printf("Sulgude paarsus ei klapi!!!");
            getchar();
            abort();
        }
        if (((valem[i] == '{') && (sulg == '{')) || ((valem[i] == '(') && (sulg == '('))) sulge++;
        if (((valem[i] == '}') && (sulg == '{')) || ((valem[i] == ')') && (sulg == '('))) sulge--;
        i++;
    }
    return i;
/*valem[i] on nüüd tegelikult peale '}'-u järgmisel sümbolil*/
}/*LEIA_LOPU_SULG*/
/*------------------------------------------------------------------------*/
void LOE(void) {
    int i = 0, k, l, kaksdollarit;
/*puhver[i] on nüüd 1.se valemi 1.dollar*/
    for (k = 0; k < valemitearv; k++) {
        while (puhver[i] != '$') i++;
/*oleme valemi k.nda dollari peal*/
        i++;
        if (puhver[i] == '$') kaksdollarit = 1; else kaksdollarit = 0;
        if (kaksdollarit) i++;
/*nüüd kindlasti puhver[i] on esimene valemi element*/
        for (l = 0; l < suurus[k]; l++) {
            valem[k][l] = puhver[i];
            i++;
        }
/*nüüd puhver[i] 1.sel l~opetaval dollaril*/
        i++;
        if (kaksdollarit) i++;
    }/*1.for*/
}/*LOE()*/
/*------------------------------------------------------------------------*/
char *ASENDA(char *sona, char *mis, char *millega) {/*ASENDAda stringis sona osavalem
'mis' k�ik esinemised osavalemiga 'millega'*/
    char *tulem, *tulem1;
/*siin jooksev valem,mida muudetakse,eemaldatakse*/
    int mitmes;
    tulem1 = sona;
    while (strstr(tulem1, mis) != NULL) {
        tulem = (char *) malloc(strlen(tulem1) + 30);
        memset(tulem, '\0', strlen(tulem1) + 30);
/*siia hakkame kopeerima teisenduse tulemust*/
        mitmes = strstr(tulem1, mis) - tulem1;
/*mitmendast positsioonist algab asendatav osavalem*/
        strncpy(tulem, tulem1, mitmes);
/*pannakse tulemusse k~oik,mis asub eespool asendatavat osa*/
        strcat(tulem, millega);
/*pannakse tulemusse asendus*/
        strcat(tulem, tulem1 + strlen(mis) + mitmes);
/*lisame l~oppu ülejäänud osa peale asendatavast osavalemist*/
        tulem1 = tulem;
    }
    return (tulem1);
}/*ASENDA*/

/*Alo loodud modifikatsioon meetodist ASENDA*/
char *ASENDA_ESIMENE(char *sona, char *mis, char *millega) {/*ASENDAda stringis sona osavalem 
'mis' osavalemiga 'millega' ainult �ks kord*/
    char *tulem, *tulem1;
/*siin jooksev valem,mida muudetakse,eemaldatakse*/
    int mitmes;
    tulem1 = sona;
    if (strstr(tulem1, mis) != NULL) {
        tulem = (char *) malloc(strlen(tulem1) + 30);
        memset(tulem, '\0', strlen(tulem1) + 30);
/*siia hakkame kopeerima teisenduse tulemust*/
        mitmes = strstr(tulem1, mis) - tulem1;
/*mitmendast positsioonist algab asendatav osavalem*/
        strncpy(tulem, tulem1, mitmes);
/*pannakse tulemusse k~oik,mis asub eespool asendatavat osa*/
        strcat(tulem, millega);
/*pannakse tulemusse asendus*/
        strcat(tulem, tulem1 + strlen(mis) + mitmes);
/*lisame l~oppu ülejäänud osa peale asendatavast osavalemist*/
        tulem1 = tulem;
    }
    return (tulem1);
}/*ASENDA_ESIMENE*/

//------------ALO AASM�E LOODUD------------------//
//kontrollitakse, kas s�nes kontrollitav leidub t�ht massiivist variandid
char* KONTROLLI_PIIRITLEJA_OLEMASOLU(int i, int variandid_suurus, char variandid[], char* kontrollitav){
	char* osadena;
	if (i == variandid_suurus){
		return "";
	}
	char* proovitav;
	proovitav = malloc(2);
	proovitav[0] = variandid[i];
	proovitav[1] = '\0';
	osadena = strtok(kontrollitav, proovitav); //poolitame kontrollitava s�ne proovitava t�he j�rgi
	osadena = strtok(NULL, proovitav); 
	if (osadena != NULL) { //kui poolitamise tulemusena jagunes s�na kontrollitav rohkem kui �heks elemendiks, siis antud piiritleja pidi olemas olema
		return proovitav;
	}
	return KONTROLLI_PIIRITLEJA_OLEMASOLU(i+1,variandid_suurus, variandid, kontrollitav);
}


/*--------------------------------------------------------------*/
char *PARAMETRISEERI(char n, char *vaartus, char *valem) {
/*kôik 'valem'-is esinevad parameetri 'n' esinemised
ASENDAda sônega 'vaartus'*/
/*ASENDAma peab ainult x_{},y_{},z_{},ja ...*/
    int valemipikkus, oli, viimane, i, j;
    char *uusvalem = NULL;
    char sulg;
    uusvalem = (char *) malloc(strlen(valem) + 200);
    memset(uusvalem, '\0', strlen(valem) + 200);
/*.....\bigvee_{
\bigwedge_{
\bigoplus_{*/
/*Leida tuleb kôik '_{',millele ei eelne:
1)atmost
2)atleast
3)level*/
    valemipikkus = strlen(valem);/*[0] kuni [valemipikkus-1]*/
    i = 0;/*1.sel 'valem'-i elemendil*/
/*kontrollima peame,kas me ei väärtusta funktsiooni
formaalseid parameetreid*/
    if ((strstr(valem, "}=") != NULL) || (strstr(valem, ")=") != NULL)) {
        i = strstr(valem, "=") - valem + 1;
    }
    if (i) for (j = 0; j < i; j++) uusvalem[j] = valem[j];
    while (i < valemipikkus) {
        sulg = '?';
/*kui tegemist funktsiooniga*/
        if ((valem[i] == '_') && (valem[i + 1] == '{') &&
            ((valem[i - 1] == 'f') || (valem[i - 1] == 'g') || (valem[i - 1] == 'h') || (valem[i - 1] == 'F') ||
             (valem[i - 1] == 'G') || (valem[i - 1] == 'H'))) {
            while (valem[i] != '}') {
                uusvalem[strlen(uusvalem)] = valem[i];
                i++;
            }
            uusvalem[strlen(uusvalem)] = valem[i];
            i++;
/*nüüd kui olen '('-l,siis vaja väärtustada*/
            if (valem[i] == '(') {
                uusvalem[strlen(uusvalem)] = '(';
                sulg = '(';
            }
        }/*if*/
/*----------------*/
        if (valem[i] == ':') sulg = '(';
        if ((valem[i] == '_') && (valem[i + 1] == '{')) sulg = '{';
        if ((sulg != '?') && ((i < 4) ||
                              (((valem[i - 2] != 's') || (valem[i - 1] != 't'))
                               && ((valem[i - 2] != 'e') || (valem[i - 1] != 'l'))
                              )
        )
                ) { /*siis siin peab asendama,kui leiame*/
            if (sulg == '{') strcat(uusvalem, "_{");
            if ((sulg == '(') && (valem[i] != '(')) strcat(uusvalem, ":");
            if (sulg == '{') i++;
            i++;
            viimane = LEIA_LOPU_SULG(valem, i, sulg) - 2;
/*valem [i] on esimene ja
valem[viimane] on viimane sümbol sulgude sees*/
            while (i <= viimane) {
                oli = 0;
                if ((valem[i] == 'm') && (valem[i + 1] == 'i') && (valem[i + 2] == 'n')) {
                    strcat(uusvalem, "min");
                    i = i + 3;
                    oli = 1;
                }
                if ((valem[i] == 'm') && (valem[i + 1] == 'a') && (valem[i + 2] == 'x')) {
                    strcat(uusvalem, "max");
                    i = i + 3;
                    oli = 1;
                }
                if ((valem[i] == 'i') && (valem[i] == 'n')) {
                    strcat(uusvalem, "in");
                    i = i + 2;
                    oli = 1;
                }
                if ((valem[i] == 'l') && (valem[i + 1] == 'e') && (valem[i + 2] == 'q')) {
                    strcat(uusvalem, "leq");
                    i = i + 3;
                    oli = 1;
                }
                if ((valem[i] == 'p') && (valem[i + 1] == 'r') && (valem[i + 2] == 'e') &&
                    (valem[i + 3] == 'c') && (valem[i + 4] == '^') && (valem[i + 5] == '+')) {
                    strcat(uusvalem, "prec^+");
                    i = i + 6;
                    oli = 1;
                }
                if ((valem[i] == 'p') && (valem[i + 1] == 'r') && (valem[i + 2] == 'e') &&
                    (valem[i + 3] == 'c') && (valem[i + 4] == '^') && (valem[i + 5] == '*')) {
                    strcat(uusvalem, "prec^*");
                    i = i + 6;
                    oli = 1;
                }
                if ((valem[i] == 'p') && (valem[i + 1] == 'r') && (valem[i + 2] == 'e') &&
                    (valem[i + 3] == 'c')) {
                    strcat(uusvalem, "prec");
                    i = i + 4;
                    oli = 1;
                }
                if ((valem[i] == 'n') && (valem[i] == 'e')) {
                    strcat(uusvalem, "ne");
                    i = i + 2;
                    oli = 1;
                }
                if ((valem[i] == 'g') && (valem[i] == 'e') && (valem[i + 2] == 'q')) {
                    strcat(uusvalem, "geq");
                    i = i + 3;
                    oli = 1;
                }
                if (!oli) {
/*siis peab kontrollima,kas on asendatav parameeter vôi mitte*/
                    if (valem[i] != n) {
                        uusvalem[strlen(uusvalem)] = valem[i];
                    } else {
                        uusvalem[strlen(uusvalem)] = '(';
                        strcat(uusvalem, vaartus);
                        uusvalem[strlen(uusvalem)] = ')';
                    }
                    i++;/*peab edasi ka liikuma "vanas" valemis*/
                }/*!(oli)*/
            }/*while*/
            if (sulg == '(') {
                uusvalem[strlen(uusvalem)] = ')';
                i++;
            }
            if (sulg == '{') {
                uusvalem[strlen(uusvalem)] = '}';
                i++;
            }
        }/*if*/
        else {/*sulg=='?'*/
            uusvalem[strlen(uusvalem)] = valem[i];
            i++;
        }
    }/*while*/
    return uusvalem;
}/*PARAMETRISEERI*/
/*------------------------------------------------------------------------*/
void LEIAPIKKUSED(void) {/*Leiab k"ikide selles failis olevate valemite sümbolite arvu.*/
    int pikkus, kaksdollarit, i = 0;
/*'puhver'-is on algtekst*/
    while (i < strlen(puhver)) {
        while ((puhver[i] != '$') && (i < strlen(puhver))) {
            i++;
        }
        kaksdollarit = 0;
        if (i == strlen(puhver)) return;
        else { /*Oleme valemi algust tähistaval dollaril*/
            i++; /*puhver[i] on kas teine dollar v~oi valemi algus*/
            if (puhver[i] == '$') { /*Siis enne valemit kaks dollarit*/
                kaksdollarit = 1;
/*Peame meeles,mitme dollariga on valem ümbritsetud*/
                i++;
            }
/*nüüd kindlasti esimesel valemi sümbolil*/
            pikkus = 0;
            while (puhver[i] != '$') {
                i++;
                pikkus++;
            }
/*Oleme kindlasti dollaril*/
            if (kaksdollarit) i++;
            i++; /*see nagunii*/
            suurus[valemitearv] = pikkus;
            valemitearv++;
        }
    }/*Oleme j~oudnud faili l~oppu*/
    return;
}/*LEIAPIKKUSED*/
/*------------------------------------------------------------------------------*/
void VOTAMALU(void) {/*Mälueraldus masiivile **valem*/
    int i;
    valem = (char **) malloc(valemitearv * sizeof(char *));/*Peavektorile*/
    for (i = 0; i < valemitearv; i++) { /*Valemitele*/
        valem[i] = (char *) malloc(suurus[i] * sizeof(char) + 2);
        memset(valem[i], '\0', suurus[i] * sizeof(char) + 2);
    }
}

/*------------------------------------------------------------------------------*/
void LEIAPEAVALEM(void) {/*Leiab peavalemi järjekorranumbri massiivis valem*/
    int i = 0, j, leitud = -1;
/*Kui leitud=-1,siis peavalemit polegi*/
    while ((i < valemitearv) && (leitud == -1)) {
        if ((strstr(valem[i], "=") == NULL) || ((valem[i][1] == '_') && (valem[i][3] == '='))) {
            peavalem = i;
            leitud = i;
        } else {/*märk '=' om olemas*/
            j = 0;

            while (valem[i][j] != '}')j++;
            if (valem[i][j + 1] != '=') {
                leitud = i;
                peavalem = i;
            }/*if*/
        }/*else*/
        i++;
    }/*while*/
    if (leitud == -1) {
        printf("Andke ikka peavalem ka ette!");
        abort();
    }
    return;
}
/*------------------------------------------------------------------------------*/
/* tekstimassiivi sisestamine kettalt */
/*olen muutnud kompileerimiseks*/
char *JARRAY(char *pealkiri) {
    FILE *tekst = NULL;
    char *B;
    char c;
    int i;
    int k;
    struct stat *buf;
    buf = (struct stat *) malloc(sizeof(struct stat));
    if (buf == NULL) {
        abort();
    }
    tekst = fopen(pealkiri, "r");
    if (tekst == NULL) {
        abort();
    }
    if (stat(pealkiri, buf)==-1){
        abort();
    }
    k = buf->st_size;
    B = (char *) malloc(k + 10);
    if (B == NULL) {
        abort();
    }
    memset(B, '\0', k + 10);
/* fill buffer */
    rewind(tekst);
    i = 0;
    while (!feof(tekst)) {
        c = fgetc(tekst);
        B[i] = c;
        i++;
    }
    P_length = i;
    for (i = P_length; i > 0; i--) {
        if (isgraph(B[i])) {
            i++;
            B[i] = '\n';
            i++;
            B[i] = '\0';
            P_length = i;
            goto out;
        }
    }
    out:
    fclose(tekst);
    return (B);
}/*---------------------------*JARRAY-----------------------------------------*/
/*----------------------------------------------------------------------------*/
void KORRIGEERI_soned(void) {
    int i = 0;
    for (i = 0; i < valemitearv; i++) {
        valem[i] = ASENDA(valem[i], "Odd", "odd");
        valem[i] = ASENDA(valem[i], "Even", "even");
        valem[i] = ASENDA(valem[i], "Atleastone", "atleastone");
        valem[i] = ASENDA(valem[i], "at_least_one", "atleastone");
        valem[i] = ASENDA(valem[i], "At_Least_One", "atleastone");
        valem[i] = ASENDA(valem[i], "Exactlyone", "exactlyone");
        valem[i] = ASENDA(valem[i], "exactly_one", "exactlyone");
        valem[i] = ASENDA(valem[i], "Exactly_One", "exactlyone");
        valem[i] = ASENDA(valem[i], "Atmostone", "atmostone");
        valem[i] = ASENDA(valem[i], "atmost_one", "atmostone");
        valem[i] = ASENDA(valem[i], "Atmost_One", "atmostone");
        valem[i] = ASENDA(valem[i], "Atleast", "atleast");
        valem[i] = ASENDA(valem[i], "at_least", "atleast");
        valem[i] = ASENDA(valem[i], "At_Least", "atleast");
        valem[i] = ASENDA(valem[i], "Atmost", "atmost");
        valem[i] = ASENDA(valem[i], "at_most", "atmost");
        valem[i] = ASENDA(valem[i], "At_Most", "atmost");
        valem[i] = ASENDA(valem[i], "Level", "level");
        valem[i] = ASENDA(valem[i], "Treshold", "treshold");
        valem[i] = ASENDA(valem[i], "None", "none");
        valem[i] = ASENDA(valem[i], "\\Rightarrow", "\\to");
        valem[i] = ASENDA(valem[i], "\\rightarrow", "\\to");
        valem[i] = ASENDA(valem[i], "\\Leftrightarrow", "\\sim");
        valem[i] = ASENDA(valem[i], "\\leftrightarrow", "\\sim");
        valem[i] = ASENDA(valem[i], "\\wedge", "\\&");
        valem[i] = ASENDA(valem[i], "\\land", "\\&");
        valem[i] = ASENDA(valem[i], "\\lor", "\\vee");
        valem[i] = ASENDA(valem[i], "\\rbrace", "\\}");
        valem[i] = ASENDA(valem[i], "\\lbrace", "\\{");
        valem[i] = ASENDA(valem[i], "\\leftarrow", "peitu");
        valem[i] = ASENDA(valem[i], "\\leq", "magister");
        valem[i] = ASENDA(valem[i], "\\left(", "(");
        valem[i] = ASENDA(valem[i], "\\le", "korraks");
        valem[i] = ASENDA(valem[i], "korraks", "\\leq");
        valem[i] = ASENDA(valem[i], "magister", "\\leq");
        valem[i] = ASENDA(valem[i], "\\leqslant", "\\leq");
        valem[i] = ASENDA(valem[i], "\\ast", "*");
        valem[i] = ASENDA(valem[i], "\\cdot", "*");
        valem[i] = ASENDA(valem[i], "\\times", "*");
        valem[i] = ASENDA(valem[i], "\\ ", "");/*tex-i kujuline tühik*/
/*hüpped k~orvale*/
        valem[i] = ASENDA(valem[i], "\\enskip", "");
        valem[i] = ASENDA(valem[i], "\\quad", "");
        valem[i] = ASENDA(valem[i], "\\qquad", "");
        valem[i] = ASENDA(valem[i], "\\,", "");
        valem[i] = ASENDA(valem[i], "\\>", "");
        valem[i] = ASENDA(valem[i], "\\;", "");
        valem[i] = ASENDA(valem[i], "\\!", "");
        valem[i] = ASENDA(valem[i], "\\nonumber", "");
        valem[i] = ASENDA(valem[i], "& &", "");
        valem[i] = ASENDA(valem[i], "&&", "");
        valem[i] = ASENDA(valem[i], " & ", "");
        valem[i] = ASENDA(valem[i], " ", "");/*ülearused tühikud ära!*/
        valem[i] = ASENDA(valem[i], "\\\\", "");
        valem[i] = ASENDA(valem[i], "\\break", "");
        valem[i] = ASENDA(valem[i], "\\big(", "(");
        valem[i] = ASENDA(valem[i], "\\Big(", "(");
        valem[i] = ASENDA(valem[i], "\\bigg(", "(");
        valem[i] = ASENDA(valem[i], "\\Bigg(", "(");
        valem[i] = ASENDA(valem[i], "\\lgroup", "(");
        valem[i] = ASENDA(valem[i], "\\big)", ")");
        valem[i] = ASENDA(valem[i], "\\Big)", ")");
        valem[i] = ASENDA(valem[i], "\\bigg)", ")");
        valem[i] = ASENDA(valem[i], "\\Bigg)", ")");
        valem[i] = ASENDA(valem[i], "\\lgroup", ")");
        valem[i] = ASENDA(valem[i], "\\right)", ")");
        valem[i] = ASENDA(valem[i], "\\big\\}", "\\}");
        valem[i] = ASENDA(valem[i], "\\Big\\}", "\\}");
        valem[i] = ASENDA(valem[i], "\\bigg\\}", "\\}");
        valem[i] = ASENDA(valem[i], "\\Bigg\\}", "\\}");
        valem[i] = ASENDA(valem[i], "\\big\\{", "\\{");
        valem[i] = ASENDA(valem[i], "\\Big\\{", "\\{");
        valem[i] = ASENDA(valem[i], "\\bigg\\{", "\\{");
        valem[i] = ASENDA(valem[i], "\\Bigg\\{", "\\{");
        valem[i] = ASENDA(valem[i], "\\limits", "");
        valem[i] = ASENDA(valem[i], "\\neq", "\\ne");
        valem[i] = ASENDA(valem[i], "\\geq", "magster");
        valem[i] = ASENDA(valem[i], "\\ge", "korraks");
        valem[i] = ASENDA(valem[i], "korraks", "\\geq");
        valem[i] = ASENDA(valem[i], "magster", "\\geq");
        valem[i] = ASENDA(valem[i], "peitu", "\\gets");
        valem[i] = ASENDA(valem[i], "\\mid", "|");
        valem[i] = ASENDA(valem[i], "\\vert", "|");
        valem[i] = ASENDA(valem[i], "\\arrowvert", "|");
        valem[i] = ASENDA(valem[i], "\\bracecert", "|");
        valem[i] = ASENDA(valem[i], "\\gets", "\\leftarrow");
        valem[i] = ASENDA(valem[i], "[", "x_{");
        valem[i] = ASENDA(valem[i], "\\lbrack", "x_{");
        valem[i] = ASENDA(valem[i], "]", "}");
        valem[i] = ASENDA(valem[i], "\\rbrack", "}");
        valem[i] = ASENDA(valem[i], "\\thicksim", "\\sim");
        valem[i] = ASENDA(valem[i], "\\indent", "");/*taane*/
        valem[i] = ASENDA(valem[i], "\\lnot", "\\neg");
/*\overline asendamisel \neg-iga tekib sulgudega probleem */
        valem[i] = EEMALDA_OVERLINE(valem[i]);
        valem[i] = VISKA_LOPUST_ALATES(valem[i], "\\eqno");
        valem[i] = VISKA_LOPUST_ALATES(valem[i], "\\leqno");
    }/*for-tsükkel*/
}/*KORRIGEERI_soned*/
/*--------------------------------------------------------------------------*/
char *EEMALDA_OVERLINE(char *sona) { /*valem[i]-ga tegelikult teeb midagi*/
    char *tulem;
    int i, j, k, sulge;
    tulem = (char *) malloc(strlen(sona) + 3);
    memset(tulem, '\0', strlen(sona) + 3);
    i = 0;
/*sellest positsioonist alates otsime s~ona "overline"*/
    j = 0;
/*mitmendale kohale paneme*/
    while (i < strlen(sona)) {
        if (strstr(sona + i, "overline") != sona + i) {
/*s.t.kui pole sona+i alguses*/
            tulem[j] = sona[i];
            j++;
            i++;
        } else {
            tulem[j] = 'n';
            tulem[j + 1] = 'e';
            tulem[j + 2] = 'g';
            j = j + 3;
            i = i + 8;
/*esimene sümbol peale "overline"-i*/
            if (sona[i] == '{') {
                sona[i] = '(';
/*loogelise sulu asendus tavalisega*/
/*aga sona tulem'isse veel ei kirjuta*/
                sulge = 1;
                k = i + 1;
/*järgmisele elemendile sonas*/
                while (sulge) {
                    if (sona[k] == '{') sulge++;
                    if (sona[k] == '}') sulge--;
                    k++;
                }
                sona[k - 1] = ')';
            }/*if*/
        }/*else*/
    }/*while*/
    return tulem;
}
/*EEMALDA_OVERLINE*/
/*-------------------------------------------------------------------------*/
char *VISKA_LOPUST_ALATES(char *sona, char *visatav) {
    char *tulem;
    tulem = (char *) malloc(strlen(sona) + 1);
    memset(tulem, '\0', strlen(sona) + 1);
    if (strstr(sona, visatav) == NULL) return sona;
    else {
        strcat(tulem, sona);
        tulem[strstr(sona, visatav) - sona] = '\0';
        return tulem;
    }
}/*VISKA_LOPUST_ALATES*/
/*--------------------------------------------------------------------------*/
void LAHTI(int a, int b, char **d, int paramvaartustusi) {
/*Kirjutame faili valemi järjekorranumbriga a massiivist valem[][],alustades
kirjutamist positsioonist b s.t.valem[a][b],valem[a][b+1],jne.*/
/*d on parameetrite väärtustuste algaadress*/
/*d ühe liikme s.t.väärtustuse pikkus ei tohi ületada 5 sümbolit.
Teeme sellise kitsenduse*/
/*paramvaartustusi näitab,palju parameetreid tegelt väärtustada tuleb.
Kui paramvaartustusi=0,siis tuleb
kirjutada valem[a] ühelegi parameetrile väärtust andmata.*/
    char *uusvalem = NULL, *uusvalem1 = NULL, *sonake;
    char forall_kontroll[8] = "\\forall("; //forall_indeksi j�rgi saab kontrollida, kas olema forall osa juures
    int forall_indeks = 0; //Vajalik selleks, et kontrollida, kas oleme valemis l�binud teksti muutujas "forall_kontroll"
    int forall_lopp;
    int uuepikkus;
    int kohtabi, sulge;
    int mitu, i = 0, j;
    int parameetreid = 0;
/*Kui palju on valem[a]-l parameetreid*/
    char *paramnimi;
/*Valem[a] parameetrid esinemise järjekorras*/
    int koht = b;/*Näitab,mitmendast kohast alates on veel töötlemata ja
faili panemata,käesolevas valemis*/
/*Sellest positsioonist alustame valemi kirjutamist faili. */
    char **paramvaartused;
/*Kui valem[a]-s on vaja parameetreid väärtustada,
siis need asuvad siin*/
    int parasid = 0, leidmisabi, i1;
    int paramalgus, leitud, lopumargend;
/*Määrame kindlaks esinevad parameetrite nimed ja kui palju neid on*/
    kohtabi = 4;
/*mitmendast kohast alates tasub otsima
hakata f-ni indeksit lôpetavat '}'*/
    if (b) {/*Siis pole tegu peavalemiga*/
        while (valem[a][kohtabi] != '}') kohtabi++;
        kohtabi++;
/*on kas '=' vôi parameetrite leidumisel '('*/
/*Kui parameetreid on,siis need 1. neist on positsioonil kohtabi+1*/
        if (valem[a][kohtabi] == '(') { /*Vastasel juhul oleme "="-il*/
            kohtabi++;
            paramalgus = kohtabi;/*1.sel parameetril*/
            while (valem[a][kohtabi] != ')') {
                if (valem[a][kohtabi] == ', ') {
                    parameetreid++;
                }
                parameetreid++;
/*Neid on ühe vôrra rohkem kui komasid*/
                kohtabi++;
            }/*while*/
            kohtabi = paramalgus;
            paramnimi = (char *) malloc((parameetreid + 1) * sizeof(char));
            memset(paramnimi, '\0', (parameetreid + 1) * sizeof(char));
            for (i = 0; i < parameetreid; i++) {
                paramnimi[i] = valem[a][kohtabi];
                kohtabi++;
                kohtabi++;
            }/*for*/
        }/*valem[a] parameetrid leitud ja salvestatud*/
        if (parameetreid < paramvaartustusi) {
            printf("Parameetrite arv ei klapi");
            abort();
        }
    }/*if (b)*/
/*Nüüd tuleb valem[a] parameetrite esinemiskohad asendama
väljakutse kohast saadetud väärtustustega*/
    if (!b) uusvalem1 = valem[a];/*ei pea PARAMETRISEERIma*/
    else {
        uusvalem1 = valem[a];
        for (i = 0; i < paramvaartustusi; i++) {
            uusvalem = (char *) malloc(strlen(uusvalem1) + 200);
            memset(uusvalem, '\0', strlen(uusvalem1) + 200);
            uusvalem = PARAMETRISEERI(paramnimi[i], d[i], uusvalem1);
/*väärtustused asuvad massiivis '**d'*/
            uusvalem1 = uusvalem;
        }/*for*/
    }/*else*/
    uuepikkus = strlen(uusvalem1);
/*Valem, mida me läbima hakkame on 'uusvalem1'*/
    while (koht < uuepikkus) { /*uuepikkus v~ordub a.nda valemist saadud
valemi (uusvalem1) sümbolite arvuga*/
/*Senikaua,kuni pole j~oudnud l~opetavale dollarile*/
        if (((uusvalem1[koht] == 'f') || (uusvalem1[koht] == 'g')
             || (uusvalem1[koht] == 'h') || (uusvalem1[koht] == 'F')
             || (uusvalem1[koht] == 'G') || (uusvalem1[koht] == 'H'))
            && (uusvalem1[koht + 1] == '_')) {
            leitud = 1;
        } else {
        	leitud = 0;
        	if (forall_indeks == 0) {
        		sonake = (char *) malloc(9);
            	memset(sonake, '\0', 9);
            	sonake[0] = uusvalem1[koht];
			}

//------------------------------ ALO AASM�E OSA ALGUS --------------------------------//
        //Forall algus            
        if (forall_indeks == 7) { 

		forall_indeks = 0; //Juhul, kui valemis on rohkem kui �ks \forall, siis saab peale esimese teisendust j�rgmist otsida
		forall_lopp = LEIA_LOPU_SULG(uusvalem1, koht+1, '('); //leitakse indeks, kus l�peb \forall(...)
		int forall_algus = koht - 7; //liigume tagasi /forall algusindeksini
		int forall_pikkus = forall_lopp-forall_algus;
		char forall[forall_pikkus+1]; //+1 '\0' jaoks
		int forall_sisu_pikkus = forall_pikkus-8; //8 on "\forall(" pikkus
		char forall_sisu[forall_pikkus-8]; //pikkus - "\forall(" - l�pusulg + '\0'
		char forall_sisu_koopia[forall_pikkus-8]; //teiseks l�bimiseks, kui toimub laiendamine
		for (int j = 0; j < forall_pikkus; j++) {
			forall[j] = uusvalem1[j+forall_algus];
		}
		forall[forall_pikkus] = '\0';
		for (int j = 8; j < forall_pikkus-1; j++) { //forall_pikkus-1 et sulge mitte arvestada
			forall_sisu[j-8] = forall[j];
			forall_sisu_koopia[j-8] = forall[j];
		}
		uusvalem1 = ASENDA_ESIMENE(uusvalem1, forall, ""); //l�ppvalemis peaks olema \forall(...) asemel laiendatud sisu
		koht = koht - 7;
		uuepikkus = strlen(uusvalem1);

		forall_sisu[forall_sisu_pikkus-1] = '\0';
		forall_sisu_koopia[forall_sisu_pikkus-1] = '\0';

		char* delim; //\forall sulgude sees on parameetrid eraldatud komadega
		delim = malloc(2);
		delim[0] = ',';
		delim[1] = '\0';
		char *alumine_piir;
		char *piiritleja = ""; // On tehtud eeldus, et parameetrite piiritleja algab alati s�mboliga '\', '<'
		char *token = strtok(forall_sisu, delim); //token forall sisu sujuvaks l�bimiseks
		char *viimane; //viimane parameeter, mile k�ljes on ka piiritleja
		char *esimene;
		int esimene_piirita = 0;
		char *koopia;
		char *piiritleja_identifikaator;
		char piiritleja_identifikaatorid[2] = {'\\', '<'};
		int massiivi_suurus = sizeof(piiritleja_identifikaatorid) / sizeof(char);
		//piiritleja leidmine viimasest parameetrist
		if(token != NULL) {
			esimene = malloc(strlen(token) + 1);
			strcpy(esimene, token);
			esimene[strlen(token)] = '\0';
			koopia = malloc(strlen(token) + 1);
			strcpy(koopia, token);
			koopia[strlen(token)] = '\0';
			piiritleja_identifikaator = KONTROLLI_PIIRITLEJA_OLEMASOLU(0, massiivi_suurus, piiritleja_identifikaatorid, koopia);
			strcpy(koopia, token);
			//Kui tegemist ei ole viimase elemendiga ja leidub piiritleja
			if (strcmp(piiritleja_identifikaator, "") != 0 ) {
				if(strlen(token) != forall_sisu_pikkus) {
					alumine_piir = LEIA_ALUMINE_PIIRITLEJA(piiritleja_identifikaator, koopia);
				} else {
					esimene_piirita = 0;
				}
			} else {
				alumine_piir = malloc(3);
				strcpy(alumine_piir, "1<");
				alumine_piir[2] = '\0';
				int esimene_pikkus = strlen(token) + strlen(alumine_piir);
				esimene = malloc(esimene_pikkus + 1);
				strcpy(esimene, alumine_piir);
				strcat(esimene, token);
				esimene[esimene_pikkus] = '\0';
				esimene_piirita = 1;
			}
			
		}
		strcpy(forall_sisu, forall_sisu_koopia);
		token = strtok(forall_sisu, delim); //token forall sisu sujuvaks l�bimiseks
		while(token != NULL) {
			koopia = malloc(strlen(token) + 1);
			strcpy(koopia, token);
			token = strtok(NULL, delim);
			if (token == NULL) { //viimane
				viimane = malloc(strlen(koopia) + 1); 
				strcpy(viimane, koopia);		
				//Tehakse eeldus, et viimase elemendi k�ljes on �ks neljast piiritleja identifikaatorist, mille kaudu leida piiritleja
				piiritleja_identifikaator = KONTROLLI_PIIRITLEJA_OLEMASOLU(0, massiivi_suurus, piiritleja_identifikaatorid, koopia);
				strcpy(koopia, viimane);		
				if (strcmp(piiritleja_identifikaator, "") != 0) {
					piiritleja = LEIA_ULEMINE_PIIRITLEJA(piiritleja_identifikaator, koopia);
				}
				break;
			}
		}
		//j�rgnevalt otsitakse �les tekst, millele lisatakse antud parameetreid k�lge
		//on tehtud eeldus, et see algab s�mboliga '\'
		//kui ei alga s�mboliga '\', siis pannakse '\bigwedge' �mber
		int kaldkriips_esinenud = 0;
		int korrutatava_koht = koht;
		int korrutatava_algus = koht; //kui esialgses valemis pole \, siis kasutatakse seda muutujat kui korrutatava sisu algusindeksina
		while(uusvalem1[korrutatava_koht] != '(') {
			if (uusvalem1[korrutatava_koht] == '\\') {
				kaldkriips_esinenud = 1;
				korrutatava_algus = korrutatava_koht;
			}
			if(uusvalem1[korrutatava_koht] == ')'){
				korrutatava_algus = korrutatava_koht+1;
			}
			korrutatava_koht += 1;
		}
		int korrutatava_lopp;
		int korrutatava_pikkus;
		char* korrutatav;
		//Kui ei esine \\ algusega teksti peale forall'i ja enne sulgusid, siis lisatakse juurde \\bigwedge ehk konjuktsioon
		if(!kaldkriips_esinenud) {
			korrutatava_pikkus = strlen("\\bigwedge");
			korrutatav = malloc(korrutatava_pikkus+1);
			korrutatav[korrutatava_pikkus] = '\0';
			korrutatav = "\\bigwedge";
		}else{
			//Eraldame laiendatava teksti muutujasse korrutatav
			//korrutatava j�rel sulgude sees oleva sisu eraldame muutujasse korrutatava_sisu
			korrutatava_lopp = korrutatava_koht-1;
			korrutatava_pikkus = korrutatava_lopp-korrutatava_algus + 1;
			korrutatav = malloc(korrutatava_pikkus+1);
			korrutatav[korrutatava_pikkus] = '\0';
			for (int j = 0; j < korrutatava_pikkus; j++) {
				korrutatav[j] = uusvalem1[j+korrutatava_algus];
			}
		
		}

		
		int korrutatava_sisu_algus;
		int korrutatava_sisu_lopp; //-1 et mitte l�pusulgu arvestada
		int korrutatava_sisu_pikkus;
		char* korrutatava_sisu;	
		//kui kaldkriipsu polnud, siis see t�hendab, et lisati juurde \\bigwedge, mist�ttu ka korrutatava sisu algus nihkub
		if(kaldkriips_esinenud) {
			korrutatava_sisu_algus = korrutatava_koht;
		} else {
			korrutatava_sisu_algus = korrutatava_algus;
		}
		korrutatava_sisu_lopp = LEIA_LOPU_SULG(uusvalem1, korrutatava_sisu_algus+1, '(')-1; //-1 et mitte l�pusulgu arvestada
		korrutatava_sisu_pikkus = korrutatava_sisu_lopp-korrutatava_sisu_algus + 1;
		korrutatava_sisu = malloc(korrutatava_sisu_pikkus+1);
		korrutatava_sisu[korrutatava_sisu_pikkus] = '\0';
		for (int j = 0; j < korrutatava_sisu_pikkus; j++) {
			korrutatava_sisu[j] = uusvalem1[j+korrutatava_sisu_algus];
		}
		
		token = strtok(forall_sisu_koopia, delim);

		char* laiendatud;
		laiendatud = malloc(1);
		laiendatud[0] = '\0';
		int laiendatud_suurus;
		int lisamalu = 6; 				//M�lu jaoks 6 lisa�hikut: }() ja '\0'
		//esimese parameetri korral paneme paneme korrutatava sisu �mber laiendatud teksti kujul + korrutatav + "_{" + parameeter + piiritleja + "}" + korrutatava sisu
		if (token != NULL) {
		 	char* laiendatud_koopia;
			laiendatud_koopia =calloc(strlen(korrutatava_sisu) + 1, 1);
			strcpy(laiendatud_koopia, korrutatava_sisu);		
			//iga j�rgneva korral paneme sarnaselt eelmise laiendatud teksti �mber uue korrutatav + "_{" + parameeter + piiritleja + "}" +  "(" + eelmine + ")"
			while(token != NULL) {
				laiendatud_suurus = strlen(korrutatav) + 3;
				
				if (esimene_piirita) {laiendatud_suurus += strlen(alumine_piir);}
				free(laiendatud);
				laiendatud = calloc(laiendatud_suurus, 1); 
				strcpy(laiendatud, korrutatav);
				strcat(laiendatud, "_{");
				if (esimene_piirita) {strcat(laiendatud, alumine_piir);}
				esimene_piirita = 1;
				
				laiendatud_suurus = lisamalu + strlen(laiendatud) + strlen(token) + strlen(laiendatud_koopia) ;
				if (strcmp(token, viimane) != 0) {laiendatud_suurus += strlen(piiritleja);}	
				
				laiendatud = realloc(laiendatud, laiendatud_suurus); // seekord on piiritleja tokeni sees
				if (!laiendatud) {
					printf("Ei leitud sobivat m�lu.");
					abort();
				}
				strcat(laiendatud, token);
				laiendatud[laiendatud_suurus-1] = '\0';		
				
				if (strcmp(token, viimane) != 0) {strcat(laiendatud, piiritleja);}
				
				strcat(laiendatud, "}");					
											
				// Iga laiendatud osa l�heb j�rgmise laiendatud osa alla
				strcat(laiendatud, "(");
				strcat(laiendatud, laiendatud_koopia);				
				strcat(laiendatud, ")");				
			
				token = strtok(NULL, delim);
				free(laiendatud_koopia);
				laiendatud_koopia = strdup(laiendatud);

		}

		}
		//leiame \\forall-le eelneva ja j�rgneva valemijupi ning liidame k�ik kokku uueks valemiks
		char algus[koht+1];
		algus[koht] = '\0';
		for (int j = 0; j < koht ; j++){
			algus[j] = uusvalem1[j];
		}
		int lopuosa_algus = korrutatava_sisu_lopp + 1;
		int lopuosa_lopp = uuepikkus;
		int lopuosa_pikkus = 0;
		if (lopuosa_lopp >= lopuosa_algus) {
			lopuosa_pikkus = lopuosa_lopp - lopuosa_algus;
			char lopuosa[lopuosa_pikkus + 1];
			lopuosa[lopuosa_pikkus] = '\0';
			for (int j = 0; j < lopuosa_pikkus ; j++){
				lopuosa[j] = uusvalem1[j + lopuosa_algus];
			}
			uusvalem1 = malloc (strlen(algus) + strlen(laiendatud) + strlen(lopuosa));
			strcpy(uusvalem1, algus);
			strcat(uusvalem1, laiendatud);
			strcat(uusvalem1, lopuosa);	
			
			uuepikkus = strlen(uusvalem1);


			free(algus);
			free(lopuosa);
			
			continue;
		}
		else {
			printf("L�puosa indeksid on valed");
			abort();
		}
		
		} else {
			//seni, kuni pole kindel, kas tegemist on \forall esinemisega, ei kirjuta uut valemit faili
        if (uusvalem1[koht] == forall_kontroll[forall_indeks]) {
        	sonake[forall_indeks] = uusvalem1[koht];
        	forall_indeks += 1;
		}
		else {
			sonake[forall_indeks] = uusvalem1[koht];
			fprintf(nimi2, "%s", sonake);
			forall_indeks = 0;
		}	
		}
            koht++;
        }
        
        //----------------------------------ALO AASM�E OSA L�PP----------------------------//


        
/*oleme '{'-l.Tagastada temale vastava '}' "kaugus"
s.t. sulgude sisse jääva valemi pikkus*/
        if (leitud) { /*see funktsioon siis asendada*/

/*uusvalem1[koht] on esimene funktsiooni nime sümbol*/
            mitu = 0;
            while ((uusvalem1[koht + mitu] != '}') && (koht + mitu < uuepikkus)) mitu++;
            mitu++;
/*mitu on nüüd asendatava funktsiooni nime pikkus*/
            if (koht + mitu - 1 == uuepikkus) {
                printf("Valemi indeksi l~opetav loogiline sulg puudub!");
                abort();
            }
/*Mitu elementi vôrrelda tuleb*/
            while ((i < valemitearv) &&
                   ((i == a) || (memcmp(&uusvalem1[koht], &valem[i][0], mitu) != 0))) {
                i++;
            }
            if (i == valemitearv) {
                printf("Puudub funktsioon ");
                for (j = 0; j < mitu; j++) {
                    printf("%c", uusvalem1[koht + mitu]);
                }
                abort();
            }
/*Nüüd vaja trükkida valem[i] koos uusvalem1-s väärtustatud
parameetritega(kui neid on)*/
/*Kui parameetreid pole,siis uusvalem1[koht+mitu]=='=',
vastasel korral algavad sealt
parameetrite väärtused*/
/*Mitu näitab funktsiooni nime pikkust baitides*/
            koht = koht + mitu;
/*Koht on parameetrite alguskohal*/
            if (valem[a][koht] != '(') {/*Siis parameetreid pole*/
                j = 0;
                while (valem[i][j] != '=') {
                    j++;
                }
                j++;
                sonake = (char *) malloc(4);
                memset(sonake, '\0', 4);
                sonake[0] = '(';
                fprintf(nimi2, "%s", sonake);
                LAHTI(i, j, 0, 0);
                sonake = (char *) malloc(4);
                memset(sonake, '\0', 4);
                sonake[0] = ')';
                fprintf(nimi2, "%s", sonake);
            } else {/*Parameetrite väärtustused tuleb leida*/
                sulge = 1;
                leidmisabi = 1;
                while (sulge) {
                    if (uusvalem1[koht + leidmisabi] == ')') { sulge--; }
                    if (uusvalem1[koht + leidmisabi] == '(') { sulge++; }
                    leidmisabi++;
                }
/*parameetrite väärtustuste osa uusvalem1-s asub
uusvalem1[koht] . . . uusvalem1[koht+leidmisabi-1] */
                lopumargend = koht + leidmisabi - 1;/*s.t.lôpusulg*/
                parasid = 0;
                for (j = 0; j < leidmisabi; j++) {
                    if (uusvalem1[koht + j] == ', ') {
                        parasid++;
                    }
                }/*for-tsükkel*/
                parasid++;
/*Parameetreid on ühe vôrra rohkem komade arvust*/
/*'koht' on argumentide alustaval ')'-l */
/*'lopumargend' on lopetaval ')'-l*/
                koht++;
                paramvaartused = (char **) malloc(parasid * sizeof(char *));
                for (j = 0; j < parasid; j++) {
                    paramvaartused[j] = (char *) malloc(arg_vaartustus_pikkus * sizeof(char));
                    memset(paramvaartused[j], '\0', arg_vaartustus_pikkus * sizeof(char));
                }
/*Nullidega täitmine*/
/*Nüüd parameetrite väärtused kirjutada massiivi
paramvaartused*/
                j = 0;
                i1 = 0;
/*Mitmendale kohale paramvaartused[j] vastava elemendi
uusvalem1-st kirjutame*/
                while (j < parasid) {
                    if ((uusvalem1[koht] != ', ') && (koht < lopumargend)) {
                        paramvaartused[j][i1] = uusvalem1[koht];
                        i1++;
                        koht++;
                    } else {
                        j++;
                        koht++;
                        i1 = 0;
                    }
                }/*while*/
/*koht on 1.sümbol peale lôpetavat ')'-t*/
                j = 0;
                while (valem[i][j] != '=') j++;
                j++;
/*j.ndast sümbolist algab valem[i] tegelik sisu*/
                sonake = (char *) malloc(4);
                memset(sonake, '\0', 4);
                sonake[0] = '(';
                fprintf(nimi2, "%s", sonake);
/*Sest funktsiooni ümber tekib vahest vajadus sulge
panna ja kindluse m~ottes paneme need*/
                LAHTI(i, j, paramvaartused, parasid);
                sonake = (char *) malloc(4);
                memset(sonake, '\0', 4);
                sonake[0] = ')';
                fprintf(nimi2, "%s", sonake);
/*koht peaks nüüd olema valemis ~oigel järjel*/
            }/*else*//*parameetrite väärtused on vaja leida*/
        }/*leitud*/
    }/*while(koht<uuepikkus)*/

} /* Funktsiooni "LAHTI" l~opp */
/*---------------------------------------------------------------------------*/
int main(int argc, char **argv) {
    int i;
    if (argc < 2) {
        printf("not found a input-file!\n");
        abort();
    }
    if ((strstr(argv[1], ".ltx") == NULL) &&
        (strstr(argv[1], ".tex") == NULL)) {
        printf("this is not a .tex-file!\n");
       abort();
    }
/*fail1-st v~otab ja fail2-e paneb*/
    fail1 = (char *) malloc(256);
    fail2 = (char *) malloc(256);
    memset(fail1, '\0', 256);
    memset(fail2, '\0', 256);
/*Nüüd faili nimes punkt olemas*/
    i = 0;
    while (argv[1][i] != '.') {
        fail2[i] = argv[1][i];
        i++;
    }
    strcat(fail2, ".frm");
    strcat(fail1, argv[1]);
    printf("output-%s\n", fail2);
    puhver = JARRAY(fail1);
    puhver = ASENDA(puhver, "\\begin{displaymath}", "$$");
    puhver = ASENDA(puhver, "\\end{displaymath}", "$$");
    puhver = ASENDA(puhver, "\\begin{equation}", "$$");
    puhver = ASENDA(puhver, "\\end{equation}", "$$");
    puhver = ASENDA(puhver, "\\begin{eqnarray}", "$$");
    puhver = ASENDA(puhver, "\\end{eqnarray}", "$$");
    LEIAPIKKUSED();
/*Täidame massiivi "suurus" valemite pikkuseid väljendavate arvudega*/
    VOTAMALU();
/*Reserveerime massiivile **valem vajaliku mälu*/
    LOE();
/*loeme valemid algtekstist massiivi **valem*/
    LEIAPEAVALEM();
/*Leiab peavalemi järjekorranumbri massiivis ** valem*/
    KORRIGEERI_soned();
/*asendab,kustutab nii,et peale seda oleks k~oik standardsel kujul*/
/*Peale seda vaja teha vaid funktsioonide asendused*/
    nimi2 = fopen(fail2, "w");
/*Siia l~opptulemus*/
    fprintf(nimi2, "%s", "#");
    LAHTI(peavalem, 0, 0, 0);
/*Kirjutab l~oppfaili valem[peavalem],tehes vastavad asendused*/
    fprintf(nimi2, "%s", "#");
    fflush(nimi2);
    fclose(nimi2);
    printf("OK!\n");
    return 1;
}
