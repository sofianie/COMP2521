////////////////////////////////////////////////////////////////////////
// COMP1521 22T2 --- Assignment 2: `bytelocker', a simple file encryptor
// <https://www.cse.unsw.edu.au/~cs1521/22T2/assignments/ass2/index.html>
//
// Written by Sofia Nie (z5416810) on 2022-07-25.
// 
// 2022-07-22   v1.2    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "bytelocker.h"


// ADD ANY EXTRA #defines HERE


char *generate_random_string(int seed);
void sort_by_count(struct text_find *files, size_t n);
void sort_by_name(char *filenames[], size_t num_files);

// ADD YOUR FUNCTION PROTOTYPES HERE
void print_perms(char filename[MAX_PATH_LEN]);
int recursive(char *curr_path, char **result_array, int index, char *filename);
void content_recursive(char *curr_path, int *array_size, char *text, struct text_find *finds);

// Some provided strings which you may find useful. Do not modify.
const char *const MSG_ERROR_FILE_STAT  = "Could not stat file\n";
const char *const MSG_ERROR_FILE_OPEN  = "Could not open file\n";
const char *const MSG_ERROR_CHANGE_DIR = "Could not change directory\n";
const char *const MSG_ERROR_DIRECTORY  =
    "%s cannot be encrypted: bytelocker does not support encrypting directories.\n";
const char *const MSG_ERROR_READ       =
    "%s cannot be encrypted: group does not have permission to read this file.\n";
const char *const MSG_ERROR_WRITE      =
    "%s cannot be encrypted: group does not have permission to write here.\n";
const char *const MSG_ERROR_SEARCH     = "Please enter a search string.\n";
const char *const MSG_ERROR_RESERVED   =
    "'.' and '..' are reserved filenames, please search for something else.\n";


//////////////////////////////////////////////
//                                          //
//              SUBSET 0                    //
//                                          //
//////////////////////////////////////////////

//
//  Read the file permissions of the current directory and print them to stdout.
//
void print_perms(char filename[MAX_PATH_LEN]) {
    struct stat s;
    stat(filename, &s);
    if (S_ISDIR(s.st_mode)) {
        printf("d");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IRUSR) {
        printf("r");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IWUSR) {
        printf("w");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IXUSR) {
        printf("x");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IRGRP) {
        printf("r");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IWGRP) {
        printf("w");
    } else {
        printf("-");
    }
    
    if (s.st_mode & S_IXGRP) {
        printf("x");
    } else {
        printf("-");
    }

    if (s.st_mode & S_IROTH) {
        printf("r");
    } else {
        printf("-");
    }

    if (s.st_mode & S_IWOTH) {
        printf("w");
    } else {
        printf("-");
    }

    if (s.st_mode & S_IXOTH) {
        printf("x");
    } else {
        printf("-");
    }
}
void show_perms(char filename[MAX_PATH_LEN]) { 
    struct stat s;

    if (stat(filename, &s) != 0) {
        printf("%s", MSG_ERROR_FILE_STAT);
        return;
    }

    printf("%s: ", filename);
    
    print_perms(filename);
    
    printf("\n");
    return;
}

//
//  Prints current working directory to stdout.
//
void print_current_directory(void) {
    char pathname[MAX_PATH_LEN];
    getcwd(pathname, sizeof pathname);
    printf("Current directory is: %s\n", pathname);
    return;
}

//
//  Changes directory to the given path.
//
void change_directory(char dest_directory[MAX_PATH_LEN]) {
    if (dest_directory[0] == '~') {
        chdir(getenv("HOME"));
        printf("Moving to %s\n", getenv("HOME"));
        return;
    }
    if (chdir(dest_directory) != 0) {
        printf("%s", MSG_ERROR_CHANGE_DIR);
        return;
    } 
    
    printf("Moving to %s\n", dest_directory);   
}

//
//  Lists the contents of the current directory to stdout.
//
void list_current_directory(void) {
    DIR *d; 
    struct dirent *dir;
    d = opendir(".");

    char *filename[MAX_NUM_FINDS];
    
    int i = 0;
    while ((dir = readdir(d)) != NULL) {
        filename[i] = malloc(strlen(dir->d_name)+1);
        strcpy(filename[i], dir->d_name);
        i++;
    }
    sort_by_name(filename, i);
    int j = 0;
    while (j < i) {
        print_perms(filename[j]);
        printf("\t");
        printf("%s\n", filename[j]);
        j++;
    }
}

