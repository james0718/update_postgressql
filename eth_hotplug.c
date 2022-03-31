/*
* This function is to handle the hot plug function of the network cable.
*
*/

#include <sys/types.h>  
#include <sys/socket.h>  
#include <asm/types.h>  
#include <linux/netlink.h>  
#include <linux/rtnetlink.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>  
#include <string.h>  
#include <unistd.h>  
#include <libpq-fe.h>

#define BUFLEN 20480  
  
//#define DEBUG         1

#ifdef DEBUG
#define DEBUGPRINT      printf
#else
#define DEBUGPRINT(a, ...)
#endif

int main(int argc, char *argv[])  
{  
    int nl_socket_fd, rece_data;
    char buf[BUFLEN] = {0};  
    int len = BUFLEN;  
    struct sockaddr_nl nl_local_addr;  
    struct nlmsghdr *nh;  
    struct ifinfomsg *ifinfo;  
    struct rtattr *attr;  
    int flag_up = 0;

  
    nl_socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);  
    setsockopt(nl_socket_fd, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));  
    memset(&nl_local_addr, 0, sizeof(nl_local_addr));  
    nl_local_addr.nl_family = AF_NETLINK;  
    nl_local_addr.nl_groups = RTNLGRP_LINK;  
    bind(nl_socket_fd, (struct sockaddr*)&nl_local_addr, sizeof(nl_local_addr));  
    rece_data = read(nl_socket_fd, buf, BUFLEN);
    DEBUGPRINT("receive data:%d\n", rece_data);

    while ((rece_data = read(nl_socket_fd, buf, BUFLEN)) > 0) {  
        for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, rece_data); nh = NLMSG_NEXT(nh, rece_data)) {  
		if (nh->nlmsg_type == NLMSG_DONE)  
                	break;  
            	else if (nh->nlmsg_type == NLMSG_ERROR)  
                	return -1;  
		else if (nh->nlmsg_type != RTM_NEWLINK)  
                	continue;  

           	attr = (struct rtattr*)(((char*)nh) + NLMSG_SPACE(sizeof(*ifinfo)));  
            	len = nh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));  

            	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {  
                	if (attr->rta_type == IFLA_IFNAME) {  
                    		printf("%s:", (char*)RTA_DATA(attr));  
                    		break;  
                	}  
            	}

            	ifinfo = NLMSG_DATA(nh);  
            	printf("%s\n", (ifinfo->ifi_flags & IFF_LOWER_UP) ? "up" : "down" );  
		DEBUGPRINT("ifinfo->ifi_flags=%d\n", ifinfo->ifi_flags);
		DEBUGPRINT("IFF_LOWER_UP=%d\n", IFF_LOWER_UP);
		DEBUGPRINT("(ifinfo->ifi_flags & IFF_LOWER_UP)=%d\n", (ifinfo->ifi_flags & IFF_LOWER_UP));

	    	if ((ifinfo->ifi_flags & IFF_LOWER_UP) != 0) {
			if(flag_up == 0) {
				sleep(10);
				system("/bin/systemctl restart update-postgressql.service");
			}
			flag_up++;
		} else {
			flag_up = 0;	
		}	
		printf("\n");  
        }  
    }   
    return 0;  
}
