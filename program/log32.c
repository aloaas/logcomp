/* log_com1.c: Formula Interpreter for gcc. May 2001 */
/*Autor:Ain Isotamm*/
/*ja LOOGIKAVALEMITE KOMPILAATOR */
/*Autor:Ahti Peder*/
/*versioon 2.4*/
/*22.05.2001 kell 9.38*/
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "two32.h"
#define idtl 50
#define ctl 50
struct indeeks{                  /*ahti oma*/
	char alus[20];               /*Suurendada!*/
    struct indeeks *jarg;
	};
struct paarid{
	int algus;
	int lopp;
	char nimi;
	struct paarid *jarg;
};
struct hetkevaartus{              /*ahti oma*/
	char mark;
	int vaartus;
	struct hetkevaartus *jarg;
	};
/* identifikaatorite tabeli kirje */
struct itr{
	int nr;         /* jrk-nr tï¿½hestikus V */
	int loc;
	int value;      /* interpretaator: muutuja vï¿½ï¿½rtus */
	struct top *t;  /* kui id=mï¿½rgend: viit operaatorile */
	int io;         /* 0, kui pole, 1: read, 2: write, 3: r&w */
	};
/* konstantide tabeli kirje */
struct ctr{
	int nr;         /* jrk-nr tï¿½hestikus V */
	int loc;
	int value;      /* konstandi vï¿½ï¿½rtus */
	};
/* analyysipuu tipp */
struct top{
	int kood;          /* vahekeelne kood */
	int leks;          /* kui tipp=ident/const, siis selle jrk-nr. */
	int sem;           /* semantikakood */
	int label;         /* kui tipp on mï¿½rgendatud operaator - mï¿½rgendinr */
	int truel;         /* kompilaator: go to true */
	int falsel;        /* kompilaator: go to false */
	struct top *up;    /* puuviidad: ï¿½les, */
	struct top *right; /* naabrile ja */
	struct top *down;  /* alluvale */
	};
	
/*struct serv{
	int alg_tipp;
	int lopp_tipp;
	struct serv *jarg;
};*/
struct graafpea{
	char nimi;
	struct paarid *graph;
	struct graafpea *jarg;
};
struct paarid *servade_pea=NULL;
struct indeeks *ind=NULL;               /*ahti*/
struct hetkevaartus *Muut=NULL;			/*ahti*/
FILE *FF=NULL;                          /*ahti*/ 
FILE *FF1=NULL;                         /*ahti*/ 
FILE *FF2=NULL;							/*ahti*/
int voib=0;								/*ahti*/
/*voib=0, siis koma kirjutamise keeld*/

  int KOOD;
  char *PBuf=NULL;     /* sisendpuhver: programm */
  int Plen=0;          /* sisestatud teksti pikkus: programm */
  char *gr_name;
  char *pr_name;
  struct graafpea *GRAAFIDE_MASSIIV;/*ahti*/
  char **argumendid;            /*ahti*/
  int arg_arv;
		/*nii palju on kï¿½sureal parameetreid*/
  char *fail1;                  /*ahti*/
  char *fail;                   /*ahti*/
  int pane=0;					/*ahti*/
  char *TT[HTL];      /* pikkuse jï¿½rgi jï¿½rjestatud eraldajad VT-st */
  int vkl;                 /* VK pikkus */
  int *VK=NULL;           /* vahekeelne programm */
  int VEAD=0;              /* programmivigade loendaja */
  int VKL=0;               /* vahekeelse programmi symbolite arv */
  int bok;      /* tell_about(): 0 - ok, 1 - paha */
  int k_k;    /* konstandi vahekeelne kood */
  int m_k;    /* muutuja (identifikaatori) vahekeelne kood */
  int itl;    /* identifikaatorite tabeli index/pikkus */
  int ktl;    /* konstantide tabeli indeks/pikkus */
  int IT[50];     /* identifikaatorite tabel */
  int KT[50];     /* konstantide tabel */
  struct ctr *CT[ctl];
  struct top *t;   /* top of the parsing tree */
  struct itr *IDT[idtl];
  int logi=0;           /* kui 1, siis teeb kettale  xxx.log  */
  struct h *t_;
  int Stack[200];/*oli 20*/
  char rela[200];/*oli 20*/
  int lausevorm[200];/*oli 20*/
  struct top *puu[200];/*oli 20 */
  struct top *p_=NULL;/*analï¿½ï¿½sipuu tipp*/
  struct top *cp=NULL;
  int rel,I,J;
  int n_,l_,s_,N_,x_;
  int sem,marker;  
  int dummy;
  int flag;
  int Rd;
  int Wr;

char *gM(int p);                     /* w_bin.c */
struct top *r_ptree(void);             /* r_bin.c */
struct top *set_up(struct top *root);  /* r_bin.c */
struct top *analyzer(void);       /* parser.c */
void print_VK(int k);      /* scanner.c */
int Ident(int k);          /* scanner.c */
int Const(int k);          /* scanner.c */
int scanner(void);         /* scanner.c */
int reduce(struct h *t,int x, int y);                         /* parser.c */
int p_stack(char *name,int *v,char *rel,int a,int n,int k);  /* parser.c */
struct top *newtop(int x,int i,int sem);                      /* parser.c */
void red_tree(struct top *puu[],struct top *p,int s,int n);   /* parser.c */
int pp2html(struct top *p);                    /* parser.c */
void sentential_form(void);                    /* parser.c */
int correct(void);                             /* parser.c */
struct top *parser(void);                      /* parser.c */
struct ctr *make_ctr(void);                    /* parser.c */
int make_CT(void);                             /* parser.c */
struct itr *make_itr(void);                    /* parser.c */
void free_CT(void);                            /* parser.c */
int make_IDT(void);                            /* parser.c */
void free_IDT(void);                           /* parser.c */
void p_itr(struct itr *id);                    /* parser.c */
void print_variables(void);                    /* parser.c */
void p_ctr(struct ctr *id);                    /* parser.c */
struct item *make_item(void);          /* interp.c */
void freestack(int i,int k);           /* interp.c */
void prist(int i,int ic);              /* interp.c */
int get_x(int i,int k);                /* interp.c */
void write_x(int x,int i);             /* interp.c */
void aritm(int i,int r);               /* interp.c */
void loogik(int i,int r);              /* interp.c */
int print_Op(struct top *t);           /* interp.c */
void p_prog(struct top *root);         /* interp.c */
void trigol_Int(struct top *root);     /* interp.c */
void prop(int s);                  /* parser.c */
void tprop(int s);                 /* parser.c */
FILE *opr(void);                     /* r_bin.c */
struct D *r_V(void);                 /* r_bin.c */
struct h *grec(void);                /* r_bin.c */
struct h *r_hr(void);                /* r_bin.c */
void r_HT(void);                     /* r_bin.c */
int r_tabs(void);                    /* r_bin.c */
void ctree(void);
void print_TREEK(void);
void print_data(int x,int y);
void print_mx(char *M,int x);
void print_sem(void);
void p_top(char *t,struct top *p);
void pf(const char *format,...);
void print_cxt(char **L);                       /* dep.c */
/*AHTI omad*/

void LABI_VALEM(struct top *p);
void LABI_RAJADEGA_VALEM_EEL(struct top *p,struct top *v);
void LABI_RAJADEGA_VALEM(int algraja,struct top *metamuutujad,int loppraja,struct top *tingimused,struct top *v);
int KAS_TINGIMUSED(struct top *p);
int KAS_TINGIMUS(struct top *r);
int KAS_EELNEB(int a,int b);
int KAS_EELLANE(int a,int b);
int ASTE(int a,int b);
int LEIA_ALAMPUU_VAARTUS(struct top *l);
void LISA_MUUTUJATESSE(char a);
void PANE_UUS_VAARTUS(int a,char b);
void KUSTUTA_MUUTUJATEST(char b);
int LEIA_VAARTUS(char a);
int MITMES(struct top *r);
char *LEIA_SONA(struct top *r);
struct paarid *GRAAF(char *failinimi);
void EEMALDA_GRAAF(char nimi);
struct paarid *ANNA_GRAAF(char nimi);
void SALVESTA_INPUT(struct top *p);
void SALVESTA_MUUTUJANIMED(int a,int b,struct top *p);
void SALVESTA_GRAAFINIMED(int a,int b,struct top *r);
void PANE_UUS_VAARTUS_SERVALE(char nimetus,int a_tipp,int l_tipp);
void LISA_SERV(char nimetus);
int KAS_INTSIDENTSUS(struct top *p);
void LABI_RAJADEGA_GRAAF(struct top *s,struct top *tingimused,struct paarid *graafile_viit,struct top *v);
int KAS_ON_SERV(struct top *p);
void KUSTUTA_SERV(char a);

/* Alo Aasmäe loodud*/
struct paarid *LOO_GRAAF_SUURUSEGA(int moode);
/*=========================AHTI==============================*/

