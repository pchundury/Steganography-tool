//Redundant Encoding WAVE
#include<stdio.h>
#include<conio.h>
#include<IO.h>
#include<fcntl.h>
#include<string.h>
#include"mymath.h"
#include"mytime.h"

void main()
{
 char 	       awav[11],bwav[11],atxt[11],ctxt[11],password[11],x,newcontents[1200];
 unsigned int  divtxtfilsiz=0,modtxtfilsiz=0;
 unsigned long i,j,h;
 unsigned long filhandletxt,txtfilsiz=0,filhandlewav;
 unsigned long coverfilsiz=0,tmp;
 unsigned char text,b,z;
int l;
 ret 	       txtarr,wavarr;
 FILE 	       *fwav,*fswav,*ftxt,*st,*ed;

 clrscr();

 printf("\t\t\tSteg Maker-Redundant Encoding-WAV\n");
 printf("The Cover WAV File Name w/o extn: ");
 scanf("%s",awav);
 strcat(awav,".wav");                 //Cover WAV file
 if((fwav=fopen(awav,"rb"))==NULL)
  {
   printf("File error");
   getch();
   exit(0);
  }
 filhandlewav=open(awav,O_CREAT);

 printf("The Text File Name w/o extn:");
 scanf("%s",atxt);
 strcat(atxt,".txt");                  // Text file.

 if((ftxt=fopen(atxt,"rb"))==NULL)
  {
   printf("File error");
   getch();
   exit(0);
  }
 filhandletxt=open(atxt,O_CREAT);     //Create file handle

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


printf("The Stego  File Name w/o extn: ");
 scanf("%s",bwav);
 strcat(bwav,".wav");                 //Stego file
 if((fswav=fopen(bwav,"wb"))==NULL)
  {
   printf("File error");
   getch();
   exit(0);
  }
 flushall();

 coverfilsiz=filelength(filhandlewav);//get cover file size
 printf("\nCover WAV file :%ld Bytes\n",coverfilsiz);
 tmp=coverfilsiz/2;
 txtfilsiz=filelength(filhandletxt);  //get txt file size
 printf("Text File :%ld Bytes\n",txtfilsiz);
 if(txtfilsiz>tmp/2)
 {
 printf("Sorry, more text than what can be embedded in the audio");
getch();
exit(0);
}

 starttime=gettimenow();
//----------------copy process-----------------

for(i=0;i<44;i++)
{
fread(&b,sizeof(b),1,fwav);
fwrite(&b,sizeof(b),1,fswav);
}


 divtxtfilsiz=txtfilsiz/255;          //Find Txt File..
 modtxtfilsiz=txtfilsiz%255;          //    ..Details
 printf("\nstart posn: %ld",ftell(fswav));
 printf("\n2nd half: %ld\n",tmp);

 bin(divtxtfilsiz,txtarr);
 for(j=0;j<8;j++)                     //write quo txt file size to..
    {                                 //               ..stego wav
     fread(&b,sizeof(b),1,fwav);
       bin(b,wavarr);
       wavarr[7]=txtarr[j];
       b=dec(wavarr);
     fwrite(&b,sizeof(b),1,fswav);

     fread(&b,sizeof(b),1,fwav);
     fwrite(&b,sizeof(b),1,fswav);
    }

 bin(modtxtfilsiz,txtarr);

 for(j=0;j<8;j++)                     //write mod txt file size to..
    {                                 //              ..stego wav
     fread(&b,sizeof(b),1,fwav);
      bin(b,wavarr);
      wavarr[7]=txtarr[j];
      b=dec(wavarr);
     fwrite(&b,sizeof(b),1,fswav);

     fread(&b,sizeof(b),1,fwav);
     fwrite(&b,sizeof(b),1,fswav);
    }

    for(i=0;i<txtfilsiz;i++)
    {
     fread(&text,sizeof(text),1,st);//read char by char from text file
     bin(text,txtarr);                //covert each char to binary

     for(j=0;j<8;j++)
      {
       fread(&b,sizeof(b),1,fwav);    //read each byte from cover file
	bin(b,wavarr);                //convert value to binary
	wavarr[7]=txtarr[j];          //LSB insertion
	b=dec(wavarr);                //convert back to new color byte
       fwrite(&b,sizeof(b),1,fswav);

       fread(&b,sizeof(b),1,fwav);
       fwrite(&b,sizeof(b),1,fswav);  //write to stego file
      }                               //end of for j
    }                                 //end of for i
     rewind(st);
while(ftell(fwav)<tmp)
{
    fread(&b,sizeof(b),1,fwav);
     fwrite(&b,sizeof(b),1,fswav);
}

   for(i=0;i<txtfilsiz;i++)
    {
     fread(&text,sizeof(text),1,st);//read char by char from text file
     bin(text,txtarr);                //covert each char to binary

     for(j=0;j<8;j++)
      {
       fread(&b,sizeof(b),1,fwav);    //read each byte from cover file
	bin(b,wavarr);                //convert value to binary
	wavarr[7]=txtarr[j];          //LSB insertion
	b=dec(wavarr);                //convert back to new color byte
       fwrite(&b,sizeof(b),1,fswav);

       fread(&b,sizeof(b),1,fwav);
       fwrite(&b,sizeof(b),1,fswav);  //write to stego file
      }                               //end of for j
    }                                 //end of for i

while(!feof(fwav))
{
    fread(&b,sizeof(b),1,fwav);
    fwrite(&b,sizeof(b),1,fswav);
}

fcloseall();
 endtime=gettimenow();

 printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
 printf("\nDone");
 fire();
 getch();

}
