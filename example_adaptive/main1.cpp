/*
 * unix_server.c
 * A UNIX socket server used to retrieve the creditial of peer socket.
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
 
#define SOCK_PATH "/home/bartek/unix_server_sock"
 
struct my_data {
    int index;
    char msg[32];
};
 
static int non_blocking = 0;
 
int main(int argc, char *argv[])
{
    struct msghdr msgh;
    struct iovec iov;
    struct ucred *ucredp, ucred;
    int lfd, sfd, optval, opt;
    ssize_t nr, ns;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
                        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;
    socklen_t len;
    struct sockaddr_un addr;
    struct sockaddr_un client_addr;
    struct my_data data;
 
 
    /* Create socket bound to well-known address */
    if (remove(SOCK_PATH) == -1 && errno != ENOENT) {
    printf("Error: remove failed\n");
    return -1;
    }
 
    /* Build the UNIX socket using UDP */
    printf("Receiving via datagram socket\n");
    if (strlen(SOCK_PATH) >= sizeof(addr.sun_path)-1) {
    printf("Error: path length exceeds the max\n");
        return -1;
    }
 
    memset(&addr, 0x0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);
 
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
 
    /* We must set the SO_PASSCRED socket option in order to receive
       credentials */
 
    optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
    printf("Error: setsockopt failed [%s]\n", strerror(errno));
    return -1;
    }
 
    /* Set 'control_un' to describe ancillary data that we want to receive */
 
    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(struct ucred));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_CREDENTIALS;
 
    /* Set 'msgh' fields to describe 'control_un' */
 
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);
 
    /* Set fields of 'msgh' to point to buffer used to receive (real)
       data read by recvmsg() */
 
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);
    memset(&data, 0x0, sizeof(data));
 
    /* Save the client address for sendmsg */
    memset(&client_addr, 0x0, sizeof(client_addr));
    msgh.msg_name = (void *)&client_addr;
    msgh.msg_namelen = sizeof(client_addr);
 
    /* Receive real plus ancillary data */
    if (non_blocking) {
    /* NonBlocking I/O */
    do {
       nr = recvmsg(sfd, &msgh, MSG_DONTWAIT);
       if (nr <= 0) {
        printf("non-blocking wait again [%s]\n", strerror(errno));
        sleep(2);
       } else {
        break;
       }
    } while (nr <= 0);
    } else {
    /* Blocking I/O */
    nr = recvmsg(sfd, &msgh, 0);
    if (nr == -1) {
        printf("Error: recvmsg failed [%s]\n", strerror(errno));
        return -1;
    }
    }
    printf("recvmsg() returned %ld\n", (long)nr);
 
    /* Dump the client sock path */
    printf("Debug: client sock path [%s]\n", client_addr.sun_path);
 
    if (nr > 0)
        printf("Received data: index=[%d], msg=[%s]\n",
        data.index, data.msg);
 
    /* Extract credentials information from received ancillary data */
 
    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(struct ucred))) {
        printf("Error: bad cmsg header / message length\n");
    return -1;
    }
    if (cmhp->cmsg_level != SOL_SOCKET) {
        printf("Error: cmsg_level != SOL_SOCKET\n");
    return -1;
    }
    if (cmhp->cmsg_type != SCM_CREDENTIALS) {
        printf("Error: cmsg_type != SCM_CREDENTIALS\n");
    return -1;
    }
 
    ucredp = (struct ucred *) CMSG_DATA(cmhp);
    printf("Received credentials pid=[%ld], uid=[%ld], gid=[%ld]\n",
        (long) ucredp->pid, (long) ucredp->uid, (long) ucredp->gid);
 
    /* Send an ACK to the client */
    memset(&data, 0x0, sizeof(data));
    data.index = 888;
    snprintf(data.msg, 32, "%s", "ACK");
    msgh.msg_name = (void *)&client_addr;
    msgh.msg_namelen = sizeof(client_addr);
    msgh.msg_control = NULL;
    msgh.msg_controllen = 0;
    printf("Debug: msg.name [%p], msg.namelen [%d], path [%s]\n",
    msgh.msg_name, msgh.msg_namelen, client_addr.sun_path);
    ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1) {
    printf("Error: sendmsg failed [%s]\n", strerror(errno));
    return -1;
    }
    printf("sendmsg() returned %ld\n", (long) ns);
 
    return 0;
}