void KUSTUTA_SERV(char a){
struct paarid *servad,*servad_eel;
	servad=servade_pea->jarg;
	servad_eel=servade_pea;
	while((servad->nimi)!=a) {
		servad_eel=servad;
		servad=servad->jarg;
	}
	servad_eel->jarg=servad->jarg;
}
/*==========================================*/
void PANE_UUS_VAARTUS_SERVALE(char nimetus,int a_tipp,int l_tipp){	
struct paarid *servad;
servad=servade_pea->jarg;
while(servad->nimi!=nimetus) servad=servad->jarg;
	/*kuni leian serva ules*/
servad->algus=a_tipp;
servad->lopp=l_tipp;
}
/*==========================================*/
int KAS_ON_SERV(struct top *p){
	/*p on 'expression1'-l  voi indexes1*/
struct paarid *servad;
	/*tuleme 4 korda allapoole*/
	if (p->sem==81) p=p->down;
	
	
	p=p->down;
p=p->down;
	p=p->down;
p=p->down;	
	
	if (p->sem==100){/*#i#*/
		/*siis on ï¿½ldse motet kontrollida,kas on tegemist servaga*/
			servad=servade_pea;
			if (servade_pea==NULL) return 0;
				else{
					/*on olemas servi*/
					servad=servade_pea;
					while((servad!=NULL)&&((servad->nimi)!=T[p->leks][0]))
						servad=servad->jarg;
					if (servad==NULL) return 0;
						return 1;
				}
	}/*kui p->sem==100*/
		else return 0;
}
/*========================================*/
int KAS_INTSIDENTSUS(struct top *p){
	/*kas on semantiliselt ï¿½ige ja kas siin on mingi intsidentsus*/
	/*x_{e},kus e on serv,on jama sisestaja poolt*/
	/*p->sem==77*/
struct paarid *servad;
struct top *s;






	
	p=p->down;p=p->right;
		/*p on indexes1*/
	
	if (p->sem==81){
		s=p->down;s=s->down;
		s=s->down;s=s->down;s=s->down;

		
		servad=servade_pea;
		while((servad->jarg!=NULL)&&(servad->nimi!=T[s->leks][0]))
			servad=servad->jarg;
        if (servad->nimi) {
            if (servad->nimi == T[s->leks][0]) {
                printf("Forbidden index which single component is an arc variable!\n");
                /*printf("Ei tohi olla indeksit,mille ainus komponent on mingi serva muutuja\n");*/

                abort();
            }
        }
        return 0;/*oli 1  :))*/
	}/*kas p->sem==81*/
	else{
		/*p->sem==80*/
		/*kas on nuud kaks indeksi komponenti?*/
		p=p->down;
		p=p->right;
		p=p->right;
		if (p->sem==81) return 1;
	      else return 0;/*rohkem kui 2 komp.-i*/
	}
}
/*============================================*/
void LISA_SERV(char nimetus){
	/*servade_pea on viit esimesele servale*/
	struct paarid *uus_serv;
	uus_serv=(struct paarid *)malloc(sizeof(struct paarid));
	memset(uus_serv,'\0',sizeof(struct paarid));
	uus_serv->algus=-1;
	uus_serv->lopp=-1;
	uus_serv->nimi=nimetus;
		/*paneme esimeseks*/
	uus_serv->jarg=servade_pea->jarg;
	servade_pea->jarg=uus_serv;
}
/*===========================================*/
void SALVESTA_GRAAFINIMED(int a,int b,struct top *r){
	/*graafinimed on argumendid[a], ..., argumendid[b] */

	/*on 'graphnames1'-l*/
struct paarid *graaf;
int i;
struct graafpea *graafi_pea;
struct top *p,*s;

p=r;




for(i=b;i>a;i--){
	p=p->down;s=p->right;s=s->right;
	graaf=GRAAF(argumendid[i]); 
		 
		/*tagastab osuti selle graafi esimesele servale.*/  			
		/*s.t. graaf->jarg on esimene serv*/ 
		/*paneme graafi graafide sï¿½ilitusmassiivi*/ 
	graafi_pea=(struct graafpea *)malloc(sizeof(struct graafpea)); 
	memset(graafi_pea,'\0',sizeof(struct graafpea)); 
	graafi_pea->graph=graaf; 
	  			
	graafi_pea->nimi=T[s->leks][0];  	
	graafi_pea->jarg=GRAAFIDE_MASSIIV->jarg;  	
	GRAAFIDE_MASSIIV->jarg=graafi_pea;  		
	 /*leitud graafi paneme esimeseks*/  	 

	
}/*for*/ 

 p=p->down;
graaf=GRAAF(argumendid[a]); 
		 
		/*tagastab osuti selle graafi esimesele servale.*/  			
		/*s.t. graaf->jarg on esimene serv*/ 
		/*paneme graafi graafide sï¿½ilitusmassiivi*/ 
	graafi_pea=(struct graafpea *)malloc(sizeof(struct graafpea)); 
	memset(graafi_pea,'\0',sizeof(struct graafpea)); 
	graafi_pea->graph=graaf; 
	  			
	graafi_pea->nimi=T[p->leks][0];  	
	graafi_pea->jarg=GRAAFIDE_MASSIIV->jarg;  	
	GRAAFIDE_MASSIIV->jarg=graafi_pea;  		
	 /*leitud graafi paneme esimeseks*/  	 
}
/*=============================================*/
void SALVESTA_MUUTUJANIMED(int a,int b,struct top *p){
int i,muutuja;
struct top *r;
	/*p on 'varnames1'-l*/
		/*muutujad on argumendid[a], ... ,argumendid[b]*/
	for(i=b;i>a;i--){

		p=p->down;r=p->right;r=r->right;
		muutuja=atoi(argumendid[i]);
		LISA_MUUTUJATESSE(T[r->leks][0]);
		PANE_UUS_VAARTUS(muutuja,T[r->leks][0]);
		   
	}
	p=p->down;
		/*nuud siis viimane muutuja*/
	
	muutuja=atoi(argumendid[i]);
	LISA_MUUTUJATESSE(T[p->leks][0]);
	PANE_UUS_VAARTUS(muutuja,T[p->leks][0]);
}
/*======================================================*/
void SALVESTA_INPUT(struct top *p){/*hetkel on siin jama*/
int parameetreid,param1;
struct top *r,*s;
	/*p->sem==4,5,6*/
	/*kasurealt tuli arg_arv-2 parameetrit*/
	
	if (p->sem==4){
			/*ainult graafinimed*/
		p=p->down;
		r=p;
		parameetreid=1;
		while (r->sem==7){
			r=r->down;
			parameetreid++;
		}
		if (parameetreid!=arg_arv-2){  			
			if(parameetreid>arg_arv-2)
				printf("Graph files are lacking\n");
				/*printf("Graafide faile on puudu\n");  */
	 		else printf("Too many graph files\n"); 
				/*printf("Graafide faile on ï¿½learu!Jama!\n"); */
	 	 	  			
			abort(); 
		} 
		SALVESTA_GRAAFINIMED(2,parameetreid+1,p);
	}
		
	if (p->sem==5){
				/*ainult muutujanimed*/
			p=p->down;
			r=p;
			parameetreid=1;
			while (r->sem==8){
				r=r->down;
				parameetreid++;
			}
		
		if (parameetreid!=arg_arv-2){  			
			if(parameetreid>arg_arv-2)
				printf("Too few parameters!\n");
				/*printf("Parameetreid on puudu\n");  */
	 		else printf("Too many parameters!\n");
				/*printf("Parameetreid on ï¿½learu!Jama!\n"); */
	 	 	  			
			abort(); 
		} 
		SALVESTA_MUUTUJANIMED(2,parameetreid+1,p);
	}

	if (p->sem==6){
			/*muutujad+graafid */
		p=p->down;
			/*p on 'varnames1*/
		r=p->right;
			/*r on 'graphnames1*/
			/*leiame var-ide arvu*/
		s=p;
		parameetreid=1;
		while (s->sem==8){
			s=s->down;
			parameetreid++;
		}
		param1=parameetreid;
		s=r;
		parameetreid=1;
		while (s->sem==7){
			s=s->down;
			parameetreid++;
		}
			/*kas argumentide arv klapib?*/	
		if ((parameetreid+param1)!=(arg_arv-2)){  	
	 			if(parameetreid+param1>arg_arv-2)
					printf("Too few parameters!\n");
				/*printf("Parameetreid on puudu\n");  */
	 		else printf("Too many parameters\n"); 
				/*printf("Parameetreid on ï¿½learu!Jama!\n"); */
	 	 	  			
			abort(); 
		} 
		/*nuud on korras*/
		SALVESTA_MUUTUJANIMED(2,2+param1-1,p);
		SALVESTA_GRAAFINIMED(2+param1,arg_arv-1,r);
	}/*p->sem==6*/
		
}/*SALVESTA_INPUT*/
/*======================================================*/
void LABI_VALEM(struct top *p){
/*Siia tuleb midagi*/
struct top *s;
int abi;
	
	
switch(p->sem){
		
		/*P1=1 	$   P 1: `program' -> # `family' # */
	case 1:p=p->down;
		p=p->right;
		LABI_VALEM(p);
		break;
	/*---------------------------------------*/
		/*P3=2 	$   P 3: `metaformula' -> `input' `bigformula'*/
	case 2: p=p->down;
		
		SALVESTA_INPUT(p);
	

		LABI_VALEM(p->right);
	         break;
	/*---------------------------------------*/
		/*P4=3 	$   P 4: `metaformula' -> `bigformula'*/ 
	case 3: p=p->down;
		LABI_VALEM(p);
		break;
	/*---------------------------------------*/
		
	 /*P16=9 $  P17: `bigformula' -> \bigvee_{ `limits' `lbra'`bigformula'*/
 	/*P17=10 $   P16: `bigformula' -> \bigwedge_{ `limits'`lbra' `bigformula'*/
	/*P18=11 	$   P18: `bigformula' -> \bigoplus_{ `limits' `lbra' `bigformula' */
	case 9:
	case 10:
	case 11:if (voib){
			     fprintf(FF1,",");
				 if (pane) fprintf(FF2,",");
			     fflush(FF1);
				 if (pane) fflush(FF2);
		   }
		   if(p->sem==9)fprintf(FF1,"or(");
		   if(p->sem==9){if (pane) fprintf(FF2,"+(");}
   		   if(p->sem==10)fprintf(FF1,"and(");
		   if(p->sem==10){if (pane) fprintf(FF2,"*(");}
		   if(p->sem==11)fprintf(FF1,"xor(");	
			voib=0;
		   fflush(FF1);
		   if (pane) fflush(FF2);
		   p=p->down;/*p on 'limits'*/
		   s=p->right;s=s->right;
		   LABI_RAJADEGA_VALEM_EEL(p,s);/*p on 'limits',s on 'bigformula'*/
		   fprintf(FF1,")");
		   if (pane) fprintf(FF2,")");
			voib=1;
		   fflush(FF1);
		   if (pane) fflush(FF2);
		   
		break; 
/*-------------------------------------------------------------------------------------*/
	 	/*P19=12 	$   P19:`bigformula' -> odd `prefpar'`bigformula1' : `limits' )*/
 	/*P20=13 	$   P20:`bigformula' -> even`prefpar'`bigformula1':`limits' )*/  	  
	/*P21=14	$   P21:`bigformula'->exactlyone`prefpar'`bigformula1' :`limits' )*/  	   	 
	/*P22=15 	$   P22:`bigformula'-> atmostone`prefpar'`bigformula1': `limits' )*/   	   	 
	/*P23=16 	$   P23:`bigformula'->atleastone `prefpar' `bigformula1':`limits' )*/  	    	
	/*P24=17	$  P24:`bigformula' -> none`prefpar'`bigformula1' :`limits' )*/ 
	/*P25=18	$  P25:`bigformula' ->eq `prefpar'`bigformula1':`limits' )*/
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:if (voib){
				fprintf(FF1,",");
				fflush(FF1);
				if (pane) fprintf(FF2,",");
				if (pane) fflush(FF2);
				}
		
		        if (p->sem==12) fprintf(FF1,"odd("); 
				if (p->sem==12) {if (pane) fprintf(FF2,"xor(");}
			if (p->sem==13) fprintf(FF1,"even(");
			if (p->sem==14) fprintf(FF1,"exactlyone("); 
			if (p->sem==15) fprintf(FF1,"atmostone("); 
			if (p->sem==16) fprintf(FF1,"atleastone("); 
			if (p->sem==17) fprintf(FF1,"none("); 
			if (p->sem==18) fprintf(FF1,"eq(");  
			if (p->sem==18) {if (pane) fprintf(FF2,"=(");}  

				voib=0;
			fflush(FF1);
			if (pane) fflush(FF2);
			p=p->down;p=p->right;
				/*p on 'bigformula'-l*/
			
 			s=p->right;/*s on 'limits'*//*siit vï¿½tsin ï¿½he ï¿½ra*/
				
			LABI_RAJADEGA_VALEM_EEL(s,p);/*s on 'limits'*/
				
			
			fprintf(FF1,")");
			if (pane) fprintf(FF2,")");

				voib=1;
			fflush(FF1);
			if (pane) fflush(FF2);
	
		break;
/*-----------------------------------------------------------------------------*/
		/*P26=19$P26:`valem'->`atmost'`aloetelusulg'`laveloetelu' )*/
		/*P27=20$P27:`valem'->`atleast'`aloetelusulg'`laveloetelu' )*/
		/*P28=21$P28:`valem'->`level'`aloetelusulg'`laveloetelu' )*/  
	      
         	 	
	case 19:
	case 20:
	case 21 :if (voib){ 
			     fprintf(FF1,","); 
			     fflush(FF1); 
				 
		} 
		if (p->sem==19)fprintf(FF1,"atmost(");
		if (p->sem==20)fprintf(FF1,"atleast(");
		if (p->sem==21)fprintf(FF1,"level(");
		
		fflush(FF1);
 
		p=p->down;;/*d on nï¿½ï¿½d 'expression'-il*/ 
			 
		fprintf(FF1,"%d",LEIA_ALAMPUU_VAARTUS(p)); 
			voib=1;
		fflush(FF1); 
	
			/*p on `expression'*/ 
		p=p->right;p=p->right;p=p->right;
			/*p on 'bigformula'-l*/ 
		 
		s=p->right; /*t on 'limits'*/ 
	
		LABI_RAJADEGA_VALEM_EEL(s,p);
			/*Esimene argument 'limits' teine argument 'bigformula'-l*/   			  			
		fprintf(FF1,")");
			voib=1; 
		fflush(FF1); 
		 
		break; 
/*------------------------------------------------------------------------------------------*/
	
		/*P29=22        $   P29: `bigformula' -> `formula' */ 
	case 22:LABI_VALEM(p->down); 
		break;
/*--------------------------------------------------------------------------------------------*/
		/*P91=66 	$   P91: `formula' -> `leftpart' \sim `formula'
		P92=67 	$   P92: `formula' -> `leftpart' \oplus `formula'
		P93=68 	$   P93: `formula' -> `leftpart' \to `formula'
		P95=70 	$   P95: `leftpart' -> `leftpart' \vee `clause'
		P97=72 	$   P97: `clause' -> `logterm' \& `clause'*/
	case 66:
	case 67:
	case 68:
	case 70:
	case 72: if (voib){ 
			     fprintf(FF1,","); 
			      fflush(FF1); 
				  if (pane) fprintf(FF2,","); 
			      if (pane) fflush(FF2); 
		 } 
		 if (p->sem==66)fprintf(FF1,"eq("); 
		 if (p->sem==67)fprintf(FF1,"xor(");
		 if (p->sem==68)fprintf(FF1,"impl(");  
		 if (p->sem==70)fprintf(FF1,"or("); 
		 if (p->sem==72)fprintf(FF1,"and(");

		 if (p->sem==66){if (pane) fprintf(FF2,"=("); }
		 if (p->sem==70){if (pane) fprintf(FF2,"+("); }
		 if (p->sem==72){if (pane) fprintf(FF2,"*(");}

 			voib=0;
		fflush(FF1); 
		if (pane) fflush(FF2);
		 p=p->down; 
		 LABI_VALEM(p); 
		 
		 fflush(FF1);
		 if (pane) fflush(FF2);
 
		
		 p=p->right; 
		 LABI_VALEM(p); 
		 fprintf(FF1,")");
		 if (pane) fprintf(FF2,")");
			voib=1;
		 fflush(FF1);
		 if (pane) fflush(FF2);
		  
	break; 
/*---------------------------------------------------------------------------------------------*/
/*			
		P94=69 	$   P94: `formula' -> `leftpart'
		P96=71 	$   P96: `leftpart' -> `clause'
		P98=73 	$   P98: `clause' -> `logterm'
		P100=75 $   P100: `logterm' -> `literal'
		P103=78 $   P103: `literal' -> `prefixformula'*/
	case 69:
	case 71:
	case 73:
	case 75:
	case 78:
		p=p->down;
		LABI_VALEM(p);
		
	break;
/*----------------------------------------------------------------*/
		/* P99=74 	$   P99: `logterm' -> \neg `literal'*/
case 74: if (voib){ 
			     fprintf(FF1,","); 
			      fflush(FF1);
				  if (pane) fprintf(FF2,","); 
			      if (pane) fflush(FF2);
				  
		 } 
		voib=0;
		p=p->down;
		fprintf(FF1,"not(");
		if (pane) fprintf(FF2,"-(");
		fflush(FF1);
		if (pane) fflush(FF2);
		LABI_VALEM(p);
		fprintf(FF1,")");
		fflush(FF1);
		if (pane) fprintf(FF2,")");
		if (pane) fflush(FF2);
		voib=1;
		
	break;
/*---------------------------------------------------------------*/
/*
P104=79 	$   P104: `literal' -> `logicpar' `bigformula1' ) */
	case 79:if (voib){ 
			     fprintf(FF1,","); 
				fflush(FF1);
				 if (pane) fprintf(FF2,","); 
				if (pane) fflush(FF2);
		} 
			voib=0;
	
		fflush(FF1);
		if (pane) fprintf(FF2,"(");
		if (pane) fflush(FF2);
		p=p->down;p=p->right;
		LABI_VALEM(p);
		
		fflush(FF1);
		if (pane) fprintf(FF2,")");
		if (pane) fflush(FF2);
		voib=1;
	      break;
/*-------------------------------------------------------------------*/
/*
		P101=76 	$   P101: `literal' -> `logvar'
		P102=77 	$   P102: `literal' -> `logvar' _{ `indexes' }*/
	case 76:
	case 77:if(voib) { 
			fprintf(FF1,",");  
			fflush(FF1);  
			if (pane) fprintf(FF2,",");  
			if (pane) fflush(FF2);
		}
		voib=0;
		abi=MITMES(p);/*(p->sem==76 voi 77*/
	
		if (abi==-2) fprintf(FF1,"or()");
		if (abi==-1) fprintf(FF1,"and()");
		if (abi==-2) {if (pane) fprintf(FF2,"+()");}
		if (abi==-1) {if (pane) fprintf(FF2,"*()");}
		if (abi>-1){ 
			fprintf(FF1,"%s","x"); 
			 fflush(FF1); 
			 if (pane) fflush(FF2);
			       /*saab int tï¿½ï¿½pi vï¿½ï¿½rtuse*/ 
			       /*Mitmes erinev leitud muutuja see on*/ 
		        fprintf(FF1,"%d",abi); 
		        fflush(FF1); 
				if (pane) fprintf(FF2,"%d",abi); 
		        if (pane) fflush(FF2); 
		}      
		voib=1;
			/*Peab meelde jï¿½tma,et mingit kala ei teki*/ 
			/*muutujate loend(komadega eraldatud) tohib tekkida vaid  
			'loetelu' kaudu,seal paneme koma,seega siin pole vaja.*/ 
		break; 
/*-----------------------------------------------------------------------------*/
		/*P117=89 	$   P117: `prefixformula' -> `atmost' `prefpar' `formlist' )
		P118=90 	$   P118: `prefixformula' -> `atleast' `prefpar' `formlist' )
		P119=91 	$   P119: `prefixformula' -> `level' `prefpar' `formlist' )*/
	case 89:
	case 90:
	case 91:if(voib) { 
			fprintf(FF1,",");  
			fflush(FF1);  
		} 
		voib=0;
		if(p->sem==89)fprintf(FF1,"atmost(");
		if(p->sem==90)fprintf(FF1,"atleast(");
		if(p->sem==91)fprintf(FF1,"level(");
		
		p=p->down;/*d on nï¿½ï¿½d 'expression'-il*/ 
		fprintf(FF1,"%d",LEIA_ALAMPUU_VAARTUS(p)); 
	
		p=p->right;p=p->right;/*'formlist1-l'*/
		
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:if ((p->sem<89)&&(p->sem>81)){
			if(voib)  fprintf(FF1,",");  
			fflush(FF1); 
			if(voib)  {if (pane) fprintf(FF2,",");  }
			if (pane) fflush(FF2); 
		 
		}
			voib=0; 
			if(p->sem==82)fprintf(FF1,"odd(");
			if(p->sem==82)fprintf(FF2,"xor(");

			if(p->sem==83)fprintf(FF1,"even(");		
			if(p->sem==84)fprintf(FF1,"exactlyone(");
			if(p->sem==85)fprintf(FF1,"atmostone(");
			if(p->sem==86)fprintf(FF1,"atleastone(");
			if(p->sem==87)fprintf(FF1,"none(");
			if(p->sem==88)fprintf(FF1,"eq(");
			if(p->sem==88)fprintf(FF2,"=(");
				
			fflush(FF1);
			if (pane) fflush(FF2);
			p=p->down;p=p->right;/*'formlist1-l'*/
		
		
		/*molemad juhtumite klassid koos,p on 'formlist1'*/
		while(p->sem==123){
			p=p->down;
			s=p->right;s=s->right;
			LABI_VALEM(s);  
			
		
		}
		/*nuud p->sem==122*/
		p=p->down;
		LABI_VALEM(p);
		fprintf(FF1,")");  
		fflush(FF1); 
		if (pane) fprintf(FF2,")");  
		if (pane) fflush(FF2); 
		voib=1;
}/*---------------switch-lause-------------*/
}/*LABI_VALEM*/
/*======================================================*/
void LABI_RAJADEGA_VALEM_EEL(struct top *p,struct top *v){
	/*p on 'limits', v on 'bigformula'(mida saab lï¿½bida LABI_VALEM-it kasutades)*/
 	 	
	int moode=-1,rel1,rel2,algraja,loppraja;
		/*rajade mï¿½rkide jaoks*/
	char serv;
 	
	struct top *r,*t,*s,*tingimused;
	struct paarid *graafile_viit;
	
	char a_tipp,l_tipp;
		/*serva otspunktide tï¿½histamiseks*/
	
		r=p->down;      /*r on 'limit'    */
		if (p->sem==24) tingimused=NULL;
		if (p->sem==23) tingimused=r->right;
			/*tingimused on kas NULL voi
			'constraint'-il*/	
		r=p->down;    /*   limit'    */
/*........................B^n.....................algus*/
		if (r->sem==25){
			t=r->down;  /*t on  'metavariables1'*/
			s=t->right;  /*   s  on  '  expression '*/
			algraja=0;
			loppraja=ASTE(2,LEIA_ALAMPUU_VAARTUS(s))-1;
			LABI_RAJADEGA_VALEM(algraja,t,loppraja,tingimused,v);
		}
/*..........................B^n......................lopp*/
/*.......................1<i<5.................algus*/
		if (r->sem==30){
			t=r->down;
			algraja=LEIA_ALAMPUU_VAARTUS(t); 
			t=t->right;
			rel1=t->sem;
				/*teine relatsiooni mï¿½rk s.t. 'lessrel'*/
			s=t->right;
				/*s on 'metavariable1'*/
			t=s->right;
			rel2=t->sem;
			t=t->right;
			loppraja=LEIA_ALAMPUU_VAARTUS(t);
			
			if (rel2==33) loppraja--;
			/*kui rel. on '<', siis asendan '<='-ga*/		
			
			if (rel1==33) algraja++;
			LABI_RAJADEGA_VALEM(algraja,s,loppraja,tingimused,v);
		}
/*........................1<i<5................lopp*/	
/*.......................a,b,c\in V(G).......graafi tipp.....algus*/	  
		
		if (r->sem==26){
			
			t=r->down;
			s=t;
				/*s on 'metavariable1'*/
			t=t->right;
				/*t on 'graphname' */
			graafile_viit=ANNA_GRAAF(T[t->leks][0]);
			/*leiame antud graafi suurima tipunumbri s.t. môôtme*/

			while(graafile_viit->jarg!=NULL){
				graafile_viit=graafile_viit->jarg;
				if (moode<graafile_viit->algus) moode=graafile_viit->algus;
				if (moode<graafile_viit->lopp) moode=graafile_viit->lopp;
			}
				/*moode on nüüd suurim tipunumber*/
			algraja=1;
			loppraja=moode;
			LABI_RAJADEGA_VALEM(algraja,s,loppraja,tingimused,v); 
		}
/*.........................a,b,c\in V(G).......graafi tipp......lopp*/
/*............................a,b,c\in E(G)......graafi serv.......algus*/ 		
		if (r->sem==27){	
			r=r->down;	
			s=r;
				/*s on 'metavariables1'-el*/ 
			
			p=r->right;	
				/*p on 'graafinimi'*/
			
			graafile_viit=ANNA_GRAAF(T[p->leks][0]);
				/*kui selle nimega graafi pole,siis kï¿½sib faili,kus see on*/                            
			
			LABI_RAJADEGA_GRAAF(s,tingimused,graafile_viit,v);
				/*s on 'metavariables1'*/
				/*v on valem,mida labitakse*/
		}
/*....................a,b,c \in E(G).......graafi serv........lopp*/
/*.....................e \leftarrow \{a,b\}\in E(G).....graafi serv.....algus*/ 	
	if (r->sem==28){
		p=r->down;
		serv=T[p->leks][0];
		p=p->right;
			/*p on 'graphbra'-l*/
		p=p->right;
		a_tipp=T[p->leks][0];
		p=p->right;p=p->right;
		l_tipp=T[p->leks][0];
		p=p->right;
			/*peaks olema \in-l, sellel pole aga semantikat*/
			/*seega on graphname-il*/
		graafile_viit=ANNA_GRAAF(T[p->leks][0]);
		LISA_SERV(serv);	
		LISA_MUUTUJATESSE(a_tipp); 
		LISA_MUUTUJATESSE(l_tipp); 
		while(graafile_viit->jarg!=NULL){ 
			graafile_viit=graafile_viit->jarg;/*lï¿½bimata servale*/ 
			PANE_UUS_VAARTUS(graafile_viit->algus,a_tipp); 
			PANE_UUS_VAARTUS(graafile_viit->lopp,l_tipp); 
			PANE_UUS_VAARTUS_SERVALE(serv,graafile_viit->algus,graafile_viit->lopp);
								 
			if (KAS_TINGIMUSED(tingimused)){ 
				
				LABI_VALEM(v); 
			}
			                        
		}/*while*/ 
		 
		KUSTUTA_MUUTUJATEST(a_tipp); 
		KUSTUTA_MUUTUJATEST(l_tipp); 
		KUSTUTA_SERV(serv);
	}
/*.........................e \leftarrow {a,b}\in E(G).....graafi serv....lopp*/
 		
/*.............................\{a,b\} \in E(G)......graafi serv......algus*/ 
	if (r->sem==29){
		p=r->down;
			/*p on 'graphbra'*/
		p=p->right;
		a_tipp=T[p->leks][0];
		p=p->right;
		p=p->right;
		l_tipp=T[p->leks][0];
		p=p->right;
			/*peaks olema \in peal,kuna sellel aga semantikat
			pole,siis on graphname-il*/
		graafile_viit=ANNA_GRAAF(T[p->leks][0]);
			/*labime graafi koik servad*/ 
		LISA_MUUTUJATESSE(a_tipp); 
		LISA_MUUTUJATESSE(l_tipp); 
		while(graafile_viit->jarg!=NULL){ 
			graafile_viit=graafile_viit->jarg;/*lï¿½bimata servale*/ 
			PANE_UUS_VAARTUS(graafile_viit->algus,a_tipp); 
			PANE_UUS_VAARTUS(graafile_viit->lopp,l_tipp); 
				 
			if (KAS_TINGIMUSED(tingimused)){
				
				LABI_VALEM(v); 
			}
			                        
		}/*while*/ 
		
		KUSTUTA_MUUTUJATEST(a_tipp); 
		KUSTUTA_MUUTUJATEST(l_tipp); 
	}
	
	//ALO AASMÄE OSA ALGUS//
	/*P138=138 	$   P138: `limit' -> `metavariables' \notin E( `graphname1' )*/
	if (r->sem==138) {		

					
			r=r->down;	
			s=r;
				/*s on 'metavariables1'-el*/ 
			
			p=r->right;	
				/*p on 'graafinimi'*/
			
			graafile_viit=ANNA_GRAAF(T[p->leks][0]);
				/*kui selle nimega graafi pole,siis kï¿½sib faili,kus see on*/     
				
			struct paarid *esialgne_viit = graafile_viit;
			/*leiame antud graafi suurima tipunumbri s.t. mï¿½ï¿½tme*/
			int algtipp, lopptipp;
			while(graafile_viit->jarg!=NULL){
				graafile_viit=graafile_viit->jarg;
				algtipp = graafile_viit->algus;
				lopptipp = graafile_viit->lopp;
				if (moode<algtipp) moode=algtipp;
				if (moode<lopptipp) moode=lopptipp;
			
			}
				
				
			struct paarid *taisgraaf, *taisgraaf_eelmine;
			taisgraaf =(struct paarid *)malloc(sizeof(struct paarid));
			taisgraaf = LOO_GRAAF_SUURUSEGA(moode);    
			
			struct paarid *taisgraaf_algus = taisgraaf;
			
			int algtipp_vordlemiseks, lopptipp_vordlemiseks;
			while(esialgne_viit->jarg!=NULL){
				esialgne_viit = esialgne_viit->jarg;
				algtipp = esialgne_viit->algus;
				lopptipp = esialgne_viit->lopp;
				while (taisgraaf->jarg != NULL) {
					taisgraaf_eelmine = taisgraaf;
					taisgraaf = taisgraaf->jarg;
					algtipp_vordlemiseks = taisgraaf->algus;
					lopptipp_vordlemiseks = taisgraaf->lopp;
					if (algtipp == algtipp_vordlemiseks && lopptipp == lopptipp_vordlemiseks) {
						taisgraaf_eelmine-> jarg = taisgraaf -> jarg;
						break;
					}
				}
				taisgraaf = taisgraaf_algus;
			}
			
			LABI_RAJADEGA_GRAAF(s,tingimused,taisgraaf_algus,v);

			
	}
	
	
	/*.....................e \leftarrow \{a,b\}\notin E(G).....graafi serv.....*/ 	
		if (r->sem==139) {		


		p=r->down;
		serv=T[p->leks][0];
		p=p->right;
			/*p on 'graphbra'-l*/
		p=p->right;
		a_tipp=T[p->leks][0];
		p=p->right;p=p->right;
		l_tipp=T[p->leks][0];
		p=p->right;
			/*peaks olema \in-l, sellel pole aga semantikat*/
			/*seega on graphname-il*/
		graafile_viit=ANNA_GRAAF(T[p->leks][0]);
		
		struct paarid *esialgne_viit = graafile_viit;
			/*leiame antud graafi suurima tipunumbri s.t. mï¿½ï¿½tme*/
			int algtipp, lopptipp;
			while(graafile_viit->jarg!=NULL){
				graafile_viit=graafile_viit->jarg;
				algtipp = graafile_viit->algus;
				lopptipp = graafile_viit->lopp;
				if (moode<algtipp) moode=algtipp;
				if (moode<lopptipp) moode=lopptipp;
			
			}
				
				
			struct paarid *taisgraaf, *taisgraaf_eelmine;
			taisgraaf =(struct paarid *)malloc(sizeof(struct paarid));
			taisgraaf = LOO_GRAAF_SUURUSEGA(moode);    
			
			struct paarid *taisgraaf_algus = taisgraaf;
			
			int algtipp_vordlemiseks, lopptipp_vordlemiseks;
			while(esialgne_viit->jarg!=NULL){
				esialgne_viit = esialgne_viit->jarg;
				algtipp = esialgne_viit->algus;
				lopptipp = esialgne_viit->lopp;
				while (taisgraaf->jarg != NULL) {
					taisgraaf_eelmine = taisgraaf;
					taisgraaf = taisgraaf->jarg;
					algtipp_vordlemiseks = taisgraaf->algus;
					lopptipp_vordlemiseks = taisgraaf->lopp;
					if (algtipp == algtipp_vordlemiseks && lopptipp == lopptipp_vordlemiseks) {
						taisgraaf_eelmine-> jarg = taisgraaf -> jarg;
						break;
					}
				}
				taisgraaf = taisgraaf_algus;
			}
		
		
		
		LISA_SERV(serv);	
		LISA_MUUTUJATESSE(a_tipp); 
		LISA_MUUTUJATESSE(l_tipp); 
		while(taisgraaf->jarg!=NULL){ 
			taisgraaf=taisgraaf->jarg;/*lï¿½bimata servale*/ 
			PANE_UUS_VAARTUS(taisgraaf->algus,a_tipp); 
			PANE_UUS_VAARTUS(taisgraaf->lopp,l_tipp); 
			PANE_UUS_VAARTUS_SERVALE(serv,taisgraaf->algus,taisgraaf->lopp);
								 
			if (KAS_TINGIMUSED(tingimused)){ 
				
				LABI_VALEM(v); 
			}
			                        
		}/*while*/ 
		 
		KUSTUTA_MUUTUJATEST(a_tipp); 
		KUSTUTA_MUUTUJATEST(l_tipp); 
		KUSTUTA_SERV(serv);
	
			
	}
	
	/*.............................\{a,b\} \notin E(G)......graafi serv......algus*/ 
	if (r->sem==140) {		


		p=r->down;
			/*p on 'graphbra'*/
		p=p->right;
		a_tipp=T[p->leks][0];
		p=p->right;
		p=p->right;
		l_tipp=T[p->leks][0];
		p=p->right;
			/*peaks olema \in peal,kuna sellel aga semantikat
			pole,siis on graphname-il*/
		graafile_viit=ANNA_GRAAF(T[p->leks][0]);
			/*labime graafi koik servad*/ 
			
				
		struct paarid *esialgne_viit = graafile_viit;
			/*leiame antud graafi suurima tipunumbri s.t. mï¿½ï¿½tme*/
			int algtipp, lopptipp;
			while(graafile_viit->jarg!=NULL){
				graafile_viit=graafile_viit->jarg;
				algtipp = graafile_viit->algus;
				lopptipp = graafile_viit->lopp;
				if (moode<algtipp) moode=algtipp;
				if (moode<lopptipp) moode=lopptipp;
			
			}
				
				
			struct paarid *taisgraaf, *taisgraaf_eelmine;
			taisgraaf =(struct paarid *)malloc(sizeof(struct paarid));
			taisgraaf = LOO_GRAAF_SUURUSEGA(moode);    
			
			struct paarid *taisgraaf_algus = taisgraaf;
			
			int algtipp_vordlemiseks, lopptipp_vordlemiseks;
			while(esialgne_viit->jarg!=NULL){
				esialgne_viit = esialgne_viit->jarg;
				algtipp = esialgne_viit->algus;
				lopptipp = esialgne_viit->lopp;
				while (taisgraaf->jarg != NULL) {
					taisgraaf_eelmine = taisgraaf;
					taisgraaf = taisgraaf->jarg;
					algtipp_vordlemiseks = taisgraaf->algus;
					lopptipp_vordlemiseks = taisgraaf->lopp;
					if (algtipp == algtipp_vordlemiseks && lopptipp == lopptipp_vordlemiseks) {
						taisgraaf_eelmine-> jarg = taisgraaf -> jarg;
						break;
					}
				}
				taisgraaf = taisgraaf_algus;
			}
			
		LISA_MUUTUJATESSE(a_tipp); 
		LISA_MUUTUJATESSE(l_tipp); 
		while(taisgraaf->jarg!=NULL){ 
			taisgraaf=taisgraaf->jarg;/*lï¿½bimata servale*/ 
			PANE_UUS_VAARTUS(taisgraaf->algus,a_tipp); 
			PANE_UUS_VAARTUS(taisgraaf->lopp,l_tipp); 
				 
			if (KAS_TINGIMUSED(tingimused)){
				
				LABI_VALEM(v); 
			}
			                        
		}/*while*/ 
		
		KUSTUTA_MUUTUJATEST(a_tipp); 
		KUSTUTA_MUUTUJATEST(l_tipp); 
			
	}
	//ALO AASMÄE OSA LÕPP//

}/*LABI_RAJADEGA_VALEM_EEL*/
/*======================================================*/
void LABI_RAJADEGA_VALEM(int algraja,struct top *metamuutujad,int loppraja,struct top *tingimused,struct top *v){
	/*v asub bigfaormulal,mida saab lï¿½bida LABI_VALEM abil */
	struct top *s,*p;
	int i;
	

	
	p=metamuutujad->down;/*esimesel tsï¿½klimuutujal*/
	if (metamuutujad->sem==32){/*oli enne 32!!!  :)*/
		/*p on #i#-l*/
			 /*p=p->down;*/
	
           LISA_MUUTUJATESSE(T[p->leks][0]);/*p all on tsï¿½klimuutuja*/
				
		for(i=algraja;i<=loppraja;i++){

			PANE_UUS_VAARTUS(i,T[p->leks][0]);

			if (KAS_TINGIMUSED(tingimused)){
					LABI_VALEM(v);
 			}
			
		}/*for*/
			
		KUSTUTA_MUUTUJATEST(T[p->leks][0]);
	}
	if (metamuutujad->sem==31){
		
				
		s=p->right;s=s->right;
			/*s on nyyd 'metavariable*/
	
		LISA_MUUTUJATESSE(T[s->leks][0]);/*s-i alt*/
	
		for(i=algraja;i<=loppraja;i++){
			
			PANE_UUS_VAARTUS(i,T[s->leks][0]);
			LABI_RAJADEGA_VALEM(algraja,p,loppraja,tingimused,v);    
		}/*for*/
		KUSTUTA_MUUTUJATEST(T[s->leks][0]);
		
	}
}/*LABI_RAJADEGA_VALEM*/
/*======================================================*/
int KAS_TINGIMUSED(struct top *p){/* p on 'constraints' s.t.
	p->sem==38 voi 39*/
	struct top *r;
if (p==NULL) return(1);
	else{
	  if (p->sem==39){/*siis on vaid "veel" ï¿½ks tingimus*/
		p=p->down;/*p->sem==40*/
				
		return(KAS_TINGIMUS(p));
	  }else{
	       if (p->sem==38){
		   p=p->down;
		   r=p->right;r=r->right;
		   return(KAS_TINGIMUS(r)&&KAS_TINGIMUSED(p));
	       }
	   }/*else*/
	}/*else*/
}/*KAS_TINGIMUSED*/
/*==============================================================*/
 int KAS_TINGIMUS(struct top *r){  /*r asub 'tingimus'-l, r->sem==40*/
  int vp,pp,abi;
  struct top *d;
  d=r->down;
  vp=LEIA_ALAMPUU_VAARTUS(d);
  d=d->right;d=d->right;
  pp=LEIA_ALAMPUU_VAARTUS(d);
  d=r->down;d=d->right;
  switch(d->sem){
	      case 41:
	     case 33:if (vp<pp) return(1); else return(0);
	     case 46:if (vp==pp) return(1); else return(0);
	     case 48:if (vp>pp) return(1); else return(0);
   
	     case  42:	     
	    case  34:if (vp<=pp) return(1); else return(0);
	     case 49:if (vp>=pp) return(1); else return(0);
	     case  44:
	     case 36:abi=KAS_EELNEB(vp,pp);/*saadud mingi arv bitte 
							1-ks muutmise teel*/
		  return(abi);
	     case 43:
	     case 35:abi=KAS_EELLANE(vp,pp);/*saadud ï¿½he biti 
							1-ks muutmise teel*/
		  return(abi);
	      case 37:
            case 45:
			return(1);	
				/*tuleb vastav funktsioon teha*/
						
	      case 47: if (vp==pp) return(0);else return(1);
	     }
   }/*KAS_TINGIMUS*/
