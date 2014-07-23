#include"compute.h"

void checkout (unsigned char xdata *buff,int len,unsigned char *c1,unsigned char *c2)
{
	int i;
	unsigned char ch1=buff[0],ch2=buff[0];

	for(i=1;i<len;i++)
	{
		ch1 ^= buff[i];
		ch2 += buff[i];
	}
	*c1=ch1;
	*c2=ch2;
}



