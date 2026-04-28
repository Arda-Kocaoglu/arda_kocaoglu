#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Kitap {
    char isbn[20];
    char ad[50];
    char yazar[50];
    char tur[30];
    int sayfa;
    struct Kitap *onceki;
    struct Kitap *sonraki;
};

struct Kuyruk {
    struct Kitap *veri;
    struct Kuyruk *sonraki;
};

struct Kuyruk *kuyrukBas = NULL;
struct Kuyruk *kuyrukSon = NULL;
struct Kitap *listeBas = NULL;

void kuyrugaEkle(struct Kitap *yeni) {
    struct Kuyruk *yeniDugum = (struct Kuyruk*)malloc(sizeof(struct Kuyruk));
    yeniDugum->veri = yeni;
    yeniDugum->sonraki = NULL;

    if (kuyrukSon == NULL) {
        kuyrukBas = kuyrukSon = yeniDugum;
    } else {
        kuyrukSon->sonraki = yeniDugum;
        kuyrukSon = yeniDugum;
    }
}

struct Kitap* kuyruktanCikar() {
    if (kuyrukBas == NULL)
        return NULL;

    struct Kuyruk *temp = kuyrukBas;
    struct Kitap *veri = temp->veri;
    kuyrukBas = kuyrukBas->sonraki;

    if (kuyrukBas == NULL)
        kuyrukSon = NULL;

    free(temp);
    return veri;
}

void listeyeEkle(struct Kitap *yeni) {
    if (listeBas == NULL) {
        listeBas = yeni;
        yeni->onceki = yeni->sonraki = yeni;
        return;
    }

    struct Kitap *gec = listeBas;
    do {
        if (strcmp(yeni->isbn, gec->isbn) < 0) {
            yeni->onceki = gec->onceki;
            yeni->sonraki = gec;
            gec->onceki->sonraki = yeni;
            gec->onceki = yeni;
            if (gec == listeBas)
                listeBas = yeni;
            return;
        }
        gec = gec->sonraki;
    } while (gec != listeBas);

    yeni->onceki = listeBas->onceki;
    yeni->sonraki = listeBas;
    listeBas->onceki->sonraki = yeni;
    listeBas->onceki = yeni;
}

void listeyiYazdir() {
    if (listeBas == NULL) {
        printf("\nListe bos!\n");
        return;
    }

    struct Kitap *gec = listeBas;
    printf("\n--- KITAP LISTESI ---\n");
    do {
        printf("%s | %s | %s | %s | %d sayfa\n",
               gec->isbn, gec->ad, gec->yazar, gec->tur, gec->sayfa);
        gec = gec->sonraki;
    } while (gec != listeBas);
}

void kitapSil(char *isbn) {
    if (listeBas == NULL) {
        printf("Liste bos!\n");
        return;
    }

    struct Kitap *gec = listeBas;
    do {
        if (strcmp(gec->isbn, isbn) == 0) {
            if (gec->sonraki == gec) {
                listeBas = NULL;
            } else {
                gec->onceki->sonraki = gec->sonraki;
                gec->sonraki->onceki = gec->onceki;
                if (gec == listeBas)
                    listeBas = gec->sonraki;
            }
            printf("%s (%s) listeden silindi.\n", gec->ad, gec->yazar);
            free(gec);
            return;
        }
        gec = gec->sonraki;
    } while (gec != listeBas);

    printf("Kitap bulunamadi.\n");
}

void turAra(char *tur) {
    if (listeBas == NULL) {
        printf("Liste bos!\n");
        return;
    }

    struct Kitap *gec = listeBas;
    int bulundu = 0;
    do {
        if (strcmp(gec->tur, tur) == 0) {
            bulundu = 1;
            if (gec->sayfa <= 200)
                printf("%s - %d sayfa (Kisa)\n", gec->ad, gec->sayfa);
            else if (gec->sayfa <= 400)
                printf("%s - %d sayfa (Orta)\n", gec->ad, gec->sayfa);
            else
                printf("%s - %d sayfa (Uzun)\n", gec->ad, gec->sayfa);
        }
        gec = gec->sonraki;
    } while (gec != listeBas);

    if (!bulundu)
        printf("Bu turde kitap bulunamadi.\n");
}

