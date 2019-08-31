#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int cycle = 1;
void SigintHandler(int sig)
{
    cycle = 0;
    printf("\nServer out\n");
    exit(0);
}

int main()
{
    struct sigaction sigint;
    sigint.sa_handler = SigintHandler;
    sigint.sa_flags = 0;
    sigemptyset(&sigint.sa_mask);
    sigaddset(&sigint.sa_mask, SIGINT);
    sigaction(SIGINT, &sigint, 0);
    char buf[16];
    char buf2[] = "priv\n";
    int sock;
    struct sockaddr_in addr, child;
    socklen_t size = sizeof(child);
    pid_t child_pid;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }
    while(cycle)
    {
        recvfrom(sock, buf, 16, MSG_WAITALL, (struct sockaddr*)&child, &size);
        child_pid = fork();
        if(child_pid == 0)
        {
            printf("%s", buf);
            sendto(sock, buf2, sizeof(buf2), MSG_CONFIRM, (struct sockaddr *)&child, size);
            exit(0);
        }
    }
    close(sock);
    exit(0);
}
