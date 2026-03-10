#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    char buffer[128];
    int bytes;

    fd = open("file.txt", O_RDONLY);

    if (fd == -1) {
        return 1;
    }

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes);
    }

    close(fd);

    return 0;
}