//////////////////////////////////////////////
//                                          //
//              SUBSET 1                    //
//                                          //
//////////////////////////////////////////////

// Checks if a file can be encrypted 
bool test_can_encrypt(char filename[MAX_PATH_LEN]) {
    struct stat s;
    // File must exist
    if (stat(filename, &s) != 0) {
        printf(MSG_ERROR_FILE_STAT);
        return false;
    }
    // Must be a regular file
    bool Flag = S_ISREG(s.st_mode);
    if (Flag == false) {
        printf(MSG_ERROR_DIRECTORY, filename);
        return false;   
    }
    // Group must be able to read file
    if (s.st_mode & S_IRGRP) {
    } else {
        printf(MSG_ERROR_READ, filename);
        return false;
    }
    // Group must be able to write in target directory
    if (s.st_mode & S_IWGRP) {
    } else {
        printf(MSG_ERROR_WRITE, filename);
        return false;
    }
    return true;
}

// XORs each bytes of given file with 0xFF and outputs result into new file
void simple_xor_encryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;
    
    FILE *fog = fopen(filename, "rb"); 
    // copies filename and adds .xor extension
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".xor\0");
    // opens new file
    FILE *fnew = fopen(outfile, "wb");
    // get the first character
    int c = fgetc(fog); 
    int result = 0;
    while (c != EOF) {
        result = c ^ 0xFF;
        fputc(result, fnew);
        c = fgetc(fog); 
    }
    
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}

// Reverses the process of xor encryption 
void simple_xor_decryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;

    FILE *fog = fopen(filename, "rb"); 
    // copies filename and adds .dec extension
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".dec\0");
    // opens new file 
    FILE *fnew = fopen(outfile, "wb");
    // get the first character
    int c = fgetc(fog); 
    int result = 0;
    while (c != EOF) {
        result = c ^ 0xFF;
        fputc(result, fnew);
        c = fgetc(fog); 
    }
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 2                    //
//                                          //
//////////////////////////////////////////////

// Traverses entire directory tree in search for matching files and directories
int recursive(char *curr_path, char **result_array, int index, char *filename){
    DIR *dir;
    struct dirent *dp;
 
    if (!(dir = opendir(curr_path)))  
        return index;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name,".") != 0 || strcmp(dp->d_name,"..") != 0) {
            // If file contains the searched name string, store filename in array
            if (strstr(dp->d_name, filename) != NULL) { 
                result_array[index] = malloc(strlen(curr_path)+2+strlen(dp->d_name));
                strcpy(result_array[index], curr_path);
                strcat(result_array[index], "/");
                strcat(result_array[index], dp->d_name); 
                index = index + 1;
            }        
            // If file is a subdirectory, create new path name and function repeats
            if (dp->d_type == DT_DIR 
                && (strcmp(dp->d_name, ".") != 0) 
                && (strcmp(dp->d_name, "..") != 0)) {
                char *new_path = malloc(strlen(curr_path)+2+strlen(dp->d_name));
                strcpy(new_path, curr_path);
                strcat(new_path, "/");
                strcat(new_path, dp->d_name);
                index = recursive(new_path, result_array, index, filename);
                free(new_path);
            }
        } 
    }
    return index;
}

// Prints out name and permissions of files and directories that matches search name 
void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {
    // error checking
    // if user enters empty string
    if (strlen(filename) == 0) {
        printf(MSG_ERROR_SEARCH);
        return;
    } 
    // if user searches for . or ..
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
        printf(MSG_ERROR_RESERVED);
        return;
    }
    // counter counts array size
    int counter = 0;
    char *matched_files[MAX_NUM_FINDS];
    counter = recursive(".", matched_files, counter, filename);
    sort_by_name(matched_files, counter);
    int j = 0;
    while (j < counter) {
        print_perms(matched_files[j]);
        printf("\t");
        printf("%s\n",matched_files[j]);
        j++;
    }
}

