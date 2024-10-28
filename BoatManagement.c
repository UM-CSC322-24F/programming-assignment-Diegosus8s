#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
typedef enum { // Enum for Boats
    SLIP = 0,
    LAND,
    TRAILOR,
    STORAGE,
    NO_PLACE
} PlaceType;

typedef struct { //Structure for Boats
    char name[128];
    int length;
    PlaceType type;
    union {
        int slipNumber;
        char bayLetter;
        char licenseTag[10];
        int storageNumber;
    } details;
    double amountOwed;
} Boat;

Boat *boats[120] = {NULL}; // Array of Boats , up to 120 pointers

PlaceType StringToPlaceType(char *placeString) { //String to PlaceType function
    if (!strcasecmp(placeString, "slip")) return SLIP;
    if (!strcasecmp(placeString, "land")) return LAND;
    if (!strcasecmp(placeString, "trailor")) return TRAILOR;
    if (!strcasecmp(placeString, "storage")) return STORAGE;
    return NO_PLACE;
}

char *PlaceTypeToString(PlaceType place) { // PlaceType to String function
    switch (place) {
        case SLIP: return "slip";
        case LAND: return "land";
        case TRAILOR: return "trailor";
        case STORAGE: return "storage";
        default: return "no_place";
    }
}
void sortBoatsByName(Boat *boats[], int count) {
    int i, j;
    Boat *temp;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            if (strcmp(boats[j]->name, boats[j + 1]->name) > 0) {
                temp = boats[j];
                boats[j] = boats[j + 1];
                boats[j + 1] = temp;
            }
        }
    }
}


    void loadBoats(const char* filename) { // Method for loading CSV.file information
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        printf("Error: Could not open %s\n", filename);
        return;
    }
    printf("File opened successfully.\n");
    

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Boat *newBoat = malloc(sizeof(Boat));
        if (!newBoat) {
            perror("Memory allocation failed");
            fclose(file);
            return;
        }

        char typeStr[20], extraInfo[20];
        sscanf(line, "%[^,],%d,%[^,],%[^,],%lf", newBoat->name, &newBoat->length, typeStr, extraInfo, &newBoat->amountOwed);
        newBoat->type = StringToPlaceType(typeStr);

        switch (newBoat->type) {
            case SLIP: newBoat->details.slipNumber = atoi(extraInfo); break;
            case LAND: newBoat->details.bayLetter = extraInfo[0]; break;
            case TRAILOR: strncpy(newBoat->details.licenseTag, extraInfo, 9); break;
            case STORAGE: newBoat->details.storageNumber = atoi(extraInfo); break;
            default: free(newBoat); continue;
        }

        for (int i = 0; i < 120; i++) {
            if (!boats[i]) {
                boats[i] = newBoat;
                break;
            }
        }
    }
    fclose(file);
}


void saveBoats(const char* filename) {// Method to save boat data to a CSV.file
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    for (int i = 0; i < 120; i++) {
        if (boats[i]) {
            fprintf(file, "%s,%d,%s,", boats[i]->name, boats[i]->length, PlaceTypeToString(boats[i]->type));
            switch (boats[i]->type) {
                case SLIP: fprintf(file, "%d,", boats[i]->details.slipNumber); break;
                case LAND: fprintf(file, "%c,", boats[i]->details.bayLetter); break;
                case TRAILOR: fprintf(file, "%s,", boats[i]->details.licenseTag); break;
                case STORAGE: fprintf(file, "%d,", boats[i]->details.storageNumber); break;
                default: break;
            }
            fprintf(file, "%.2f\n", boats[i]->amountOwed);
        }
    }
    fclose(file);
}

void printInventory() { // Method to print boat inventory
int count = 0;
    // Compact the array to remove NULLs from the sorting process
    for (int i = 0, j = 0; i < 120; i++) {
        if (boats[i] != NULL) {
            boats[j++] = boats[i];
            count++;
        }
        if (i >= j) boats[i] = NULL; // Nullify the remaining part of the array
    }

    // Sort the boats by name if there is more than one boat
    if (count > 1) {
        sortBoatsByName(boats, count);
    }
    for (int i = 0; i < 120; i++) {
        if (boats[i]!= NULL) {
            printf("%-20s %3d' %-10s", boats[i]->name, boats[i]->length, PlaceTypeToString(boats[i]->type));
             switch (boats[i]->type) {
                case SLIP:
                    printf("    #%3d   ", boats[i]->details.slipNumber); 
                    break;
                case LAND:
                    printf("    %c      ", boats[i]->details.bayLetter); 
                    break;
                case TRAILOR:
                    printf("    %s ", boats[i]->details.licenseTag); 
                    break;
                case STORAGE:
                    printf("    #%3d   ", boats[i]->details.storageNumber); 
                    break;
                default:
                    
                    break;
            }
            printf("Owes $%-9.2f\n", boats[i]->amountOwed);
        }
    }
}