/*======================================================*/
 int KAS_EELNEB(int a,int b){
  int ast,abitt,bbitt;
  if (a>=b) return(0); 
  ast=1;
  while(ast<b) ast=ast*2;
  ast=ast*2; 
  while (1){
	    abitt=bbitt=0;
		  ast=ast/2;
		  if(ast<=a){
		       a=a-ast;
				abitt=1;
				}
		  if(ast<=b){
		       b=b-ast;
				bbitt=1;
				}
	       if((abitt>bbitt) || (ast==1)) break;
		  }
  if(abitt>bbitt) return(0); else return(1);
  }/*KAS_EELNEB*/
/*======================================================*/
int KAS_EELLANE(int a,int b){
int ast,abitt,bbitt,h;
  if(a>=b) return(0);
  h=0;  /*on h paari,millede korral abitt<bbitt*/
  ast=1;
  while(ast<b) ast=ast*2;
  ast=ast*2;
  while (1){
	    abitt=bbitt=0;
	    ast=ast/2;
	       if(ast<=a){
		       a=a-ast;
			     abitt=1;
			     }
	    if(ast<=b){
		       b=b-ast;
			     bbitt=1;
			     }
	    if(abitt<bbitt) h++;
	    if(abitt>bbitt) return(0);
	    if(h>1) return(0);
	    if(ast==1) break;
	    }
 if(h==1) return(1);else return(0);
}/*KAS_EELLANE*/
/*======================================================*/
int ASTE(int a,int b){
  int sum,i;
  if(a==0) return(0);
  sum=1;
  for(i=0;i<b;i++) sum=sum*a;
  return(sum);
  }/*ASTE*/
