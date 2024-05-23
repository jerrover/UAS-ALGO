#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TIKET 100
#define MAX_NAMA_HOTEL 100
#define FILE_HOTEL "hotel.txt"
#define FILE_PESANAN "pesanan.txt"

struct Hotel {
    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];
};

struct Tiket {
    struct Hotel hotel;
    char tanggal_checkin[20];
    int durasi;
    int jumlah_tamu;
    int jumlah_kamar;
};

struct TiketNode {
    struct TiketNode *next;
    struct Tiket tiket;
};
struct TiketNode *top = NULL;

long int konversiHarga(const char *harga) {
    long int nilai = 0;
    int panjang = strlen(harga);
    int faktor = 1;
    int i;
    if (harga[panjang - 1] == 'M') {
        faktor = 1000000;
        panjang--;
    }
    else if (harga[panjang - 1] == 'R') {
        faktor = 100000;
        panjang--;
    }
    for (i = panjang - 1; i >= 0; i--) {
        if (harga[i] != '.' && harga[i] != ',') {
            nilai += (harga[i] - '0') * faktor;
            faktor *= 10;
        }
    }

    return nilai;
}

void push(struct Tiket tiket) {
    struct TiketNode *newNode = (struct TiketNode *)malloc(sizeof(struct TiketNode));
    if (newNode == NULL) {
        printf("Memori penuh, tidak dapat menambahkan pemesanan tiket.\n");
        return;
    }

    newNode->tiket = tiket;
    newNode->next = top;
    top = newNode;

    FILE *file = fopen(FILE_PESANAN, "a");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }
    long int hargaNumerik = konversiHarga(tiket.hotel.harga);
    fprintf(file, "%s#%s#%d#%d#%d#%ld\n", tiket.hotel.nama_hotel, tiket.tanggal_checkin, tiket.durasi,
            tiket.jumlah_tamu, tiket.jumlah_kamar, hargaNumerik);
    fclose(file);
}


void pesanTiket() {
    int jumlah_hotel = 0;

    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n\n");
        return;
    }

    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];
    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        jumlah_hotel++;
    }
    fclose(file);

    printf("Masukkan tanggal check-in (dd/mm/yyyy): ");
    char tanggal_checkin[20];
    scanf("%s", tanggal_checkin);

    printf("Masukkan durasi menginap (hari): ");
    int durasi;
    scanf("%d", &durasi);

    printf("Masukkan jumlah tamu (1-4 orang): ");
    int jumlah_tamu;
    scanf("%d", &jumlah_tamu);

    printf("Masukkan jumlah kamar: ");
    int jumlah_kamar;
    scanf("%d", &jumlah_kamar);

    file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n\n");
        return;
    }
    printf("\nDaftar Hotel:\n");
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga");
    int nomor_hotel = 1;
    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", nomor_hotel, nama_hotel, bintang_hotel, rating,
               jenis_penginapan == 'H' ? "Hotel" : (jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), harga);
        nomor_hotel++;
    }
    fclose(file);

    printf("\nSilakan pilih hotel dari daftar di atas (masukkan nomor hotel):\n");
    printf("\nMasukkan nomor hotel: ");
    int nomor_terpilih;
    scanf("%d", &nomor_terpilih);

    if (nomor_terpilih < 1 || nomor_terpilih > jumlah_hotel) {
        printf("Nomor hotel tidak valid.\n");
        return;
    }

    file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }
    for (int i = 1; i <= nomor_terpilih; i++) {
        fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga);
    }
    fclose(file);

    struct Tiket tiket;
    strcpy(tiket.hotel.nama_hotel, nama_hotel);
    tiket.hotel.bintang_hotel = bintang_hotel;
    tiket.hotel.rating = rating;
    tiket.hotel.jenis_penginapan = jenis_penginapan;
    strcpy(tiket.hotel.harga, harga);
    strcpy(tiket.tanggal_checkin, tanggal_checkin);
    tiket.durasi = durasi;
    tiket.jumlah_tamu = jumlah_tamu;
    tiket.jumlah_kamar = jumlah_kamar;

    push(tiket); 
    printf("\nTiket berhasil dipesan.\n");
}

