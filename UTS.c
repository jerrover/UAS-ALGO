#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

long int konversiHarga(const char *harga) {
    long int nilai = 0;
    int panjang = strlen(harga);
    int faktor = 1;
    int i;
    if (harga[panjang - 1] == 'M') {
        faktor = 1000000;
        panjang--;
    } else if (harga[panjang - 1] == 'R') {
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

struct TreeNode {
    struct Tiket tiket;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode* createNode(struct Tiket tiket) {
    struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    if (newNode != NULL) {
        newNode->tiket = tiket;
        newNode->left = NULL;
        newNode->right = NULL;
    }
    return newNode;
}

void insertNode(struct TreeNode **node, struct Tiket tiket) {
    if (*node == NULL) {
        *node = createNode(tiket);
    } else {
        if (strcmp(tiket.tanggal_checkin, (*node)->tiket.tanggal_checkin) < 0) {
            insertNode(&((*node)->left), tiket);
        } else {
            insertNode(&((*node)->right), tiket);
        }
    }
}

void push(struct TreeNode **root, struct Tiket tiket) {
    insertNode(root, tiket);

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

struct TreeNode* cariTiket(struct TreeNode *node, const char *tanggal_checkin) {
    if (node == NULL) {
        return NULL;
    }
    int cmp = strcmp(tanggal_checkin, node->tiket.tanggal_checkin);
    if (cmp == 0) {
        return node;
    } else if (cmp < 0) {
        return cariTiket(node->left, tanggal_checkin);
    } else {
        return cariTiket(node->right, tanggal_checkin);
    }
}

void loadPesananToBST(struct TreeNode **root) {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    struct Tiket tiket;
    long int hargaNumerik;
    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%ld\n", tiket.hotel.nama_hotel, tiket.tanggal_checkin,
                  &tiket.durasi, &tiket.jumlah_tamu, &tiket.jumlah_kamar, &hargaNumerik) != EOF) {
        sprintf(tiket.hotel.harga, "%ld", hargaNumerik);
        insertNode(root, tiket);
    }

    fclose(file);
}

void pesanTiket(struct TreeNode **root) {
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
    scanf("%19s", tanggal_checkin);

    int dd, mm, yy;
    while (sscanf(tanggal_checkin, "%d/%d/%d", &dd, &mm, &yy) != 3 ||
           dd < 1 || dd > 31 || mm < 1 || mm > 12 || yy < 0 || yy > 9999) {
        printf("Format tanggal tidak valid. Silahkan masukkan tanggal check-in yang benar (dd/mm/yyyy): ");
        scanf("%19s", tanggal_checkin);
    }
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
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga per malam");
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
    while (scanf("%d", &nomor_terpilih) && (nomor_terpilih < 1 || nomor_terpilih > jumlah_hotel)) {
        printf("Nomor hotel tidak valid. Silakan masukkan nomor hotel: ");
    }
    file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }

    for (int i = 0; i < nomor_terpilih - 1; i++) {
        fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga);
    }
    fscanf(file, "%[^#]#%c#%f#%c#%s\n", nama_hotel, &bintang_hotel, &rating, &jenis_penginapan, harga);
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

    push(root, tiket);
    printf("\nTiket berhasil dipesan.\n\n");
}

