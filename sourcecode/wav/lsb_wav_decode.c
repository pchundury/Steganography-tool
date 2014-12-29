#include<stdio.h>
#include<IO.h>
#include"dcodhuff.c"
#include<conio.h>
#include<string.h>
#include<dos.h>
#include"mytime.h"
#include"mymath.h"


 void main()
  {
   unsigned int divtxtsiz,modtxtsiz,l;
   unsigned long txtsiz;
   unsigned long i,j,offset;
   unsigned char b;
   char newtxt[11],stegwav[11],tmptxt[11],password[11],newcontents[1200];
   ret txtarr,picarr;
   FILE *fswav,*ftxt,*st,*ed;

   clrscr();
   printf("\t\tSteg Riliz-LSb Insertion-WAV Extended.\n");
   printf("The Stego  File Name w/o extn:");
   scanf("%s",stegwav);
   strcat(stegwav,".wav");            //Stego file;
   if((fswav=fopen(stegwav,"rb"))==NULL)
     {
      printf("File error");
      getch();
      exit(0);
     }
   printf("The  Name For the text file w/o extn:");
   scanf("%s",newtxt);    	      //New Text file
   strcpy(tmptxt,newtxt);
   strcat(newtxt,".txt");
   //strcat(tmptxt,".cmp");
   if((ftxt=fopen(newtxt,"wb"))==NULL)
     {
      printf("File error");
      getch();
      exit(0);
     }

   starttime=gettimenow();
   flushall();

   fseek(fswav,44+16,0);
   for(j=0;j<8;j++)
    {
     fread(&b,sizeof(b),1,fswav);     // Read Text Size
     bin(b,picarr);                   // from stego file
     txtarr[j]=picarr[7];
     fread(&b,sizeof(b),1,fswav);
    }
   divtxtsiz=dec(txtarr);

   for(j=0;j<8;j++)
    {
     fread(&b,sizeof(b),1,fswav);     //Read Text Size
     bin(b,picarr);                   //from stego file
     txtarr[j]=picarr[7];
     fread(&b,sizeof(b),1,fswav);
    }
   modtxtsiz=decnum(txtarr);
   txtsiz=((divtxtsiz*255)+modtxtsiz);

   printf("\nText File Size %ld bytes\n",txtsiz);

   for(i=0;i<txtsiz;i++)
    {
     for(j=0;j<8;j++)
      {
       fread(&b,sizeof(b),1,fswav);   //Read text char by
       bin(b,picarr);                 //char and write to
       txtarr[j]=picarr[7];           //new file
       fread(&b,sizeof(b),1,fswav);
      }
     b=dec(txtarr);
     fwrite(&b,sizeof(b),1,ftxt);
    }
   fclose(ftxt);
  fclose(fswav);
  unlink(tmptxt);
fcloseall();
   //huffmandecoding(tmptxt,newtxt);


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
  printf("\n\n\n\n\nDone");

  fire();
  getch();
}
