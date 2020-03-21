#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <malloc.h>

#define dim 2
#define MM 80
#define NN MM
#define clear    0
#define pass     1
#define false    2
#define reper    4
#define boundary 8
#define red      16
#define closed   0
#define open1    1
#define open2    2

void CopyDesk(short c1[MM][NN], short c2[MM][NN]);
void PrintDesk(void);
int Connect(int v, int i, int j);
int OneOfAround(int v, int i, int j);
int Colorize(int col, int i, int j);
int path(int trace, int i0, int j0, int i1, int j1);
int spread(int, ...);
void SaveLab(void);
void DrawHTML(void);
void DrawPath(void);
void DrawWalls(void);

int m,n,M,N;
int pr1=0;
int pr2=0;
#define points 2
int pnt[points][2];
short ce[MM][NN], cc[MM][NN], door[MM][NN], dr[MM][NN];
int way[MM*NN][2];
int http, init, ist,jst, ifin,jfin;
char *env;

FILE *fil;

 
void main(int argc, char *argv[]) 
{
    int i,j, ii,jj, t,tt, l,ll, p,p1,p2, b;
    int i0,j0,i1,j1;
    long ltime, ltime0;
    char param[1024];
    char *par;

    env=getenv("REQUEST_METHOD");
    http=(env!=NULL);

    if (http) {
        env=getenv("QUERY_STRING");
//        env=getenv("AAA");

        if (env==NULL) return;
        par=param;
        for (i=0; i<1023; i++) {*par=*env;par++;env++;if ((*env==0)||(*env=='?')) break;}
        *par=0;
        m=atoi(param); if ((m<6)||(m>(MM-2))) return;
        if (*env==0) return;
        par=param; env++;
        for (i=0; i<1023; i++) {*par=*env;par++;env++;if ((*env==0)||(*env=='?')) break;}
        *par=0;
        n=atoi(param); 
        if ((n<6)||(n>(NN-2))) return;
        if (*env!=0) {
            par=param; env++;
            for (i=0; i<1023; i++) {*par=*env;par++;env++;if ((*env==0)||(*env=='?')) break;}
            *par=0;
            init=atoi(param); if (init<=0) return;
        }
        else init=(int)(time(&ltime)%30000);
    }    
    else {
        if (argc<3) {
            printf("Use parameters:  <horizontal> <vertical> (less or equal to %i)\n",MM-2); return;
        };
        m=atoi(argv[1]); n=atoi(argv[2]);
        if (m<0) {m=-m; pr1=1;}
        if (n<0) {n=-n; pr2=1;}
        
        if (argc>3) init=atoi(argv[3]); else init=(int)(time(&ltime)%30000);
    }
    if (!http) printf("init: %8i\n",init);
    srand(init);
    M=m+2; N=n+2;
    ist=1; jst=1; ifin=m; jfin=n;
//    t=0;
    for (j=0; j<points; j++) {
        pnt[j][0]=rand()*(m-6)/RAND_MAX+4;
        pnt[j][1]=rand()*(n-6)/RAND_MAX+4;
//        printf("%10i%10i\n",pnt[j][0],pnt[j][1]);
    }
    start:;
    p1=ist; p2=jst;
    time(&ltime0);
    
/* Initiation of the array c (ces) */
    for (j=0; j<N; j++) for (i=0; i<M; i++) {
        ce[i][j]=clear; door[i][j]=closed;
    };
    for (i=0; i<M; i++) ce[i][0]=ce[i][N-1]=boundary;
    for (j=0; j<N; j++) ce[0][j]=ce[M-1][j]=boundary;
    for (i=0; i<M; i++) door[i][0]=door[i][N-1]|=open1;
    for (j=0; j<N; j++) door[0][j]=door[M-1][j]|=open2;
    door[0][p2]|=open1;door[m][jfin]|=open1;

/* Random move */                    


    for (p=0; p<=points; p++) {
        i0=p1; j0=p2;
        if (p==points) {p1=ifin; p2=jfin;}
        else {p1=pnt[p][0];p2=pnt[p][1];}


        i1=p1; j1=p2;

//        printf("%10i%10i%10i%10i\n", i0, j0, i1, j1);
//        getchar();


        tt=path(pass,i0,j0,i1,j1);
        if (!tt) goto start;
//        t+=tt;

//        DrawPath();
//        getchar();

    }
//    printf("\nTrials: %i\n\n",t);

//    DrawPath();
//    printf("\n");
    if (pr2) DrawWalls(); 
    do {
        CopyDesk(ce,cc);
        b=0;
        for (i=1; (i<=m)&&!b; i++) for (j=1; (j<=n)&&!b; j++) b|=Colorize(red,i,j);
        if (b) {
            l=0;

//            PrintDesk();
            

            for (i=1;i<=m;i++) for (j=1;j<=n;j++)
                if ((ce[i][j]==red)&& OneOfAround(pass|false,i,j))
                    for (ii=i;ii<=m;ii++) for(jj=1;jj<=n;jj++)
                        if ((ce[ii][jj]==red)&& OneOfAround(pass|false,ii,jj)) {
                            ll=ii-i+abs(jj-j);
                            if (ll>=l) {l=ll; i0=i; j0=j; i1=ii; j1=jj;}
                        }

            CopyDesk(cc,ce);

//            DrawPath();
//            printf("A:   %6i%6i%6i%6i\n", i0,j0,i1,j1);

            if (spread(2,100,100)&& Connect(pass,i1,j1));
            else Connect(pass|false,i0,j0);
            ce[i0][j0]=false;

//            DrawPath();
//            printf("B:   %6i%6i%6i%6i\n", i0,j0,i1,j1);
            
            if (l!=0) while (!path(false,i0,j0,i1,j1));

            if (pr1) {PrintDesk(); DrawPath(); pr1=(getchar()!='q');}
        }
    }
    while (b);
    SaveLab();
    if (http) DrawHTML(); else DrawWalls();

//    printf("\nDiameter: %i\n\n",l);
//    printf("%5i%5i\n%5i%5i\n",i0,j0,i1,j1);

}