void lihatTiket(struct TreeNode *root) {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    printf("------------------------------------------------------------------------------------------------------------------\n");
    printf("| No |              Nama Hotel             | Tanggal Check-in | Durasi | Tamu | Kamar |        Harga       |\n");
    printf("------------------------------------------------------------------------------------------------------------------\n");

    int nomor_pesanan = 1;
    struct Tiket pesanan;
    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%[^\n]\n", pesanan.hotel.nama_hotel, pesanan.tanggal_checkin,
                  &pesanan.durasi, &pesanan.jumlah_tamu, &pesanan.jumlah_kamar, pesanan.hotel.harga) != EOF) {

        long int hargaNumerik = konversiHarga(pesanan.hotel.harga);
        hargaNumerik *= pesanan.jumlah_kamar * pesanan.durasi;

        printf("| %-2d | %-35s | %-16s | %-6d | %-4d | %-5d | Rp %-15ld |\n", nomor_pesanan, pesanan.hotel.nama_hotel, pesanan.tanggal_checkin,
               pesanan.durasi, pesanan.jumlah_tamu, pesanan.jumlah_kamar, hargaNumerik);
        nomor_pesanan++;
    }

    printf("------------------------------------------------------------------------------------------------------------------\n\n");

    fclose(file);

    while (1) {
        printf("Masukkan tanggal check-in yang ingin dicari (dd/mm/yyyy) atau 0 untuk kembali: ");
        char tanggal_checkin[20];
        scanf("%19s", tanggal_checkin);
        if (strcmp(tanggal_checkin, "0") == 0) {
            break;
        }

        struct TreeNode *node = cariTiket(root, tanggal_checkin);
        if (node != NULL) {
            long int hargaNumerik = konversiHarga(node->tiket.hotel.harga);
            hargaNumerik *= node->tiket.jumlah_kamar * node->tiket.durasi;
            printf("\nDetail Pemesanan Ditemukan:\n");
            printf("-------------------------------------------------------------------------------------------------------\n");
            printf("|              Nama Hotel             | Tanggal Check-in | Durasi | Tamu | Kamar |        Harga       |\n");
            printf("-------------------------------------------------------------------------------------------------------\n");
            printf("| %-35s | %-16s | %-6d | %-4d | %-5d | Rp %-15ld |\n", node->tiket.hotel.nama_hotel, node->tiket.tanggal_checkin,
                   node->tiket.durasi, node->tiket.jumlah_tamu, node->tiket.jumlah_kamar, hargaNumerik);
            printf("-------------------------------------------------------------------------------------------------------\n\n");
        } else {
            printf("Pesanan dengan tanggal %s tidak ditemukan. Silakan coba lagi.\n", tanggal_checkin);
        }
    }
}

struct TreeNode* findMin(struct TreeNode* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

struct TreeNode* deleteNode(struct TreeNode* root, const char* tanggal_checkin) {
    if (root == NULL) return root;

    int cmp = strcmp(tanggal_checkin, root->tiket.tanggal_checkin);
    if (cmp < 0) {
        root->left = deleteNode(root->left, tanggal_checkin);
    } else if (cmp > 0) {
        root->right = deleteNode(root->right, tanggal_checkin);
    } else {
        if (root->left == NULL) {
            struct TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        struct TreeNode* temp = findMin(root->right);
        root->tiket = temp->tiket;
        root->right = deleteNode(root->right, temp->tiket.tanggal_checkin);
    }
    return root;
}

struct Tiket heap[MAX_TIKET];
int heapSize = 0;

void heapifyDown(int idx) {
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int smallest = idx;

    if (left < heapSize && strcmp(heap[left].tanggal_checkin, heap[smallest].tanggal_checkin) < 0) {
        smallest = left;
    }

    if (right < heapSize && strcmp(heap[right].tanggal_checkin, heap[smallest].tanggal_checkin) < 0) {
        smallest = right;
    }

    if (smallest != idx) {
        struct Tiket temp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = temp;
        heapifyDown(smallest);
    }
}

void heapifyUp(int idx) {
    int parent = (idx - 1) / 2;

    if (parent >= 0 && strcmp(heap[idx].tanggal_checkin, heap[parent].tanggal_checkin) < 0) {
        struct Tiket temp = heap[idx];
        heap[idx] = heap[parent];
        heap[parent] = temp;
        heapifyUp(parent);
    }
}

void insertHeap(struct Tiket tiket) {
    heap[heapSize] = tiket;
    heapSize++;
    heapifyUp(heapSize - 1);
}

struct Tiket extractMin() {
    struct Tiket rootTiket = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    heapifyDown(0);
    return rootTiket;
}

void deleteFromHeap(int idx) {
    if (idx < 0 || idx >= heapSize) {
        printf("Indeks tidak valid.\n");
        return;
    }

    heap[idx] = heap[heapSize - 1];
    heapSize--;

    heapifyDown(idx);
    heapifyUp(idx);
}

void loadPesananToHeap() {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    struct Tiket tiket;
    long int hargaNumerik;
    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%ld\n", tiket.hotel.nama_hotel, tiket.tanggal_checkin,
                  &tiket.durasi, &tiket.jumlah_tamu, &tiket.jumlah_kamar, &hargaNumerik) != EOF) {
        sprintf(tiket.hotel.harga, "%ld", hargaNumerik);
        insertHeap(tiket);
    }

    fclose(file);
}