/*=======================================================*/
int LEIA_ALAMPUU_VAARTUS(struct top *l){/*aritmeetilise avaldise vï¿½ï¿½rtus*/
  /*tipuks 'expression'-ist lahtuv produktsioon,l->sem==50 kuni 65*/
  struct top *d1,*d2,*r;
  struct paarid *graafile_viit;
  int v,vp,pp,moode;
  char abike;
  v=0;
  switch (l->sem)
	 {case 50:d1=l->down;
		  d2=d1->right;
		  v=LEIA_ALAMPUU_VAARTUS(d1)+LEIA_ALAMPUU_VAARTUS(d2);
		  break;
	  case 51:d1=l->down;
		  d2=d1->right;
		  v=LEIA_ALAMPUU_VAARTUS(d1)-LEIA_ALAMPUU_VAARTUS(d2);
		  break;
	case 59:r=l->down;
		l=r;
		abike=T[l->leks][0];      /*l asub muutujal*/
			
		v=LEIA_VAARTUS(abike);
		
		  break;
	  case 53:d1=l->down;
		  d2=d1->right;
		  v=LEIA_ALAMPUU_VAARTUS(d1)*LEIA_ALAMPUU_VAARTUS(d2);
		  break;
	  case 54:d1=l->down;
		  d2=d1->right;
		  v=LEIA_ALAMPUU_VAARTUS(d1)/LEIA_ALAMPUU_VAARTUS(d2);
		  break;
	  case 81:
	  case 57:
	  case 55:
	  case 52:  v=LEIA_ALAMPUU_VAARTUS(l->down);
		  break;
	  case 56:d1=l->down;
		  d2=d1->right;
			/*kui ' ^{ analuusipuus pole*/
		  v=ASTE(LEIA_ALAMPUU_VAARTUS(d1),LEIA_ALAMPUU_VAARTUS(d2));
		  break;
	  case 58:v=LEIA_ALAMPUU_VAARTUS(l->down);
		  break;
	      
	   case 63:l=l->down;/*min*/
				  
			l=l->right;
			vp=LEIA_ALAMPUU_VAARTUS(l);
			l=l->right;
			pp=LEIA_ALAMPUU_VAARTUS(l);
			if (vp<pp) v=vp; else v=pp;
		break;
	    case 64:l=l->down;/*max*/
			l=l->right;
			vp=LEIA_ALAMPUU_VAARTUS(l);
			l=l->right;
			pp=LEIA_ALAMPUU_VAARTUS(l);
			if (vp>pp) v=vp; else v=pp;
		break;
	  case 65: l=l->down;
			l=l->right;
			return(LEIA_ALAMPUU_VAARTUS(l));
		break;
	  case 100:printf("LEIA_A 1\n");
		  abike=T[l->leks][0];      /*l asub muutujal*/
		  v=LEIA_VAARTUS(abike);
		  break;
	  case 101:
			sscanf(T[l->leks],"%d",&v);
		  		/*d1 asub string-tï¿½ï¿½pi konstandil*/
		break;
	   case  62: l=l->down;
		if  (LEIA_ALAMPUU_VAARTUS(l)<0)
			return((-1)*LEIA_ALAMPUU_VAARTUS(l)); 		
		else return (LEIA_ALAMPUU_VAARTUS(l));
		break;
	   case  61: l=l->down;
				/*oleme '#i#-l'*/
		graafile_viit=ANNA_GRAAF(T[l->leks][0]);
		moode=0;	
		while(graafile_viit->jarg!=NULL){ 
			graafile_viit=graafile_viit->jarg;
				/*lï¿½bimata servale*/
			moode++;
		}
		return(moode);
	    case 60: l=l->down;
		 	graafile_viit=ANNA_GRAAF(T[l->leks][0]);
			moode=0;
			while(graafile_viit->jarg!=NULL){ 
			graafile_viit=graafile_viit->jarg;
				/*lï¿½bimata servale*/
			if (moode<graafile_viit->algus)
				moode=graafile_viit->algus; 		
			if (moode<graafile_viit->lopp)
				moode=graafile_viit->lopp; 
		}
		return moode;
	  }/*switch*/
   return(v);
 }/*LEIA_ALAMPUU_VAARTUS*/
/*======================================================*/
void LISA_MUUTUJATESSE(char a){/*uue tsï¿½klimuutuja kasutuselevï¿½tt*/
  struct hetkevaartus *p;
  p=(struct hetkevaartus *)malloc(sizeof(struct hetkevaartus));
  memset(p,'\0',(sizeof(struct hetkevaartus)));
  p->mark=a;
  if (Muut->jarg==NULL) {
		p->jarg=NULL;
		Muut->jarg=p;
	}
	else{
		p->jarg=Muut->jarg;
		Muut->jarg=p;/*p sai esimeseks muutujaks selles sï¿½ilitusmassiivis*/
	}
 }/*LISA_MUUTUJATESSE*/
/*=======================================================*/
void PANE_UUS_VAARTUS(int a,char b){/*tsï¿½klimuutujale b uus vï¿½ï¿½rtus*/
  struct hetkevaartus *p;
  p=Muut->jarg;/*Eeldame, et vï¿½hemalt 1 muutuja on siin!*/
  while (p->mark!=b) p=p->jarg;
  p->vaartus=a;
 }/*PANE_UUS_VAARTUS*/
/*========================================================*/
void KUSTUTA_MUUTUJATEST(char b){/*tsï¿½klimuutuja lï¿½petab tï¿½ï¿½*/
  struct hetkevaartus *p,*s,*d;
  p=Muut;
  s=p->jarg;
  while (s->mark!=b){ 
		     p=p->jarg;
		     s=p->jarg;
		     }
  d=s->jarg;
  p->jarg=d;
  free(s);
 }/*KUSTUTA_MUUTUJATEST*/
/*=========================================================*/
int LEIA_VAARTUS(char a){/*mingile muutujale vï¿½ï¿½rtuse leidmine,
		kui seda pole,siis kï¿½sitakse selle vï¿½ï¿½rtust*/
  struct hetkevaartus *p,*r;
  p=Muut;
	
  while ((p->mark!=a)&&(p->jarg!=NULL)) p=p->jarg;
	
  if (p->mark==a) {
		
		return(p->vaartus);
		}
	else{
		
	 r=(struct hetkevaartus *)malloc(sizeof(struct hetkevaartus));
	    memset(r,'\0',(sizeof(struct hetkevaartus)));
	    r->jarg=(struct hetkevaartus *)NULL;
	 p->jarg=r;
	 r->mark=a;
		printf("Input %c=",a);
	    /*printf("Muutuja %c on vï¿½ï¿½rtustamata.%c=",a,a);*/
	    scanf("%d",&r->vaartus);
	    printf("\n");
	    return(r->vaartus);
	}/*else*/
	
 }/*LEIA_VAARTUS*/