void lihatTiket() {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("| No |              Nama Hotel             | Tanggal Check-in | Durasi | Tamu | Kamar |        Harga       |\n");
    printf("------------------------------------------------------------------------------------------------------------\n");

    int nomor_pesanan = 1;
    struct Tiket pesanan;
    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%s\n", pesanan.hotel.nama_hotel, pesanan.tanggal_checkin,
                  &pesanan.durasi, &pesanan.jumlah_tamu, &pesanan.jumlah_kamar, pesanan.hotel.harga) != EOF) {
        
        long int hargaNumerik = konversiHarga(pesanan.hotel.harga);
        hargaNumerik *= pesanan.jumlah_kamar * pesanan.durasi;

        printf("| %-2d | %-35s | %-16s | %-6d | %-4d | %-5d | Rp %-15ld |\n", nomor_pesanan, pesanan.hotel.nama_hotel, pesanan.tanggal_checkin,
               pesanan.durasi, pesanan.jumlah_tamu, pesanan.jumlah_kamar, hargaNumerik);
        nomor_pesanan++;
    }

    printf("------------------------------------------------------------------------------------------------------------\n\n");

    fclose(file);
}



void lihatHotel() {
    int pilihan;
    printf("\nPilih opsi:\n");
    printf("1. Lihat Hotel Berdasarkan Harga Tertinggi\n");
    printf("2. Lihat Hotel Berdasarkan Harga Terendah\n");
    printf("3. Urutkan Hotel Berdasarkan Rating Tertinggi\n");
    printf("4. Urutkan Hotel Berdasarkan Rating Terendah\n");
    printf("5. Urutkan Hotel Berdasarkan Bintang Terkecil\n");
    printf("6. Urutkan Hotel Berdasarkan Bintang Terbesar\n");
    printf("7. Lihat Hotel Berdasarkan Tipe Penginapan\n");
    printf("8. Kembali\n");
    printf("Pilihan Anda: ");
    scanf("%d", &pilihan);

    switch (pilihan) {
        case 1:
            lihatHotelBerdasarkanHarga('T');
            break;
        case 2:
            lihatHotelBerdasarkanHarga('K');
            break;
        case 3:
            urutHotelBerdasarkanRating('T');
            break;
        case 4:
            urutHotelBerdasarkanRating('K');
            break;
        case 5:
            urutHotelBerdasarkanBintang('K');
            break;
        case 6:
            urutHotelBerdasarkanBintang('T');
            break;
        case 7:
            lihatHotelBerdasarkanTipe();
            break;
        case 8:
            printf("\n");
            break;
        default:
            printf("Pilihan tidak valid.\n");
    }
}

void lihatHotelBerdasarkanTipe() {
    char tipe;
    printf("\nPilih tipe penginapan (H = Hotel, A = Apartemen, G = Guest House): ");
    scanf(" %c", &tipe);

    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }

    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];

    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        if (jenis_penginapan == tipe) {
            strcpy(hotels[jumlah_hotel].nama_hotel, nama_hotel);
            hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
            hotels[jumlah_hotel].rating = rating;
            hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
            strcpy(hotels[jumlah_hotel].harga, harga);
            jumlah_hotel++;
        }
    }

    fclose(file);

    printf("\nDaftar Hotel Berdasarkan Tipe Penginapan '%c':\n", tipe);
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}


void urutHotelBerdasarkanBintang(char urutan) {
    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }

    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];
     while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        strcpy(hotels[jumlah_hotel].nama_hotel, nama_hotel);
        hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
        hotels[jumlah_hotel].rating = rating;
        hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
        strcpy(hotels[jumlah_hotel].harga, harga);
        jumlah_hotel++;
    }

    fclose(file);

    if (urutan == 'T') {
        printf("\nDaftar Hotel Berdasarkan Bintang Terbesar:\n");
    } else {
        printf("\nDaftar Hotel Berdasarkan Bintang Terkecil:\n");
    }

    for (int i = 0; i < jumlah_hotel - 1; i++) {
        for (int j = i + 1; j < jumlah_hotel; j++) {
            if ((urutan == 'T' && hotels[i].bintang_hotel < hotels[j].bintang_hotel) ||
                (urutan == 'K' && hotels[i].bintang_hotel > hotels[j].bintang_hotel)) {
                struct Hotel temp = hotels[i];
                hotels[i] = hotels[j];
                hotels[j] = temp;
            }
        }
    }
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}