void CopyDesk(short c1[MM][NN], short c2[MM][NN]) 
{
    int i,j;
    for (i=0;i<MM;i++) for (j=0;j<NN;j++) c2[i][j]=c1[i][j];
}



void PrintDesk() 
{
    int i,j;
    printf("\n");
    for (j=0; j<N; j++) {
        for (i=0; i<M; i++) printf(" %1x",ce[i][j]);
        printf(" ");
        for (i=0; i<M; i++) printf("%1x",door[i][j]);
        printf("\n");
    }
    printf("\n");
}







int Connect(int v, int i, int j)
{
    if      (ce[i-1][j]  & v) door[i-1][j]  |=open1; 
    else if (ce[i+1][j]  & v) door[i]  [j]  |=open1;
    else if (ce[i]  [j-1]& v) door[i]  [j-1]|=open2;
    else if (ce[i]  [j+1]& v) door[i]  [j]  |=open2;
    else return 0;
    return 1;
}  



int OneOfAround(int v, int i, int j)
{
    return (ce[i-1][j]&v)||(ce[i+1][j]&v)||(ce[i][j-1]&v)||(ce[i][j+1]&v);
}



int Colorize(int col, int i, int j)
{
    if (ce[i][j]==clear) {
        ce[i][j]=col;
        Colorize(col,i-1,j);
        Colorize(col,i+1,j);
        Colorize(col,i,j-1);
        Colorize(col,i,j+1);
        return 1;
    }
    else return 0;
}





