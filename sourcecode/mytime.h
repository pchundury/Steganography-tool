#include<dos.h>

unsigned long    starttime,endtime;
struct dostime_t t;

unsigned long gettimenow()
{
unsigned long MILLISECSINSEC = 100;
unsigned long MILLISECSINMIN = MILLISECSINSEC*60;
unsigned long MILLISECSINHOUR = MILLISECSINMIN*60;
_dos_gettime(&t);
return((t.hour*MILLISECSINHOUR)
	+(t.minute*MILLISECSINMIN)
	+(t.second*MILLISECSINSEC)
	+(t.hsecond));
}
void fire()
{
gotoxy(40,23);
printf("Coded By:Pramod,Vijay,Prashanth\n");
gotoxy(44,wherey());
printf("Dept of CSE,Hindustan college of engineering\n");
gotoxy(67,wherey());
printf("2007-2011");
}