// Traverses dir tree, enters file and scans for text that matches user entered text
void content_recursive(char *curr_path, int *array_size, char *text, struct text_find *finds) {
    DIR *dir;
    struct dirent *direc;
    if (!(dir = opendir(curr_path))) 
        return;

    while ((direc = readdir(dir)) != NULL) {
        // If it's a file then open and scan it for matching text
        if (direc->d_type == DT_REG) {
            char fname[MAX_PATH_LEN];
            strcpy(fname, curr_path);
            strcat(fname, "/");
            strcat(fname, direc->d_name);
            FILE *f = fopen(fname, "r");
            if (f == NULL) continue;
            char buffer[MAX_LINE_LEN];
            int counter = 0;
            char *position;
            int index;
            // How many matches i can find within a file
            while (fgets(buffer, MAX_LINE_LEN, f) != NULL) { 
                index = 0;
                // Counting matches with the substring
                while ((position = strstr(buffer + index, text)) != NULL) { 
                    index = (position - buffer) + 1;
                    counter++;
                }
            } 
            // If theres a match with the substring, store into struct array
            if (counter != 0) {
                strcpy(finds[*array_size].path, curr_path);
                strcat(finds[*array_size].path, "/");
                strcat(finds[*array_size].path, direc->d_name);
                finds[*array_size].count = counter;
                (*array_size)++;
            }
            fclose(f);
        // If we found a subdirectory, enter and repeat the function
        } else if (direc->d_type == DT_DIR && (strcmp(direc->d_name, ".") != 0) && (strcmp(direc->d_name, "..") != 0)) {// FILE IS A SUBDIR
            char *new_path = malloc(MAX_PATH_LEN * sizeof(char));
            strcpy(new_path, curr_path);
            strcat(new_path, "/");
            strcat(new_path, direc->d_name);
            content_recursive(new_path, array_size, text, finds);
            free(new_path);
        }
        
    }
  
}

// Prints out name of files that has matches and number of matches c
void search_by_text(char text[MAX_SEARCH_LENGTH]) {
    // error checking
    // if user enters empty string
    if (strlen(text) == 0) {
        printf(MSG_ERROR_SEARCH);
        return;
    } 
    int array_size = 0;
    struct text_find text_finds[MAX_NUM_FINDS];
    content_recursive(".", &array_size, text, text_finds);
    sort_by_count(text_finds, array_size);
    int i = 0;
    while (i < array_size) {
        printf("%d:\t%s\n", text_finds[i].count, text_finds[i].path);
        i++;
    }
}

//////////////////////////////////////////////
//                                          //
//              SUBSET 3                    //
//                                          //
//////////////////////////////////////////////

// Encrypts content of given file with user entered password and outputs to new file
void electronic_codebook_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;
    FILE *fog = fopen(filename, "rb+"); 
    // Gets file size
    fseek(fog, 0L, SEEK_END);
    int size = ftell(fog);
    int filesize = size;
    rewind(fog);

    if (filesize % 16 != 0) {
        // Rounds up to the next multiple of 16
        filesize = ((filesize / 16) + 1) * 16;
    }
    // Copies filename, adds .ecb extension and create new file
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".ecb\0");
    
    FILE *fnew = fopen(outfile, "wb+");

    char buffer[CIPHER_BLOCK_SIZE + 1];
       
    int read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
    if (read < CIPHER_BLOCK_SIZE) {
        while (read < CIPHER_BLOCK_SIZE) {
            // Padding if fread did not read in 16 bytes
            buffer[read] = '\x00';
            read++;
        }
    }
    // Read until all file content has been encrypted
    int x = 0;
    while (x < filesize) {
        int i = 0;
        // Calling helper function to encrypt byte by byte
        while (i < CIPHER_BLOCK_SIZE) {
            buffer[i] = *shift_encrypt(&buffer[i], &password[i]);             
            i++;
        }
        // Output encrypted file content to new .ecb file
        int counter = 0;
        while (counter < CIPHER_BLOCK_SIZE) {
            int bit = buffer[counter];
            fputc(bit, fnew);
            counter++;
        }
        // Read in the next 16 bytes and padding if needed
        read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
        if (read < CIPHER_BLOCK_SIZE) {
            while (read < CIPHER_BLOCK_SIZE) {
                buffer[read] = '\x00';
                read++;
            }
        }
        x = x + CIPHER_BLOCK_SIZE;
    }
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}

