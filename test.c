#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int f = open("/proc/my-driver", O_RDWR);
	write(f , "Test C", 6);
}
