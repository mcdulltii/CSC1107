/*
A simple C application program named CSC1107_7_user.c, to access the 
“character device” via the new read-only device driver, CSC1107_7_kernel.c in the kernel. 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <termios.h>
#include "CSC1107_7_user.h"

char* getCurrentDateTime() {
    // Get the current time
    time_t currentTime;
    time(&currentTime);

    // Convert the current time to the local time
    struct tm *localTime = localtime(&currentTime);

    // Extract the individual components of the date and time
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;
    int day = localTime->tm_mday;
    int month = localTime->tm_mon + 1;  // tm_mon is zero-based, so add 1
    int year = localTime->tm_year + 1900;  // tm_year is the number of years since 1900

    // Create a string buffer to store the formatted sentence
    char* sentence = (char*)malloc(100 * sizeof(char));
    sprintf(sentence, "It is %02d:%02d:%02d on %02dth %s %d now.", hour, minute, second, day,
            month == 1 ? "January" :
            month == 2 ? "February" :
            month == 3 ? "March" :
            month == 4 ? "April" :
            month == 5 ? "May" :
            month == 6 ? "June" :
            month == 7 ? "July" :
            month == 8 ? "August" :
            month == 9 ? "September" :
            month == 10 ? "October" :
            month == 11 ? "November" :
            "December",
            year);

    return sentence;
}

void sha512_encode(const char* input, unsigned char* output) {
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, input, strlen(input));
    SHA512_Final(output, &ctx);
}

void sha384_encode(const char* input, unsigned char* output) {
    SHA512_CTX ctx;
    SHA384_Init(&ctx);
    SHA384_Update(&ctx, input, strlen(input));
    SHA384_Final(output, &ctx);
}

void sha256_encode(const char* input, unsigned char* output) {
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, input, strlen(input));
    SHA256_Final(output, &ctx);
}

void sha1_encode(const char* input, unsigned char* output) {
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, input, strlen(input));
    SHA1_Final(output, &ctx);
}

void md5_encode(const char* input, unsigned char* output) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, input, strlen(input));
    MD5_Final(output, &ctx);
}

int main() {
    int fd, c, rtn;
    fd = open("/dev/qn1", O_RDWR);  // open the qn1 driver
    if (fd == -1)   // if fail to open the driver (the drive is not in kernel)
    {
        perror("open /dev/qn1");
        exit(EXIT_FAILURE);
    }

    int choice;

    printf("Select which hashing algorithm to use: \n");
    printf("(1) SHA-512\n");
    printf("(2) SHA-384\n");
    printf("(3) SHA-256\n");
    printf("(4) SHA-1\n");
    printf("(5) MD5\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    char* dateTime = getCurrentDateTime();

    userspace_t userspace;

    memset(&userspace.hashed_sentence, 0, sizeof(userspace.hashed_sentence));

    // Hash the sentence based on the user's choice, and
    // store the hashed sentence, hash type and digest length in userspace struct
    switch (choice) {
        case 1:
            userspace.hash_type = sha512;
            userspace.digest_length = SHA512_DIGEST_LENGTH;
            sha512_encode(dateTime, userspace.hashed_sentence);
            break;
        case 2:
            userspace.hash_type = sha384;
            userspace.digest_length = SHA384_DIGEST_LENGTH;
            sha384_encode(dateTime, userspace.hashed_sentence);
            break;
        case 3:
            userspace.hash_type = sha256;
            userspace.digest_length = SHA256_DIGEST_LENGTH;
            sha256_encode(dateTime, userspace.hashed_sentence);
            break;
        case 4:
            userspace.hash_type = sha1;
            userspace.digest_length = SHA_DIGEST_LENGTH;
            sha1_encode(dateTime, userspace.hashed_sentence);
            break;
        case 5:
            userspace.hash_type = md5;
            userspace.digest_length = MD5_DIGEST_LENGTH;
            md5_encode(dateTime, userspace.hashed_sentence);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            exit(EXIT_FAILURE);
    }

    // Store the original sentence in userspace struct
    strncpy(userspace.original_sentence, dateTime, strlen(dateTime));

    printf("\nWriting to /dev/qn1: \n");  // write info into the driver

    ssize_t bytes_written = write(fd, &userspace, sizeof(userspace_t));
    if (bytes_written < 0) {
        perror("writing /dev/qn1");
        return -1;
    }

    printf("Written %ld bytes to /dev/qn1\n\n", bytes_written);

    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);          // Get the current terminal attributes
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and local echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr); // Set the new terminal attributes

    printf("Press Enter or Space to read from /dev/qn1\n\n");

    // Wait for user to press Enter or Space
    char key;
    read(STDIN_FILENO, &key, 1);
    while (key != '\n' && key != ' ') {
        read(STDIN_FILENO, &key, 1);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); // Restore the original terminal attributes

    // Successfully open the qn1 driver
    printf("Reading from /dev/qn1: \n\n");

    hash_result_t hash_result;

    // Read the hash_result struct from the driver
    read(fd, &hash_result, sizeof(hash_result_t));

    // Print the results
    printf("Original sentence: %s\n", userspace.original_sentence);

    printf("Hashed sentence in user space: ");

    for (int i = 0; i < userspace.digest_length; i++) {
        printf("%02x", userspace.hashed_sentence[i]);
    }

    printf("\n");

    printf("Type of hashing algorithm: %s\n", 
            userspace.hash_type == sha512 ? "SHA-512" :
            userspace.hash_type == sha384 ? "SHA-384" :
            userspace.hash_type == sha256 ? "SHA-256" :
            userspace.hash_type == sha1 ? "SHA-1" :
            "MD5");

    printf("Hashed sentence in kernel space: %s");

    for (int i = 0; i < userspace.digest_length; i++) {
        printf("%02x", hash_result.hashed_sentence[i]);
    }

    printf("\n");

    printf("Received comparison result: %s\n", 
            hash_result.match ? "Equal" : "Not equal");

    exit(EXIT_SUCCESS);
}
