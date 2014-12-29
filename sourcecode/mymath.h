#include<math.h>
typedef unsigned char ret[8];
void bin(int b,ret arr)          //given no.- b --> binary - arr
{
int i=0,x,j;
char a[8]={'0'};
do
{
x=b%2;
a[i]=x;
b=b/2;
i++;
}while(b!=0);

for(i=7,j=0;i>=0;i--,j++)
arr[j]=a[i];
}


char dec(ret decarr)                //given binary - decarr return
{
int i;                             //decimal as char
double num=0;
for(i=0;i<8;i++)
num=num+(decarr[i]*pow(2,(7-i)));
return((char)num);
}

int decnum(ret decarr)                //given binary - decarr return
{
int i;                             //decimal as number
double num=0;
for(i=0;i<8;i++)
num=num+(decarr[i]*pow(2,(7-i)));
return(num);
}
