#include <zephyr.h>
#include <sys/printk.h>
#include "multiply.h"

void main(void)
{
	int x = 5;
	int y = 2;
	int result = multiply(x, y);
	printk("%d times %d equals %d\n", x, y, result);
}