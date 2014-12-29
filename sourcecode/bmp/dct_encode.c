#include<stdio.h>
#include<conio.h>
#include<IO.h>
#include<fcntl.h>
#include<string.h>
#include"mytime.h"
#include"mymath.h"


ret txtarr,temp;

float	      r_in[8][8] = {0};
float 	      g_in[8][8] = {0};
float	      b_in[8][8] = {0};
float  	      dct_r[8][8],    dct_g[8][8],    dct_b[8][8];
float         r_out[8][8],    g_out[8][8],    b_out[8][8];
int 	      i,j,c1,c2,l;
int 	      colcnt,rowcnt,txtcnt = 8;
char	      abmp[11],bbmp[11],atxt[11],password[11],x,newcontents[1200];
unsigned int  filhandle;
unsigned char b,text;
unsigned long column,rows,sptr = 0,dptr = 0,txtfilsiz,coverfilsiz,offset;
FILE 	      *fsrc,*fdest,*ftext,*st,*ed;


void dct (float pic_in[8][8], float enc_out[8][8])
{
 int        u,v,x,y;
 float u_cs,v_cs,Pi;

 Pi=3.1415927;
   for ( u=0 ; u<8 ; u++ ){
     for ( v=0 ; v<8 ; v++ ){
       for ( x=0 ; x<8 ; x++ ){
	 for ( y=0 ; y<8 ; y++ ){
	  u_cs=cos(((2*x+1)*u*Pi)/16);
	  if (u==0) u_cs=(1/(sqrt(2)));
	  v_cs=cos(((2*y+1)*v*Pi)/16);
	  if (v==0) v_cs=(1/(sqrt(2)));
	  enc_out[v][u]+=0.25*pic_in[y][x]*u_cs*v_cs;
	 }
       }
     }
   }
}


void inv_dct (float enc_in[8][8], float rec_out[8][8])
{
 int        u,v,x,y;
 float u_cs,v_cs,Pi;

 Pi=3.1415927;
   for ( x=0 ; x<8 ; x++ ){
     for ( y=0 ; y<8 ; y++ ){
       for ( u=0 ; u<8 ; u++ ){
	 for ( v=0 ; v<8 ; v++ ){
	  u_cs=cos(((2*x+1)*u*Pi)/16);
	  if (u==0) u_cs=(1/(sqrt(2)));
	  v_cs=cos(((2*y+1)*v*Pi)/16);
	  if (v==0) v_cs=(1/(sqrt(2)));
	  rec_out[y][x]+=0.25*enc_in[v][u]*u_cs*v_cs;
	 }
       }
     }
   }
}

void init()
{
int cnt1,cnt2;
for(cnt1=0;cnt1<8;cnt1++)
 for(cnt2=0;cnt2<8;cnt2++)
 {
  r_in[cnt1][cnt2]=0;     g_in[cnt1][cnt2]=0;     b_in[cnt1][cnt2]=0;
  r_out[cnt1][cnt2]=0;    g_out[cnt1][cnt2]=0;    b_out[cnt1][cnt2]=0;
  dct_r[cnt1][cnt2]=0;    dct_g[cnt1][cnt2]=0;    dct_b[cnt1][cnt2]=0;
 }
}

void trace (float data_to_dump1[8][8], float data_to_dump2[8][8], float data_to_dump3[8][8],char filnam[8])
{
int u,v;
FILE *fp;
fp=fopen(filnam,"wb");
for ( u=0 ; u<8 ; u++ ){
  for ( v=0 ; v<8 ; v++ ){
       fprintf(fp,"%3.0f ",data_to_dump1[u][v]);
       fprintf(fp,"%3.0f ",data_to_dump2[u][v]);
       fprintf(fp,"%3.0f ",data_to_dump3[u][v]);
     fprintf(fp,"\n");
  }
  fprintf(fp,"\n");
}
}
void gettch()
{
rewind(st);
fseek(fdest,54,0);
for(i=0;i<txtfilsiz;i++)
{
fread(&text,sizeof(text),1,st);
bin(text,txtarr);
for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fsrc);
bin(b,temp);
temp[7]=txtarr[j];
b=dec(temp);
fwrite(&b,sizeof(b),1,fdest);
}
}
printf("\nExecution Time: %.2fs",(float)(endtime-starttime)/100);
printf("\nDone");
fire();
getch();
}

void convert()
{
if(txtcnt>7)
	{
	fread(&text,sizeof(text),1,st);
	bin((int)text,txtarr);
	txtcnt=0;
	}//end if
}