void lihatTiketTerurut() {
    loadPesananToHeap();

    printf("------------------------------------------------------------------------------------------------------------------\n");
    printf("| No |              Nama Hotel             | Tanggal Check-in | Durasi | Tamu | Kamar |        Harga       |\n");
    printf("------------------------------------------------------------------------------------------------------------------\n");

    int nomor_pesanan = 1;
    while (heapSize > 0) {
        struct Tiket pesanan = extractMin();

        long int hargaNumerik = konversiHarga(pesanan.hotel.harga);
        hargaNumerik *= pesanan.jumlah_kamar * pesanan.durasi;

        printf("| %-2d | %-35s | %-16s | %-6d | %-4d | %-5d | Rp %-15ld |\n", nomor_pesanan, pesanan.hotel.nama_hotel, pesanan.tanggal_checkin,
               pesanan.durasi, pesanan.jumlah_tamu, pesanan.jumlah_kamar, hargaNumerik);
        nomor_pesanan++;
    }

    printf("------------------------------------------------------------------------------------------------------------------\n\n");
}

void batalkanPesan(struct TreeNode **root) {
    FILE *file = fopen(FILE_PESANAN, "r");
    if (file == NULL) {
        printf("Gagal membuka file pesanan.\n\n");
        return;
    }

    struct Tiket pesanan[MAX_TIKET];
    int jumlah_pesanan = 0;

    while (fscanf(file, "%[^#]#%[^#]#%d#%d#%d#%[^\n]\n", pesanan[jumlah_pesanan].hotel.nama_hotel,
                  pesanan[jumlah_pesanan].tanggal_checkin, &pesanan[jumlah_pesanan].durasi,
                  &pesanan[jumlah_pesanan].jumlah_tamu, &pesanan[jumlah_pesanan].jumlah_kamar,
                  pesanan[jumlah_pesanan].hotel.harga) != EOF) {
        jumlah_pesanan++;
    }
    fclose(file);

    if (jumlah_pesanan > 0) {
        printf("Pesanan yang telah dibuat:\n");
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("| No |              Nama Hotel             | Tanggal Check-in | Durasi | Tamu | Kamar |        Harga       |\n");
        printf("------------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < jumlah_pesanan; i++) {
            long int hargaNumerik = konversiHarga(pesanan[i].hotel.harga);
            hargaNumerik *= pesanan[i].jumlah_kamar * pesanan[i].durasi;
            printf("| %-2d | %-35s | %-16s | %-6d | %-4d | %-5d | Rp %-15ld |\n", i + 1, pesanan[i].hotel.nama_hotel, pesanan[i].tanggal_checkin,
                   pesanan[i].durasi, pesanan[i].jumlah_tamu, pesanan[i].jumlah_kamar, hargaNumerik);

        }
        printf("------------------------------------------------------------------------------------------------------------\n\n");

        int nomor;
        while (1) {
            printf("\nMasukkan nomor pesanan yang ingin dibatalkan (tekan 0 untuk kembali): ");
            scanf("%d", &nomor);
            if (nomor >= 1 && nomor <= jumlah_pesanan) {
                break;
            } else if (nomor == 0) {
                printf("\n");
                break;
            } else {
                printf("Nomor pesanan tidak valid. Silahkan masukkan angka yang valid.\n");
            }
        }

        if (nomor != 0) {
            file = fopen(FILE_PESANAN, "w");
            if (file == NULL) {
                printf("Gagal membuka file pesanan.\n");
                return;
            }
            for (int i = 0; i < jumlah_pesanan; i++) {
                if (i != nomor - 1) {
                    fprintf(file, "%s#%s#%d#%d#%d#%s\n", pesanan[i].hotel.nama_hotel, pesanan[i].tanggal_checkin,
                            pesanan[i].durasi, pesanan[i].jumlah_tamu, pesanan[i].jumlah_kamar,
                            pesanan[i].hotel.harga);
                }
            }
            fclose(file);

            *root = deleteNode(*root, pesanan[nomor - 1].tanggal_checkin);

            int heapIndex = -1;
            for (int i = 0; i < heapSize; i++) {
                if (strcmp(heap[i].tanggal_checkin, pesanan[nomor - 1].tanggal_checkin) == 0) {
                    heapIndex = i;
                    break;
                }
            }
            if (heapIndex != -1) {
                deleteFromHeap(heapIndex); 
            }

            printf("Pesanan nomor %d berhasil dibatalkan.\n", nomor);
        }
    } else {
        printf("Tidak ada pesanan yang bisa dibatalkan.\n");
    }
}

