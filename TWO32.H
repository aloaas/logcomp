#define HTL 256
#define sk '\xBA'
#define ap '\xFA'   /* xFA on relatsiooni tryki "punkt":  ˙ */
#define MAX_FAILINIME_PIKKUS 256

  FILE *of;        /* w_bin & r_bin */
  int X;              /* 0 - Gx.grm,  1 - tri.grm  */
  char *Nimi;
  char rida[44];
  char word[20];      /* terminaalse vìi mitteterminaalse tÑhestiku tÑht */
  char def[20];       /* produktsiooni parem pool vahekeeles */
  int dl;             /* "def" pikkus */
  char sat[44];             /* ={"trigol.txt"};  */
  char *fname;        /*  grammatika teksti nimi (vìib koos teega) */
  char Rn[256];
  FILE *rules;        /* grammatikafaili handle */
  char *Buf;          /* sisendpuhver */
  int PF_length;      /* sisestatud teksti pikkus (Milam) */
  int P_length;       /* sisestatud teksti pikkus */
  char *GBuf;         /* sisendpuhver: grammar */
  int Glen;           /* sisestatud teksti pikkus: grammar */
  int Pnr;            /* produktsiooni jrk-nr */
  int olek;           /* automaadi jaoks */
  char *P;            /* sisendpuhvri jÑrg */
  char T[2*HTL][20];  /* tÑhestiku V symbolite nimed */
  int left;
  int right;
  int V;
  int semflag;
  int *semantika;    /* semantikamassiiv */
  char *p_M;          /* puutrÅki indeksite vektor */
  int p_ML;                /* indeksite vektori pikkus */
  char *PM;           /* eelnevusmaatriks */
  char *LM;           /* leftmost-maatriks */
  char *RM;           /* rightmost-maatriks */
  char *LC;           /* vasak sìltumatu kontekst */
  char *RC;           /* parem sìltumatu kontekst */
  int indeX;          /* eelnevusteisenduste uute produktsioonide suffiks */
  int dep;            /* dep>0; analysaator kasutab BRC(1,1)-konteksti */
  int dc;    /* dc=0 kui sìltuvat kontexti ei saa arvutada vìi see ei eristu */
  int si;    /* stack index */

/* Produktsiooni parem pool */
struct R{
	int P;         /* produktsiooni jrk-nr */
	int sem;       /* semantikakood */
	int n;         /* NT definitsiooni pikkus */
	int *d;       /* NT definitsioon */
	struct R *alt; /* NT alternatiivne definitsioon */
	};

/* TÑhestiku V symboli (NT vìi T) kirje */
struct D{
	int tunnus;  /* 0: NT, 1: T */
	int code;    /* vahekeelne kood */
	int L;       /* NT vìi id/const nime pikkus  */
	int loc;     /* ident | const : nr */
	int icx;     /* itl vìi ktl - IT vìi KT index */
	struct D *left;   /* kahendpuu viidad (vìti on s) */
	struct D *right;
	struct R *def;    /* NT: vahekeelne definitsioon */
	};

struct parm{
	int nr;
	int tnr;
	int BRC;
	int Pnr;
	int dep;
	};

/* Produktsioonide paremate poolte jÑrgi paisatud tabeli kirje */
struct h{
	int P;          /* produktsiooni jrk-nr */
	int n;          /* definitsiooni pikkus  */
	int NT;         /* defineeritav nonterminal  */
	int *d;        /* definitsioon */
	int nc;         /* 1 - sìltuv kontekst */
	char *c;        /* reservis */
	struct h *same; /* sama parem pool */
	struct h *col;  /* pìrkeviit */
	};

  struct D *DT;     /* first   */
  struct D *DC;     /* current */
  struct D *DL;     /* last    */
  struct D *DN;     /* new     */

  int nr;     /* tÑhestiku V jooksev pikkus */
  int tnr;    /* terminaalse tÑhestiku jooksev pikkus */
  int ttl;    /* eraldajate arv */
  int NR;     /* eelnevusteisenduste 'uute produktsioonide' jooksev jrk-nr */
  int Sn;     /* 'nr' enne eelnevusteisendusi */
  int BRC;    /* Åhesuguste paremate pooltega produktsioonide arv */
  int context;  /* 0 kui G on pîîratav, 1 kui pole */
  struct parm *PARM;      /* parameetrid */
  struct h *hc;           /* BRC-analyysil detekteeritud kirje viit */
  struct h *HT[HTL];      /* hashtabel */

  char ***DEP;	/* sìltuv kontekst */
  FILE *Logi;
  char *L_name;
  