void main()
{

clrscr();
printf("\t\tSteg Maker-Transform Domain Encoding- bmp Images\n");
printf("The Cover Image Name(.bmp) w/o extn:");
scanf("%s",abmp);                      //Cover BMP file
strcat(abmp,".bmp");
if((fsrc=fopen(abmp,"rb"))==NULL)
{printf("File error");getch();exit(0);}

printf("The Text File Name with extn:");
scanf("%s",atxt);                       //Text file
if((ftext=fopen(atxt,"rb"))==NULL)
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

printf("The Stego  File Name w/o extn:");
scanf("%s",bbmp);                       //Stego file
strcat(bbmp,".bmp");
if((fdest=fopen(bbmp,"wb"))==NULL)
{printf("File error");getch();exit(0);}

flushall();
fseek(fsrc,2,0);
fread(&coverfilsiz,4,1,fsrc);             //get cover file size
printf("\nCover Image Bytes:%ld\n",coverfilsiz);
txtfilsiz=filelength(filhandle);          //get txt file size
printf("Text File Bytes  :%d\n",txtfilsiz);

fseek(fsrc,10,0);
fread(&offset,4,1,fsrc);                   //get data offset of cover image
fseek(fsrc,18,0);
fread(&column,4,1,fsrc);                   //get cols
fread(&rows,4,1,fsrc);                     //get rows

if(column%8!=0&&rows%8!=0||offset!=54)
{
printf("Image Unsuitable for Transform Encoding");
getch();
exit(0);
}

if(txtfilsiz>rows*column)
{
printf("Text too large to embed in this image");
getch();
exit(0);
}

starttime=gettimenow();

fseek(fsrc,0,0);
for(i=0;i<54;i++)                 //copy the header info
{                                 //from cover to stego file
fread(&b,sizeof(b),1,fsrc);
fwrite(&b,sizeof(b),1,fdest);
}
fseek(fdest,34,0);
fwrite(&(char)txtfilsiz,sizeof(txtfilsiz),1,fdest);
fseek(fdest,54,0);

for(rowcnt=0;rowcnt<rows/8;rowcnt++)
for(colcnt=0;colcnt<column/8;colcnt++)
{

  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
     {          sptr=((colcnt*8)+(rowcnt*column*8)+(i*column)+j)*3;
		fseek(fsrc,54+sptr,0);

		fread(&b,sizeof(b),1,fsrc);
		r_in[i][j]=(float)b;
		fread(&b,sizeof(b),1,fsrc);
		g_in[i][j]=(float)b;
		fread(&b,sizeof(b),1,fsrc);
		b_in[i][j]=(float)b;
	}//end for(j)
     }//end for(i)


//DCT
dct(r_in,dct_r); dct(g_in,dct_g); dct(b_in,dct_b);


/*Detect for values <= treshold and perform LSB insertion*/
for(c1=0;c1<8;c1++)
for(c2=0;c2<8;c2++)
{
convert();

if((c1==0)&&(c2==0))
continue;

if((dct_r[c1][c2]<=3)&&(dct_r[c1][c2]>=2))
{
bin(dct_r[c1][c2],temp);
temp[7]=txtarr[txtcnt++];
dct_r[c1][c2]=decnum(temp);
}
convert();

if((dct_g[c1][c2]<=3)&&(dct_g[c1][c2]>=2))
{
bin(dct_g[c1][c2],temp);
temp[7]=txtarr[txtcnt++];
dct_g[c1][c2]=decnum(temp);
}
convert();

if((dct_b[c1][c2]<=3)&&(dct_b[c1][c2]>=2))
{
bin(dct_b[c1][c2],temp);
temp[7]=txtarr[txtcnt++];
dct_b[c1][c2]=decnum(temp);
}

}//end for(c2)--detection
//inverse DCT
inv_dct(dct_r,r_out); inv_dct(dct_g,g_out); inv_dct(dct_b,b_out);

//Negative Correction
	for(c1=0;c1<8;c1++)
	for(c2=0;c2<8;c2++)
	{
	if(r_out[c1][c2]<0)
	r_out[c1][c2]*=-1;
	if(g_out[c1][c2]<0)
	g_out[c1][c2]*=-1;
	if(b_out[c1][c2]<0)
	b_out[c1][c2]*=-1;
	}//end for(c2)--neg cor

 for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
     {
	      dptr=((colcnt*8)+(rowcnt*column*8)+(i*column)+j)*3;
	      fseek(fdest,54+dptr,0);

	       b=(char)r_out[i][j];
	       fwrite(&b,sizeof(b),1,fdest);
	       b=(char)g_out[i][j];
	       fwrite(&b,sizeof(b),1,fdest);
	       b=(char)b_out[i][j];
	       fwrite(&b,sizeof(b),1,fdest);

       }//end for(j)
    }//end for(i)


init();
}//end for(colcnt)

endtime=gettimenow();

gettch();
fcloseall();
}//end-main
