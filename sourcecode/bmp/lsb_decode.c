#include<stdio.h>
#include<conio.h>
#include"mytime.h"
#include"mymath.h"
#include<string.h>
ret txtarr,picarr;

char          newtxt[11],stegbmp[11],password[11],newcontents[1200];
unsigned int  txtsiz,divtxtsiz,modtxtsiz;
unsigned long i,j,offset;
int l,f;
unsigned char b;
FILE 	      *fp,*fw,*st,*ed;


void main()
{

clrscr();

printf("\t\t\tSteg Riliz-LSb Insertion-bmp Images\n");
printf("The Stego  File Name w/o extn:");
scanf("%s",stegbmp);
strcat(stegbmp,".bmp");                //Stego file;
if((fp=fopen(stegbmp,"rb"))==NULL)
{printf("File error");getch();exit(0);}

printf("The  Name For the text file w/o extn:");
scanf("%s",newtxt);    		      //New Text file
strcat(newtxt,".txt");
if((fw=fopen(newtxt,"wb"))==NULL)
{printf("File error");getch();exit(0);}

flushall();
fseek(fp,10,0);
fread(&offset,4,1,fp);

starttime=gettimenow();
fseek(fp,offset,0);
for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fp);              //Read Text Size
bin(b,picarr);                         //from stego file
txtarr[j]=picarr[7];
}
divtxtsiz=decnum(txtarr);

for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fp);              //Read Text Size
bin(b,picarr);                         //from stego file
txtarr[j]=picarr[7];
}
modtxtsiz=decnum(txtarr);

txtsiz=((divtxtsiz*255)+modtxtsiz);
printf("\nText File Size %d bytes",txtsiz);

for(i=0;i<txtsiz;i++)
{
for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fp);             //Read text char by
bin(b,picarr);                        //char and write to
txtarr[j]=picarr[7];                  //new file
}
b=dec(txtarr);
fwrite(&b,sizeof(b),1,fw);
}
fcloseall();


ed=fopen(newtxt,"r");
fgets(newcontents,100,ed);
printf("\nenter the password\n");
scanf("%s",password);
l=strlen(password);
fclose(ed);
st=fopen(newtxt,"w");



for(i=0;i<strlen(newcontents);i++)
{newcontents[i]=newcontents[i]-password[i%l];
}

fputs(newcontents,st);
endtime=gettimenow();
printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
printf("\nDone");
fire();
getch();



}// end-main
