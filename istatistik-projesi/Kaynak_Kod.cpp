#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX 1000

typedef struct {
    double yas;
    double vki;
    double tansiyon;
    double hastalik;
} Veri;

void satiriDuzenle(char satir[]) {
    for (int i = 0; satir[i] != '\0'; i++) {
        if (satir[i] == ';') satir[i] = ' '; 
        if (satir[i] == ',') satir[i] = '.'; 
    }
}

double korelasyon(double x[], double y[], int n) {
    double sumX=0, sumY=0, sumXY=0, sumX2=0, sumY2=0;

    for(int i=0;i<n;i++){
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i]*y[i];
        sumX2 += x[i]*x[i];
        sumY2 += y[i]*y[i];
    }

    double pay = n*sumXY - sumX*sumY;
    double payda = sqrt((n*sumX2 - sumX*sumX)*(n*sumY2 - sumY*sumY));

    if(payda==0) return 0;
    return pay/payda;
}

void regresyon(double x[], double y[], int n, double *a, double *b){
    double sumX=0, sumY=0, sumXY=0, sumX2=0;

    for(int i=0;i<n;i++){
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i]*y[i];
        sumX2 += x[i]*x[i];
    }

    *b = (n*sumXY - sumX*sumY)/(n*sumX2 - sumX*sumX);
    *a = (sumY - (*b)*sumX)/n;
}

double sse(double x[], double y[], int n, double a, double b){
    double toplam=0;
    for(int i=0;i<n;i++){
        double tahmin = a + b*x[i];
        double hata = y[i] - tahmin;
        toplam += hata*hata;
    }
    return toplam;
}

int main(){

    FILE *f = fopen("veriseti.csv","r");
    if(f==NULL){
        printf("Dosya acilamadi!\n");
        return 1;
    }

    Veri veri[MAX];
    char satir[200];
    int n=0;

    fgets(satir,200,f); 

    while(fgets(satir,200,f)!=NULL){
        satiriDuzenle(satir);

        sscanf(satir,"%lf %lf %lf %lf",
            &veri[n].yas,
            &veri[n].vki,
            &veri[n].tansiyon,
            &veri[n].hastalik
        );
        n++;
    }
    fclose(f);

    printf("Toplam veri: %d\n",n);

    srand(time(NULL));

    for(int i=0;i<n;i++){
        int r = rand()%n;
        Veri tmp = veri[i];
        veri[i]=veri[r];
        veri[r]=tmp;
    }

    int egitimN = n*70/100;
    int testN = n - egitimN;

    double x1[MAX],x2[MAX],x3[MAX],y[MAX];

    for(int i=0;i<egitimN;i++){
        x1[i]=veri[i].yas;
        x2[i]=veri[i].vki;
        x3[i]=veri[i].tansiyon;
        y[i]=veri[i].hastalik;
    }

    double r1 = korelasyon(x1,y,egitimN);
    double r2 = korelasyon(x2,y,egitimN);
    double r3 = korelasyon(x3,y,egitimN);

    printf("\nr1(yas): %.4lf\n",r1);
    printf("r2(vki): %.4lf\n",r2);
    printf("r3(tansiyon): %.4lf\n",r3);

    double secilen[MAX];
    char isim[20];

    if(fabs(r1)>=fabs(r2) && fabs(r1)>=fabs(r3)){
        strcpy(isim,"yas");
        for(int i=0;i<egitimN;i++) secilen[i]=x1[i];
    }
    else if(fabs(r2)>=fabs(r1) && fabs(r2)>=fabs(r3)){
        strcpy(isim,"vki");
        for(int i=0;i<egitimN;i++) secilen[i]=x2[i];
    }
    else{
        strcpy(isim,"tansiyon");
        for(int i=0;i<egitimN;i++) secilen[i]=x3[i];
    }

    printf("\nEn iyi degisken: %s\n",isim);

    double a,b;
    regresyon(secilen,y,egitimN,&a,&b);

    printf("\nModel: y = %.4lf + %.4lf * %s\n",a,b,isim);

    double egitimSSE = sse(secilen,y,egitimN,a,b);
    printf("Egitim SSE: %.4lf\n",egitimSSE);

    FILE *f1 = fopen("egitim_tahmin.txt","w");

    printf("\nIlk 10 tahmin:\n");

    for(int i=0;i<egitimN;i++){
        double tahmin = a + b*secilen[i];

        if(i<10)
            printf("%d -> %.2lf\n",i+1,tahmin);

        fprintf(f1,"%lf %lf\n",y[i],tahmin);
    }
    fclose(f1);

    double testX[MAX],testY[MAX];

    for(int i=0;i<testN;i++){
        if(strcmp(isim,"yas")==0) testX[i]=veri[egitimN+i].yas;
        else if(strcmp(isim,"vki")==0) testX[i]=veri[egitimN+i].vki;
        else testX[i]=veri[egitimN+i].tansiyon;

        testY[i]=veri[egitimN+i].hastalik;
    }

    double testSSE = sse(testX,testY,testN,a,b);
    printf("Test SSE: %.4lf\n",testSSE);

    FILE *f2 = fopen("test_tahmin.txt","w");

    for(int i=0;i<testN;i++){
        double tahmin = a + b*testX[i];
        fprintf(f2,"%lf %lf\n",testY[i],tahmin);
    }
    fclose(f2);

    printf("\nDosyalar olusturuldu.\n");

    return 0;
}