void yazarSirala() {
    if (listeBas == NULL || listeBas->sonraki == listeBas)
        return;

    int degisti;
    do {
        degisti = 0;
        struct Kitap *gec = listeBas;
        do {
            struct Kitap *sonraki = gec->sonraki;
            if (strcmp(gec->yazar, sonraki->yazar) > 0 ||
               (strcmp(gec->yazar, sonraki->yazar) == 0 &&
                strcmp(gec->ad, sonraki->ad) > 0)) {

                struct Kitap temp = *gec;

                strcpy(gec->isbn, sonraki->isbn);
                strcpy(gec->ad, sonraki->ad);
                strcpy(gec->yazar, sonraki->yazar);
                strcpy(gec->tur, sonraki->tur);
                gec->sayfa = sonraki->sayfa;

                strcpy(sonraki->isbn, temp.isbn);
                strcpy(sonraki->ad, temp.ad);
                strcpy(sonraki->yazar, temp.yazar);
                strcpy(sonraki->tur, temp.tur);
                sonraki->sayfa = temp.sayfa;

                degisti = 1;
            }
            gec = gec->sonraki;
        } while (gec->sonraki != listeBas);
    } while (degisti);

    printf("\nListe yazar adina gore siralandi.\n");
}

void kutuphaneBilgisi() {
    if (listeBas == NULL) {
        printf("Liste bos!\n");
        return;
    }

    int sayac = 0, toplamSayfa = 0;
    struct Kitap *gec = listeBas;

    do {
        sayac++;
        toplamSayfa += gec->sayfa;
        gec = gec->sonraki;
    } while (gec != listeBas);

    printf("\nToplam kitap sayisi: %d\n", sayac);
    printf("Toplam sayfa sayisi: %d\n", toplamSayfa);
    printf("Ortalama sayfa sayisi: %.2f\n", (float)toplamSayfa / sayac);
}


int main() {
    FILE *dosya = fopen("kitaplar.txt", "r");
    if (dosya == NULL) {
        printf("kitaplar.txt bulunamadi!\n");
        return 1;
    }

    char satir[200];
    while (fgets(satir, sizeof(satir), dosya)) {
        satir[strcspn(satir, "\r\n")] = 0;

        struct Kitap *yeni = (struct Kitap*)malloc(sizeof(struct Kitap));
        sscanf(satir, "%[^|]|%[^|]|%[^|]|%[^|]|%d",
               yeni->isbn, yeni->ad, yeni->yazar, yeni->tur, &yeni->sayfa);
        yeni->onceki = yeni->sonraki = NULL;
        kuyrugaEkle(yeni);
    }
    fclose(dosya);

    struct Kitap *alinan;
    while ((alinan = kuyruktanCikar()) != NULL) {
        listeyeEkle(alinan);
    }

    listeyiYazdir();

    char secim;
    do {
        printf("\n1- Kitap sil\n2- Tur ara\n3- Kutuphane bilgisi\n4- Yazar adina gore sirala\n5- Cikis\nSecim: ");
        scanf(" %c", &secim);

        if (secim == '1') {
            char isbn[20];
            printf("ISBN: ");
            scanf("%s", isbn);
            kitapSil(isbn);
            listeyiYazdir();
        }
        else if (secim == '2') {
            char tur[30];
            printf("Tur: ");
            scanf("%s", tur);
            turAra(tur);
        }
        else if (secim == '3') {
            kutuphaneBilgisi();
        }
        else if (secim == '4') {
            yazarSirala();
            listeyiYazdir();
        }

    } while (secim != '5');

    printf("\nProgram sonlandi.\n");
    return 0;
}
