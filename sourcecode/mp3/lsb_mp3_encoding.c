#include<conio.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<IO.h>
#include<process.h>
#include<fcntl.h>
#include"mytime.h"
#include "mymath.h"


void main()
{
 int 	       i,k,chk,l;
 char 	       mp3coverfile[15],txtfilename[15],mp3stegfile[15],password[11],x,newcontents[120];
 unsigned int  divtxtfilsiz=0,modtxtfilsiz=0;
 unsigned char b,c;
 unsigned long filhandle,txtfilsiz=0,filhandlemp3,coverfilsiz=0;
 ret 	       txtarr,picarr,checkbit;
 FILE 	       *fmp3,*ftxt,*fsteg,*st,*ed;

 clrscr();

 printf("\t\t\tSteg Maker-LSb insertion-MP3\n");
 printf("Enter the mp3 cover file name w/o extn.:");
 scanf("%s",mp3coverfile);
 strcat(mp3coverfile,".mp3");
 if ((fmp3 = fopen(mp3coverfile,"rb")) == NULL)
 { fprintf(stderr,"Cannot open mp3 cover file.\n"); exit(0); }
/////////////////////////////////////////////////////////////
 printf("Enter the text file name to be embedded:");
 scanf("%s",txtfilename);
 if ((ftxt = fopen(txtfilename,"rb")) == NULL)
 { fprintf(stderr,"Cannot open input text file.\n"); exit(0); }
/////////////////////////////////////////////////////////////

printf("enter the password\n");
scanf("%s",password);

l=strlen(password);

ed=fopen(txtfilename,"r");
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


//Get steg mp3 steg file name and open it
 printf("Enter the mp3 stego file name w/o extn.:");
 scanf("%s",mp3stegfile);
 strcat(mp3stegfile,".mp3");
 if((fsteg = fopen(mp3stegfile,"wb")) == NULL)
 { printf("Cannot open stego file for embedding"); exit(0); }
/////////////////////////////////////////////////////////////
//Calculate text file and cover file size
 filhandle=open(txtfilename,O_CREAT);
 txtfilsiz=filelength(filhandle);
 printf("\nText File :%ld Bytes",txtfilsiz);
 filhandlemp3=open(mp3coverfile,O_CREAT);
 coverfilsiz=filelength(filhandlemp3);
 printf("\nCover MPEG file :%ld Bytes\n",coverfilsiz);
/////////////////////////////////////////////////////////////
//copy the header information onto mp3 steg file
starttime=gettimenow();
  fseek(fmp3,0,0);
  fseek(fsteg,0,0);
  for(i=0;i<15;i++)
  {
   fread(&b,sizeof(b),1,fmp3);
   fwrite(&b,sizeof(b),1,fsteg);
  }
//////////////////////////////////////////////////////////////

// embed the text file size onto the steg file
 divtxtfilsiz=txtfilsiz/255;          //Find Txt File..
 modtxtfilsiz=txtfilsiz%255;          //    ..Details

 bin(divtxtfilsiz,txtarr);
 for(i=0;i<8;i++)                     //write quo txt file size to..
    {                                 //               ..stego mp3
     fread(&b,sizeof(b),1,fmp3);
     bin(b,picarr);
     picarr[7]=txtarr[i];
     b=dec(picarr);
     fwrite(&b,sizeof(b),1,fsteg);
	for(k=0;k<10;k++)
	{
	 fread(&b,sizeof(b),1,fmp3);
	 fwrite(&b,sizeof(b),1,fsteg);
	}
    }

 bin(modtxtfilsiz,txtarr);
 for(i=0;i<8;i++)                     //write mod txt file size to..
    {                                 //              ..stego mp3
     fread(&b,sizeof(b),1,fmp3);
     bin(b,picarr);
     picarr[7]=txtarr[i];
     b=dec(picarr);
     fwrite(&b,sizeof(b),1,fsteg);
	for(k=0;k<10;k++)
	{
	 fread(&b,sizeof(b),1,fmp3);
	 fwrite(&b,sizeof(b),1,fsteg);
	}
    }

//////////////////////////////////////////////////////////////

// embed the text file onto the steg file
 while(!feof(st))
 {
   fread(&b,sizeof(b),1,st);
   bin(b,txtarr);
   for(i=0;i<8;i++)
   {
      fread(&b,sizeof(b),1,fmp3);
      bin(b,picarr);
      chk=dec(picarr);
      if(chk==255)// if same as header byte
       {
	fread(&b,sizeof(b),1,fmp3);
	bin(b,checkbit);
	if(checkbit[0]==1 && checkbit[1]==1 && checkbit[2]==1)
		fseek(fmp3,15,1);
	else
		fseek(fmp3,-1,1);
     //	goto l2;
       }
      picarr[7]=txtarr[i];
      b=dec(picarr);
      fwrite(&b,sizeof(b),1,fsteg);
// offset so that continous bytes are not embedded into, reducing noise
	for(k=0;k<10;k++)
	{
	 fread(&b,sizeof(b),1,fmp3);
	 fwrite(&b,sizeof(b),1,fsteg);
	}
   }
 }
////////////////////////////////////////////////////////////////

// copy the remaining mp3 cover file data
 while(!feof(fmp3))
 {
  fread(&b,sizeof(b),1,fmp3);
  fwrite(&b,sizeof(b),1,fsteg);
 }
/*****************************************************************/

 fclose(fmp3); fclose(fsteg); fclose(ftxt);
 endtime=gettimenow();
 printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
 printf("\nDone");
 fire();
 getch();
}
