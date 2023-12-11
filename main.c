#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define CLEAR "cls" // buat windows
#define PAUSE "pause"
#else
#define PAUSE "read -n1 -r -p \"Press any key to continue...\" key"
#define CLEAR "clear" // buat Unix/Linux
#endif


#define DEVICE_MAX 30
#define CATEGORY_MAX 20
#define PERIOD_MAX 5
typedef unsigned int uint;

// Struct Declarations
typedef struct Category Category;
typedef struct Period Period;
typedef struct Device Device;


typedef enum bool
{
    false, true
} bool;


struct Category {
    char name[16]; // Nama (maksimal 16 karakter)
    uint id;       // identifikasi
    float totalConsumption;
    Device *devices[5]; // Device pada kategori ini
    uint device_num;
};

struct Period {
    char begin[6]; // Jam mulai
    char end[6];   // Jam selesai
};

struct Device {
    Category *category;
    Period periods[PERIOD_MAX];    // Maksimal 5 periode penggunaan
    uint activeMinutes; // Menit aktif
    uint period_num;    // Jumlah periode penggunaan
    float power;        // Watt
    float energyDaily;  // Total konsumsi kwh
};

// Function Declarations
void printDevice(Device *device);
void addDevice(Device devices[], uint *deviceCount, Category categories[], uint *categoryCount);
void addPeriod(Device *device);
bool checkTimeFormat(char time[]);
void addCategory(Category categories[], uint *categoryCount);
uint parsePeriod(Period *period);
void updateConsumption(Device *device);
void printTips(uint id);

void printDevice(Device *device)
{
    printf("Konsumsi Daya: %.2f watt\n", device->power);
    printf("Periode Penggunaan:\n");
    uint i;
    for (i = 0; i < device->period_num; ++i)
    {
        printf("* %s - %s\n", device->periods[i].begin, device->periods[i].end);
    }
    printf("Waktu Aktif Harian: %u menit\n", device->activeMinutes);
    printf("Konsumsi Harian: %.2f kwh\n", device->energyDaily);
    printf("-------------\n");
}

void addDevice(Device devices[], uint *deviceCount, Category categories[], uint *categoryCount)
{
    system(CLEAR);
    uint categoryChoice;
    uint i;

    if (*deviceCount >= DEVICE_MAX)
    {
        printf("PERANGKAT PENUH!\n");
        return;
    }

    printf("--------\nPilih kategori perangkat\n------------\n");
    printf("Jumlah Perangkat Sekarang: %u\n", *deviceCount);
    printf("Jumlah Kategori Tersedia: %u\n", *categoryCount);
    printf("--------------------\n");
    for (i = 0; i < *categoryCount; ++i) // Kategori yang sudah ada
    {
        printf("%d. %s\n", i + 1, categories[i].name);
    }
    printf("---------------\n");
    printf("%d. Tambah Kategori Baru\n", i + 1);
    printf("Pilih Opsi: ");
    scanf("%u", &categoryChoice);

    if (categoryChoice == i + 1)
    {
        if (*categoryCount >= CATEGORY_MAX)
        {
            printf("KATEGORI PENUH!\n");
        }
        else
        {
            addCategory(categories, categoryCount);
        }
        addDevice(devices, deviceCount, categories, categoryCount);
        return;
    }
    else if (categoryChoice > i + 1)
    {
        printf("BUKAN OPSI!\n");
        system(PAUSE);
    }
    categoryChoice--;

    Device *currentDevice = &devices[*deviceCount];

    currentDevice->category = &categories[categoryChoice];
    categories[categoryChoice].devices[categories[categoryChoice].device_num] = currentDevice;
    categories[categoryChoice].device_num++;

    system(CLEAR);
    printf("--------------\nMenambahkan Perangkat ke Kategori %s\n-----------\n", categories[categoryChoice].name);

    printf("Daya Perangkat (WATT): ");
    scanf("%f", &(currentDevice->power));

    currentDevice->period_num = 0;
    addPeriod(currentDevice);

    do {
        system(CLEAR);
        printf("=Device Baru=\n");
        printf("----------\nKategori: %s\n----------\n", currentDevice->category->name);
        printDevice(currentDevice);
        printf("=================\n");
        printf("1. Tambah Periode\n");
        printf("2. Selesai\n");
        printf("Pilih Opsi: ");
        scanf("%u", &categoryChoice);

        if (categoryChoice == 1)
        {
            if (currentDevice->period_num >= PERIOD_MAX)
            {
                printf("PERIODE PENUH!\n");
            }
            else
            {
                addPeriod(currentDevice);
            }
        }
    } while (categoryChoice == 1);

    (*deviceCount)++;
}

