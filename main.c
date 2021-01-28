#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
    int i, j;
    int culoare;
    float scor;
} Detectie;

void citire(char **numeImagine)
{
    *numeImagine = malloc(30);
    scanf("%s", *numeImagine);
}

int cond(const void *a, const void *b)
{
    Detectie x = *(Detectie *)a;
    Detectie y = *(Detectie *)b;

    if(x.scor > y.scor)
        return -1;
    if(x.scor < y.scor)
        return 1;
    return 0;
}

void scriere1(unsigned int **p, int w, int h, char *numeImagineNoua, unsigned char *s)
{
    FILE *fout = fopen(numeImagineNoua, "wb");
    fwrite(s, 54, 1, fout);
    fseek(fout, 54, SEEK_SET);
    int i, j, n=0;
    int padding;
    if(w % 4 != 0)
        padding = 4 - (3 * w) % 4;
    else
        padding = 0;
    for(i=0; i<h; i++)
    {
        for(j=0; j<w; j++)
            fwrite(&p[i][j], 3, 1, fout);
        int k;
        for(k=0; k<padding; k++)
            fwrite(&n, 1, 1, fout);
    }
    fclose(fout);
}

void grayScale(char* numeImagineColor, char* numeImagineGrayscale)
{
    FILE *fin;

    fin = fopen(numeImagineColor, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned int dim, w, h;
    unsigned char *pRGB = malloc(3), aux;

    FILE *fout = fopen(numeImagineGrayscale, "wb+");

    fseek(fin, 2, SEEK_SET);
    fread(&dim, sizeof(unsigned int), 1, fin);

    fseek(fin, 18, SEEK_SET);
    fread(&w, sizeof(unsigned int), 1, fin);
    fread(&h, sizeof(unsigned int), 1, fin);

    fseek(fin,0,SEEK_SET);
    unsigned char c;
    while(fread(&c,1,1,fin)==1)
    {
        fwrite(&c,1,1,fout);
        fflush(fout);
    }
    fclose(fin);

    int padding;
    if(w%4 != 0)
        padding = 4 - (3 * w) % 4;
    else
        padding = 0;

    fseek(fout, 54, SEEK_SET);
    int i,j;
    for(i=0; i<h; i++)
    {
        for(j = 0; j<w; j++)
        {
            fread(pRGB, 3, 1, fout);
            aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
            pRGB[0] = pRGB[1] = pRGB[2] = aux;
            fseek(fout, -3, SEEK_CUR);
            fwrite(pRGB, 3, 1, fout);
            fflush(fout);
        }
        fseek(fout,padding,SEEK_CUR);
    }
    fclose(fout);
}

void formaLiniarizata(unsigned int ***p, int *w, int *h, char *numeImagine)
{
    FILE *fin = fopen(numeImagine, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    fseek(fin, 18, SEEK_SET);
    fread(w, 4, 1, fin);
    fread(h, 4, 1, fin);

    fseek(fin, 54, SEEK_SET);

    *p = malloc((*h)*sizeof(unsigned int));

    int i, j;
    for(i=0; i<(*h); i++)
        (*p)[i] = malloc((*w)*sizeof(unsigned int));

    unsigned int padding = 0;
    if((*w) % 4 != 0)
        padding = 4 - (3 * (*w)) % 4;
    else
        padding = 0;

    for(i=0; i<(*h); i++)
    {
        for(j=0; j<(*w); j++)
        {
            unsigned int q=0;
            fread(&q, 3, 1, fin);
            (*p)[i][j] = q;
        }
        fseek(fin, padding, SEEK_CUR);
    }

    fclose(fin);
}

double intersectie(int i, int j, int k, int l, int w, int h)
{
    int m, n;
    double nr = 0;

    for(m=i; m<i+h; m++)
        for(n=j; n<j+w; n++)
            if(m >= k && m <= k+h && n >= l && n <= l+w)
                nr++;

    return nr;
}

double suprapunere(Detectie a, Detectie b, int w, int h)
{
    return intersectie(a.i, a.j, b.i, b.j, w, h) / (w*h + w*h - intersectie(a.i, a.j, b.i, b.j, w, h));
}

void eliminare(Detectie **a, unsigned int *nr, int k)
{
    int i;
    for(i=k; i<(*nr)-1; i++)
        (*a)[i] = (*a)[i+1];
    (*nr)--;
}

unsigned int color(unsigned int r, unsigned int g, unsigned int b)
{
    unsigned int culoare = 0;
    culoare = r<<16;
    culoare = culoare + (g<<8);
    culoare = culoare + b;
    return culoare;
}

void patrat(unsigned int ***p, int i, int j, int w, int h, unsigned int culoare)
{
    int k, l;
    for(k=0; k<h; k++)
    {
        (*p)[k+i][j] = culoare;

        (*p)[k+i][j+w] = culoare;
    }

    for(l=0; l<w; l++)
    {
        (*p)[i][l+j] = culoare;

        (*p)[i+h][l+j] = culoare;
    }
}

unsigned int alegereCuloare(char *numeSablon)
{
    if(!strcmp(numeSablon, "cifra0.bmp"))
        return color(255, 0, 0);
    if(!strcmp(numeSablon, "cifra1.bmp"))
        return color(255, 255, 0);
    if(!strcmp(numeSablon, "cifra2.bmp"))
        return color(0, 255, 0);
    if(!strcmp(numeSablon, "cifra3.bmp"))
        return color(0, 255, 255);
    if(!strcmp(numeSablon, "cifra4.bmp"))
        return color(255, 0, 255);
    if(!strcmp(numeSablon, "cifra5.bmp"))
        return color(0, 0, 255);
    if(!strcmp(numeSablon, "cifra6.bmp"))
        return color(192, 192, 192);
    if(!strcmp(numeSablon, "cifra7.bmp"))
        return color(255, 140, 0);
    if(!strcmp(numeSablon, "cifra8.bmp"))
        return color(128, 0, 128);
    if(!strcmp(numeSablon, "cifra9.bmp"))
        return color(128, 0, 0);
    return -1;
}

float deviatiaStandardFereastra(unsigned int **p, int w, int h, int i, int j)
{
    float fmed=0, suma=0;
    int k, l;
    for(k=i; k<i+h; k++)
        for(l=j; l<j+w; l++)
            fmed += (p[k][l]>>16);

    fmed = fmed / (w*h);

    for(k=i; k<i+h; k++)
        for(l=j; l<j+w; l++)
            suma += ((p[k][l]>>16) - fmed) * ((p[k][l]>>16) - fmed);

    suma = (1.0/(w*h - 1)) * suma;

    suma = sqrt(suma);

    return suma;
}

float deviatiaStandardSablon(unsigned int **p, int w, int h)
{
    float smed=0, suma=0;
    int k, l;
    for(k=0; k<h; k++)
        for(l=0; l<w; l++)
            smed += (p[k][l]>>16);

    smed = smed / (w*h);

    for(k=0; k<h; k++)
        for(l=0; l<w; l++)
            suma += ((p[k][l]>>16) - smed) * ((p[k][l]>>16) - smed);

    suma = (1.0/(w*h - 1)) * suma;

    suma = sqrt(suma);

    return suma;
}

float corr(unsigned int **pimg, unsigned int **psbl, int i, int j, int w, int h)
{
    float valoareFinala = 0;
    float suma = 0;

    float dsfer = deviatiaStandardFereastra(pimg, w, h, i, j), dssbl = deviatiaStandardSablon(psbl, w, h);

    float fmed = 0, smed = 0;

    int k, l;
    for(k=i; k<i+h; k++)
        for(l=j; l<j+w; l++)
        {
            fmed += (pimg[k][l]>>16);
        }

    for(k=0; k<h; k++)
        for(l=0; l<w; l++)
        {
            smed += (psbl[k][l]>>16);
        }

    smed = smed / (w*h);
    fmed = fmed / (w*h);

    for(k=0; k<h; k++)
        for(l=0; l<w; l++)
            suma += (1/(dsfer*dssbl))*((pimg[i+k][j+l]>>16) - fmed)*((psbl[k][l]>>16) - smed);

    valoareFinala = (1.0/(w*h))*suma;

    return valoareFinala;
}

void eliminareaNonMaximelor(Detectie **D, unsigned int **pimg, int wimg, int himg, int wsbl, int hsbl, float prag, unsigned int *nr)
{
    int k, i,j, l;
    unsigned int **psbl;
    char *numeSablon;
    numeSablon = malloc(11);
    *D = malloc(sizeof(Detectie)*(*nr));
    (*nr) = 0;
    for(k=0; k<10; k++)
    {
        strcpy(numeSablon, "cifrax.bmp");
        numeSablon[5] = k + '0';
        numeSablon[11] = '\0';
        formaLiniarizata(&psbl, &wsbl, &hsbl, numeSablon);

        for(i=0; i<himg-hsbl; i++)
            for(j=0; j<wimg-wsbl; j++)
            {
                if(corr(pimg, psbl, i, j, wsbl, hsbl) >= prag)
                {
                    (*D)[(*nr)].culoare = alegereCuloare(numeSablon);
                    (*D)[(*nr)].i = i;
                    (*D)[(*nr)].j = j;
                    (*D)[(*nr)++].scor = corr(pimg, psbl, i, j, wsbl, hsbl);
                }
            }
    }

    qsort(*D, *nr, sizeof(Detectie), cond);

    for(k=0; k<(*nr)-1; k++)
        for(l=k+1; l<(*nr); l++)
        {
            if(suprapunere((*D)[k], (*D)[l], wsbl, hsbl) >= 0.2)
            {
                eliminare(D, nr, l);
                l--;
            }
        }
}

void templateMatch(char *numeImagineGrayscale, char *numeImagine, float prag, char *numeImagineRezultat)
{
    unsigned char *simg;

    FILE *fimg = fopen(numeImagineGrayscale, "rb");
    if(fimg == NULL)
    {
        printf("Eroare la deschidere\n");
        return;
    }

    simg = malloc(54);
    fread(simg, 54, 1, fimg);
    fclose(fimg);

    unsigned int **pimg, **psbl, **poriginal;
    int wimg=0, himg=0, wsbl=0, hsbl=0;
    formaLiniarizata(&pimg, &wimg, &himg, numeImagineGrayscale);
    formaLiniarizata(&poriginal, &wimg, &himg, numeImagine);

    Detectie *D;
    unsigned int nr = 0;
    char *numeSablon;
    numeSablon = malloc(11);
    int i, j, k;
    for(k=0; k<10; k++)
    {
        strcpy(numeSablon, "cifrax.bmp");
        numeSablon[5] = k + '0';
        numeSablon[11] = '\0';
        formaLiniarizata(&psbl, &wsbl, &hsbl, numeSablon);

        for(i=0; i<himg-hsbl; i++)
            for(j=0; j<wimg-wsbl; j++)
            {
                if(corr(pimg, psbl, i, j, wsbl, hsbl) >= prag)
                    nr++;
            }
    }

    eliminareaNonMaximelor(&D, pimg, wimg, himg, wsbl, hsbl, prag, &nr);

    for(i=0; i<nr; i++)
        patrat(&poriginal, D[i].i, D[i].j, wsbl, hsbl, D[i].culoare);

    scriere1(poriginal, wimg, himg, numeImagineRezultat, simg);

    free(D);
    free(simg);
    free(numeSablon);
    free(poriginal);
    free(pimg);
    free(psbl);
}

void scriere(unsigned int *p, int w, int h, char *numeImagineNoua, unsigned char *s)
{
    FILE *fout = fopen(numeImagineNoua, "wb");
    fwrite(s, 54, 1, fout);
    fseek(fout, 54, SEEK_SET);
    int i, j=0, padding, n=0;
    if(w % 4 != 0)
        padding = 4 - (3 * w) % 4;
    else
        padding = 0;
    for(i=0; i<w*h; i++)
    {
        fwrite(&p[i], 3, 1, fout);
        if(j == w)
        {
            j = 0;
            int k;
            for(k=0; k<padding; k++)
                fwrite(&n, 1, 1, fout);
        }
        j++;
    }
    fclose(fout);
}

void formaLiniarizata1(unsigned int **p, int *w, int *h, char *numeImagine)
{
    FILE *fin = fopen(numeImagine, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    fseek(fin, 18, SEEK_SET);
    fread(w, 4, 1, fin);
    fread(h, 4, 1, fin);

    fseek(fin, 54, SEEK_SET);

    *p = malloc((*w)*(*h)*sizeof(int));

    unsigned int padding = 0;
    if((*w) % 4 != 0)
        padding = 4 - (3 * (*w)) % 4;
    else
        padding = 0;

    int i, j, x=0;
    for(i=0; i<(*h); i++)
    {
        for(j=0; j<(*w); j++)
        {
            int q=0;
            fread(&q, 3, 1, fin);
            (*p)[x++] = q;
        }
        fseek(fin, padding, SEEK_CUR);
    }
    fclose(fin);
}

void XORSHIFT32(unsigned int **p, int n, int seed)
{
    int i, nr=seed;
    *p = malloc(n*sizeof(int));
    for(i=1; i<=n; i++)
    {
        nr = nr ^ nr << 13;
        nr = nr ^ nr >> 17;
        nr = nr ^ nr << 5;
        (*p)[i] = nr;
    }
}

void permutare(unsigned int **p, int n, unsigned int *r)
{
    int i, j;
    *p = malloc(n*sizeof(int) + 1);
    for(i=0; i<n; i++)
        (*p)[i] = i;
    int x=1;
    for(i=n-1; i>=0; i--)
    {
        j = (r[x++] % (i+1));
        unsigned int aux;
        aux = (*p)[i];
        (*p)[i] = (*p)[j];
        (*p)[j] = aux;
    }
}

void permutareInversa(unsigned int *p, unsigned int **q, int n)
{
    int i;
    *q = malloc(n*sizeof(int));
    for(i=0; i<n; i++)
        (*q)[p[i]] = i;
}

void seed(char *numeFisierCheie, int *r0, int *sv)
{
    FILE *fin = fopen(numeFisierCheie, "r");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }
    fscanf(fin, "%d %d", r0, sv);
    fclose(fin);
}

void criptare1(char *numeImagine, char *numeImagineSemiCriptata, char *numeFisierCheie)
{
    FILE *fin = fopen(numeImagine, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned char *s;
    s = malloc(54);
    fread(s, 54, 1, fin);

    unsigned int *p, *q;
    int w, h;
    formaLiniarizata1(&p, &w, &h, numeImagine);

    int r0, sv;
    unsigned int *r;
    seed(numeFisierCheie, &r0, &sv);
    XORSHIFT32(&r, 2*w*h-1, r0);

    unsigned int *perm;
    permutare(&perm, w*h, r);

    q = malloc(w*h*sizeof(int));

    int i;
    for(i=0; i<w*h; i++)
        q[perm[i]] = p[i];

    scriere(q, w, h, numeImagineSemiCriptata, s);
    free(s);
    free(p);
    free(q);

    fclose(fin);
}

void criptare2(char *numeImagineSemiCriptata, char *numeImagineCriptata, char *numeFisierCheie)
{
    FILE *fin = fopen(numeImagineSemiCriptata, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned char *s;
    s = malloc(54);
    fread(s, 54, 1, fin);

    unsigned int *p, *q;
    int w, h;
    formaLiniarizata1(&p, &w, &h, numeImagineSemiCriptata);

    int r0, sv;
    unsigned int *r;
    seed(numeFisierCheie, &r0, &sv);
    XORSHIFT32(&r, 2*w*h-1, r0);

    q = malloc(w*h*sizeof(int));
    q[0] = sv ^ p[0] ^ r[w*h];
    int i;
    for(i=1; i<w*h; i++)
        q[i] = q[i-1] ^ p[i] ^ r[w*h + i];
    scriere(q, w, h, numeImagineCriptata, s);
    free(s);
    free(p);
    free(q);
    fclose(fin);
}

void decriptare1(char *numeImagineCriptata, char *numeImagineSemiDecr, char *numeFisierCheie)
{
    FILE *fin = fopen(numeImagineCriptata, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned char *s;
    s = malloc(54);
    fread(s, 54, 1, fin);

    unsigned int *p, *q;
    int w, h;
    formaLiniarizata1(&p, &w, &h, numeImagineCriptata);

    int r0, sv;
    unsigned int *r;
    seed(numeFisierCheie, &r0, &sv);
    XORSHIFT32(&r, 2*w*h-1, r0);

    int i;
    q = malloc(w*h*sizeof(int));
    q[0] = sv ^ p[0] ^ r[w*h];
    for(i=1; i<w*h; i++)
        q[i] = p[i-1] ^ p[i] ^ r[w*h+i];

    scriere(q, w, h, numeImagineSemiDecr, s);
    free(s);
    free(p);
    free(q);
    fclose(fin);
}

void decriptare2(char *numeImagineSemiDecr, char *numeImagineDecr, char *numeFisierCheie)
{
    FILE *fin = fopen(numeImagineSemiDecr, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned char *s;
    s = malloc(54);
    fread(s, 54, 1, fin);

    unsigned int *p, *q;
    int w, h;
    formaLiniarizata1(&p, &w, &h, numeImagineSemiDecr);

    int r0, sv;
    unsigned int *r;
    seed(numeFisierCheie, &r0, &sv);
    XORSHIFT32(&r, 2*w*h-1, r0);

    unsigned int *permInv, *perm;
    permutare(&perm, w*h, r);
    permutareInversa(perm, &permInv, w*h);

    q = malloc(w*h*sizeof(int));

    int i;
    for(i=0; i<w*h; i++)
        q[permInv[i]] = p[i];

    scriere(q, w, h, numeImagineDecr, s);
    free(s);
    free(p);
    free(q);
    fclose(fin);
}

void chipatrat(char *numeImagine)
{
    FILE *fin = fopen(numeImagine, "rb");
    if(fin == NULL)
    {
        printf("Eroare la deschidere");
        return;
    }

    unsigned int *p;
    int w, h;
    formaLiniarizata1(&p, &w, &h, numeImagine);

    int i, j, nr1, nr2, nr3;
    float chi1 = 0, chi2 = 0, chi3 = 0;
    const float fmed = (w*h)/256.0;

    for(i=0; i<256; i++)
    {
        nr1 = 0;
        nr2 = 0;
        nr3 = 0;
        for(j=0; j<w*h; j++)
        {
            unsigned int c1, c2, c3;
            c1 = p[j] >> 16;
            c2 = (p[j] << 16) >> 24;
            c3 = (p[j] << 24) >> 24;
            if(c1 == i)
                nr1++;
            if(c2 == i)
                nr2++;
            if(c3 == i)
                nr3++;
        }
        chi1 = chi1 + ((nr1-fmed)*(nr1-fmed))/fmed;
        chi2 = chi2 + ((nr2-fmed)*(nr2-fmed))/fmed;
        chi3 = chi3 + ((nr3-fmed)*(nr3-fmed))/fmed;
    }

    printf("R: %.2f\nG: %.2f\nB: %.2f\n", chi1, chi2, chi3);

    free(p);
    fclose(fin);

}

int main()
{
    float prag;
    char *numeImagine, *numeImagineColor, *numeFisierCheie, *numeImagineCriptata, *numeImagineGrayscale, *numeImagineRezultat;
    printf(" 1. Numele imaginii initiale\n 2. Numele imaginii criptate\n 3. Numele imaginii decriptate\n 4. Numele imaginii grayscale\n 5. Numele fisierului continand cheia secreta\n 6. Pragul (nr. real)\n 7. Numele imaginii continand rezultatul final\n");
    citire(&numeImagine);
    citire(&numeImagineCriptata);
    citire(&numeImagineColor);
    citire(&numeImagineGrayscale);
    citire(&numeFisierCheie);
    scanf("%f", &prag);
    citire(&numeImagineRezultat);

    criptare1(numeImagine, "imagine_aux1.bmp", numeFisierCheie);
    criptare2("imagine_aux1.bmp", numeImagineCriptata, numeFisierCheie);
    decriptare1(numeImagineCriptata, "imagine_aux2.bmp", numeFisierCheie);
    decriptare2("imagine_aux2.bmp", numeImagineColor, numeFisierCheie);

    chipatrat(numeImagine);
    chipatrat(numeImagineCriptata);

    grayScale(numeImagineColor, numeImagineGrayscale);

    templateMatch(numeImagineGrayscale, numeImagineColor, prag, numeImagineRezultat);
    return 0;
}
