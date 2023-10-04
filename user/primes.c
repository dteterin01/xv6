#include "kernel/types.h"
#include "user/user.h"

void
primes_pipes(int lpipe[]) 
{
    int primes[50];
    char sym[1];
    int count = 0;
    while (read(lpipe[0], sym, sizeof(sym)))
    {
        primes[count++] = sym[0];
    }
    close(lpipe[0]);

    if (count == 0) return;

    int rpipe[2];
    pipe(rpipe);

    int first = primes[0];

    printf("prime %d\n", first);
    for (int i = 1; i < count; i++) 
    {
        if (primes[i] % first != 0)
        {
            char number = primes[i];
            write(rpipe[1], &number, 1);   
        }
    }
    close(rpipe[1]);

    if (fork() == 0)
    {
        primes_pipes(rpipe);
    }
}

int
main(void)
{
    int lpipe[2];
    pipe(lpipe);

    for (int i = 2; i <= 35; i++)
    {
        char number = i;
        write(lpipe[1], &number, 1);  
    }
    close(lpipe[1]);
    
    primes_pipes(lpipe);
    
    wait((int*) 0);
    exit(0);
}