// Inverse of decryption and outputs decrypted content to new file with .dec extension
void electronic_codebook_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;
    FILE *fog = fopen(filename, "rb+"); 
    // Gets file size
    fseek(fog, 0L, SEEK_END);
    int size = ftell(fog);
    int filesize = size;
    rewind(fog);

    if (filesize % 16 != 0) {
        // Rounds up to the next multiple of 16
        filesize = ((filesize / 16) + 1) * 16;
    }
    // Copies filename, adds .dec extension and create new file
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".dec\0");
    
    FILE *fnew = fopen(outfile, "wb+");

    char buffer[CIPHER_BLOCK_SIZE + 1];
       
    int read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
    // Read until all file content has been decrypted
    int x = 0;
    while (x < filesize) {
        int i = 0;
        while (i < CIPHER_BLOCK_SIZE) { 
            // Calling helper function to decrypt byte by byte
            buffer[i] = *shift_decrypt(&buffer[i], &password[i]);
            i++;
        } 
        // Output decrypted file content to new .dec fiile
        int counter = 0;
        while (counter < CIPHER_BLOCK_SIZE) {
            int bit = buffer[counter];
            fputc(bit, fnew);
            counter++;
        }
        // Read in the next 16 bytes and padding if needed
        read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
        if (read < CIPHER_BLOCK_SIZE) {
            while (read < CIPHER_BLOCK_SIZE) {
                buffer[read] = '\x00';
                read++;
            }
        }
        x = x + CIPHER_BLOCK_SIZE;
    }
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}

// Cyclic left shifts characters of plaintext by character of password
char *shift_encrypt(char *plaintext, char *password) {
    uint8_t bit = *plaintext;
    int result = 0;
    int shift = *password;
    if (shift % 8 == 0) {
        result = bit;
    } else {
        shift = shift % 8;
        result = bit << shift;
        result = result | (bit >> (8 -(shift)));
    }
    *plaintext = result;
    return plaintext;
}

// Cyclic right shifts characters of ciphertext by character of password
char *shift_decrypt(char *ciphertext, char *password) {
    uint8_t bit = *ciphertext;
    int result = 0;
    int shift = *password;
    if (shift % 8 == 0) {
        result = bit;
    } else {
        shift = shift % 8;
        result = bit >> shift;
        result = result | (bit << (8 -(shift)));
    }
    *ciphertext = result;
    return ciphertext;
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 4                    //
//                                          //
//////////////////////////////////////////////

// sidenote: feels like i made significant progress on this subset but it fails to pass the last 2 autotests, could this be manually checked over?

// Read content of file, encrypt with password and outputs to new .cbc file
void cyclic_block_shift_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;
    // Generate random string and made that our initialisation vector
    char init_vector[RAND_STR_LEN + 1];
   
    memcpy(init_vector, generate_random_string(1), RAND_STR_LEN);
    init_vector[RAND_STR_LEN] = '\0';

    FILE *fog = fopen(filename, "rb");
    // Find size of file
    fseek(fog, 0L, SEEK_END);
    int filesize = ftell(fog);
    rewind(fog);
    // Checking if filesize is a multiple of 16, if not, round up to next multiple of 16
    if (filesize % 16 != 0) {
        filesize = ((filesize / 16) + 1) * 16;
    }
    // Create new encrypted file
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".cbc\0");

    FILE *fnew = fopen(outfile, "wb+");

    // Read in the first 16 bytes of the file and padding if needed
    char buffer[CIPHER_BLOCK_SIZE + 1];
    int read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
    if (read < CIPHER_BLOCK_SIZE) {
        while (read < CIPHER_BLOCK_SIZE) {
            buffer[read] = '\x00';
            read++;
        }
    }
    
    // XOR the first 16 bytes of the file with the init vector
    int counter = 0;
    while (counter < RAND_STR_LEN) {
        init_vector[counter] = buffer[counter] ^ init_vector[counter];
        counter++;
    }
    // Keep track of how much bytes from the file has been read
    int scan_track = 0;
    // Keep encrypting until all of file has been read
    while (scan_track < filesize) {
        int i = 0;
        while (i < RAND_STR_LEN) {
            init_vector[i] = *shift_encrypt(&init_vector[i], &password[i]);
            i++;
        }
        int x = 0;
        while (x < RAND_STR_LEN) {
            int bit = init_vector[x];
            fputc(bit, fnew);
            x++;
        }
        int index = 0;
        while (index < RAND_STR_LEN) {
            read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
            if (read < CIPHER_BLOCK_SIZE) {
                while (read < CIPHER_BLOCK_SIZE) {
                    buffer[read] = '\x00';
                    read++;
                }
            }
            init_vector[index] = buffer[index] ^ init_vector[index];
            index++;
        }
        scan_track = scan_track + CIPHER_BLOCK_SIZE;
    }
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}
// Read content of file, encrypt with password and outputs to new .dec file
void cyclic_block_shift_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;
    
    char init_vector[RAND_STR_LEN + 1];
   
    memcpy(init_vector, generate_random_string(1), RAND_STR_LEN);
    init_vector[RAND_STR_LEN] = '\0';
 
    FILE *fog = fopen(filename, "rb");
    // Find size of file
    fseek(fog, 0L, SEEK_END);
    int filesize = ftell(fog);
    rewind(fog);
    // Checking if filesize is a multiple of 16, if not, round up to next multiple of 16
    if (filesize % 16 != 0) {
        filesize = ((filesize / 16) + 1) * 16;
    }
    // Create new decrypted file
    int len = strlen(filename);
    char *outfile = malloc(len + 4 + 1);
    strcpy(outfile, filename);
    strcat(outfile, ".dec\0");

    FILE *fnew = fopen(outfile, "wb+");

    // Read in the first 16 bytes of the file 
    char buffer[CIPHER_BLOCK_SIZE + 1];
    char cipherbytes[CIPHER_BLOCK_SIZE + 1];
    int read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
    // Copy 16 characters from buffer to cipherbytes
    memcpy(cipherbytes, buffer, CIPHER_BLOCK_SIZE);
    
    // Keep track of how much bytes from the file has been read
    int scan_track = 0;
    // Keep decrypting until all of file has been read
    while (scan_track < filesize) {
        int i = 0;
        while (i < RAND_STR_LEN) {
            buffer[i] = *shift_decrypt(&buffer[i], &password[i]); 
            i++;
        }
        // XOR the result with init_vector and output as the decrypted text
        int counter = 0;
        while (counter < RAND_STR_LEN) {
            buffer[counter] = buffer[counter] ^ init_vector[counter];
            int bit = buffer[counter];
            fputc(bit, fnew);
            counter++;
        }
        // Previous 16 bytes of file is now the init_vector
        memcpy(init_vector, cipherbytes, CIPHER_BLOCK_SIZE);
        read = fread(buffer, 1, CIPHER_BLOCK_SIZE, fog);
        if (read < CIPHER_BLOCK_SIZE) {
            while (read < CIPHER_BLOCK_SIZE) {
                buffer[read] = '\x00';
                read++;
            }
        }
        memcpy(cipherbytes, buffer, CIPHER_BLOCK_SIZE);    
        scan_track = scan_track + CIPHER_BLOCK_SIZE;
    }
    fclose(fnew);
    fclose(fog); 
    free(outfile);
}


