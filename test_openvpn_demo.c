#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getuname(char *iface_name, int len);

int main()
{	
	char iface_name[20] = {0};
	if(getuname(iface_name, sizeof(iface_name)) < 0)
	{
		printf("get interface name error!\n");
		return -1;
	}
	printf("iface_name=%s sizeof(iface_name)=%ld strlen(iface_name)=%ld\n", iface_name, sizeof(iface_name), strlen(iface_name));
	return 0;
}

int getuname(char *iface, int len)
{
	int r = -1;
	char name[20] = {0};
	char *str = NULL;

	FILE *fp = popen("cat /proc/net/route |grep tun |awk '{printf $1}'", "r");
	fgets(name, sizeof(name), fp);
	printf("Info: name=%s sizeof(name)=%ld strlen(name)=%ld\n", name, sizeof(name), strlen(name));
	if (strncmp(name, "tun", 3) != 0)
	{
		printf("Error: get name tun is failed\n");
		fclose(fp);
		return -1;
	}
	printf("Info: get tun0 successful\n");
	printf("name=%s sizeof(name)=%ld strlen(name)=%ld\n", name, sizeof(name), strlen(name));
	strncpy(iface, name, 4);
	printf("Getuname Info: iface=%s sizeof(iface)=%ld strlen(iface)=%ld\n", iface, sizeof(iface), strlen(iface));
	fclose(fp);
	return 0;	
}
