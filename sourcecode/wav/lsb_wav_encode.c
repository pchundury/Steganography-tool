
#include<stdio.h>
#include<conio.h>
#include"codhuff.c"
#include<IO.h>
#include<fcntl.h>
#include"mymath.h"
#include<dos.h>
#include"mytime.h"
#include<string.h>



void check();

struct HeaderType {       //Header Length -> 44 Bytes

  long         RIFF;      //RIFF header
  char         NI1 [18];  //not important
  unsigned int Channels;  //channels 1 = mono; 2 = stereo
  long         Frequency; //sample frequency
  char         NI2 [6];   //not important
  unsigned int BitRes;    //bit resolution 8/16 bit
  char         NI3 [8];  //not important
} Header;




void main()
{
 unsigned long i,j,h,l;
 unsigned long  filhandletxt,txtfilsiz=0,filhandlewav;
 unsigned int  divtxtfilsiz=0,modtxtfilsiz=0;
 unsigned long coverfilsiz=0;
 unsigned char text,b,z;
 char awav[11],bwav[11],atxt[11],ctxt[11],password[11],x,newcontents[120];
 ret txtarr,wavarr;
 FILE *fwav,*fswav,*ftxt,*st,*ed;

 clrscr();

 printf("\t\tSteg Maker-LSb insertion-WAV Extended.\n");
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
 strcpy(ctxt,atxt);                    // Copy txt fil name to tmp.
 strcat(ctxt,".cmp");                  // Compressed text fil name.
 strcat(atxt,".txt");                  // Text file.

 //huffmanencoding(atxt,ctxt);      //Use Huffman encoding to
				       //to compress txt file.
				       //atxt->src_file
				       //ctxt->com_file
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
 txtfilsiz=filelength(filhandletxt);  //get txt file size
 printf("Text File :%ld Bytes\n",txtfilsiz);

 starttime=gettimenow();

 fread(&Header,sizeof(Header), 1, fwav);//Write Wav Header to..
 fwrite(&Header,sizeof(Header),1,fswav);//       ...Steg File

  for (h=0;h<16;h++)
  {
   fread(&z,1,1,fwav);
   fwrite(&z,1,1,fswav);
  }


 check();                             //Display WAV Details.

 divtxtfilsiz=txtfilsiz/255;          //Find Txt File..
 modtxtfilsiz=txtfilsiz%255;          //    ..Details
 printf("%d,%d",modtxtfilsiz,divtxtfilsiz);

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


       fread(&b,sizeof(b),1,fwav);
       do {
	fwrite(&b,sizeof(b),1,fswav);
	fread(&b,sizeof(b),1,fwav);
       } while(!feof(fwav));

 fclose(fwav);
 fclose(fswav);
 fclose(ftxt);

 endtime=gettimenow();

 printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
 printf("\nDone");
 fire();
 getch();
 unlink(ctxt);                         //Delete the Compressed file
}

void check()
 {
  if (Header.RIFF != 0x46464952)      //Check if Vaild Wav or Not!
    printf ("\nNot a wave file\n");

  if (Header.Channels != 1)           //Check channels
	printf("WAV is a Stero wave file\n");
  else  printf("WAV is a Mono file\n");

  if (Header.BitRes != 8)             //Check bit resolution
	printf ("WAV is 16-bit wave file\n");
  else  printf ("WAV is 8-bit wave file\n");
 }