void lihatHotelBerdasarkanHarga(char urutan) {
    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }
    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];

    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        strcpy(hotels[jumlah_hotel].nama_hotel, nama_hotel);
        hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
        hotels[jumlah_hotel].rating = rating;
        hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
        strcpy(hotels[jumlah_hotel].harga, harga);
        jumlah_hotel++;
    }

    fclose(file);

    for (int i = 0; i < jumlah_hotel - 1; i++) {
        for (int j = i + 1; j < jumlah_hotel; j++) {
            long int harga_i = konversiHarga(hotels[i].harga);
            long int harga_j = konversiHarga(hotels[j].harga);
            if ((urutan == 'T' && harga_i < harga_j) ||
                (urutan == 'K' && harga_i > harga_j)) {
                struct Hotel temp = hotels[i];
                hotels[i] = hotels[j];
                hotels[j] = temp;
            }
        }
    }

    if (urutan == 'T') {
             printf("\nDaftar Hotel Berdasarkan Harga Tertinggi:\n");
    } else {
        printf("\nDaftar Hotel Berdasarkan Harga Terendah:\n");
    }

    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}

void urutHotelBerdasarkanRating(char urutan) {
    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }

    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char nama_hotel[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];
     while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        strcpy(hotels[jumlah_hotel].nama_hotel, nama_hotel);
        hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
        hotels[jumlah_hotel].rating = rating;
        hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
        strcpy(hotels[jumlah_hotel].harga, harga);
        jumlah_hotel++;
    }

    fclose(file);

    if (urutan == 'T') {
        printf("\nDaftar Hotel Berdasarkan Rating Tertinggi:\n");
    } else {
        printf("\nDaftar Hotel Berdasarkan Rating Terendah:\n");
    }

    for (int i = 0; i < jumlah_hotel - 1; i++) {
        for (int j = i + 1; j < jumlah_hotel; j++) {
            if ((urutan == 'T' && hotels[i].rating < hotels[j].rating) ||
                (urutan == 'K' && hotels[i].rating > hotels[j].rating)) {
                struct Hotel temp = hotels[i];
                hotels[i] = hotels[j];
                hotels[j] = temp;
            }
        }
    }
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}



void batalkanPesan() {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    struct Tiket pesanan[MAX_TIKET];
    int jumlah_pesanan = 0;

    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%s\n", pesanan[jumlah_pesanan].hotel.nama_hotel,
                  pesanan[jumlah_pesanan].tanggal_checkin, &pesanan[jumlah_pesanan].durasi,
                  &pesanan[jumlah_pesanan].jumlah_tamu, &pesanan[jumlah_pesanan].jumlah_kamar,
                  pesanan[jumlah_pesanan].hotel.harga) != EOF) {
        jumlah_pesanan++;
    }
    fclose(file);

    if (jumlah_pesanan > 0) {
        printf("Pesanan terakhir akan dibatalkan.\n");
        jumlah_pesanan--;
        file = fopen(FILE_PESANAN, "w");
        if (file == NULL) {
            printf("Gagal membuka file pesanan.\n");
            return;
        }
        for (int i = 0; i < jumlah_pesanan; i++) {
            fprintf(file, "%s#%s#%d#%d#%d#%s\n", pesanan[i].hotel.nama_hotel, pesanan[i].tanggal_checkin,
                    pesanan[i].durasi, pesanan[i].jumlah_tamu, pesanan[i].jumlah_kamar,
                    pesanan[i].hotel.harga);
        }
        fclose(file);

        printf("Pesanan terakhir berhasil dibatalkan.\n");
    } else {
        printf("Tidak ada pesanan yang bisa dibatalkan.\n");
    }
}
int main(){
    int pilihan;
    do{
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("----------------------------------------------  book@serpong  ----------------------------------------------\n");
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("\nPilih menu:\n");
        printf("1. Pesan Tiket\n");
        printf("2. Lihat Tiket\n");
        printf("3. Batalkan Pesanan\n");
        printf("4. Lihat Hotel\n");
        printf("5. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                pesanTiket();
                break;
            case 2:
                lihatTiket();
                break;
            case 3:
                batalkanPesan();
                break;
                case 4:
                lihatHotel();
                break;
            case 5:
                printf("Keluar dari program.\n");
                break;
            default:
                printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 5);
     while (top != NULL) {
        struct TiketNode *temp = top;
        top = top->next;
        free(temp);
    }
    return 0;
}
