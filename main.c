#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define  TemplateCols 11
#define  TemplateLines 15

unsigned int xorshift32(unsigned int *v)
{
    unsigned int x=v[0];
    x^=x<<13;
    x^=x>>17;
    x^=x<<5;
    v[0]=x;
    return x;
}
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}pixel;
typedef struct
{
    unsigned char *header;
    pixel *image_data;
    unsigned int latime;
    unsigned int inaltime;
    int padding;
}imagine;
typedef struct
{
int x;
int y;
double corelatie;
int cifra;
}punct;
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}culoare;
typedef struct
{
    pixel **image_data;
    unsigned char *header;
    unsigned int latime;
    unsigned int inaltime;
    int padding;
}imaginecamatrice;
imagine loadimage(char *path)
{
    FILE *fin=fopen(path,"rb");
    if(fin==NULL)
        printf("Eroare la deschidere imagine");

     unsigned int dim_img, latime_img, inaltime_img;

     imagine x;

     x.header=(unsigned char*)malloc(54);
     if(x.header==NULL)
        printf("Eroare la alocare");

       fread(x.header,54,1,fin);
       fseek(fin, 2, SEEK_SET);
       fread(&dim_img, sizeof(unsigned int), 1, fin);
       fseek(fin, 18, SEEK_SET);
       fread(&latime_img, sizeof(unsigned int), 1, fin);
       fread(&inaltime_img, sizeof(unsigned int), 1, fin);

       dim_img=latime_img*inaltime_img; // in pixeli

       x.image_data=(pixel*)malloc(dim_img*sizeof(pixel));
       if(x.image_data==NULL)
        printf("Eroare la alocare");


       int i,j,indice=0;
       x.inaltime=inaltime_img;
       x.latime=latime_img;
       int padding;

       if(latime_img % 4 != 0)
        padding = (4 - (3 * latime_img) % 4)%4;
    else
        padding = 0;
       x.padding=padding;


       for(i=0;i<inaltime_img;i++)
       {
        for(j=0;j<latime_img;j++)
       {
           fread(&x.image_data[indice].b,1,1,fin);
           fread(&x.image_data[indice].g,1,1,fin);
           fread(&x.image_data[indice].r,1,1,fin);
           indice++;

       }
       fseek(fin,x.padding*sizeof(unsigned char),SEEK_CUR);
       }

       fclose(fin);

       return x;

}
void salvare(imagine x, char *unde)
{
    FILE *out=fopen(unde,"wb");
    if(out==NULL)
        printf("Eroare la deschidere fisier");

     int i,j,indice=0,y;
     char pad=0;
     fwrite(x.header,54,1,out);

     for(i=0;i<x.inaltime;i++)
     {
        for(j=0;j<x.latime;j++)
     {
         fwrite(&x.image_data[indice].g,1,1,out);
         fwrite(&x.image_data[indice].r,1,1,out);
         fwrite(&x.image_data[indice].b,1,1,out);
         indice++;
     }
       for(y=0;y<x.padding;y++)
       {
           fwrite(&pad,1,1,out);

       }
     }
     fclose(out);
}
void criptare(imagine v, char *criptat, char *secretkey)
{

    unsigned int *nraleatoare=(unsigned int*)malloc(2*v.inaltime*v.latime*sizeof(unsigned int));
    if(nraleatoare==NULL)
        printf("Eroare la alocare");

    FILE *secret=fopen(secretkey,"rt");
    if(secret==NULL)
        printf("Eroare la deschidere fisier");

    unsigned int cheie;

    fscanf(secret,"%u",&cheie);

    nraleatoare[0]=cheie;
    unsigned int *d=malloc(sizeof(unsigned int));
    if(d==NULL)
        printf("Eroare la alocare");
        d[0]=cheie;

    int i;
    for(i=1;i<2*v.inaltime*v.latime;i++)
    nraleatoare[i]=xorshift32(d);
    free(d);

    unsigned int dim_img=v.inaltime*v.latime;
    int indiceR=1;
    int poz;
    pixel aux;
    for(i=dim_img;i>0;i--)
    {
        poz=nraleatoare[indiceR]%(i+1);
        indiceR++;

        aux=v.image_data[i];
        v.image_data[i]=v.image_data[poz];
        v.image_data[poz]=aux;
    }

    unsigned char *octet;
    fscanf(secret,"%u",&cheie);
    fclose(secret);

    octet=&cheie;
    octet++;
    v.image_data[0].r^=*octet;
    octet++;
    v.image_data[0].g^=*octet;
    octet++;
    v.image_data[0].b=*octet;

    octet=&nraleatoare[indiceR];
    octet++;
    v.image_data[0].r^=*octet;
    octet++;
    v.image_data[0].g^=*octet;
    octet++;
    v.image_data[0].b^=*octet;

    indiceR++;
    for(i=1;i<dim_img;i++)
    {

        octet=&nraleatoare[indiceR];
        octet++;
        v.image_data[i].r^=v.image_data[i-1].r;
        v.image_data[i].r^=*octet;

        octet++;
        v.image_data[i].g^=v.image_data[i-1].g;
        v.image_data[i].g^=*octet;

        octet++;
        v.image_data[i].b^=v.image_data[i-1].b;
        v.image_data[i].b^=*octet;

        indiceR++;
    }
    free(nraleatoare);

    salvare(v,criptat);

}
void decriptare(imagine v, char *rezultat, char *secretkey)
{

  unsigned int *nraleatoare=(unsigned int*)malloc(2*v.inaltime*v.latime*sizeof(unsigned int)+4);
    if(nraleatoare==NULL)
        printf("Eroare la alocare");

  FILE *secret=fopen(secretkey,"rt");
    if(secret==NULL)
        printf("Eroare la deschidere fisier");

    unsigned int cheie;

    fscanf(secret,"%u",&cheie);

    nraleatoare[0]=cheie;
    unsigned int *d=malloc(sizeof(unsigned int));
    if(d==NULL)
        printf("Eroare la alocare");
        d[0]=cheie;

    int i;
    for(i=1;i<2*v.inaltime*v.inaltime;i++)
    nraleatoare[i]=xorshift32(d);
    free(d);


    fscanf(secret,"%u",&cheie);
    int indiceR=2*v.inaltime*v.latime;
    unsigned char *octet;

    for(i=v.inaltime*v.latime-1;i>0;i--)
    {

        octet=&nraleatoare[indiceR];
        octet++;

         v.image_data[i].r^=v.image_data[i-1].r;
         v.image_data[i].r^=*octet;

         octet++;
         v.image_data[i].g^=v.image_data[i-1].g;
         v.image_data[i].g^=*octet;

         octet++;
         v.image_data[i].b^=v.image_data[i-1].b;
         v.image_data[i].b^=*octet;

         indiceR--;
    }

     octet=&nraleatoare[indiceR];
     octet++;
     v.image_data[0].r^=*octet;
     octet++;
     v.image_data[0].g^=*octet;
     octet++;
     v.image_data[0].b^=*octet;

     octet=&cheie;
     octet++;
     v.image_data[0].r^=*octet;
     octet++;
     v.image_data[0].g^=*octet;
     octet++;
     v.image_data[0].b^=*octet;



     indiceR=v.inaltime*v.latime+1;
     int poz;
     pixel aux;
     for(i=0;i<v.inaltime*v.latime;i++)
     {
         poz=nraleatoare[indiceR]%(i+1);

          aux=v.image_data[i];
          v.image_data[i]=v.image_data[poz];
          v.image_data[poz]=aux;
         indiceR--;

     }

   free(nraleatoare);
   salvare(v,rezultat);

}
void testchipatrat(imagine v)
{

     int *blue=calloc(256,sizeof(int));
     int *green=calloc(256,sizeof(int));
     int *red=calloc(256,sizeof(int));
     if(blue==NULL || green==NULL || red==NULL)
        printf("Eroare la alocare");

     int i,indice=0,j;

  for(i=0;i<v.inaltime;i++)
    for(j=0;j<v.latime-v.padding;j++)
  {
      red[v.image_data[indice].r]++;

      green[v.image_data[indice].g]++;

      blue[v.image_data[indice].b]++;
      indice++;
  }

   double f=(v.latime*v.inaltime)/256.0;
   double sumaB=0,sumaG=0,sumaR=0;
   for(i=0;i<=255;i++)
   {
       sumaB+=(((blue[i]-f)*(blue[i]-f))/f);
       sumaG+=(((green[i]-f)*(green[i]-f))/f);
       sumaR+=(((red[i]-f)*(red[i]-f))/f);
   }


  printf("R:%.2f G:%.2f B:%.2f\n",sumaR,sumaG,sumaB);
}
void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie)
{
   FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], aux;

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);
   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;


	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
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
imaginecamatrice citire(char *sursa)
{
    imaginecamatrice p;

    FILE *in=fopen(sursa,"rb");
    if(in==NULL)
        printf("eroare la deschidere fisier");

    p.header=(unsigned char*)malloc(54*sizeof(unsigned char));
    if(p.header==NULL)
        printf("eroare la alocare");

    fread(p.header,54,1,in);

    fseek(in, 18, SEEK_SET);
    fread(&p.latime, sizeof(unsigned int), 1, in);
    fread(&p.inaltime, sizeof(unsigned int), 1, in);

    p.image_data=(pixel**)malloc(p.inaltime*sizeof(pixel*));
    if(p.image_data==NULL)
        printf("eroare la alocare");

    int i,j;
    for(i=0;i<p.inaltime;i++)
    {
        p.image_data[i]=(pixel*)malloc(p.latime*sizeof(pixel));
        if(p.image_data[i]==NULL)
            printf("eroare la alocare");
    }


    if(p.latime % 4 != 0)
        p.padding = (4 - (3 * p.latime) % 4)%4;
    else
        p.padding = 0;


          fseek(in,54,SEEK_SET);
        for(i=0;i<p.inaltime;i++)
        {
            for(j=0;j<p.latime;j++)
            {
                fread(&p.image_data[i][j].b,1,1,in);
                fread(&p.image_data[i][j].g,1,1,in);
                fread(&p.image_data[i][j].r,1,1,in);
            }
        fseek(in,p.padding,SEEK_CUR);
        }
        return p;

}
void afisare(imaginecamatrice p)
{

    FILE *out=fopen("iesire.bmp","wb");
    if(out==NULL)
        return NULL;

    fwrite(p.header,54,1,out);
    int i,j,y;
    char pad=0;
    for(i=0;i<p.inaltime;i++)
    {

        for(j=0;j<p.latime;j++)
    {
        fwrite(&p.image_data[i][j].b,1,1,out);
        fwrite(&p.image_data[i][j].g,1,1,out);
        fwrite(&p.image_data[i][j].r,1,1,out);

    }
    for(y=0;y<p.padding;y++)
    {
        fwrite(&pad,1,3,out);


    }
    }
        fclose(out);

}
void colorare(imaginecamatrice *orig, punct a, culoare cul)
{
      int i;

     for(i=0;i<TemplateLines;i++)
       {
           (*orig).image_data[a.y+i][a.x].b=cul.b;
           (*orig).image_data[a.y+i][a.x].g=cul.g;
           (*orig).image_data[a.y+i][a.x].r=cul.r;

           (*orig).image_data[a.y+i][a.x+10].b=cul.b;
           (*orig).image_data[a.y+i][a.x+10].g=cul.g;
           (*orig).image_data[a.y+i][a.x+10].r=cul.r;
       }

       for(i=0;i<TemplateCols;i++)
       {
           (*orig).image_data[a.y][a.x+i].b=cul.b;
          (*orig).image_data[a.y][a.x+i].g=cul.g;
          (*orig).image_data[a.y][a.x+i].r=cul.r;

          (*orig).image_data[a.y+14][a.x+i].b=cul.b;
           (*orig).image_data[a.y+14][a.x+i].g=cul.g;
           (*orig).image_data[a.y+14][a.x+i].r=cul.r;
       }
}
void matching(imaginecamatrice h, char *sablon, float prag, punct **detect, int *dimensiunevector, int *indiceactual, culoare ok,int *cifra)
{

//citesc sablonul si imaginea principala

    grayscale_image(sablon,"sablonactualgri.bmp");
    imaginecamatrice orig=h;
    imaginecamatrice temp=citire("sablonactualgri.bmp");

   int i,j,x,y;
  double corelation;


 //calculez media pixelilor din sablon


        double mediapixdinsablon=0;

        for(i=0;i<TemplateLines;i++)
            for (j=0;j<TemplateCols;j++)
                mediapixdinsablon+=temp.image_data[i][j].r;


         mediapixdinsablon/=165.0;

        double sigmas=0;
       for (i=0;i<TemplateLines;i++)
             for (j=0;j<TemplateCols;j++)
                sigmas+=((temp.image_data[i][j].r-mediapixdinsablon)*(temp.image_data[i][j].r-mediapixdinsablon));

            sigmas/=164.0;
            sigmas=sqrt(sigmas);


    double sigmafi;
    double mediafereastra;

   //parcurg toata imaginea principala fara punctele in care latimea este prea mica ca sa incapa un sablon

   for (y=0;y<=orig.inaltime - TemplateLines; y++) {
    for (x=0;x<=orig.latime - TemplateCols; x++ ) {
      mediafereastra=0;
      sigmafi=0;
      corelation=0;

        for (i=0;i<TemplateLines;i++)
            for(j=0;j<TemplateCols;j++)
                    mediafereastra+=orig.image_data[i+y][j+x].r;
            mediafereastra/=165.0;


    //calculez sigmafi
        for (i =0;i<TemplateLines;i++)
            for (j=0;j<TemplateCols;j++)
                sigmafi+=((orig.image_data[i+y][j+x].r-mediafereastra)*(orig.image_data[i+y][j+x].r-mediafereastra));


        sigmafi/=164.0;
        sigmafi=sqrt(sigmafi);

        //calculez corelatia

         for(i = 0; i < TemplateLines; i++ )
            for (j = 0; j < TemplateCols; j++ )
                    corelation+=((orig.image_data[i+y][j+x].r-mediafereastra)*(temp.image_data[i][j].r-mediapixdinsablon)/(sigmas*sigmafi));

       corelation/=165.0;

                 if(corelation>prag)
                {


                    (*detect)[*indiceactual].x=x;
                    (*detect)[*indiceactual].y=y;
                    (*detect)[*indiceactual].corelatie=corelation;
                    (*detect)[*indiceactual].cifra=*cifra;
                    (*indiceactual)++;
                    if((*indiceactual)>=2000)
                    {
                        (*detect)=realloc((*detect),2*(*indiceactual)*sizeof(punct));
                        (*indiceactual)++;
                        (*dimensiunevector)*=2;
                    }

                  punct e;
                  e.x=x;
                  e.y=y;

               colorare(&orig,e,ok);
             }
        }
       }

       for(i=0;i<temp.inaltime;i++)
        free(temp.image_data[i]);
        free(temp.image_data);
        (*cifra)++;


}
int cmp(const void *p, const void *q)
{
    punct *a=(punct*)p;
    punct *b=(punct*)q;

    if((*a).corelatie>(*b).corelatie)
        return -1;
        else
            return 1;
    return 0;
}
int min(int a,int b)
{
    if(a>b)
        return b;
    else return a;
}
int max(int a,int b)
{
    if(a>b)
        return a;
    else return b;

}
double overlap(punct x, punct y)
{
    punct stangasus1=x;
    punct stangasus2=y;
    punct dreaptajos1;
    punct dreaptajos2;
    dreaptajos1.x=stangasus1.x+14;
    dreaptajos1.y=stangasus1.y-10;
    dreaptajos2.x=stangasus2.x+14;
    dreaptajos2.y=stangasus2.y-10;
    if(stangasus1.x>dreaptajos2.x || stangasus2.x>dreaptajos1.x)
        return 0;
    if(stangasus1.y<dreaptajos2.y || stangasus2.y<dreaptajos1.y)
        return 0;

    float x_dist=abs(stangasus1.x-dreaptajos1.x);
    float y_dist=abs(stangasus1.y-dreaptajos1.y);

    float aria1=x_dist*y_dist;
    x_dist=abs(stangasus2.x-dreaptajos2.x);
    y_dist=abs(stangasus2.y-dreaptajos2.y);
    float aria2=x_dist*y_dist;

    float ariaintersectiei;
    x_dist=min(dreaptajos1.x,dreaptajos2.x)-max(stangasus1.x,stangasus2.x);
    y_dist=min(dreaptajos1.y,dreaptajos2.y)-max(stangasus1.y,stangasus2.y);
    ariaintersectiei=x_dist*y_dist;
    float sup=ariaintersectiei/(aria1+aria2);
    if(sup<0)
        return -sup;
    else return sup;


}
void initializareculori(culoare** p)
{
    (*p)[0].b=0;
    (*p)[0].g=0;
    (*p)[0].r=255;
    (*p)[1].b=0;
    (*p)[1].g=255;
    (*p)[1].r=255;
    (*p)[2].b=0;
    (*p)[2].g=255;
    (*p)[2].r=0;
    (*p)[3].b=255;
    (*p)[3].g=255;
    (*p)[3].r=0;
    (*p)[4].b=255;
    (*p)[4].g=0;
    (*p)[4].r=255;
    (*p)[5].b=255;
    (*p)[5].g=0;
    (*p)[5].r=0;
    (*p)[6].b=192;
    (*p)[6].g=192;
    (*p)[6].r=192;
    (*p)[7].b=0;
    (*p)[7].g=140;
    (*p)[7].r=255;
    (*p)[8].b=128;
    (*p)[8].g=0;
    (*p)[8].r=128;
    (*p)[9].b=128;
    (*p)[9].g=0;
    (*p)[9].r=0;

}
void eliminaremaxime(culoare *col, punct **detect, int *indicedetect)
{
    imaginecamatrice g=citire("rec.bmp");

      int i,j,m;
       for(i=0;i<(*indicedetect);i++)
        for(j=0;j<(*indicedetect);j++)
            if(overlap((*detect)[i],(*detect)[j])>0.2)
            {
            for(m=j;m<(*indicedetect)-1;m++)
            (*detect)[m]=(*detect)[m+1];
            (*indicedetect)--;
            }


      for(i=0;i<(*indicedetect);i++)
        colorare(&g,(*detect)[i],col[(*detect)[i].cifra]);

      afisare(g);


}
int main()
{

     //INCEPUT CRIPTARE
     /////////////////
     /////////////////

    char *start=malloc(100);
     if(!start)
        printf("Eroare la alocare");

     printf("Numele imaginii pentru criptare: ");
     gets(start);
     printf("\n");

     imagine v=loadimage(start);
     //salvare(v,"out.bmp"); ->salveaza imaginea in out.bmp


     char *sec=malloc(100);
     if(sec==NULL)
        printf("Eroare la alocare");

     printf("Numele fisierului cu cheia secreta: ");
     gets(sec);
     printf("\n");



     criptare(v,"criptat.bmp",sec);
     printf("Test chi-squared criptat: ");
     testchipatrat(v);
     printf("\n");

     decriptare(v,"rezultat.bmp",sec);
     printf("Test chi-squared decriptat: ");
     testchipatrat(v);


     free(start);
     free(v.header);
     free(v.image_data);
     free(sec);
     printf("\n\n\n\n\n");


     //INCEPUT RECUNOASTERE CIFRE
     ////////////
     ////////////

      char *s=malloc(100);
    if(!s)
        printf("Eroare la alocare memorie");
    printf("Numele imaginii cu cifre: ");
    gets(s);

    grayscale_image(s,"rec.bmp");
    imaginecamatrice h=citire("rec.bmp");


    punct *detectii=malloc(2000*sizeof(punct));
    if(detectii==NULL)
        printf("eroare la alocare");
    int dimensiunedetectii=2000;
    int indiceactualdetectie=0;

    culoare *col=(culoare*)malloc(10*sizeof(culoare));
    if(col==NULL)
        printf("eroare la alocare");

    initializareculori(&col);
    int cifraactuala=0;



       matching(h,"cifra0.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[0],&cifraactuala);
       printf("Verificare sablon cifra 0 completa\n");
       matching(h,"cifra1.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[1],&cifraactuala);
        printf("Verificare sablon cifra 1 completa\n");
       matching(h,"cifra2.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[2],&cifraactuala);
        printf("Verificare sablon cifra 2 completa\n");
       matching(h,"cifra3.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[3],&cifraactuala);
        printf("Verificare sablon cifra 3 completa\n");
       matching(h,"cifra4.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[4],&cifraactuala);
        printf("Verificare sablon cifra 4 completa\n");
       matching(h,"cifra5.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[5],&cifraactuala);
        printf("Verificare sablon cifra 5 completa\n");
       matching(h,"cifra6.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[6],&cifraactuala);
        printf("Verificare sablon cifra 6 completa\n");
       matching(h,"cifra7.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[7],&cifraactuala);
        printf("Verificare sablon cifra 7 completa\n");
       matching(h,"cifra8.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[8],&cifraactuala);
        printf("Verificare sablon cifra 8 completa\n");
       matching(h,"cifra9.bmp",0.5,&detectii,&dimensiunedetectii,&indiceactualdetectie,col[9],&cifraactuala);
        printf("Verificare sablon cifra 9 completa\n");

       qsort(detectii,indiceactualdetectie,sizeof(punct),cmp);



     //afisare(h) ->PENTRU AFISARE FARA MAXIME ELIMINATE

       eliminaremaxime(col,&detectii,&indiceactualdetectie);
       //SE AFISEAZA CU MAXIMELE ELIMINATE IN IESIRE.BMP




      //Eliberare memorie
       int i;
       for(i=0;i<h.inaltime;i++)
        free(h.image_data[i]);
        free(h.image_data);

        free(col);
        free(detectii);

     return 0;
}