void ExiT(void);
int constr(void);            /* constr.c */
void ctree(void);            /* constr.c */
void print_h(struct h *t);              /* hash.c */
int hfunc(int keylen,int *key);        /* hash.c */
struct h *ReadHash(int n,int *key);    /* hash.c */
int HashRule(struct D *t,struct R *r);  /* hash.c */
void HashNT(struct D *t);               /* hash.c */
void HashRules(struct D *t);            /* hash.c */
void print_L(struct h *CP);             /* hash.c */
void print_HT(void);                    /* hash.c */
void brc(int NT);                               /* indep.c */
void print_indepC(void);                        /* indep.c */
void print_cxt(char **L);                       /* dep.c */
void print_all_cxt(void);                       /* dep.c */
void print_all_cxtp(void);                      /* dep.c */
int g1(char **L,int X,int D);                   /* dep.c */
int g2(char **L,struct h *t,int X);             /* dep.c */
int g3(char **L,struct h *t,int D);             /* dep.c */
int g4(char **L,struct h *t);                   /* dep.c */
int search_springs(struct h *t,int A);          /* dep.c */
int test_dep_con(struct h *x,struct h *y);      /* dep.c */
int brctest(struct h *t);                       /* indep.c */
int make_BRC(void);                             /* indep.c */
void pT(void);                                  /* dict.c */
struct D *newD(void);                           /* dict.c */
struct R *newR(void);                           /* dict.c */
void free_R(struct R *r);                       /* dict.c */
void free_tree(struct D *t);                    /* dict.c */
void printR(struct R *r);                       /* dict.c */
int print_rule(struct D *t);                    /* dict.c */
char *jarray(char *pealkiri);                   /* r_text.c */
int put_semant(void);                           /* sem.c */
struct D *search(int x);                        /* dict.c */ 
struct D *getV(char *key);                      /* dict.c */
void viga(int i);                               /* dict.c */
int p_viga(int i);                              /* scanner.c */
int print_rules(struct D *t);                   /* dict.c */
struct D *leftside(int l);                      /* dict.c */
void nonterm(int l);                            /* dict.c */
void term(int l,int flag);                      /* dict.c */
void makerul(void);                             /* dict.c */
int Terms(void);                                /* dict.c */
int Rules(void);                                /* dict.c */
void first_LR(struct D *t);                     /* lrm.c */
void sec_LR(int lrf,int nt);                    /* lrm.c */
void make_LRB(void);                            /* lrm.c */
void print_def(struct R *r);                    /* dict.c */
int relac(struct D *t);                         /* lrm.c */
int make_PM(int ret);                              /* lrm.c */
void print_rela(void);                          /* lrm.c */
void s_to_right(int lim, struct D *t,struct R *d);  /* conf.c */
void P2(int x,int y);                               /* conf.c */
void s_to_left(int lim, struct D *t,struct R *d);   /* conf.c */ 
void P1(int x,int y);                               /* conf.c */
void tell_conf(int p,int i,int j);                  /* conf.c */
int conflicts(void);                                /* conf.c */
void print_LRM(char *M);   /* lrm.c */ 
void print_PM(void);       /* lrm.c */
void fprint_PM(void);      /* lrm.c */
int w_tabs(void);                      /* w_bin.c */

