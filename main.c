#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define CLEAR "cls" // buat windows
#else
#define CLEAR "clear" // buat Unix/Linux
#endif

#define DEVICE_MAX 30
#define CATEGORY_MAX 20
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
    float *totalConsumption;
    Device *devices[5]; // Device pada kategori ini
    uint device_num;
};

struct Period {
    char begin[6]; // Jam mulai
    char end[6];   // Jam selesai
};

struct Device {
    Category *category;
    Period periods[5];    // Maksimal 5 periode penggunaan
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

void printDevice(Device *device)
{
    printf("Konsumsi Daya: %.2f watt\n", device->power);
    printf("Periode Penggunaan:\n");
    uint i;
    for (i = 0; i < device->period_num; ++i)
    {
        printf("O %s - %s\n", device->periods[i].begin, device->periods[i].end);
    }
    printf("Konsumsi Harian: %.2f kwh\n\n", device->energyDaily);
}

void addDevice(Device devices[], uint *deviceCount, Category categories[], uint *categoryCount)
{
    printf("Jumlah Perangkat Sekarang: %u\n", *deviceCount);
    uint categoryChoice;
    uint i;

    if (*deviceCount >= DEVICE_MAX)
    {
        printf("PERANGKAT PENUH!\n");
        return;
    }

    printf("Pilih kategori perangkat\n");
    printf("Jumlah Kategori Sekarang: %u\n", *categoryCount);
    for (i = 0; i < *categoryCount; ++i) // Kategori yang sudah ada
    {
        printf("%d. %s\n", i + 1, categories[i].name);
    }

    printf("%d. Tambah Kategori Baru\n", i + 1);
    printf("%d. Hapus Kategori\n", i + 2);
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
    categoryChoice--;

    Device *currentDevice = &devices[*deviceCount];

    currentDevice->category = &categories[categoryChoice];
    categories[categoryChoice].devices[categories[categoryChoice].device_num] = currentDevice;
    categories[categoryChoice].device_num++;

    printf("Jumlah Perangkat pada Kategori %s: %u\n", categories[categoryChoice].name, categories[categoryChoice].device_num);

    printf("Daya Perangkat: ");
    scanf("%f", &(currentDevice->power));

    currentDevice->period_num = 0;
    addPeriod(currentDevice);

    do {
        printf("Kategori: %s\n", currentDevice->category->name);
        printDevice(currentDevice);
        printf("1. Tambah Periode\n");
        printf("2. Selesai\n");
        scanf("%u", &categoryChoice);

        if (categoryChoice == 1)
        {
            addPeriod(currentDevice);
        }
    } while (categoryChoice == 1);

    (*deviceCount)++;
}

void addPeriod(Device *device)
{
    printf("Tambah Periode Baru\n");
    printf("Format Input: JJ.MM\n");
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
}

bool checkTimeFormat(char time[])
{
    if (time[2] != '.')
    {
        return false;
    }
    uint i;
    for (i = 0; i < 4; ++i)
    {
        if ((time[i] < '0' || time[i] > '9') & time[i] != '.')
        {
            return false;
        }
    }
    return true;
}

void addCategory(Category categories[], uint *categoryCount)
{
    printf("Tambah Kategori Baru\n");
    printf("Nama Kategori: ");
    scanf("%s", categories[*categoryCount].name);
    categories[*categoryCount].id = *categoryCount;
    categories[*categoryCount].totalConsumption = (float*)malloc(4);
    categories[*categoryCount].device_num = 0;
    (*categoryCount)++;
}

void addCategoryManual(Category categories[], uint *categoryCount, char name[])
{
    strcpy(categories[*categoryCount].name, name);
    categories[*categoryCount].id = *categoryCount;
    categories[*categoryCount].totalConsumption = (float*)malloc(4);
    categories[*categoryCount].device_num = 0;
    (*categoryCount)++;
}

uint parsePeriod(Period *period) // Mengubah periode menjadi menit
{
    // Parsing (10 * puluhan jam + satuan jam) * 60 + (puluhan menit) * 10 +
    // satuan menit
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
    uint i;
    printf("===========\n");
    for (i = 0; i < *categoryCount; ++i)
    {
        printf("Kategori: %s\n", categories[i].name);
        printf("------------------\n");
        uint j;
        for (j = 0; j < categories[i].device_num; ++j)
        {
            printf("Device %d:\n", i + 1);
            printDevice(categories[i].devices[j]);
        }
        printf("Total Konsumsi Harian: %.2f\n", *(categories[i].totalConsumption));
        printf("========\n");
    }
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
    printf("\n\t|=====================================================================================|");
    printf("\n\t|------------------------------------------HELP---------------------------------------|");
    printf("\n\t|=====================================================================================|");
    printf("\n\t| 1. Tambahkan Device: Menambahkan perangkat baru ke dalam sistem.                    |");
    printf("\n\t| 2. Tampilkan Device: Menampilkan daftar perangkat yang sudah ditambahkan.           |");
    printf("\n\t| 3. Hapus Device: Menghapus perangkat dari sistem.                                   |");
    printf("\n\t| 4. Statistik: Melakukan analisis terhadap data perangkat.                           |");
    printf("\n\t| 5. Help: Menampilkan menu bantuan.                                                  |");
    printf("\n\t| 6. Exit: Keluar dari program.                                                       |");
    printf("\n\t|=====================================================================================|\n");
}

uint getMenu() {
    printLogo();
    printf("\n\n");
    printf("|=================================|\n");
    printf("|              MENU               |\n");
    printf("|=================================|\n");
    printf("| 1. Tambahkan Device             |\n");
    printf("| 2. Tampilkan Device             |\n");
    printf("| 3. Hapus Device                 |\n");
    printf("| 4. Analisis                     |\n");
    printf("| 5. Help                         |\n");
    printf("| 6. Exit                         |\n");
    printf("|=================================|\n");
    
    uint menuInput;
    printf("\nMasukkan input (1 - 6) : ");
    scanf("%d", &menuInput);
    return menuInput;
}

void defaultCategories(Category categories[], uint *categoryCount)
{
    addCategoryManual(categories, categoryCount, "Kipas");
    addCategoryManual(categories, categoryCount, "AC");
    addCategoryManual(categories, categoryCount, "TV");
    addCategoryManual(categories, categoryCount, "Lampu");
    addCategoryManual(categories, categoryCount, "Vibrator");
}

int main(void) 
{
    Device devices[50];
    Category categories[20];
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
                printAllDevices(categories, &categoryCount);
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                help();
                break;
            case 6:
                break;
            default:
            printf("Input tidak valid!\n");
        }
    } while (menuInput != 6);

    return 0;
}

// Parkiran Kursor: [][][][]