/*======================================================*/
int MITMES(struct top *r){/*Mitmes leitud muutuja (koos indeksiga) on
	muutujate sï¿½ilitusmassiivis,kui seda pole,siis see lisatakse*/
	/*[serv,serv] ei ole intsidentsed,siis tagastab  -2 */
	/*[serv,serv] on  intsidentsed , sii stagastab -1*/
	/*[serv,tipp] ([tipp,serv]) ei ole   intsidentsed , siis tagastab -2*/
	/*[serv,tipp] ([tipp,serv])on  intsidentsed , sii stagastab -1*/
	/*nendel ja ainult nendel kordadel tagastab -1 voi -2*/	
	/*resultaatfaili siis vastavalt TRUE=and() ja FALSE=or()*/
	/*nt.'x2,5,10->23'*/
	/*r->sem==76 voi 77*/
  int jrk,pikim;
  struct indeeks *s=ind,*t=NULL;
  struct top *d1,*p,*d2;
  char *sona;
  struct paarid *servad;
  int esim_algus,esim_lopp,tein_algus,tein_lopp,esim_on_serv,tein_on_serv,tode;
	
/*'servade_pea' on  viit esimesele servmuutujale*/


  if (r->sem==76) {/*indeksiteta muutuja*/
	  d1=r->down;/* d1 on nï¿½ï¿½d #i#-l*/
	  sona=T[d1->leks];
		/*kui "leks"-il on mï¿½lu just tï¿½pselt niipalju kui vaja*/
  } 
  else{	
		/*indeksitega muutuja*/
	if (KAS_INTSIDENTSUS(r)) {
	
			/*kui esineb mingi serva  muutujanimi,
				aga indeks koosneb rohkem kui kahest
				muutujast voi 1 indeks ,mis on serva
				tahis,siis  abort()*/  	
		sona=(char *)malloc(3); 
	 	memset(sona,'\0',3);  		
	}
	else{  	
		/*kas indeksites sisaldub mingi serva 
				nimi ja kas siis on indekseid tapselt 2 tukki*/
		
	 	sona=LEIA_SONA(r);/*r->sem==77*/ 
		
	}
 		
  }
  	
	
if (!strlen(sona)){
	
	/*siin peame f-ni  vaartuse  tagastama kas -2 voi -1(FALSE voi TRUE)*/
	
	d2=r;r=r->down;
	
	r=r->right;	
	r=r->down;
		/*r on    expression1*/
		/*nuud indeks kujul {a,b},kus mina olen a-l*/
	esim_on_serv=0;
	tein_on_serv=0;
	
		
	if (KAS_ON_SERV(r)){
		
		esim_on_serv=1;
		if (r->sem==81){d1=r->down;d1=d1->down;} 
			else d1=r->down;
			
		d1=d1->down;
		d1=d1->down;d1=d1->down;
			/*serva nimi on T[d1->leks][0]*/
		servad=servade_pea;
		while((servad->nimi)!=T[d1->leks][0]) servad=servad->jarg;
		esim_algus=servad->algus;
		esim_lopp=servad->lopp;
	}
	else {
		
		esim_algus=LEIA_ALAMPUU_VAARTUS(r);
		
		
	}
	
	p=r->right;p=p->right;
	
		/*jargmisele indeksile*/
	if (KAS_ON_SERV(p)){
			
		tein_on_serv=1;
		if (p->sem==81){d1=p->down;d1=d1->down;} 
		else d1=p->down;
		
		d1=d1->down;
		d1=d1->down;d1=d1->down;
			/*serva nimi on T[d1->leks][0]*/
		servad=servade_pea;
		while((servad->nimi)!=T[d1->leks][0]) servad=servad->jarg;
		tein_algus=servad->algus;
		tein_lopp=servad->lopp;
	}
	else {
		tein_lopp=LEIA_ALAMPUU_VAARTUS(p);
		
	}
	
	
		/*nuud inf teada,hakkame toevaartust tagastama*/
	

	tode=0;
	
	if ((esim_on_serv)&&(tein_on_serv)){printf("Valik 1\n");
		if((esim_algus==tein_algus)||(esim_algus==tein_lopp)||
		(esim_lopp==tein_algus)||(esim_lopp==tein_lopp))
			tode=1;
	}
		
	if ((!esim_on_serv)&&(tein_on_serv)){
		if ((esim_algus==tein_algus)||(esim_algus==tein_lopp))
			tode=1;
	}
		
	if ((esim_on_serv)&&(!tein_on_serv)){
		if ((tein_lopp==esim_algus)||(tein_lopp==esim_lopp))
			tode=1;
	}
	
	if (!((!esim_on_serv)&&(!tein_on_serv)))return (tode-2);
		else{	
			/*siis on kï¿½ll kaks komponenti,kuid molemad on tipu tï¿½hised*/
			/*tegemist tegelikult kaarega*/
		
			sona=LEIA_SONA(d2);
		
				/*otsime 'sona'-le vastava jarjekorranumbri*/
		if(s->jarg==NULL) {
			/*kui esimene kirjutatav muutuja ï¿½ldse*/
		
	      	t=(struct indeeks *)malloc(sizeof(struct indeeks));
						
	      	memset(t,'\0',sizeof(struct indeeks));
		
	      	memcpy(t->alus,sona,20);
	      	t->jarg=NULL;
	      	ind->jarg=t;
		
	      	return(1);
		}
		else{	
				/*kui muutujaid leidub juba */
			s=ind->jarg;/**/
  			jrk=1;/*Oleme esimesel*/
  
	  		while(1){
				if(strlen(sona)>=strlen(s->alus)) pikim=strlen(sona);
	    			else pikim=strlen(s->alus);
				
	   			if(memcmp(s->alus,sona,pikim)==0) return(jrk);
	  			jrk++;
	   			if(s->jarg==NULL) break;
					/*siis tuleb lisada lï¿½ppu*/
	      		else s=s->jarg;
	   		}/*while*/
  			t=(struct indeeks *)malloc(sizeof(struct indeeks));
      		memset(t,'\0',sizeof(struct indeeks));
		
      		memcpy(t->alus,sona,20);
      		t->jarg=NULL;
      		s->jarg=t;
      		return(jrk);
		}/*else*/

		}/*kui molemad on tegelikult tipud*/
}/*kui leidub indeksites serv*/
	else{
		
		/*kui indeksites serva ei ole*/
  	if(s->jarg==NULL) {
		/*kui esimene kirjutatav muutuja ï¿½ldse*/
		
	      t=(struct indeeks *)malloc(sizeof(struct indeeks));
	      memset(t,'\0',sizeof(struct indeeks));
		
	      memcpy(t->alus,sona,20);
	      t->jarg=NULL;
	      ind->jarg=t;
		
	      return(1);
	}
	else{	/*kui muutujaid leidub juba */
		s=ind->jarg;/**/
  		jrk=1;/*Oleme esimesel*/
  
	  while(1){
		if(strlen(sona)>=strlen(s->alus)) pikim=strlen(sona);
	    else pikim=strlen(s->alus);
			
	   if(memcmp(s->alus,sona,pikim)==0) return(jrk);
	   jrk++;
	   if(s->jarg==NULL) break;/*siis tuleb lisada lï¿½ppu*/
	      else s=s->jarg;
	   }
  t=(struct indeeks *)malloc(sizeof(struct indeeks));
      memset(t,'\0',sizeof(struct indeeks));
      memcpy(t->alus,sona,20);
      t->jarg=NULL;
      s->jarg=t;
      return(jrk);
	}/*kui pole tegemist servaga*/
}/*pikk else-osa*/ 
 }/*MITMES*/
/*===========================================================*/
 char *LEIA_SONA(struct top *r){/*r->sem==77*/
	 
  struct top *d1;
  char *sone,*sone1,*sone2,*soneabi;
  
  int suurus;
  sone=(char *)malloc(sizeof(char)*20);
  sone1=(char *)malloc(sizeof(char)*20);
  sone2=(char *)malloc(sizeof(char)*20);
  soneabi=(char *)malloc(sizeof(char)*20);
  memset(sone,'\0',sizeof(char)*20);
  memset(sone1,'\0',sizeof(char)*20);
  memset(sone2,'\0',sizeof(char)*20);
  memset(soneabi,'\0',sizeof(char)*20);
  d1=r->down;
	/*#i#-l*/
  soneabi[0]=T[d1->leks][0];
	/*Muutuja "tï¿½ht"*/
  strcat(sone,soneabi);
  strcat(sone,"_{");
  d1=r->down;d1=d1->right;
		/*d1 on vasakult 1.'indexes1'*/
  		/*d1->sem==80 vï¿½i 81*/
  		/*sone    sone2+sone1*/

  while (d1->sem==80){
		/*siis indeks koosneb rohkem kui ï¿½hest argumendist*/
		
			r=d1->down;/*'expression1'*/
			
			 d1=r->right;
					
			d1=d1->right;
				
				/*d1 asub 'indexes1'-el*/
		
			 suurus=LEIA_ALAMPUU_VAARTUS(r);
			 sprintf(sone2,"%d",suurus);
				/*teisendab arvudena antud info stringiks*/
			 strcat(sone,sone2);/*oli vastupidi*/
			
			memset(sone2,'\0',sizeof(char)*20);/*oli sone1*/
			strcat(sone,",");
			/*strcat(sone1,sone2);*/
		          
	
		
   }/*while*/
	
  		/*Nï¿½ï¿½d oleme  jï¿½udnud indeksi esimese ,liikmeni*/
  		/*d1->sem==81*/
	r=d1->down;
	
		/*r on nï¿½ï¿½d 'expression'-il*/
	suurus=LEIA_ALAMPUU_VAARTUS(r);
  sprintf(sone2,"%d",suurus);/*oli sone1*/   
  strcat(sone,sone2);/*oli teine liige 'sone2'*/
/*  strcat(sone,sone1);*//*oi teine liige 'sone1'*/
  strcat(sone,"}");
  free(sone1);
  free(sone2);


  return(sone);
 }/**LEIA_SONA*/
/*===========================================================*/
struct paarid *GRAAF(char *failinimi){
	/*tagastab osuti esimesele graafi servale.*/
					/*s.t. graaf->jarg on esimene serv*/
		/*praegu eeldame, et graaf on orienteerimata*/
	char *puhver=NULL;
	int algtipp,lopptipp,i,j,k;
	
	char *alg,*lopp,*s;
	struct paarid *servake,*algus;
	puhver=jarray(failinimi);
	algus=(struct paarid *)malloc(sizeof(struct paarid));
	servake=(struct paarid *)malloc(sizeof(struct paarid));
	memset(algus,'\0',sizeof(struct paarid));
	memset(servake,'\0',sizeof(struct paarid));
	algus->jarg=(struct paarid *)NULL;
	servake->jarg=(struct paarid *)NULL;
	s=(char *)malloc(1000);
	memset(s,'\0',1000);
	
	
	
	i=0;
	while(i<strlen(puhver)){/*oli puhver[i]!='.'*/
		while((i<strlen(puhver))&&(puhver[i]!='e')) i++;
			
		if (i<strlen(puhver)){/*siis kindlasti 'e'-l*/
				/*puhver[i]=='e',puhver[i+1]==' ',....*/
				/*leida 1.tipp*/
			while(puhver[i+1]==' ') i++;
			i++;/*puhver[i] on arvu  1.*/
				
			j=i;
			while(puhver[j]!=' ') j++;
				/*algtipp on nï¿½ï¿½d puhver[i]...puhver[j-1]*/
			alg=(char *)malloc(sizeof(char)*(j-i+1));
			memset(alg,'\0',sizeof(char)*(j-i+1));
			for(k=0;k<j-i;k++) alg[k]=puhver[i+k]; 
			algtipp=atoi(alg);
		
				/*puhver[j]==' '*/
			while(puhver[j]==' ') j++;
			i=j;
				/*i.ndast elemendist algab serva lï¿½pptipp*/
			while((puhver[j]<='9')&&(puhver[j]>='0')) j++;
				/*lï¿½pptipp on nï¿½ï¿½d puhver[i]...puhver[j-1]*/
			lopp=(char *)malloc(sizeof(char)*(j-i+1));
			memset(lopp,'\0',sizeof(char)*(j-i+1));
			for(k=0;k<j-i;k++) lopp[k]=puhver[i+k]; 
			lopptipp=atoi(lopp);
			servake=(struct paarid *)malloc(sizeof(struct paarid));
			servake->algus=algtipp;
			servake->lopp=lopptipp;
			servake->jarg=algus->jarg;
			algus->jarg=servake;
				/*panime saadud serva esimeseks*/
					
			i=j;
		}/*if-lause*/
	}/*while*/
	/*algus->jarg on esimene serv graafis*/
	return(algus);
}/*GRAAF*/
/*===========================================================*/

struct paarid *LOO_GRAAF_SUURUSEGA(int moode){
		/*tagastab osuti esimesele graafi servale.*/
					/*s.t. graaf->jarg on esimene serv*/

	int algtipp,lopptipp,i,j;
	
	char *alg,*lopp,*s;
	struct paarid *serv,*algus;
	
	algus=(struct paarid *)malloc(sizeof(struct paarid));
	serv=(struct paarid *)malloc(sizeof(struct paarid));
	memset(algus,'\0',sizeof(struct paarid));
	memset(serv,'\0',sizeof(struct paarid));
	algus->jarg=(struct paarid *)NULL;
	serv->jarg=(struct paarid *)NULL;
	
	for(i=1;i<=moode;i++) {
		algtipp = i;
		for(j=1;j<=moode;j++) {
			lopptipp  = j;
			serv=(struct paarid *)malloc(sizeof(struct paarid));
			serv->algus=algtipp;
			serv->lopp=lopptipp;
			serv->jarg=algus->jarg;
			algus->jarg=serv;
		}
	}
	return (algus);
}



void EEMALDA_GRAAF(char nimi){
			/*Eemaldab graafide massiivist graafi, mille nimi on 'nimi' */
	struct graafpea *abi1,*abi2;
	abi1=GRAAFIDE_MASSIIV;
	while((abi1->jarg!=NULL)&&(abi1->nimi!=nimi)) {
			abi2=abi1;
			abi1=abi1->jarg;
			
	}
	if (abi1->nimi==nimi){
			abi2->jarg=abi1->jarg;
		}
}/*EEMALDA_GRAAF*/
/*===========================================================*/
struct paarid *ANNA_GRAAF(char nimi){
			/*tagastab viida graafile,kui see olemas on*/
			/*vastasel juhul kï¿½sib kasutajalt graafifaili nime*/
struct graafpea *abi,*abi1;
struct paarid *abi5;
char *failinimi;
	
	abi=GRAAFIDE_MASSIIV;
	
	while((abi->jarg!=NULL)&&(abi->nimi!=nimi)) abi=abi->jarg;
	