void addBoat(const char* boatData) {// Method to add a new boat
    Boat *newBoat = malloc(sizeof(Boat));
    if (!newBoat) {
        perror("Memory allocation failed");
        return;
    }

    char typeStr[20], extraInfo[20];
    sscanf(boatData, "%[^,],%d,%[^,],%[^,],%lf", newBoat->name, &newBoat->length, typeStr, extraInfo, &newBoat->amountOwed);
    newBoat->type = StringToPlaceType(typeStr);

    switch (newBoat->type) {
        case SLIP: newBoat->details.slipNumber = atoi(extraInfo); break;
        case LAND: newBoat->details.bayLetter = extraInfo[0]; break;
        case TRAILOR: strncpy(newBoat->details.licenseTag, extraInfo, 9); break;
        case STORAGE: newBoat->details.storageNumber = atoi(extraInfo); break;
        default: free(newBoat); printf("Invalid place type.\n"); return;
    }

    for (int i = 0; i < 120; i++) {
        if (!boats[i]) {
            boats[i] = newBoat;
            printf("Boat added successfully.\n");
            return;
        }
    }
    printf("No available slots to add new boat.\n");
    free(newBoat);
}

// Remove a boat by name
void removeBoat(const char* boatName) {
    for (int i = 0; i < 120; i++) {
        if (boats[i] && strcasecmp(boats[i]->name, boatName) == 0) {
            free(boats[i]);
            boats[i] = NULL;
            printf("Boat removed successfully.\n");
            return;
        }
    }
    printf("No boat with that name found.\n");
}

// Accept a payment
void acceptPayment(const char* boatName, double amount) {
    for (int i = 0; i < 120; i++) {
        if (boats[i] && strcasecmp(boats[i]->name, boatName) == 0) {
            if (amount > boats[i]->amountOwed) {
                printf("Payment exceeds amount owed.\n");
                return;
            }
            boats[i]->amountOwed -= amount;
            printf("Payment accepted. Remaining balance: $%.2f\n", boats[i]->amountOwed);
            return;
        }
    }
   
}

// Update charges for a new month
void updateCharges() {
    for (int i = 0; i < 120; i++) {
        if (boats[i]) {
            switch (boats[i]->type) {
                case SLIP: boats[i]->amountOwed += boats[i]->length * 12.5; break;
                case LAND: boats[i]->amountOwed += boats[i]->length * 14.0; break;
                case TRAILOR: boats[i]->amountOwed += boats[i]->length * 25.0; break;
                case STORAGE: boats[i]->amountOwed += boats[i]->length * 11.2; break;
                default: break;
            }
        }
    }
    printf("Monthly charges updated.\n");
}
    
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <BoatData.csv>\n", argv[0]);
        return 1;
    }

    

    printf("Attempting to load boats...\n");
    loadBoats(argv[1]);
    printf("Boats loaded successfully.\n");

    // Print welcome message
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    char option;
    char input[256];

    while (1) {
        printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it: ");
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%c", &option);

        switch (tolower(option)) {
            case 'i':
                printInventory();
                break;
            case 'a':
                printf("Please enter the boat data in CSV format: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;  // Remove newline character
                addBoat(input);
                break;
            case 'r':
                printf("Please enter the boat name: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;  // Remove newline character
                removeBoat(input);
                break;
            case 'p':
                
                {char boatName[128];
             int boatFound = 0;  // Flag to track if the boat was found

              printf("Please enter the boat name: ");
              fgets(boatName, sizeof(boatName), stdin);
              boatName[strcspn(boatName, "\n")] = 0;  // Remove newline character

              // Loop to find the boat and request payment if found
             for (int i = 0; i < 120; i++) {
             if (boats[i] && strcasecmp(boats[i]->name, boatName) == 0) {
              boatFound = 1;  // Boat found
              printf("Please enter the amount to be paid: ");
              double amount;
              fgets(input, sizeof(input), stdin);  // Reuse input buffer
              sscanf(input, "%lf", &amount);

               // Call acceptPayment with the correct boat name and amount
               acceptPayment(boatName, amount);
              break;
             }
              }

             // Only print if no boat was found
              if (!boatFound) {
              printf("No boat with that name found.\n");
             }
              break;
             }
            
                
                
            case 'm':
                updateCharges();
                break;
            case 'x':
                saveBoats(argv[1]);
                printf("Exiting the Boat Management System\n");
                return 0;
            default:
                printf("Invalid option %c\n", option);
                break;
        }
    }
    return 0;
}
