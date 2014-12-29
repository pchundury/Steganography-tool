#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<IO.h>
#include<fcntl.h>
#include"mytime.h"
#include"mymath.h"
#include<dos.h>

ret txtarr,picarr;

int 	      i,j,l;
char 	      abmp[11],bbmp[11],atxt[11],password[11],x,newcontents[1200];
unsigned int  modtxtfilsiz = 0,divtxtfilsiz = 0,filhandle;
unsigned long coverfilsiz,offset,txtfilsiz = 0;
unsigned char text,b;
FILE 	      *fp,*fw,*ft, *st,*ed;


void main()
{

clrscr();
printf("\t\t\tSteg Maker-LSb Insertion-bmp Images\n");
printf("The Cover Image Name(.bmp) :");
scanf("%s",abmp);                        //Cover BMP file
strcat(abmp,".bmp");
if((fp=fopen(abmp,"rb"))==NULL)
{printf("File error");getch();exit(0);}

printf("The Text File Name with extn:");
scanf("%s",atxt);                        //Text file
if((ft=fopen(atxt,"rb"))==NULL)
{printf("File error");getch();exit(0);}
filhandle=open(atxt,O_CREAT);


printf("enter the password\n");
scanf("%s",password);

l=strlen(password);

ed=fopen(atxt,"r");
fgets(newcontents,100,ed);
fclose(ed);
for(i=0;i<strlen(newcontents);i++)
{
newcontents[i]=newcontents[i]+password[i%l];
}
printf("%s",newcontents);
st=fopen("temp.txt","w");
fputs(newcontents,st);
fclose(st);
st=fopen("temp.txt","rb");
printf("The Stego  File Name :");
scanf("%s",bbmp);                        //Stego file
strcat(bbmp,".bmp");
if((fw=fopen(bbmp,"wb"))==NULL)
{printf("File error");getch();exit(0);}

flushall();
fseek(fp,2,0);
fread(&coverfilsiz,4,1,fp);              //get cover file size
printf("\nCover Image Bytes:%ld\n",coverfilsiz);
txtfilsiz=filelength(filhandle);         //get txt file size
printf("Text File Bytes  :%d\n",txtfilsiz);


fseek(fp,10,0);
fread(&offset,4,1,fp);                    //get data offset of cover image

if( (txtfilsiz*8) > (coverfilsiz-(offset+16)) )
{                                         //check if pic siz sufficient
printf("Sorry, more text than what can be embedded in the Image");
getch();
exit(0);
}

starttime=gettimenow();
fseek(fp,0,0);
for(i=0;i<offset;i++)                 //copy the header info
{                                     //from cover to stego file
fread(&b,sizeof(b),1,fp);
fwrite(&b,sizeof(b),1,fw);
}

divtxtfilsiz=txtfilsiz/255;
modtxtfilsiz=txtfilsiz%255;

bin(divtxtfilsiz,txtarr);
for(j=0;j<8;j++)                      //write txt file size to
{                                     //stego image
fread(&b,sizeof(b),1,fp);
bin(b,picarr);
picarr[7]=txtarr[j];
b=dec(picarr);
fwrite(&b,sizeof(b),1,fw);
}

bin(modtxtfilsiz,txtarr);
for(j=0;j<8;j++)                     //write txt file size to
{                                    //stego image
fread(&b,sizeof(b),1,fp);
bin(b,picarr);
picarr[7]=txtarr[j];
b=dec(picarr);
fwrite(&b,sizeof(b),1,fw);
}

for(i=0;i<txtfilsiz;i++)
{
fread(&text,sizeof(text),1,st);    //read char by char from text file
bin(text,txtarr);	           //covert each char to binary
for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fp);          //read each byte from cover file
bin(b,picarr);                     //convert value to binary
picarr[7]=txtarr[j];               //LSB insertion
b=dec(picarr);                     //convert back to new color byte
fwrite(&b,sizeof(b),1,fw);         //write to stego file
} //end of for j
} //end of for i

while(!feof(fp))
{
fread(&b,sizeof(b),1,fp);          //copy rest of the data from
fwrite(&b,sizeof(b),1,fw);         //cover to stego file
}

endtime=gettimenow();
printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
printf("\nDone");
fire();
getch();
fcloseall();
}// end-main