	if (abi->nimi==nimi) return(abi->graph);
		else{
			failinimi=(char *)malloc(256);
			memset(failinimi,'\0',256);
			printf("Graph file not found\n");
			/*printf("Sellise nimega graafi pole!\n");*/
			printf("Input file name for graph %c=\n",nimi);
			/*printf("Sisestage faili nimi, milles graaf %c esineb:\n",nimi);*/
			scanf("%s",failinimi);
			abi5=GRAAF(failinimi);
			abi1=(struct graafpea *)malloc(sizeof(struct graafpea));
			memset(abi1,'\0',sizeof(struct graafpea));
			abi1->nimi=nimi;
			abi1->graph=abi5;
			abi1->jarg=GRAAFIDE_MASSIIV->jarg;
			GRAAFIDE_MASSIIV->jarg=abi1;
				/*paneme leitud graafi esimeseks*/      
			return(abi5);           
		}
}/*ANNA_GRAAF*/
/*==========================================================*/
void LABI_RAJADEGA_GRAAF(struct top *s,struct top *tingimused,struct paarid *graafile_viit,struct top *v){
	/*s on 'metavariables1',senantilises mottes serv-tï¿½ï¿½pi muutujad*/
struct paarid *servad;
struct top *p;

	if (s->sem==31){
		s=s->down;
		p=s->right;p=p->right;
			/*p on "viimane" serv*/
		LISA_SERV(T[p->leks][0]);
		servad=graafile_viit;
		while(servad->jarg!=NULL){
				servad=servad->jarg;
				PANE_UUS_VAARTUS_SERVALE(T[p->leks][0],servad->algus,servad->lopp);
				/*if (loendaja==0) loendaja++;else fprintf(FF1,"%s",",");*/
				LABI_RAJADEGA_GRAAF(s,tingimused,graafile_viit,v);
				
		}
		KUSTUTA_SERV(T[p->leks][0]);
	}/*s->sem==31*/
	/*nï¿½ï¿½d s->sem==32*/
	else{
		s=s->down;
			/*ainus serv-tï¿½ï¿½pi muutuja*/
		LISA_SERV(T[s->leks][0]);
		servad=graafile_viit;
		
		while(servad->jarg!=NULL){
				servad=servad->jarg;
				PANE_UUS_VAARTUS_SERVALE(T[s->leks][0],servad->algus,servad->lopp);				
					
					/*LABI_RAJADEGA_GRAAF(s,tingimused,graafile_viit,v);*/
				if (KAS_TINGIMUSED(tingimused)) {
					
					LABI_VALEM(v);
				}
		}	
		KUSTUTA_SERV(T[s->leks][0]);
	}
}
/*========================= AINI osa algus============================== */
void blue(char *t){
	fprintf(Logi,"<FONT COLOR=\"0000FF\">%s</FONT>",t);
}       
void green(char *t){
	fprintf(Logi,"<FONT COLOR=\"008000\">%s</FONT>",t);
}       
void red(char *t){
	fprintf(Logi,"<FONT COLOR=\"FF0000\">%s</FONT>",t);
}       
void pset(int x,int y){
	blue("{");
	fprintf(Logi,"%s",T[x]);
	blue(",");
	fprintf(Logi,"%s",T[y]);
	blue("}");
}
void print_cxt(char **L){
	int i,j;
	char *rc;
	fprintf(Logi,"cxt: L=%p<BR>",L); fflush(Logi);
	for(i=tnr+1;i<nr+1;i++){
		rc=L[i];
		if(rc!=(char *)NULL){
			fprintf(Logi,"cxt: nr=%d i=%d rc=%p<BR>",nr,i,rc); 
			fflush(Logi);
			for(j=0;j<tnr+1;j++){
				if(rc[j]==1) pset(i,j);
				}
			}
		}
	fprintf(Logi,"<BR>"); fflush(Logi);     
}
void print_cxtp(char **L){
int i;
	char *rc;
	for(i=1;i<nr+1;i++){
		rc=L[i];
		if(rc!=(char *)NULL) fprintf(Logi,"L[%d]=%p<BR>",i,rc);
		}
}
void print_DEP(void){
char **L;
char *rc;
int i,j,k;
	for(i=tnr+1;i<nr+1;i++){
		L=DEP[i];
		if(L!=(char **)NULL){
			fprintf(Logi,"<BR>DEP[%d]=%p<BR>",i,L);
			for(j=0;j<tnr+1;j++){
				rc=L[j];
				if(rc!=(char *)NULL){
					fprintf(Logi,"L[%d]=%p<BR>",j,rc);
					for(k=0;k<tnr+1;k++)
					   fprintf(Logi,"rc[%d]=%d ",k,rc[k]);
					fprintf(Logi,"<BR>");
					}
				}
			}
		}
}
void print_all_cxt(void){
char **L;
int i;
	for(i=tnr+1;i<nr+1;i++){
		L=DEP[i];
		if(L!=(char **)NULL){
			fprintf(Logi,"<BR>dependent context of %s:<BR>",T[i]);
			print_cxt(L);
			fprintf(Logi,"<BR>");
			}
		}
	print_DEP();    
}
void print_all_cxtp(void){
char **L;
int i;
	for(i=tnr+1;i<nr+1;i++){
		L=DEP[i];
		if(L!=(char **)NULL){
			fprintf(Logi,"DEP[%d]=%p<BR>",i,L);
			print_cxtp(L);
			}
		}
}
void print_L(struct h *CP){
int i,j;
char *r;
	fprintf(Logi,"<BR>L: P%2d `%s' -&gt ",CP->P,T[CP->NT]); 
	for(j=0;j<CP->n;j++){
		i=CP->d[j];
		r=T[i];
		if(i<tnr+1) fprintf(Logi,"%s ",r);
		else fprintf(Logi,"`%s' ",r);
		}
	fprintf(Logi,"<BR>");
	fflush(Logi);
}
void print_h(struct h *t){
	int i,j;
	char *r;
	fprintf(Logi,"<BR>P=%d `%s' -&gt ",t->P,T[t->NT]); 
	for(i=0;i<t->n;i++){
		j=t->d[i];
		r=T[j];
		if(j<=tnr) fprintf(Logi,"%s ",r);
		else fprintf(Logi,"`%s' ",r);
		}
	fprintf(Logi,"<BR>");
	fflush(Logi);
}
void print_top(struct top *t){
fprintf(Logi," top %p: kood=%d leks=%d sem=%d label=%d truel=%d falsel=%d up=%p right=%p down=%p",
   t,t->kood,t->leks,t->sem,t->label,t->truel,t->falsel,t->up,t->right,
   t->down);
fprintf(Logi,"  (%s)<BR>",(t->leks==0) ? T[t->kood] : T[t->leks]);
if(t->right!=(struct top *)NULL) print_top(t->right);
if(t->down!=(struct top *)NULL) print_top(t->down);
}
void print_Itr(struct itr *t){
fprintf(Logi," itr %p: nr=%d loc=%d value=%d top=%p io=%d",
   t,t->nr,t->loc,t->value,t->t,t->io);
fprintf(Logi,"   (%s)<BR>",T[t->nr]);
}
void print_Ctr(struct ctr *t){
fprintf(Logi," ctr %p: nr=%d loc=%d value=%d",
   t,t->nr,t->loc,t->value);
fprintf(Logi,"   (%s)<BR>",T[t->nr]);
}
void print_one_top(struct top *t){
fprintf(Logi," top %p: kood=%d leks=%d sem=%d label=%d truel=%d falsel=%d up=%p right=%p down=%p",
   t,t->kood,t->leks,t->sem,t->label,t->truel,t->falsel,t->up,t->right,
   t->down);
fprintf(Logi,"  (%s)<BR>",(t->leks==0) ? T[t->kood] : T[t->leks]);
}
/* analï¿½ï¿½sipuu tipu trï¿½kk */
void p_top(char *t,struct top *p){
if(logi==1){
	fprintf(Logi,"%s top:<BR>",t);
	print_one_top(p);
	}  
}
/* adresseerib maatrikseid programmi analï¿½ï¿½si ajal. Sn=nr-(ident & con arv) */
/* scanner lisab id. ja c-d V-sse, iga puhul nr++ */
int adr(int i, int j){
	int k;
	k=(i-1)*Sn+j;
	if(k>(Sn+1)*(Sn+1)){
		fprintf(Logi,"adr: indeks %d ï¿½le piiri!<BR>",k);
		k=0;
		}
	return(k);      
}
void set_show(char *title){
	fprintf(Logi,"<H4><FONT COLOR=\"#0000FF\">%s",title);
	fprintf(Logi,"</FONT></H4>");
}
char *DefArray(int d){
	int n,i;
	char *M;
	n=(d*d);
	M=(char *)malloc(n);
	if(M==(char *)NULL){
		fprintf(Logi,"DefArray: I don't have memory enough..<BR>");
		ExiT();
		}
	for(i=0;i<n;i++) M[i]='\0'; 
	return(M);
}
/* vektoritele mï¿½lu eraldamine */
char *DefVect(int n){
	int i;
	char *M;
	M=(char *)malloc(n);
	if(M==(char *)NULL){
		fprintf(Logi,"I don't have memory enough..");
		ExiT();
		}
	for(i=0;i<n;i++) M[i]='\0'; 
	return(M);
}
/* s"ltuva konteksti puu ï¿½lemine tase: m"isted */
char ***defDEP(void){
	int i,n;
	n=(nr+1)*sizeof(char ***);
	DEP=(char ***)malloc(n);
	if(DEP==(char ***)NULL){
		fprintf(Logi,"I don't have memory enough..");
		ExiT();
		}
	for(i=0;i<nr+1;i++) DEP[i]=(char **)NULL;
	return(DEP);
}
/* s"ltuva konteksti puu vahetase: vasakud kontekstid */
char **defL(void){
	int i,n;
	char **L;
	n=(nr+1)*sizeof(char **);
	L=(char **)malloc(n);
	if(L==(char **)NULL){
		fprintf(Logi,"I don't have memory enough..");
		ExiT();
		}
	for(i=0;i<nr+1;i++) L[i]=(char *)NULL;
	return(L);
}
void ps(void){
	fprintf(Logi,"<BR>");
}
void ExiT(void){
	if(logi==1){
		fprintf(Logi,"Abnormal end<BR><BR>");
		fflush(Logi);
		fclose(Logi);
		}               
	abort();
}
/* paiskfunktsioon (vï¿½ti on prod. vahekeelne parem pool) */ 
int hfunc(int keylen,int *key){
	int h=0,g;
	int i;
	for(i=0;i<keylen;i++) h=h^key[i];
	g=h;
	h=h/HTL;
	h=g-(h*HTL);
	if((h<0)||(h>HTL)){
		fprintf(Logi,"HT error!<BR>");
		ExiT();
		}
	return(h);
}
/* produktsiooni otsimine paisktabelist */
struct h *ReadHash(int n,int *key){
	struct h *t;
	int i,a;
	a=hfunc(n,key);
	t=HT[a];
otsi:	
	if(t!=(struct h *)NULL){
		if(n==t->n){
			for(i=0;i<n;i++){
				if(key[i]!=t->d[i]) goto next;
				}
			return(t);
			}		
		next: t=t->col;
		goto otsi;
		}	
	return(NULL);
}
int p_viga(int i){
	int j;
	VEAD++;
	fprintf(Logi,"<BR>\rerror at symbol #%d %s<BR>",i,word);
	for(j=0;j<i;j++) fprintf(Logi,"%c",PBuf[j]);
	if(logi==1) fprintf(Logi,"%c",sk);
	while(i<Plen){
		fprintf(Logi,"%c",PBuf[i]);
		i++;
		}
	ps();
	return(1);
}
/* parser: lï¿½hte- ja vahekeelse programmi trï¿½kk */
void print_VK(int k){
int t,i,j;
	set_show("Input program:");
	for(i=0;i<k;i++){
		j=VK[i];
		if(j!=m_k){
			if(j!=k_k) fprintf(Logi,"%s ",T[j]);
			}
		}
	fprintf(Logi,"<BR>");
	set_show("Scanned program:");
	for(i=0;i<k;i++) fprintf(Logi,"%d ",VK[i]);
	fprintf(Logi,"<BR>");
	if(itl>0){
		fprintf(Logi,"<BR><FONT COLOR=\"#0000FF\">Identifiers:</FONT>");
		for(i=0;i<itl;i++){
			t=IT[i];
			fprintf(Logi,"%s%s",T[t],(i<itl-1) ? ", " : ";");
			}
		ps();   
		}
	if(ktl>0){
		fprintf(Logi,"<BR><FONT COLOR=\"#0000FF\">Constants:</FONT>");
		for(i=0;i<ktl;i++){
			t=KT[i];
			fprintf(Logi,"%s%s",T[t],(i<ktl-1) ? ", " : ";");
			}
		ps();
		}
}
/* eraldab mï¿½lu uuele tipule tï¿½hestiku V puusse */
struct D *newD(void){
	struct D *ptr;
	ptr=(struct D *)malloc(sizeof(struct D));
	if(ptr==NULL) ExiT();
	memset(ptr,'\0',sizeof(struct D));
	ptr->left=(struct D *)NULL;
	ptr->right=(struct D *)NULL;
	ptr->def=(struct R *)NULL;
	return(ptr);
}
/* tï¿½hestiku V elemendi otsimine */
struct D *getV(char *key){
	struct D *t;
	int res;
	t=DT;
	if(t==(struct D *)NULL) goto exit;
ring: if(t->loc==0) res=strcmp(key,T[t->code]);
	 else{
	 	res=strcmp(key,T[t->loc]);        
	 	fprintf(Logi,"key=%s t->loc=%d %s<BR>",key,t->code,T[t->loc]);  
		}
	 if(res==0) goto out;
	 if(res<0){
			if(t->left != (struct D *)NULL){
				t=t->left;
				goto ring;
				}
			else goto exit;
			}
		if(res>0){
			if(t->right != (struct D *)NULL){
				t=t->right;
				goto ring;
				}
			else goto exit;
			}
exit: t=(struct D *)NULL;
out:  
	return(t);
}
/* scanner: identifikaatori t""tlemine */
int Ident(int k){
	int t,i;
	char *id;
	for(i=0;i<itl;i++){
		t=IT[i];
		id=T[t];
		if(strcmp(id,word)==0) goto itis;
		}		
	nr++;
	memcpy(T[nr],word,20);
	VK[k]=m_k;
	IT[itl]=nr;
	itl++;
	k++;
	VK[k]=nr;
	k++;
	goto ok;
itis:	
	VK[k]=m_k;
	k++;
	VK[k]=t;
	k++;
ok:	memset(word,'\0',20);   
	return(k);
}
/* scanner: konstandi t""tlemine */
int Const(int k){
	int t,i;
	char *con;
	for(i=0;i<ktl;i++){
		t=KT[i];
		con=T[t];
		if(strcmp(con,word)==0) goto itis;
		}		
	nr++;
	memcpy(T[nr],word,20);
	VK[k]=k_k;
	KT[ktl]=nr;
	ktl++;
	k++;
	VK[k]=nr;
	k++;
	goto ok;
itis:	
	VK[k]=k_k;
	k++;
	VK[k]=t;
	k++;
ok:	memset(word,'\0',20);   
	return(k);
}
int lekseem(int i){
	char *b,*g;
	struct D *t;
	int a,j,s,n;
	b=PBuf+i;
	for(s=0;s<tnr+1;s++){
		g=TT[s];
		n=strlen(g);
		if(n==0) return(0);
/*		fprintf(Logi,"TT[%d]=%s n=%d<BR>",s,g,n);  */
		j=0;
		memset(word,'\0',20);  
		for(a=0;a<n;a++){
			if(b[a]!=g[a]) goto next;
			word[j]=b[j];
			j++;
			}
		i=i+j;  
		t=getV(word);
		if(t==NULL) {
            fprintf(Logi,"lekseem=%s<BR>",word);
            ExiT();
		}
		KOOD=t->code;
		return(j);
		next: j=0;
		}
	return(0);      
}               
							