void freeMemoryBST(struct TreeNode *node) {
    if (node != NULL) {
        freeMemoryBST(node->left);
        freeMemoryBST(node->right);
        free(node);
    }
}

void lihatHotelBerdasarkanHarga(char urutan);
void urutHotelBerdasarkanRating(char urutan);
void urutHotelBerdasarkanBintang(char urutan);
void lihatHotelBerdasarkanTipe();

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
    printf("8. Cari Hotel dengan Binary Search\n");
    printf("9. Cari Hotel dengan Interpolation Search\n");
    printf("10. Kembali\n");
    printf("Pilihan Anda: ");
    scanf("%d", &pilihan);

    while ((getchar()) != '\n');

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
        case 8: {
            printf("Masukkan nama hotel yang ingin dicari: ");
            char nama_hotel[MAX_NAMA_HOTEL];
            fgets(nama_hotel, MAX_NAMA_HOTEL, stdin);
    
            nama_hotel[strcspn(nama_hotel, "\n")] = '\0';
            searchHotelBinary(nama_hotel);
            break;
        }
        case 9: {
            printf("Masukkan nama hotel yang ingin dicari: ");
            char nama_hotel[MAX_NAMA_HOTEL];
            fgets(nama_hotel, MAX_NAMA_HOTEL, stdin);
        
            nama_hotel[strcspn(nama_hotel, "\n")] = '\0';
            searchHotelInterpolation(nama_hotel);
            break;
        }
        case 10:
            printf("\n");
            break;
        default:
            printf("Pilihan tidak valid.\n");
    }
}

void swap(struct Hotel *a, struct Hotel *b) {
    struct Hotel temp = *a;
    *a = *b;
    *b = temp;
}

// Quick Sort untuk pengurutan berdasarkan harga
int partition(struct Hotel arr[], int low, int high) {
    char pivot[MAX_NAMA_HOTEL];
    strcpy(pivot, arr[high].nama_hotel);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (strcmp(arr[j].nama_hotel, pivot) < 0) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}


void quickSort(struct Hotel arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
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

    if (urutan == 'T') {
        quickSort(hotels, 0, jumlah_hotel - 1);
        printf("\nDaftar Hotel Berdasarkan Harga Tertinggi:\n");
    } else {
        printf("\nDaftar Hotel Berdasarkan Harga Terendah:\n");
        for (int i = 0; i < jumlah_hotel - 1; i++) {
            for (int j = i + 1; j < jumlah_hotel; j++) {
                long int harga_i = konversiHarga(hotels[i].harga);
                long int harga_j = konversiHarga(hotels[j].harga);
                if (harga_i > harga_j) {
                    struct Hotel temp = hotels[i];
                    hotels[i] = hotels[j];
                    hotels[j] = temp;
                }
            }
        }
    }

    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga per malam");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}

// Bubble Sort untuk pengurutan berdasarkan rating
void bubbleSort(struct Hotel arr[], int n, char urutan) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if ((urutan == 'T' && arr[j].rating < arr[j+1].rating) || (urutan == 'K' && arr[j].rating > arr[j+1].rating)) {
                swap(&arr[j], &arr[j+1]);
            }
        }
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

    bubbleSort(hotels, jumlah_hotel, urutan);

    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga per malam");
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
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga per malam");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
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
    printf("%-5s %-35s %-10s %-7s %-14s %s\n", "No", "Nama", "Bintang", "Rating", "Jenis", "Harga per malam");
    for (int i = 0; i < jumlah_hotel; i++) {
        printf("%-5d %-35s %-10c %-7.1f %-14s Rp %s\n", i + 1, hotels[i].nama_hotel, hotels[i].bintang_hotel, hotels[i].rating,
               hotels[i].jenis_penginapan == 'H' ? "Hotel" : (hotels[i].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[i].harga);
    }
}

// Binary search
int binarySearch(struct Hotel hotels[], int left, int right, char *nama_hotel) {
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(hotels[mid].nama_hotel, nama_hotel);
        if (cmp == 0)
            return mid; 
        else if (cmp < 0)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1; 
}

