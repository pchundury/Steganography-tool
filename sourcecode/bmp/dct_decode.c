#include<stdio.h>
#include<conio.h>
#include<IO.h>
#include<fcntl.h>
#include<string.h>
#include"mytime.h"
#include"mymath.h"
#include<dos.h>

ret txtarr,numarr;

float 	      r_in[8][8] = {0};
float 	      g_in[8][8] = {0};
float 	      b_in[8][8] = {0};
float         dct_r[8][8],    dct_g[8][8],    dct_b[8][8];
float         r_out[8][8],    g_out[8][8],    b_out[8][8];
int 	      i,j,c1,c2,l,f;;
int 	      colcnt,rowcnt,txtcnt = 0,temp,bytcnt = 0;
char 	      newtxt[11],stegbmp[11],password[11],newcontents[1200];
unsigned int  filhandle;
unsigned char b,text;
unsigned long column,rows,sptr = 0,dptr = 0,txtsiz,offset;
FILE 	      *fsrc,*ftext,*st,*ed;


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
void gettch()
{
fseek(fsrc,54,0);
ftext=fopen(newtxt,"wb");
for(i=0;i<(int)txtsiz;i++)
{
for(j=0;j<8;j++)
{
fread(&b,sizeof(b),1,fsrc);
bin(b,numarr);
txtarr[j]=numarr[7];
}
b=dec(txtarr);
fwrite(&b,sizeof(b),1,ftext);
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

void convert()
{
	if(txtcnt>7)
	{
	text=dec(txtarr);
	fwrite(&text,sizeof(text),1,ftext);
	txtcnt=0;
	bytcnt++;
	}
	  if(bytcnt>txtsiz)
	  {
	  fclose(ftext);
	  }
}


void main()
{

clrscr();
printf("\t\tSteg Riliz-Transform Domain Encoding- bmp Images\n");
printf("The Stego  File Name w/o extn:");
scanf("%s",stegbmp);
strcat(stegbmp,".bmp");                     //Stego file
if((fsrc=fopen(stegbmp,"rb"))==NULL)
{printf("File error");getch();exit(0);}

printf("The  Name For the text file w/o extn:");
scanf("%s",newtxt);    			    //New Text file
strcat(newtxt,".txt");
if((ftext=fopen(newtxt,"wb"))==NULL)
{printf("File error");getch();exit(0);}

fseek(fsrc,18,0);
fread(&column,4,1,fsrc);                    //get cols
fread(&rows,4,1,fsrc);                      //get rows
fseek(fsrc,10,0);
fread(&offset,4,1,fsrc);                    //get data offset of cover image

if(column%8!=0||rows%8!=0||offset!=54)
{
printf("Image Unfit!");
getch();
exit(0);
}

starttime=gettimenow();
fseek(fsrc,34,0);
fread(&txtsiz,4,1,fsrc);

fseek(fsrc,54,0);
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

/*Detect for values <= treshold and get the msg*/
for(c1=0;c1<8;c1++)
for(c2=0;c2<8;c2++)
{

if((c1==0)&&(c2==0))
continue;

if((dct_r[c1][c2]<=3)&&(dct_r[c1][c2]>=2))
{
bin(dct_r[c1][c2],numarr);
txtarr[txtcnt++]=numarr[7];
}
convert();

if((dct_g[c1][c2]<=3)&&(dct_g[c1][c2]>=2))
{
bin(dct_g[c1][c2],numarr);
txtarr[txtcnt++]=numarr[7];
}
convert();

if((dct_b[c1][c2]<=3)&&(dct_b[c1][c2]>=2))
{
bin(dct_b[c1][c2],numarr);
txtarr[txtcnt++]=numarr[7];
}
convert();

}//end for(c2)--detection

init();
}//end for(colcnt)
gettch();
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
fcloseall();

}// end-main
