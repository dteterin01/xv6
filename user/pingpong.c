#include "kernel/types.h"
#include "user.h"

const char* ping = "ping";
const char* pong = "pong";

int
main() {
    int p2c[2], c2p[2];

    pipe(p2c);
    pipe(c2p);

    char buf[8] = {0};
    if (fork() == 0)
    {
        close(p2c[0]);
        close(c2p[1]);

        read(c2p[0], buf, sizeof(buf));
        printf("%d: received %s\n", getpid(), buf);

        write(p2c[1], ping, strlen(ping));
        
        exit(0);
    }
    else
    {
        close(p2c[1]);
        close(c2p[0]);

        write(c2p[1], pong, strlen(pong));

        read(p2c[0], buf, sizeof(buf));
        printf("%d: received %s\n", getpid(), buf);

        exit(0);
    }
}