/* teisendab lï¿½hteprogrammi vahekeelseks */
int scanner(void){
	struct D *t;
	int i,j,k,r;
	int olek;
	j=0;
	itl=ktl=0;
	m_k=k_k=0;
	VEAD=0;
/*      printf("scanner started\n"); */
	t=getV("#i#");
	if(t!=(struct D *)NULL) m_k=t->code;
	t=getV("#c#");
	if(t!=(struct D *)NULL) k_k=t->code; 
	vkl=Plen*8;
	VK=(int *)malloc(vkl);
	if(VK==NULL){
		fprintf(Logi,"I don't have memory enaugh..");
		return(0);
		}
	memset(VK,'\0',vkl);
/*	memset(IT,'\0',50*sizeof(int));
	memset(KT,'\0',50*sizeof(int));  */
	if(PBuf==NULL){
		fprintf(Logi,"scanner: pole faili %s",pr_name);
		return(0);
		}
	i=0;
	olek=0;
	marker=0;
	t=getV("#");
	if(t!=(struct D *)NULL) marker=t->code; 
	if(marker==0){
	    fprintf(Logi,"aksioomi definitsioonis peavad olema markerid '#'");
	    return(0);
	    }
	VK[0]=marker; 
	k=1;
	if(PBuf[i]=='#') i=1;
	memset(word,'\0',20);
	while(i<Plen){
		switch(olek){
			case 0:{
				if(isspace(PBuf[i])){
					i++;
					break;
					}
				if(isdigit(PBuf[i])){
					j=0;
					olek=2;
					break;
					}
				r=lekseem(i);
				if(r>0){
					VK[k]=KOOD;
					k++;
					memset(word,'\0',20);
					i=i+r;
					j=0;
					break;
					}
				j=0;
				memset(word,'\0',20);   
				if(isalpha(PBuf[i])){
					j=0;
					olek=1;
					break;
					}
				if(ispunct(PBuf[i])){
					p_viga(i);
					i++;
					j=0;
					memset(word,'\0',20);
					break;
					}       
				}
			case 1:{
				if(isalpha(PBuf[i])){
					word[j]=PBuf[i];
					i++;
					j++;
					break;
					}
				if(isdigit(PBuf[i])){
					word[j]=PBuf[i];
					i++;
					j++;
					break;
					}
				if(ispunct(PBuf[i])){
					k=Ident(k);
					j=0;
					olek=0;
					break;
					}
				if(isspace(PBuf[i])){
					k=Ident(k);
					j=0;
					i++;
					olek=0;
					break;
					}
				}       
			case 2:{
				if(isalpha(PBuf[i])){
					p_viga(i);
					i++;
					j=0;
					memset(word,'\0',20);
					break;
					}
				if(isdigit(PBuf[i])){
					word[j]=PBuf[i];
					i++;
					j++;
					break;
					}
				if(ispunct(PBuf[i])){
					k=Const(k);
					j=0;
					olek=0;
					break;
					}
				if(isspace(PBuf[i])){
					k=Const(k);
					i++;
					j=0;
					olek=0;
					break;
					}
				}       
			}
	}
	if(VK[k-1]!=marker){
		VK[k]=marker;
		k++;
		}
	print_VK(k);
	VKL=k;
	memset(word,'\0',20);
	memcpy(word,"DUMMY",5);
	Ident(k);
	dummy=nr;
	set_show("Scanner ended");
	if(VEAD>0) return(0);
	nr=Sn;
	return(1);
}
FILE *opr(void){
	of=fopen(rida,"rb");
	if (of==NULL){
		fprintf(Logi,"cannot open %s. Constructor?",rida);
		return(of);
		}
	return(of);
}
/* eraldab mï¿½lu uuele NT-definitsioonile: id / const */
struct R *newR(void){
	struct R *ptr;
	ptr=(struct R *)malloc(sizeof(struct R));
	if(ptr==NULL) ExiT();
	memset(ptr,'\0',sizeof(struct R));
	ptr->alt=(struct R *)NULL;
	return(ptr);
}
struct D *r_V(void){
	struct D *t;
	struct R *od,*d;
	int f;
	t=newD();
	fread(t,sizeof(struct D),1,of);
	if(feof(of)){
		fclose(of);
		return((struct D *)NULL);
		}
	f=ferror(of);
	if(f>0){
		fprintf(Logi,"file error %d",f);
		fclose(of);
		return((struct D *)NULL);
		}
	if(t->tunnus==0){
		t->def=newR();
		fread(t->def,sizeof(struct R),1,of);
		d=t->def;
		if(d->d!=(int *)NULL&&d->n>0){
				d->d=(int *)malloc(d->n*sizeof(int));
				if(d->d==NULL) ExiT();
				fread(d->d,d->n*sizeof(int),1,of);
				}
		od=d->alt;
		while(od!=(struct R *)NULL){
			d->alt=newR();
			d=d->alt;
			fread(d,sizeof(struct R),1,of);
			if(d->d!=(int *)NULL&&d->n>0){
				d->d=(int *)malloc(d->n*sizeof(int));
				if(d->d==NULL) ExiT();
				fread(d->d,d->n*sizeof(int),1,of);
				}
			od=d->alt;
			}
		}
	if(t->left!=(struct D *)NULL) t->left=r_V();
	if(t->right!=(struct D *)NULL) t->right=r_V();
	return(t);
}
/* tabelite lugemine */
void r_DEP(void){
	int i,j;
	char **L;
	char *rc;
	DEP=defDEP();
	fread(DEP,sizeof(char ***),nr+1,of);
	for(i=0;i<nr+1;i++){
		if(DEP[i]!=(char **)NULL){
			L=defL();
			fread(L,sizeof(char **),nr+1,of);
			DEP[i]=L;
			for(j=0;j<nr+1;j++){
				if(L[j]!=(char *)NULL){
					rc=DefVect(tnr+1);
					fread(rc,tnr+1,1,of);
					L[j]=rc;
					}
				}
			}
		}
/*      print_all_cxt();  */
}                                       
struct h *grec(void){
	struct h *L;
	L=(struct h *)malloc(sizeof(struct h));
	if(L==(struct h *)NULL){
		fprintf(Logi,"I haven't memory enough..");
		ExiT();
		}
	return(L);
}               
char *gM(int p){
	char *ptr=NULL;
	ptr=(char *)malloc(p);
	if(ptr==NULL){
		fprintf(Logi,"I haven't memory enough..");
		ExiT();
		}
	return(ptr);
}
struct h *r_hr(void){
	struct h *r;
	r=grec();
	fread(r,sizeof(struct h),1,of);
	r->d=(int *)malloc(r->n*sizeof(int));
	fread(r->d,r->n*sizeof(int),1,of);
	if(r->same!=(struct h *)NULL) r->same=r_hr();
	if(r->col!=(struct h *)NULL) r->col=r_hr();
	return(r);
}
void r_HT(void){
	int i;
	fread(HT,sizeof(struct h *),HTL,of);
	for(i=0;i<HTL;i++){
		if(HT[i]!=(struct h *)NULL) HT[i]=r_hr();
		}
}                                       
struct top *r_ptree(void){
	struct top *p;
	p=(struct top *)malloc(sizeof(struct top));
	if(p==(struct top *)NULL){
		fprintf(Logi,"I haven't memory enough..");
		ExiT();
		}
	fread(p,sizeof(struct top),1,of);
	if(p->right!=(struct top *)NULL) p->right=r_ptree();
	if(p->down!=(struct top *)NULL) p->down=r_ptree();
	return(p);
}
struct top *set_up(struct top *root){
	struct top *p=NULL;
	if(root!=(struct top *)NULL){
/*              p=(struct top *)NULL; */
		if(root->down!=(struct top *)NULL) p=set_up(root->down);
		if(root->right!=(struct top *)NULL) p=set_up(root->right);
		if(p->up!=(struct top *)NULL) p->up=root;
		}
	return(p);
}
int r_t(void){
	sprintf(rida,"%s.t",Nimi);
	if(opr()!=NULL){
		fread(&T,20,nr+1,of);
		fclose(of);
		return(1);
		}
	else return(0);
}
int r_parm(void){
	PARM=(struct parm *)malloc(sizeof(struct parm));
	if(PARM==(struct parm *)NULL) ExiT();
	memset(PARM,'\0',sizeof(struct parm));
	sprintf(rida,"%s.prm",Nimi);
	if(opr()!=NULL){
		fread(PARM,sizeof(struct parm),1,of);
		fclose(of);
		nr=PARM->nr;
		tnr=PARM->tnr;
		Sn=nr;
		context=PARM->BRC;
		Pnr=PARM->Pnr;
		dc=PARM->dep;
/*              printf("nr=%d tnr=%d context=%d Pnr=%d dc=%d\n",
			nr,tnr,context,Pnr,dc); */
		return(1);
		}
	return(0);
}
int r_sm(void){
	sprintf(rida,"%s.sm",Nimi);
	if(opr()!=NULL){
		semantika=(int *)malloc(sizeof(int)*(tnr+Pnr+1));
		fread(semantika,sizeof(int)*(tnr+Pnr+1),1,of);
		fclose(of);
		semflag=1;
		return(1);
		}
	return(0);
}
int r_v(void){
	sprintf(rida,"%s.v",Nimi);
	if(opr()!=NULL){
		DT=r_V();
		fclose(of);
		return(1);
		}
	else return(0);
}       
int r_ht(void){
	sprintf(rida,"%s.ht",Nimi);
	if(opr()!=NULL){
		r_HT();
		fclose(of);
		return(1);
		}
	else return(0);
}                       
int r_pm(void){
	sprintf(rida,"%s.pm",Nimi);
	if(opr()!=NULL){
		PM=DefArray(nr+1);
		fread(PM,(nr+1)*(nr+1),1,of);
		fclose(of);
		return(1);
		}
	else return(0);
}
int inxt(void){
	if(context==1){
		sprintf(rida,"%s.lc",Nimi);
		if(opr()!=NULL){
			LC=DefArray(nr+1);
			fread(LC,(nr+1)*(nr+1),1,of);
			fclose(of);
			}
		else return(0);
		sprintf(rida,"%s.rc",Nimi);
		if(opr()!=NULL){
			RC=DefArray(nr+1);
			fread(RC,(nr+1)*(nr+1),1,of);
			fclose(of);
			}
		else return(0);
	}
	return(1);
}
int depc(void){
	if(dc==1){
		sprintf(rida,"%s.dc",Nimi);
		if(opr()!=NULL){
			r_DEP();
			fclose(of);
			}
		else return(0);
		}
	return(1);
}
int r_tabs(void){
	int flag;
	flag=1;
	flag=r_parm();               /* PARM */
	if(flag==0) goto out;
	flag=r_t();                  /* T */
	if(flag==0) goto out;
/*      printf("T is\n");       */
	flag=r_sm();                 /* semantika  == v6imalda uus */
	if(flag==0) goto out;
/*      printf("seman is\n");           */
	flag=r_v();                  /* V puu */
	if(flag==0) goto out;
/*      printf("Voca is\n");            */
	flag=r_ht();                 /* hashtabel */
	if(flag==0) goto out;
/*      printf("ht is\n");      DA v6ta conxtrucist     */
	PM=DefArray(nr+1);
	flag=r_pm();                 /* PM */
	if(flag==0) goto out;
/*      printf("PM is\n");              */
	if(context>0){
		LC=DefArray(nr+1);
		RC=DefArray(nr+1);
		flag=inxt();                 /* LC & RC */
		if(flag==0) goto out;
		}
/*      printf("LC & RC are\n");                        */
if(dc>0){
	flag=depc();                 /* dependent context */
	}
out:    if(flag==0){
		fprintf(Logi,"cannot read all the tables..");
		}
	return(flag);
}
/* TT: sorted by length terminal alphabet */
void make_TT(void){
	int i,j;
/*      memset(TT,NULL,HTL);  */
	for(i=0;i<tnr+1;i++){
		TT[i]=(char *)malloc(20);
		if(TT[i]==NULL) ExiT();  
		memcpy(TT[i],T[i],20);
		}
	for(i=0;i<tnr;i++){
		for(j=i+1;j<tnr+1;j++){
			if(strlen(TT[i])<strlen(TT[j])){
				memcpy(word,TT[i],20);
				memcpy(TT[i],TT[j],20);
				memcpy(TT[j],word,20);
				}
			}
		}                       
}
struct top *analyzer(void){
	if(r_tabs()==0) return(0);
	Sn=nr;
	P=PBuf;
	make_TT();      
	set_show("Scanner started");
	if(scanner()==0||VEAD>0) return(0);
	set_show("Parser started");
	p_=parser();
	
	if(p_==(struct top *)NULL) return(0);
	set_show("Parsing tree");                       
	pp2html(p_);    
	ctree();
	return(p_);
}
void ctree(void){
	if(ktl>0){
		set_show("Table of constants");
		make_CT();
		}
	if(itl>0){
		set_show("Table of identifiers");
		make_IDT();
		fprintf(Logi,"<BR>");
		}       
}               
int red_dep(struct h *S,int x,int y){
	char **L;
	char *rc;
	L=DEP[S->NT];
	if(L!=(char **)NULL){
		rc=L[x];
		if(rc==(char *)NULL) return(0);
		if(rc[y]==1){
			hc=S;
			return(S->NT);
			}
		}
	return(0);
}	
int red_indep(struct h *t,int x,int y){
	int ret,r=0;
	struct h *S;
	S=t;
	while(S!=(struct h *)NULL){
		if(S->nc==0){
			if(LC[adr(S->NT,x)]>0){
				r++;
				ret=S->NT;
				hc=S;
				}
			}	
		S=S->same;
		}
	if(r==0){
		fprintf(Logi,"\n%s is not in any LCs\n",T[x]);
		hc=(struct h *)NULL;
		return(0);
		}
	if(r==1) return(ret);
	S=t;
	while(S!=(struct h *)NULL){
		if(S->nc==0){
			if(RC[adr(S->NT,y)]>0){
				hc=S;
				return(S->NT);
				}
			}	
		S=S->same;
		}
	fprintf(Logi,"\n%s is not in any RCs\n",T[y]);
	hc=(struct h *)NULL;
	return(0);
}
/* redutseerib lausevormi mitteterminali koodiks */
int reduce(struct h *t,int x, int y){
	int ret;
	struct h *S;
	ret=0;
	hc=(struct h *)NULL;
	if(t->same==(struct h *)NULL){
		hc=t;
		return(t->NT);
		}
	S=t;
	while(S!=(struct h *)NULL){
		if(S->nc>0){
			ret=red_dep(S,x,y);
			if(ret>0) return(ret);
			}
		S=S->same;
		}
	ret=red_indep(t,x,y);
	if(ret>0) return(ret);
	return(0);
}
char s[7][60]={
	" ",
	"<FONT COLOR=\"#FF4500\">=&#149</FONT>",
	"<FONT COLOR=\"#FF4500\">&lt&#149</FONT>",      
	" ",
	"<FONT COLOR=\"#FF4500\">&#149&gt</FONT>",      
	};