int path(int trace, int i0, int j0, int i1, int j1)
{
    const int attr=300;
    const int norm=100;

    int i,j,ii,jj, di,dj, sd,st, z,t;
    int ws[2][dim];
    t=0;
    ce[i0][j0]=trace;
        do {
            i=ii=i0;
            j=jj=j0;
//            ce[i][j]=reper;
            CopyDesk(ce, cc);
            CopyDesk(door, dr);
            while ((i!=i1)||(j!=j1)) {
                if (cc[i-1][j]*cc[i][j+1]*cc[i+1][j]*cc[i][j-1]) return 0;
                
    /* Attraction */            
                di=i1-i; if (di<0) di=-di;
                dj=j1-j; if (dj<0) dj=-dj;
  
//                printf("%10i%10i%10i%10i\n",i,j,i1,j1);
  
                z=attr/(di+dj);
                if (i1>=i) {ws[0][0]=di*z+norm; ws[1][0]=norm;}
                else       {ws[0][0]=norm; ws[1][0]=di*z+norm;};
                if (j1>=j) {ws[0][1]=dj*z+norm; ws[1][1]=norm;}
                else       {ws[0][1]=norm; ws[1][1]=dj*z+norm;};

//                printf("%10i%10i%10i%10i\n",ws[0][0],ws[1][0],ws[0][1],ws[1][1]);
//                getchar();
              
                do {
                    ii=i;jj=j;
                    switch (st=spread(4,ws[0][0],ws[1][0],ws[0][1],ws[1][1])) {
                        case 0: ii++; sd=0; break;
                        case 1: ii--; sd=1; break;
                        case 2: jj++; sd=0; break;
                        case 3: jj--; sd=1; break;
                    }
                }
                while (cc[ii][jj]!=clear);
  
//                printf("%5i%5i", ii,jj); printf("\n");
  
                switch (st/2) {
                    case 0: dr[i-sd][j]|=open1; break; 
                    case 1: dr[i][j-sd]|=open2; break;
                }
                i=ii; j=jj;
                cc[i][j]=trace;
                
            };
            t++;
        }
        while ((i!=i1)||(j!=j1));
//        cc[i][j]=reper;
        CopyDesk(cc,ce);
        CopyDesk(dr,door);
        return t;
}

void SaveLab() {
    int i,k;
    fil=fopen("lab.dat","w");
    fprintf(fil,"Labirint AK 100 d2              %4i%4i%4i%4i                %8i        ",m,n,ifin,jfin,init);
    for (i=0;i<M;i++) k=fwrite(ce[i],N*sizeof(ce[0][0]),1,fil);
    for (i=0;i<M;i++) k=fwrite(door[i],N*sizeof(door[0][0]),1,fil);
    fclose(fil);
}



/*
void DrawHTML() {
    int i,j, l,u,r,d;
    char s[100];
    if (http) {
        fil=stdout; 
        printf("Content-Type: text/html\n\n");
    }
    else fil=fopen("mino.html","w");
    fputs("<html>\n<head>\n<title>mino ",fil);
    itoa(m,s,10);
    fputs(s,fil);
    fputc(' ',fil);
    itoa(n,s,10);
    fputs(s,fil);
    fputs("   init: ",fil);
    itoa(init,s,10);
    fputs(s,fil);
    fputs("</title>\n</head>\n<body>\n",fil);
    fputs("<table cellpadding=0 cellspacing=0>\n",fil);

    for (i=1; i<=m; i++) {
        fputs("<td>\n",fil);
        fputc(' ',fil);
        for (j=1; j<=n; j++) {
            fputs("<img src=",fil); fputc('"',fil); fputs("pix/mino",fil);
            if (door[i][j] & open1)   fputc('1',fil); else fputc('0',fil);
            if (door[i][j] & open2)   fputc('1',fil); else fputc('0',fil);
            fputs(".bmp",fil); fputc('"',fil); fputs(" border=0></a><br />",fil);
        }
        fputs("\n</td>\n",fil);
    }
    fputs("</table>\n</body>\n</html>\n",fil);
    fclose(fil);
}
*/


void DrawHTML() {
    int i,j, l,u,r,d;
    char s[100];
    if (http) {
        fil=stdout;
        printf("Content-Type: text/html\n\n");
    }
    else fil=fopen("mino.html","w");
    fputs("<html>\n<head>\n<title>mino ",fil);
    itoa(m,s,10);
    fputs(s,fil);
    fputc(' ',fil);
    itoa(n,s,10);
    fputs(s,fil);
    fputs("   init: ",fil);
    itoa(init,s,10);
    fputs(s,fil);
    fputs("</title>\n</head>\n<body>\n",fil);
    fputs("<table cellpadding=0 cellspacing=0>\n",fil);

    for (i=1; i<=m; i++) {
        fputs("<td>\n",fil);
        fputc(' ',fil);
        for (j=1; j<=n; j++) {
            fputs("<img src=",fil); fputc('"',fil); fputs("pix/mino",fil);
            if (door[i-1][j] & open1) fputc('1',fil); else fputc('0',fil);
            if (door[i][j] & open1)   fputc('1',fil); else fputc('0',fil);
            if (door[i][j-1] & open2) fputc('1',fil); else fputc('0',fil);
            if (door[i][j] & open2)   fputc('1',fil); else fputc('0',fil);
            fputs(".bmp",fil); fputc('"',fil); fputs(" border=0></a><br />",fil);
        }
        fputs("\n</td>\n",fil);
    }
    fputs("</table>\n</body>\n</html>\n",fil);
    fclose(fil);
}