// PROVIDED FUNCTIONS, DO NOT MODIFY

// Generates a random string of length RAND_STR_LEN.
// Requires a seed for the random number generator.
// The same seed will always generate the same string.
// The string contains only lowercase + uppercase letters,
// and digits 0 through 9.
// The string is returned in heap-allocated memory,
// and must be freed by the caller.
char *generate_random_string(int seed) {
    if (seed != 0) {
        srand(seed);
    }

    char *alpha_num_str =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";

    char *random_str = malloc(RAND_STR_LEN);

    for (int i = 0; i < RAND_STR_LEN; i++) {
        random_str[i] = alpha_num_str[rand() % (strlen(alpha_num_str) - 1)];
    }

    return random_str;
}

// Sorts the given array (in-place) of files with
// associated counts into descending order of counts.
// You must provide the size of the array as argument `n`.
void sort_by_count(struct text_find *files, size_t n) {
    if (n == 0 || n == 1) return;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (files[j].count < files[j + 1].count) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            } else if (files[j].count == files[j + 1].count && strcmp(files[j].path, files[j + 1].path) > 0) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}

// Sorts the given array (in-place) of strings alphabetically.
// You must provide the size of the array as argument `n`.
void sort_by_name(char *filenames[], size_t num_filenames) {
    if (num_filenames == 0 || num_filenames == 1) return;
    for (int i = 0; i < num_filenames - 1; i++) {
        for (int j = 0; j < num_filenames - i - 1; j++) {
            if (strcmp(filenames[j], filenames[j + 1]) > 0) {
                char *temp = filenames[j];
                filenames[j] = filenames[j + 1];
                filenames[j + 1] = temp;
            }
        }
    }
}
