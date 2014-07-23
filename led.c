#include"LED.h"
#include"bsp.h"
#include"delay.h"

int test_led(int cont)
{
	int i;
	for(i=0;i<cont;i++)
	{
		DI=0;
		delay(200);
		DI=1;
		delay(200);
	}

	delay(500);

}