void DrawPath() {
    int i,j, l,u,r,d;
    char ch;
    for (j=1; j<=n; j++) {
        printf(" ");
        for (i=1; i<=m; i++) {
            ch=0xf9;
            if (ce[i][j]) {
                l=door[i-1][j] & open1;r=door[i][j] & open1;
                u=door[i][j-1] & open2;d=door[i][j] & open2;
                if (l && u && r && d) ch=0xc5;
                if (!l && u && r && d) ch=0xc3;
                if (l && !u && r && d) ch=0xc2;
                if (l && u && !r && d) ch=0xb4;
                if (l && u && r && !d) ch=0xc1;
                if (!l && !u && r && d) ch=0xda;
                if (l && !u && !r && d) ch=0xbf;
                if (l && u && !r && !d) ch=0xd9;
                if (!l && u && !r && d) ch=0xb3;
                if (l && !u && r && !d) ch=0xc4;
                if (!l && u && r && !d) ch=0xc0;
            };
            if (ce[i][j]==reper) ch='*';
//            if (ce[i][j]==pass) ch='=';
            if (ce[i][j]>10) ch='0'+ce[i][j]-10;
            printf("%c",ch);
        };
        printf("\n");       
    }
}


void DrawWalls() {
    int i,j, l,u,r,d;
    char ch;
    for (j=0; j<=n; j++) {
        if (j==(jst-1)) printf("s"); else printf(" ");
        for (i=0; i<=m; i++) {
            l=door[i][j] & open2;r=door[i+1][j] & open2;
            u=door[i][j] & open1;d=door[i][j+1] & open1;
            if (l && u && r && d) ch=' ';
            if (!l && u && r && d) ch='ù';
            if (l && !u && r && d) ch='ù';
            if (l && u && !r && d) ch='ù';
            if (l && u && r && !d) ch='ù';
            if (!l && !u && r && d) ch='Ù';
            if (l && !u && !r && d) ch='À';
            if (l && u && !r && !d) ch='Ú';
            if (!l && u && !r && d) ch='Ä';
            if (l && !u && r && !d) ch='³';
            if (!l && u && r && !d) ch='¿';
            if (!l && !u && !r && d) ch='Á';
            if (l && !u && !r && !d) ch='Ã';
            if (!l && u && !r && !d) ch='Â';
            if (!l && !u && r && !d) ch='´';
            if (!l && !u && !r && !d) ch='Å';
            printf("%c",ch);
        };
        if (j==jfin) printf("f");
        printf("\n");       
    }
}


/*
  spread(int n, ...)
  ------------------
  Produces a random spreading over n values (0 to n-1) with weights
  n - number of vulues
  n must be at least 2
  weights follow next, n integer numbers, of range from 1 upto 1000,
  normalized to the total, e.g.:
  spread(3,3,7,10) - outputs numbers
  0 - at 15%
  1 - at 35%
  2 - at 50% probability.
*/

int spread(int n,...)
#define p 10000
{
    int i,r,s,t;
    int *w;
    va_list NextArg;
    w=(int*)malloc(n*sizeof(int));
    va_start(NextArg, n);
    s=t=0;
    for (i=0; i<n; i++) s+=w[i]=va_arg(NextArg,int);
    va_end(NextArg);
    s=RAND_MAX*p/s;
    n--;
    r=rand();
    for (i=0; i<n; i++) {
        t+=w[i]*s/p;
        if (r<=t) return i;
    }
    return n;
}


