/*
 * unix_client.c
 * A UNIX socket client used to unix_server.
 * NOTE: unix_server uses UDP.
 * Reference: man7.org/tlpi
 * Feb 16, 2015
 * root@davejingtian.org
 * http://davejingtian.org
 */
#define _GNU_SOURCE             /* To get SCM_CREDENTIALS definition from
                                   <sys/sockets.h> */
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
 
#define SERVER_SOCK_PATH "/home/bartek/unix_server_sock"
#define CLIENT_SOCK_PATH "/home/bartek/unix_client_sock"
 
struct my_data {
    int index;
    char msg[32];
};
 
static int non_connection = 1;
 
int main(int argc, char *argv[])
{
    struct msghdr msgh;
    struct iovec iov;
    int sfd, opt;
    ssize_t ns, nr;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
                        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;
    struct sockaddr_un addr;
    struct sockaddr_un server_addr;
    struct my_data data;
 
    /* Create socket bound to well-known address */
    if (remove(CLIENT_SOCK_PATH) == -1 && errno != ENOENT) {
        printf("Error: remove failed\n");
        return -1;
    }
 
    /* Build the UNIX socket using UDP */
    if (strlen(CLIENT_SOCK_PATH) >= sizeof(addr.sun_path)-1) {
        printf("Error: path length exceeds the max\n");
        return -1;
    }
 
    memset(&addr, 0x0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, CLIENT_SOCK_PATH, sizeof(addr.sun_path)-1);
 
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        printf("Error: socket failed [%s]\n", strerror(errno));
        return -1;
    }
 
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        printf("Error: bind failed [%s]\n", strerror(errno));
        close(sfd);
        return -1;
    }
 
 
    /* Build the data */
    memset(&data, 0x0, sizeof(data));
    data.index = 777;
    snprintf(data.msg, 32, "%s", "This is not Boeing 777!");
 
    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */
 
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);
 
    if (!non_connection) {
    /* Don't need to specify destination address, because we use
    connect() below */
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;
    }
 
    /* Don't construct an explicit credentials structure. (It
       is not necessary to do so, if we just want the receiver to
       receive our real credentials.) */
 
    printf("Not explicitly sending a credentials structure\n");
    msgh.msg_control = NULL;
    msgh.msg_controllen = 0;
 
    /* Set the server address */
    memset(&server_addr, 0x0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCK_PATH, sizeof(server_addr.sun_path)-1);
 
    if (non_connection) {
    printf("Info: no connection\n");
    msgh.msg_name = (void *)&server_addr;
    msgh.msg_namelen = sizeof(server_addr);
    } else {
    /* Connect the socket */
    if (connect(sfd, (struct sockaddr *) &server_addr,
                sizeof(struct sockaddr_un)) == -1) {
        printf("Error: connect failed [%s]\n", strerror(errno));
        close(sfd);
        return -1;
    }
    }
 
    /* Send the msg */
    ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1) {
    printf("Error: sendmsg failed [%s]\n", strerror(errno));
    return -1;
    }
    printf("sendmsg() returned %ld\n", (long) ns);
 
    /* Get PID before exit */
    printf("PID=[%d]\n", getpid());
 
    /* Wait for the ACK from the server */
    //msgh.msg_name = (void *)server_addr;
    //msgh.msg_namelen = sizeof(server_addr);
    nr = recvmsg(sfd, &msgh, 0);
    if (nr == -1) {
    printf("Error: recvmsg failed [%s]\n", strerror(errno));
    return -1;
    }
    printf("recvmsg() returned %ld\n", (long)nr);
 
    if (nr > 0)
    printf("Received data: index=[%d], msg=[%s]\n",
        data.index, data.msg);
 
    return 0;
}