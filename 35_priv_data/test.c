#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
    int fd, len;
    char buffer[64];

    if (argc < 2) {
        printf("Usage: %s <text to write>\n", argv[0]);
        return 0;
    }

    fd = open("/dev/priv_data", O_RDWR);
    if (fd < 0) {
        perror("open");
        return fd;
    }

    write(fd, argv[1], strlen(argv[1]));

    printf("Press enter to continue...");
    getchar();

    len = read(fd, buffer, 64);

    printf("I got %d bytes %s\n", len, buffer);

    close(fd);
    return 0;
}
