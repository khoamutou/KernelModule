#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int f = open("/proc/my-driver", O_RDWR);
	while (1)
	{
		write(f , "on", 2);
		usleep(500000);
		write(f , "off", 3);
		usleep(500000);
	}
	
}
