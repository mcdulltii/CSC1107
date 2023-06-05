/*
A simple C application program named chardevclient.c, to access the 
“character device” via the new read-only device driver, chardev.c in the kernel. 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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
    sprintf(sentence, "It is %02d:%02d:%02d on %02dth %s %d now.\n", hour, minute, second, day,
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

int main() {
    int fd, c, rtn;
    fd = open("/dev/qn1", O_RDWR);  // open the qn1 driver
    if (fd == -1)   // if fail to open the driver (the drive is not in kernel)
    {
        perror("open /dev/qn1");
        exit(EXIT_FAILURE);
    }


    printf("Writing to /dev/qn1: \n");  // write info into the driver

    char* dateTime = getCurrentDateTime();
    printf("%s", dateTime);

    ssize_t bytes_written = write(fd, dateTime, strlen(dateTime));
    if (bytes_written < 0) {
        perror("writing /dev/qn1");
        return -1;
    }


    // Successfully open the qn1 driver
    printf("Reading from /dev/qn1: \n");
    while ((rtn = read(fd, &c, 1)) > 0) // read info from the driver
    {
        printf("%c", c);
    }
    if (rtn == -1)  // fail to ready info
    {
        perror("reading /dev/qn1");
    } 
    else 
    {
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}