// Interpolation search
int interpolationSearch(struct Hotel hotels[], int n, char *nama_hotel) {
    int low = 0, high = n - 1;

    while (low <= high && strcmp(hotels[low].nama_hotel, nama_hotel) <= 0) {
        int pos = low + ((high - low) * (strcmp(nama_hotel, hotels[low].nama_hotel) -
                                            strcmp(hotels[low].nama_hotel, hotels[low + 1].nama_hotel))) /
                 (strcmp(hotels[low + 1].nama_hotel, hotels[low].nama_hotel));

        if (strcmp(hotels[pos].nama_hotel, nama_hotel) == 0)
            return pos; 

        else if (strcmp(hotels[pos].nama_hotel, nama_hotel) < 0)
            high = pos - 1;
        else
            low = pos + 1;
    }

    return -1; 
}

void searchHotelBinary(char *nama_hotel) {
    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }
    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char name[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];

    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", name, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        strcpy(hotels[jumlah_hotel].nama_hotel, name);
        hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
        hotels[jumlah_hotel].rating = rating;
        hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
        strcpy(hotels[jumlah_hotel].harga, harga);
        jumlah_hotel++;
    }

    fclose(file);

    quickSort(hotels, 0, jumlah_hotel - 1); 

    char *start = nama_hotel;
    while (*start == ' ') start++;
    char *end = start + strlen(start) - 1;
    while (end > start && *end == ' ') end--;
    *(end + 1) = '\0';

    int result = binarySearch(hotels, 0, jumlah_hotel - 1, start);
    if (result != -1) {
        printf("Hotel ditemukan:\n");
        printf("%-35s %-10c %-7.1f %-14s Rp %s\n", hotels[result].nama_hotel, hotels[result].bintang_hotel, hotels[result].rating,
               hotels[result].jenis_penginapan == 'H' ? "Hotel" : (hotels[result].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[result].harga);
    } else {
        printf("Hotel tidak ditemukan.\n");
    }
}

void searchHotelInterpolation(char *nama_hotel) {
    FILE *file = fopen(FILE_HOTEL, "r");
    if (file == NULL) {
        printf("Gagal membuka file hotel.\n");
        return;
    }
    struct Hotel hotels[MAX_TIKET];
    int jumlah_hotel = 0;

    char name[MAX_NAMA_HOTEL];
    char bintang_hotel;
    float rating;
    char jenis_penginapan;
    char harga[20];

    while (fscanf(file, "%[^#]#%c#%f#%c#%s\n", name, &bintang_hotel, &rating, &jenis_penginapan, harga) != EOF) {
        strcpy(hotels[jumlah_hotel].nama_hotel, name);
        hotels[jumlah_hotel].bintang_hotel = bintang_hotel;
        hotels[jumlah_hotel].rating = rating;
        hotels[jumlah_hotel].jenis_penginapan = jenis_penginapan;
        strcpy(hotels[jumlah_hotel].harga, harga);
        jumlah_hotel++;
    }

    fclose(file);

    quickSort(hotels, 0, jumlah_hotel - 1); 

    char *start = nama_hotel;
    while (*start == ' ') start++;
    char *end = start + strlen(start) - 1;
    while (end > start && *end == ' ') end--;
    *(end + 1) = '\0';

    int result = interpolationSearch(hotels, jumlah_hotel, start);
    if (result != -1) {
        printf("Hotel ditemukan:\n");
        printf("%-35s %-10c %-7.1f %-14s Rp %s\n", hotels[result].nama_hotel, hotels[result].bintang_hotel, hotels[result].rating,
               hotels[result].jenis_penginapan == 'H' ? "Hotel" : (hotels[result].jenis_penginapan == 'A' ? "Apartemen" : "Guest House"), hotels[result].harga);
    } else {
        printf("Hotel tidak ditemukan.\n");
    }
}

int main() {
    struct TreeNode *root = NULL;
    loadPesananToBST(&root); 

    int pilihan;
    do {
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("----------------------------------------------  book@serpong  ----------------------------------------------\n");
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("\nPilih menu:\n");
        printf("1. Pesan Tiket\n");
        printf("2. Lihat Tiket\n");
        printf("3. Batalkan Pesanan\n");
        printf("4. Lihat Hotel\n");
        printf("5. Lihat Tiket Terurut Berdasarkan Tanggal Check-in\n"); 
        printf("6. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                pesanTiket(&root);
                break;
            case 2:
                lihatTiket(root);
                break;
            case 3:
                batalkanPesan(&root);
                break;
            case 4:
                lihatHotel();
                break;
            case 5:
                lihatTiketTerurut(); 
                break;
            case 6:
                printf("Keluar dari program.\n");
                break;
            default:
                printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 6);
    freeMemoryBST(root);

    return 0;
}