void addPeriod(Device *device)
{
    system(CLEAR);
    printf("----------\nTambah Periode Penggunaan\n");
    printf("[Format Input JJ.MM]\n----------\n");
    do {
        printf("Waktu Mulai: ");
        scanf("%s", device->periods[device->period_num].begin);
    } while (!checkTimeFormat(device->periods[device->period_num].begin));

    do {
        printf("Waktu Selesai: ");
        scanf("%s", device->periods[device->period_num].end);
    } while (!checkTimeFormat(device->periods[device->period_num].end));

    device->activeMinutes += parsePeriod(&device->periods[device->period_num]);
    device->period_num++;
    updateConsumption(device);
    device->category->totalConsumption += device->energyDaily;
}

bool checkTimeFormat(char time[])
{
    if (time[2] != '.')
    {
        printf("FORMAT WAKTU SALAH!\n");
        return false;
    }
    uint i;
    for (i = 0; i < 4; ++i)
    {
        if ((time[i] < '0' || time[i] > '9') & time[i] != '.')
        {
            printf("FORMAT WAKTU SALAH!\n");
            return false;
        }
    }
    return true;
}

void addCategory(Category categories[], uint *categoryCount)
{
    system(CLEAR);
    printf("----------\nTambah Kategori Baru\n----------\n");
    printf("Nama Kategori: ");
    scanf("%s", categories[*categoryCount].name);
    categories[*categoryCount].id = *categoryCount;
    categories[*categoryCount].totalConsumption = 0;
    categories[*categoryCount].device_num = 0;
    (*categoryCount)++;
}

void addCategoryManual(Category categories[], uint *categoryCount, char name[])
{
    strcpy(categories[*categoryCount].name, name);
    categories[*categoryCount].id = *categoryCount;
    categories[*categoryCount].totalConsumption = 0;
    categories[*categoryCount].device_num = 0;
    (*categoryCount)++;
}

uint parsePeriod(Period *period) // Mengubah periode menjadi menit
{
    // Parsing (10 * puluhan jam + satuan jam) * 60 + (puluhan menit) * 10 +
    // satuan menit
    if (strcmp(period->begin, period->end) == 0)
    {
        return 1440;
    }
    uint begin_minutes = (period->begin[0] * 10 + period->begin[1]) * 60 + (period->begin[3] * 10 + period->begin[4]);
    uint end_minutes = (period->end[0] * 10 + period->end[1]) * 60 + (period->end[3] * 10 + period->end[4]);

    return (end_minutes - begin_minutes + 1440) % 1440; // Operasi modulo, misalkan 17.00 - 16.00 berarti 23 jam.
                // Menit/hari: 1440
}

void updateConsumption(Device *device) 
{
    device->activeMinutes = 0; // Mulai dari nol

    uint i;
    for (i = 0; i < device->period_num; ++i) {
        device->activeMinutes += parsePeriod(&device->periods[i]);
    }
    device->energyDaily = device->activeMinutes * device->power / 6000; // kwh
}

void printAllDevices(Category *categories, uint *categoryCount)
{
    system(CLEAR);
    uint i;
    for (i = 0; i < *categoryCount; ++i)
    {
        if (categories[i].device_num == 0)
        {
            continue;
        }

        printf("===========\n");
        printf("Kategori: %s\n", categories[i].name);
        printf("------------------\n");
        uint j;
        for (j = 0; j < categories[i].device_num; ++j)
        {
            printf("Device %d:\n", j + 1);
            printDevice(categories[i].devices[j]);
        }
        printf("Total Konsumsi Harian: %.2f kwh\n", categories[i].totalConsumption);
        printf("========\n");
    }
    system(PAUSE);
}

void printLogo() 
{
    printf("  _      __     __  __ _      __     __      __ \n");
    printf(" | | /| / /__ _/ /_/ /| | /| / /__ _/ /_____/ / \n");
    printf(" | |/ |/ / _ `/ __/ __/ |/ |/ / _ `/ __/ __/ _ \\ \n");
    printf(" |__/|__/\\__,_/\\__/\\__/|__/|__/\\__,___/\\__/\\_//_/ \n");
}

