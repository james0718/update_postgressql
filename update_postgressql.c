/*
* This function is to process the function of obtaining ip to update the database.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <net/route.h>
#include <errno.h>  
#include <unistd.h>  
#include <string.h>
  
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>  
#include <arpa/inet.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h>

#define  USER    	"postgres"
//#define  PASSWD		"advantech1990"
#define	 DBNAME		"aimlink"
#define  ANDROIDBSP	"androidbsp"
#define  LOGIN		"admin"

//#define TEST 	1
//#define DEBUG 	1

#ifdef DEBUG
#define DEBUGPRINT	printf
#else
#define	DEBUGPRINT(a, ...)
#endif

int getuname(char *iface_name, int len);
static int getIfaceName(char *iface_name, int len);  //read activity network interface 
static int getIpAddress(char *iface_name, char *ip_addr, int len); // get ipaddress

int main(int argc ,char* argv[])
{

	//struct in_addr  intaddr;
	char intaddr[100] = {0};
    	char iface_name[20] = {0};
	char *buffer = NULL;
	int i = 0,
	    j = 0;
	int vpnflag = 0;
	char *conninfo = NULL;
	char *querydb = NULL; // query database repoip is empty
	char *dbinfo = NULL;
	char *issep = NULL;   // if web serverip set manual mode
	/*Read postgres random password*/
	FILE * fp;
        char PASSWD[100] = {0};

	fp=popen("cat /bin/.env |grep  POSTGRES_PASSWORD|awk -F '=' '{print $2}'","r");
	fgets(PASSWD,sizeof(PASSWD),fp);

	if(getuname(iface_name, sizeof(iface_name)) == 0)
	{
#ifdef TEST
		DEBUGPRINT("%s:%d: iface_name=%s, sizeof(iface_name)=%ld, strlen(iface_name)=%ld\n", __FILE__, __LINE__, iface_name, sizeof(iface_name), strlen(iface_name));
#endif
		if(getIpAddress(iface_name, intaddr, 15) < 0) 
		{
			printf("get interface ip address error!\n");
			return -1;
		}
		
	}else{
		if(getIfaceName(iface_name, sizeof(iface_name)) < 0)
		{
        		printf("get interface name error!\n");
        		return -1;
    		}
		if(getIpAddress(iface_name, intaddr, 15) < 0) 
		{
			printf("get interface ip address error!\n");
			return -1;
		}
	}
    	printf("active ip:%s\n",(char *) &intaddr);	
	printf("active ethernet name:%s\n", iface_name);

	conninfo = (char *)malloc(200);
	if (NULL == conninfo)
	{
		perror("malloc");
		return -1;
	}
	memset(conninfo, 0, 200);
	sprintf(conninfo, "hostaddr=%s user=%s password=%s dbname=%s", intaddr, USER, PASSWD, DBNAME);
	sleep(3);

	//connect to postgresSQL 
    	PGconn *conn = PQconnectdb(conninfo);
/* Start: Modify by 2019-09-10 */
	while(1)
	{	
		if(PQstatus(conn) == CONNECTION_BAD)
		{
			sleep(3);
			printf("connect:Waiting for DataBase\n");
			if(i >= 5)
			{
				fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
				PQfinish(conn);
				free(conninfo);
    				exit(1);
			}
			i++;
			continue;
		}else {
			break;
		}
	}
/* End: Modify by 2019-09-10 */

/* Start: Modify by 2021-08-02 */
	querydb = (char *)malloc(300);  // query database repoip is empty
	issep = (char *)malloc(300);   // if web serverip set manual mode
	if ((NULL == querydb) || (NULL == issep)) {
		perror("malloc");
		return -1;
	}
	memset(querydb, 0, 300);
	memset(issep, 0, 300);
	PGresult *query_res = PQexec(conn, "SELECT repoip,issep FROM \"aimuser\"");
	sprintf(querydb, "%s", PQgetvalue(query_res, 0, 0));
	sprintf(issep, "%s", PQgetvalue(query_res, 0, 1));
/* End: Modify by 2021-08-02 */

	dbinfo = (char *)malloc(300);
	if (NULL == dbinfo)
	{
		perror("malloc");
		return -1;
	}
	memset(dbinfo, 0, 300);
	if(getuname(iface_name, sizeof(iface_name)) == 0)
	{
		vpnflag = 1;
		sprintf(dbinfo, "UPDATE \"aimuser\" SET isvpn = '%d', repoip = '%s', serverip = '%s', vncip = '%s' WHERE name = '%s';", vpnflag, intaddr, intaddr, intaddr, LOGIN);
	}else{
		vpnflag = 0;
		if (strcmp(querydb, "") == 0){
			printf("repoip data is empty!\n");
			sprintf(dbinfo, "UPDATE \"aimuser\" SET isvpn = '%d', repoip = '%s', serverip = '%s', vncip = '%s' WHERE name = '%s';", vpnflag, intaddr, intaddr, intaddr, LOGIN);
		}else{
			if (strcmp(issep, "true") == 0) {
				printf("repoip data: %s\n", querydb);
				sprintf(dbinfo, "UPDATE \"aimuser\" SET isvpn = '%d', serverip = '%s', vncip = '%s' WHERE name = '%s';", vpnflag, intaddr, intaddr, LOGIN);
			}else {
				printf("repoip data: %s\n", querydb);
				sprintf(dbinfo, "UPDATE \"aimuser\" SET isvpn = '%d', repoip = '%s', serverip = '%s', vncip = '%s' WHERE name = '%s';", vpnflag, intaddr, intaddr, intaddr, LOGIN);

			}
		}
	}
	sleep(5);
	PGresult *res = PQexec(conn, dbinfo);