/* analï¿½ï¿½s: magasini ja analï¿½ï¿½simata teksti vï¿½ljatrï¿½kk */
int p_stack(char *name,int *v,char *rel,int a,int n,int k){
	int i,j,lv;
	fprintf(Logi,"<BR><TABLE BORDER=1><B><TR><TD NOWRAP>Stack & Word</TD>");
	for(lv=n;lv>0;lv--){
		if(rel[lv]==2) break;
		}
	for(i=a;i<n;i++){
		j=rel[i];
		fprintf(Logi,"<TD NOWRAP> %s",s[j]);
		j=v[i];
		if(i<lv)
		  fprintf(Logi,"<FONT COLOR=\"#0000FF\"><STRONG>%s</FONT></TD>",T[j]);
		else
		  fprintf(Logi,"<FONT COLOR=\"#FF0000\"><STRONG>%s</FONT></TD>",T[j]);
		}
	for(i=k; i<VKL; i++){
		j=VK[i];
		fprintf(Logi,"<TD>%s </TD>",T[j]);
		}
	fprintf(Logi,"</TR></TABLE>");
	return(0);
}
/* analï¿½ï¿½sipuu moodustamine: uus tipp */
struct top *newtop(int x,int i,int sem){
	struct top *ptr;
	ptr=(struct top *)NULL; 
	if(x<tnr+1 && semantika[sem]==0) goto out;
	ptr=(struct top *)malloc(sizeof(struct top));
	if(ptr==(struct top *)NULL){
		fprintf(Logi,"newtop: lack of memory of %d bytes<BR>",
		sizeof(struct top));
		ExiT();
		}
	memset(ptr,'\0',sizeof(struct top));
	ptr->up=(struct top *)NULL;
	ptr->right=(struct top *)NULL;
	ptr->down=(struct top *)NULL;
	ptr->kood=x;
	ptr->sem=semantika[sem];
	if((x==m_k)||(x==k_k)){
		ptr->leks=(i==0) ? dummy : VK[i];
		}
out:    return(ptr);
}
/* analï¿½ï¿½sipuu moodustamine: uue tipu/alampuu lisamine */
void red_tree(struct top *puu[],struct top *p,int s,int n){
	int i;
	struct top *cp=(struct top *)NULL;
	struct top *first=(struct top *)NULL;
	struct top *last=(struct top *)NULL;
	for(i=0;i<n;i++){
		if(puu[i+s]!=(struct top *)NULL){
			cp=puu[i+s];
			if(cp->sem==0){
				if(cp->down!=(struct top *)NULL){
				cp=cp->down;
				if(first==(struct top *)NULL) first=cp;
				if(last!=(struct top *)NULL) last->right=cp;
				while(cp->right!=(struct top *)NULL){
					cp=cp->right;
					}
				cp->up=(struct top *)NULL;
				goto next;
				}
			    }   
			if(first==(struct top *)NULL) first=cp;
			if(last!=(struct top *)NULL) last->right=cp;
			}
		next: last=cp;
		}
	p->down=first;
	if(last!=(struct top *)NULL) last->up=p;
}
int pp2html(struct top *p) {
	double pr;
	int n=1;
	struct top *t;
	char s[20];
	t=p;
naaber:
	if(t->right!=NULL){
		n++;
		t=t->right;
		goto naaber;
		}
	pr=100.0/n;
	sprintf(s,"\"%2.0f&#037\"",pr); 
	fprintf(Logi,"<table border=1><tr>");   
next:
	fprintf(Logi,
	"<td width=%s valign=\"top\"><STRONG>",s);      
	if((p->kood==m_k)||(p->kood==k_k)) fprintf(Logi,"%s",T[p->leks]);
	else fprintf(Logi,"%s",T[p->kood]); 
	if(p->down!=NULL) pp2html(p->down);
	fprintf(Logi,"</td>");  
	if(p->right!=NULL){                     
		p=p->right;
		goto next;
		}
	fprintf(Logi,"</tr></table>");
	return 1;
}  
void p_sf(void){
	int i,j;
	fprintf(Logi,"<BR>Sentential form: ");
	for(i=0; i<n_; i++){
		j=lausevorm[i];
		fprintf(Logi,"%s ",T[j]);
		}
	fprintf(Logi,"<BR>");   
	fflush(Logi);
}               
void sentential_form(void){
/*      fprintf(Logi,"<BR>Sentencial form started J=%d<BR>",J);  */
	for(s_=J;s_>0;s_--){
		if(rela[s_]==2) break;
		}
	memset(lausevorm,'\0',sizeof(lausevorm));
	n_=J-s_;
/*      fprintf(Logi,"<BR>n_=%d<BR>",n_);  */
	for(l_=0;l_<n_;l_++){
	lausevorm[l_]=Stack[s_+l_];
/*      p_sf();  */
	}
}
/* analï¿½saator. Ette vahekeelne programm, vï¿½lja (h"re) analï¿½ï¿½sipuu */
struct top *parser(void){
	int i,j,r;
	char c=' ';
	memset(Stack,'\0',sizeof(Stack));
	memset(rela,'\0',20);
fflush(Logi);
	for(I=0;I<20;I++) puu[I]=(struct top *)NULL;
fflush(Logi);
	Stack[0]=marker;
	x_=marker;
	J=1;
	I=1;
while(I<VKL){
fflush(Logi);
	rel=PM[adr(x_,VK[I])];
	j=VK[I];
	fflush(Logi);           
	rela[J]=rel;
	switch(rel){
		case 0: 
			j=VK[I];
			fprintf(Logi,"<BR>no relationship between the symbols %s and %s<BR>",T[x_],T[j]);
fflush(Logi);
			fprintf(Logi,"The relationships of %s<BR>",T[x_]);
fflush(Logi);
			for(i=0;i<nr+1;i++){
fflush(Logi);
				r=PM[adr(x_,i)];
				if(r>0){
fflush(Logi);
					if(r==1) c='=';
					if(r==2) c='<';
					if(r==4) c='>';
					fprintf(Logi,"%s %c %s<BR>",T[x_],c,T[i]);
fflush(Logi);
					}
				}
			fflush(Logi);
			fprintf(Logi,"<BR>");
			p_=(struct top *)NULL;
			return(p_);
			break;
		case 1: if(VK[I]==marker){
				set_show("the parsing is completed");
				return(p_);
				}
			Stack[J]=VK[I];
			x_=Stack[J];
			if((x_==m_k)||(x_==k_k)) I++;
fflush(Logi);
			puu[J]=newtop(x_,I,x_);
fflush(Logi);
			p_stack("\n",Stack,rela,0,J+1,I+1);                     
fflush(Logi);
			J++;
			I++;
			break;
		case 2: Stack[J]=VK[I];
			x_=Stack[J];
			if((x_==m_k)||(x_==k_k)) I++;
fflush(Logi);
			puu[J]=newtop(x_,I,x_);
fflush(Logi);
			p_stack("\n",Stack,rela,0,J+1,I+1);                     
fflush(Logi);
			J++;
			I++;
			break;
		case 4: 
fflush(Logi);
			sentential_form();
fflush(Logi);
			ps();
fflush(Logi);
			t_=ReadHash(n_,lausevorm);
fflush(Logi);
			if(t_==(struct h *)NULL){
fflush(Logi);
				p_=(struct top *)NULL;
				return(p_);
				}
fflush(Logi);
			N_=reduce(t_,Stack[s_-1],VK[I]);
			fflush(Logi);
			t_=hc;
			if(N_==0){
fflush(Logi);
				set_show("reduce error");
fflush(Logi);
				p_=(struct top *)NULL;
				fflush(Logi);
				return(p_);
				}
			rela[s_]=PM[adr(Stack[s_-1],N_)];
			Stack[s_]=N_;
			x_=N_;
fflush(Logi);
			p_=newtop(N_,I,t_->P+tnr);
fflush(Logi);
			red_tree(puu,p_,s_,n_);
fflush(Logi);
			puu[s_]=p_;
			p_stack("<BR>",Stack,rela,0,s_+1,I);                    
fflush(Logi);
			if(semantika[t_->P+tnr]>0){
				set_show("Parsing tree");                       
fflush(Logi);
				pp2html(p_);
fflush(Logi);
				}
			fprintf(Logi,"<BR>");
			J=s_+1;
			break;
		}
	}
	return(p_);
}
/* mï¿½lueraldus konstantide tabeli kirjele */
struct ctr *make_ctr(void){
	struct ctr *c;
	c=(struct ctr *)malloc(sizeof(struct ctr));
	if(c==(struct ctr *)NULL) ExiT();
	memset(c,'\0',sizeof(struct ctr));
	return(c);
}       
	
/* konstantide tabeli moodustamine */
int make_CT(void){
	int t;
	struct ctr *c;
	int i;
	if(ktl==0) return(0);
	fprintf(Logi,"<TABLE BORDER=1><TR>");
	for(i=0;i<ktl;i++){
		t=KT[i];
		c=make_ctr();
		c->nr=t;
		c->value=atoi(T[c->nr]);
		CT[i]=c;
		fprintf(Logi,"<TD>c%d=%d</TD>",i+1,c->value);
		}
	fprintf(Logi,"</TR></TABLE>");
	return(ktl);
}
/* mï¿½lueraldus identifikaatorite tabeli kirjele */
struct itr *make_itr(void){
	struct itr *c;
	c=(struct itr *)malloc(sizeof(struct itr));
	if(c==(struct itr *)NULL) ExiT();
	memset(c,'\0',sizeof(struct itr));
	c->t=(struct top *)NULL;
	return(c);
}       
/* identifikaatorite tabeli moodustamine */
int make_IDT(void){
	int t;
	struct itr *c;
	int i,k;
	if(itl==0) return(0);   
	k=(flag==0) ? itl-1 : itl;
	fprintf(Logi,"<TABLE BORDER=1><TR>");
	for(i=0;i<k;i++){
		t=IT[i];
		c=make_itr();
		c->nr=t;
		IDT[i]=c;
/*		print_Itr(c);	*/
		fprintf(Logi,"<TD>i%d=%s</TD>",i+1,T[c->nr]);
		}
	fprintf(Logi,"</TR></TABLE>");          
	return(itl);
}
/* identifikaatorite tabeli kirje trï¿½kk */
void p_itr(struct itr *id){
	fprintf(Logi,"ITR nr=%d V=%s loc=%d value=%d t=%p<BR>",
		id->nr,T[id->nr],id->loc,id->value,id->t);
}
/* muutujate vï¿½ljatrï¿½kk */
void print_variables(void){
	struct itr *id;
	int i,k;
	set_show("THE VARIABLES:");
	k=(flag==0) ? itl-1 : itl;
	for(i=0;i<k;i++){
		id=IDT[i];
		if(id->t==(struct top *)NULL)
		fprintf(Logi,"%s=%d<BR>",T[id->nr],id->value);
		}
}
/* konstantide tabeli kirje trï¿½kk */
void p_ctr(struct ctr *id){
	fprintf(Logi,"CTR nr=%d V=%s loc=%d value=%d<BR>",
		id->nr,T[id->nr],id->loc,id->value);
}               
/* tekstimassiivi sisestamine kettalt */
char *jarray(char *pealkiri){
	FILE *tekst=NULL;
	char *B;
	char c;
	int  i;
	int  k;
	struct stat *buf;
	buf=(struct stat *)malloc(sizeof(struct stat));
	if(buf==NULL){
		fprintf(Logi,"r_text: I haven't %d bytes of memory..<BR>",
			sizeof(struct stat));
		ExiT();
		}
	tekst = fopen(pealkiri, "r");
	if (tekst==NULL){
		fprintf(Logi,"cannot find the file  %s <BR>",pealkiri);
		ExiT();
		}
	if(stat(pealkiri,buf)==-1){
		fprintf(Logi,"r_text: stat failed<BR>");
		ExiT();
		}
	k=buf->st_size; 
	B = (char *)malloc(k+10);
	if(B == NULL){
		fprintf(Logi,"I don't have memory enaugh..");
		ExiT();
		}
	memset(B,'\0',k+10);
/* fill buffer */
	rewind(tekst);
	i=0;
	while (!feof(tekst)){
		c=fgetc(tekst);
		B[i]=c;
		i++;
		}
	P_length=i;
	for(i=P_length;i>0;i--){
		if(isgraph(B[i])){
			i++;
			B[i]='\n';
			i++;
			B[i]='\0';
			P_length=i;
			goto out;
			}
		}
out:    fclose(tekst);
	return(B);
}
int prog(void){
	PBuf=jarray(pr_name);
	/*ahti osa algus*/
	if ((strstr(PBuf,"atleast")==NULL)&&
		(strstr(PBuf,"atmost")==NULL)&&
		(strstr(PBuf,"level")==NULL)&&
		(strstr(PBuf,"\to")==NULL)&&
		(strstr(PBuf,"even")==NULL)&&
		(strstr(PBuf,"exactlyone")==NULL)&&
		(strstr(PBuf,"atmostone")==NULL)&&
		(strstr(PBuf,"atleastone")==NULL)&&
		(strstr(PBuf,"none")==NULL))
			pane=1;
				/*vastasel juhul on see 0*/

	/*ahti osa lï¿½pp*/
	if(PBuf!=NULL){
		Plen=P_length;
		return(1);
		}
	return(0);
}
int itr(void){
	int ret=0;
	time_t t0;
	GBuf=NULL;
	Logi=fopen(L_name,"w");
	if(Logi==NULL){
		printf("Cannot open log-book\n");
		return(0);
		}
	logi=1;
	time(&t0);
	fprintf(Logi,"<HTML><HEAD><TITLE>Parser</TITLE></HEAD><BODY><B>");
	fprintf(Logi,
	"<FONT COLOR=\"#0000FF\"><H3>Start of FORM8 Interpreter for a Formula ");
	fprintf(Logi,"</FONT>%s ",pr_name);
	fprintf(Logi,"<FONT COLOR=\"#0000FF\"> at </FONT>");
	fprintf(Logi,"%s</H3><BR>",asctime(localtime(&t0)));
	if(prog()==0){
		fflush(Logi);
		fclose(Logi);   
		return(ret);
		}
	print_all_cxtp();
	/*Siit toimub analï¿½saatori vï¿½ljakutse!*/
	if(analyzer()!=NULL) fprintf(Logi,"Parser ended<BR>"); /* trigol_Int(p_); */
	time(&t0);
	fprintf(Logi,"<FONT COLOR=\"#0000FF\"><H4>Interpreter ended at </FONT> ");
	fprintf(Logi,"%s</H4>",asctime(localtime(&t0)));        
	fprintf(Logi,"</BODY></HTML>");
	return(ret);
}
/*====================== AINI osa lï¿½pp ===========================*/
int main(int argc,char **argv){
	
	
	char *p,*satfail;
	int k=1;
	
	argumendid=argv;
	arg_arv=argc;
		/*et kui kunagi vaja graafide faile lugeda*/
	pr_name=(char *)malloc(256);    
	L_name=(char *)malloc(256);
	servade_pea=(struct paarid *)malloc(sizeof(struct paarid));
	memset(servade_pea,'\0',sizeof(struct paarid));
	servade_pea->jarg=NULL;	
	GRAAFIDE_MASSIIV=(struct graafpea *)malloc(sizeof(struct graafpea));
	memset(GRAAFIDE_MASSIIV,'\0',sizeof(struct graafpea));
	GRAAFIDE_MASSIIV->jarg=NULL;
	GRAAFIDE_MASSIIV->graph=NULL;
	Nimi=(char *)malloc(256);       
	fail=(char *)malloc(256);
	fail1=(char *)malloc(256);
	memset(pr_name,'\0',256);       
	memset(L_name,'\0',256);        
	memset(Nimi,'\0',256);          
	memset(fail,'\0',256);
	memset(fail1,'\0',256);
	strcpy(Nimi,"form8");        /* "form" asemele pane 6ige */
	sprintf(pr_name,"%s",argv[1]);  
	strcpy(L_name,pr_name);
	p=strrchr(L_name,'.');
	strcat(L_name,".htm");  
	strcat(pr_name,".frm");         
	printf("formula=%s \ndeduction tree=%s\n",pr_name,L_name);
	itr();/*siin tehakse valmis analï¿½ï¿½sipuu*/
	fflush(Logi);   
			/*vï¿½tame tegelike indeksite jaoks mï¿½lu*/
	ind=(struct indeeks *)malloc(sizeof(struct indeeks));
      memset(ind,'\0',sizeof(struct indeeks));
	ind->jarg=NULL;
	/*Vï¿½tame parameetrite jaoks mï¿½lu*/
	Muut=(struct hetkevaartus *)malloc(sizeof(struct hetkevaartus));
	    memset(Muut,'\0',(sizeof(struct hetkevaartus)));
	    Muut->jarg=(struct hetkevaartus *)NULL;



		if (pane){
			satfail=(char *)malloc(256);
			memset(satfail,'\0',256);
			strcat(satfail,argv[1]);
			strcat(satfail,".sat");
			FF2=fopen(satfail,"w");
			fprintf(FF2,"Family %s\n",pr_name);
		}

	strcat(fail,argv[1]);
	strcat(fail,".tav");
	printf("formats: %s",fail);
	if (pane) printf(", %s",satfail);
	printf("\n");
	FF1=fopen(fail,"w");
	
	if (p_!=(struct top *)NULL) {
		
		
		LABI_VALEM(p_->down);
		fprintf(FF1,";");
		fflush(FF);
		}/*hakkame puud lï¿½bima*/

		/*(p_->down)->sem==2 siin!!!*/
	else {
		printf("syntax error!\n");
		return(0);
	}
	

	/*trï¿½kkida abifaili leitud indeksid ja nende leidmise jrk.-numbrid*/
	strcat(fail1,argv[1]);
	strcat(fail1,".aux");
  FF=fopen(fail1,"w");
	printf("table of correspondence: %s\n",fail1);
  
  fprintf(FF,"Table of correspondence\nFamily %s",pr_name);
fflush(FF);
while (Muut!=NULL){
	Muut=Muut->jarg;
	if (Muut!=NULL){
		fprintf(FF,",  %c=%d",Muut->mark,Muut->vaartus);
	}

}
fprintf(FF,".\n");
printf("OK\n");
 ind=ind->jarg; 
  while (ind!=NULL){
      fprintf(FF,"%s     =    x(%d)\n",ind->alus,k);
	fflush(FF);
      k++;
      ind=ind->jarg;
      }
}