void help() 
{
    system(CLEAR);
    printf("\n\t|=====================================================================================|");
    printf("\n\t|------------------------------------------HELP---------------------------------------|");
    printf("\n\t|=====================================================================================|");
    printf("\n\t| 1. Tambahkan Device: Menambahkan perangkat baru ke dalam sistem.                    |");
    printf("\n\t| 2. Tampilkan Device: Menampilkan daftar perangkat yang sudah ditambahkan.           |");
    printf("\n\t| 3. Statistik: Melakukan analisis terhadap data perangkat.                           |");
    printf("\n\t| 4. Help: Menampilkan menu bantuan.                                                  |");
    printf("\n\t| 5. Exit: Keluar dari program.                                                       |");
    printf("\n\t|=====================================================================================|\n");
    system("pause");
}

uint getMenu() {
    system(CLEAR);
    printLogo();
    printf("\n\n");
    printf("|=================================|\n");
    printf("|              MENU               |\n");
    printf("|=================================|\n");
    printf("| 1. Tambahkan Device             |\n");
    printf("| 2. Tampilkan Device             |\n");
    printf("| 3. Statistik                    |\n");
    printf("| 4. Help                         |\n");
    printf("| 5. Exit                         |\n");
    printf("|=================================|\n");
    
    uint menuInput;
    printf("\nMasukkan input (1 - 6) : ");
    scanf("%d", &menuInput);
    return menuInput;
}

#define HISTOGRAM_SCALE 1
void printHistogram(Category *categories[], uint categoryCount) {
    system(CLEAR);
    printf("=========HISTOGRAM=========\n");
    printf("Kategori Terurut Dari Konsumsi Daya Terbesar\n");
    int i, j;
    for (i = 0; i < categoryCount; i++) {
        printf("\n%d. %s : ", i + 1, categories[i]->name);
        uint histogramLength = categories[i]->totalConsumption * HISTOGRAM_SCALE;
        for (j = 0; j < histogramLength; j++) {
            printf("#");
        }
    }
    printf("\n-------------\n");
    printf("Kategori dengan konsumsi tertinggi adalah %s (id: %u) sebesar %.2f kwh\n", categories[0]->name, categories[0]->id, categories[0]->totalConsumption);
}

void swap(Category **a, Category **b)
{
    Category *temp = *a;
    *a = *b;
    *b = temp;
}

bool comp(Category *a, Category *b)
{
    return (a->totalConsumption) > (b->totalConsumption);
}

void bubbleSort(Category *array[], int length, bool (*comp)(Category*, Category*))
{
    int i, j;
    bool swapped;   
    for (i = 0; i < length - 1; i++)
    {
        swapped = false;
        for (j = 0; j < length - i - 1; j++)
        {
            if (!comp(array[j], array[j + 1]))
            {
                swap(&array[j], &array[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
        {
            break;
        }
    }
}

void showStats(Category *sort_container[], uint categoryCount)
{
    bubbleSort(sort_container, categoryCount, comp);
    printHistogram(sort_container, categoryCount);
    printTips(sort_container[0]->id);
    system(PAUSE);
}

void defaultCategories(Category categories[], uint *categoryCount)
{
    addCategoryManual(categories, categoryCount, "Kipas");
    addCategoryManual(categories, categoryCount, "AC");
    addCategoryManual(categories, categoryCount, "TV");
    addCategoryManual(categories, categoryCount, "Lampu");
}

void printTips(uint id)
{
    switch(id)
    {
        case 0:
            printf("Kalau Dingin Ga usah pake kipas!!\n");
            break;
        case 1:
            printf("Matikan jika tidak digunakan\n");
        default:
            printf("Matikan jika tidak digunakan\n");
    }
}

int main(void) 
{
    Device devices[DEVICE_MAX];
    Category categories[CATEGORY_MAX];
    Category *sort_container[CATEGORY_MAX];

    uint i;
    for (i = 0; i < CATEGORY_MAX; ++i)
    {
        sort_container[i] = &categories[i];
    }
    uint deviceCount = 0;
    uint categoryCount = 0;

    defaultCategories(categories, &categoryCount);


    uint menuInput;
    do {
        menuInput = getMenu();  
        switch (menuInput) 
        {
            case 1:
                addDevice(devices, &deviceCount, categories, &categoryCount);
                break;
            case 2:
                if (deviceCount <= 0)
                {
                    printf("PERANGKAT KOSONG!\n");
                    system(PAUSE);
                    break;
                }
                printAllDevices(categories, &categoryCount);
                break;
            case 3:
                if (deviceCount <= 0)
                {
                    printf("PERANGKAT KOSONG!\n");
                    system(PAUSE);
                    break;
                }
                showStats(sort_container, categoryCount);
                break;
            case 4:
                help();
                break;
            case 5:
                break;
            default:
            printf("Input tidak valid!\n");
        }
    } while (menuInput != 6);

    return 0;
}

// Parkiran Kursor: [][][][]