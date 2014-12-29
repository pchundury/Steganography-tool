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
 int 	       filhandle,i,k,chk,l;
 char 	       mp3stegfile[15],txtfilename[15],password[11],newcontents[1200];
 unsigned int  divtxtsiz,modtxtsiz;
 unsigned long txtsiz;
 unsigned char b,c;
 FILE 	       *fmp3,*ftxt,*st,*ed;
 ret 	       txtarr,picarr,checkbit;

 clrscr();

 /*open the mp3 file and text file */
 printf("\t\t\tSteg Riliz-LSb insertion-MP3.\n");
 printf("Enter the mp3 stego file name w/o extn.:");
 scanf("%s",mp3stegfile);
 strcat(mp3stegfile,".mp3");
 if ((fmp3 = fopen(mp3stegfile,"rb")) == NULL)
 { fprintf(stderr,"Cannot open mp3 cover file.\n"); exit(0); }

 printf("Enter the text file name w/o extn.:");
 scanf("%s",txtfilename);
 strcat(txtfilename,".txt");
 if ((ftxt = fopen(txtfilename,"wb")) == NULL)
 { fprintf(stderr,"Cannot open output text file.\n"); exit(0); }
///////////////////////////////////////////////////////////
 starttime=gettimenow();
 fseek(fmp3,15,0); //move the header offset
// obtain the text file size from the steg file
   for(i=0;i<8;i++)
    {
     fread(&b,sizeof(b),1,fmp3);     // Read Text Size
     bin(b,picarr);                   // from stego file
     txtarr[i]=picarr[7];
	for(k=0;k<10;k++)
	    fread(&b,sizeof(b),1,fmp3);
    }
   divtxtsiz=dec(txtarr);

   for(i=0;i<8;i++)
    {
     fread(&b,sizeof(b),1,fmp3);     //Read Text Size
     bin(b,picarr);                   //from stego file
     txtarr[i]=picarr[7];
	for(k=0;k<10;k++)
	    fread(&b,sizeof(b),1,fmp3);
    }
   modtxtsiz=dec(txtarr);

   txtsiz=((divtxtsiz*255)+modtxtsiz);
   printf("\nText File Size: %ld bytes\n",txtsiz);

//////////////////////////////////////////////////////////////
// obtain the text file from the steg file
 while(txtsiz!=0)
 {
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
       txtarr[i]=picarr[7];
// offset so that continous bytes are not embedded into, reducing noise
	for(k=0;k<10;k++)
	    fread(&b,sizeof(b),1,fmp3);
   }
   b=dec(txtarr);
   fwrite(&b,sizeof(b),1,ftxt);
   txtsiz--;
 }
////////////////////////////////////////////////////////////////

 fclose(fmp3); fclose(ftxt);
/////////////////// 
ed=fopen(txtfilename,"r");
fgets(newcontents,100,ed);
printf("\nenter the password\n");
scanf("%s",password);
l=strlen(password);
fclose(ed);
st=fopen(txtfilename,"w");



for(i=0;i<strlen(newcontents);i++)
{newcontents[i]=newcontents[i]-password[i%l];
}

fputs(newcontents,st);  



////////////////////
endtime=gettimenow();
 printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
 printf("\nDone");
 fire();
 getch();
}