/* Start: Modify by 2019-09-10 */
	while(1)
	{	
		if (PQresultStatus(res) != PGRES_COMMAND_OK) 
		{
			sleep(2);
			printf("dbinfo:Waiting LISTEN DataBase\n");
			if(j >= 5)
			{
				fprintf(stderr, "LISTEN command failed: %s", PQerrorMessage(conn)); 
        			PQclear(res);  
				PQfinish(conn);  
				free(conninfo);
				free(dbinfo);
    				exit(1);
			}
			j++;
			continue;
		}else {
			break;
		}
	}
/* End: Modify by 2019-09-10 */
	res = PQexec(conn, "SELECT isvpn,repoip,serverip,vncip,issep FROM \"aimuser\" WHERE name = \'admin\';");
	printf("PASSWD:%s\tisvpn:%s\trepoip:%s\tserverip:%s\tvncip:%s\tissep:%s\n", PASSWD, PQgetvalue(res, 0, 0), PQgetvalue(res, 0, 1), PQgetvalue(res, 0, 2), PQgetvalue(res, 0, 3), PQgetvalue(res, 0, 4));
	PQclear(query_res);
	free(querydb);
	PQclear(res);
	PQfinish(conn);
	free(conninfo);
	free(dbinfo);
	return 0;
}

//function getuname() support openvpn, if enable openvpn, which has a tun0 node in ifconfig info and /proc/net/route file.
int getuname(char *iface_name, int len)
{
	int r = -1;
	char name[20] = {0};
	char *str = NULL;

	FILE *fp = popen("nmcli d |grep tun |awk '{printf $1}'", "r");
	fgets(name, sizeof(name), fp);
	if (strncmp(name, "tun", 3) != 0)
	{
		DEBUGPRINT("%s:%d: get device name is not tun, default used hardware network device name!\n", __FILE__, __LINE__);
		fclose(fp);
		return -1;
	}
	DEBUGPRINT("%s:%d: Info: get network device name tun success, default used tun!\n", __FILE__, __LINE__);
#ifdef TEST
	DEBUGPRINT("%s:%d: name=%s sizeof(name)=%ld strlen(name)=%ld\n", __FILE__, __LINE__, name, sizeof(name), strlen(name));
#endif
	strncpy(iface_name, name, 4);
	DEBUGPRINT("%s:%d: Getuname Info: iface_name=%s sizeof(iface_name)=%ld strlen(iface_name)=%ld\n", __FILE__, __LINE__, iface_name, sizeof(iface_name), strlen(iface_name));
	fclose(fp);
	return 0;	
}

// read active ethernet name func()
static int getIfaceName(char *iface_name, int len)
{
    int r = -1;
    int flgs, ref, use, metric, mtu, win, ir;
    unsigned long int d, g, m;    
    char devname[20];
    FILE *fp = NULL;

    if((fp = fopen("/proc/net/route", "r")) == NULL) {
        perror("fopen error!\n");
        return -1;
    }

    if (fscanf(fp, "%*[^\n]\n") < 0) {
        fclose(fp);
        return -1;
    }

    while (1) {
        r = fscanf(fp, "%19s%lx%lx%X%d%d%d%lx%d%d%d\n",
                 devname, &d, &g, &flgs, &ref, &use,
                 &metric, &m, &mtu, &win, &ir);
        if (r != 11) {
            if ((r < 0) && feof(fp)) {
                break;
            }
            continue;
        }
        strncpy(iface_name, devname, len);
        fclose(fp);
        return 0;
    }

    fclose(fp);

    return -1;
}

// get ip address func()
static int getIpAddress(char *iface_name, char *ip_addr, int len)
{
    int sockfd = -1;
    struct ifreq ifr;
    struct sockaddr_in *addr = NULL;

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, iface_name);
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket error!\n");
        return -1;
    }
    
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) 
    {
     	strncpy(ip_addr, inet_ntoa(addr->sin_addr), len);
        close(sockfd);
        return 0;
    }

    close(sockfd);

    return -1;
}
