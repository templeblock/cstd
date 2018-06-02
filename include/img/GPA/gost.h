// GostDemo.cpp : Defines the entry point for the console application.
//
/*
���ܣ� ʵ��Gost�����㷨�ļ��ܽ��ܹ���    						 
																	 
˵�����ó����ǻ���Gost�㷨��һ������ı��ļ��ļ��ܽ��ܹ���        
		  ��512�ֽ�Ϊ������λ										 
      ʹ��ʱ�û�������5������									 
		����1. exe�ļ���(����gost)									 
			2. ��������(���ܻ����,1��������,2��������)				 
			3. �������ݵ�txt�ļ��� (���� 1.txt)						 	
			4. д�����������ļ���(���� 2.txt)						 
			5. 128λ��Կ(Ϊȷ����ȫ,������128λ������Կ  )			 		
	��: ����:gost 1 1.txt 2.txt 12345678123456781234567812345678     
     ����:gost 2 2.txt 3.txt 12345678123456781234567812345678     	
																	 
Copyright(c) 2002 by xi'an wuzhen(����)				             
																     
																     
�汾�ţ�1.0.0(6/5/2002)											 
																	 
																	 
AUTHOR:����(WUZHEN)												 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <pmalloc.h>

typedef  int INT32;
typedef  char INT8;
typedef  unsigned char  ULONG8;
typedef  unsigned short ULONG16;
//typedef  unsigned long  ULONG32;

#define SUCCESS 0 
#define FAIL -1

/*��������*/
/*���ܽӿں���*/
INT32 gost_enc(ULONG32 *data , ULONG32 *key);
/*���ܽӿں���*/
INT32 gost_dec(ULONG32 *data, ULONG32 *key);
INT32 encry_data( ULONG32 *Ldata,ULONG32 *Rdata,ULONG32 *key);
INT32 dencry_data( ULONG32 *Ldata,ULONG32 *Rdata,ULONG32 *key);
INT32 f(ULONG32 x);/*s-���滻��ѭ������11λ����*/
/*����ֵ����*/
INT32 gost_swap( ULONG32 *Ldata,ULONG32 *Rdata);

/*Gost��s-��*/
ULONG8 wz_sp[8][16] ={
	{ 0x4,0xa,0x9,0x2,0xd,0x8,0x0,0xe,0x6,0xb,0x1,0xc,0x7,0xf,0x5,0x3},
	{ 0xe,0xb,0x4,0xc,0x6,0xd,0xf,0xa,0x2,0x3,0x8,0x1,0x0,0x7,0x5,0x9},
	{ 0x5,0x8,0x1,0xd,0xa,0x3,0x4,0x2,0xe,0xf,0xc,0x7,0x6,0x0,0x9,0xb},
	{ 0x7,0xd,0xa,0x1,0x0,0x8,0x9,0xf,0xe,0x4,0x6,0xc,0xb,0x2,0x5,0x3},
	{ 0x6,0xc,0x7,0x1,0x5,0xf,0xd,0x8,0x4,0xa,0x9,0xe,0x0,0x3,0xb,0x2},
	{ 0x4,0xb,0xa,0x0,0x7,0x2,0x1,0xd,0x3,0x6,0x8,0x5,0x9,0xc,0xf,0xe},
	{ 0xd,0xb,0x4,0x1,0x3,0xf,0x5,0x9,0x0,0xa,0xe,0x7,0x6,0x8,0x2,0xc},
	{ 0x1,0xf,0xd,0x0,0x5,0x7,0xa,0x4,0x9,0x2,0x3,0xe,0x6,0xb,0x8,0xc}
};

/*������Կʹ��˳���*/
ULONG32 wz_spkey[32] = {
	0,1,2,3,4,5,6,7,
	0,1,2,3,4,5,6,7,
	0,1,2,3,4,5,6,7,
	7,6,5,4,3,2,1,0
};

/*s-���滻��ѭ������11λ����*/
INT32 f (ULONG32 x) {
	x = wz_sp[7][(x>>28)&0xf]<<28 | wz_sp[6][(x>>24)&0xf]<<24
		| wz_sp[5][(x>>20)&0xf]<<20 | wz_sp[4][(x>>16)&0xf]<<16 
		| wz_sp[3][(x>>12)&0xf]<<12 | wz_sp[2][(x>>8)&0xf]<<8 
		| wz_sp[1][(x>>4)&0xf]<< 4   |  wz_sp[0][x&0xf];

	return x <<11 | x>>21;
}
/*����ֵ����*/
INT32 gost_swap( ULONG32 *Ldata,ULONG32 *Rdata)
{
	ULONG32 tempbuf;
	tempbuf = *Rdata;
	*Rdata = *Ldata ;
	*Ldata = tempbuf;
	return SUCCESS;

}

/*32�ֽ��ܲ���*/
INT32 dencry_data( ULONG32 *Ldata,ULONG32 *Rdata,ULONG32 *key)
{
	ULONG32 i = 0 ;
	ULONG32 tempbuf =  0 ;
	for (i = 0 ; i < 32;i++)
	{
		*Rdata ^=f(*Ldata + key[wz_spkey[31-i]]);
		gost_swap(Ldata,Rdata); /*����ֵ����*/
	}
	gost_swap(Ldata,Rdata);	/*����ֵ����*/
	return SUCCESS;
}

/*���ܽӿں���*/
INT32 gost_dec(ULONG32 *data ,/*�����������׵�ַ,�����豣֤��64λ��*/
  ULONG32 *key/*�û�������Կ�׵�ַ,�����豣֤��256λ��*/
)
{
	ULONG32 *Ldata  ;
	ULONG32 *Rdata ;
	Ldata = data ;
	Rdata = &Ldata[1];/*�ֳ�������������,ÿ����32�ֽ�*/
	dencry_data( Ldata, Rdata,key ) ;
	/*���Ŀ���data����*/	
	return SUCCESS;
}

INT32 gost_enc(ULONG32 *data , /*�����������׵�ַ,�����豣֤��64λ��*/
 ULONG32 *key/*�û�������Կ�׵�ַ,�����豣֤��256λ��*/
)
{
	ULONG32 *Ldata  ;
	ULONG32 *Rdata ;
	Ldata = data ;
	Rdata = &Ldata[1];/*�ֳ�������������,ÿ����32�ֽ�*/
	encry_data( Ldata, Rdata,key ) ;
	/*���Ŀ���data����*/	
	return SUCCESS;
}
/*32�ּ��ܲ���*/
INT32 encry_data( ULONG32 *Ldata,ULONG32 *Rdata,ULONG32 *key)
{
	ULONG32 i = 0 ;
	ULONG32 tempbuf  =  0;
	for (i = 0 ; i < 32;i++)
	{
		*Rdata ^=f(*Ldata + key[wz_spkey[i]]); 
		gost_swap(Ldata,Rdata); /*����ֵ����*/
	}
	gost_swap(Ldata,Rdata);	/*����ֵ����*/
	return SUCCESS;
}


#define WZ_COMMEND_NUM 4  
#define WZUSEHELPNUM 7
#define READFILESIZE 512 /*һ�δ��ļ��ж�ȡ�����ֽ�,���Ը����ڴ�Ĵ�С����*/

INT32 encodehex(ULONG8 *tobuf,ULONG8 *frombuf,ULONG32 len)
{
	ULONG8 *readfirst = frombuf ;
	ULONG8 *readend = &frombuf[1] ;	
	INT8 *s;
	ULONG8 y[2] ;		    
	ULONG32 i;
	for ( i = 0 ; i < len ; i++)
	{
		y[0] = *readfirst ;
		y[1] = *readend ;
		readfirst += 2 ;
		readend += 2 ;
		tobuf[i] = (ULONG8)strtol((INT8*)y, &s, 16);			
	}	
	return SUCCESS;
}

#if 0

INT32 hextofile( ULONG8 *buf ,FILE *writefile, ULONG32 length)
{
	ULONG32 writelen = 0 ;
		/*��16������ʽд���ļ�*/
    while( writelen < length)
	{
		if(buf[writelen] == 0)
		{
			fprintf( writefile, "%x", 0 );
			fprintf( writefile, "%x", 0 );
		}
		else if (buf[writelen] < 0x10)
		{
			fprintf( writefile, "%x", 0 );
			fprintf( writefile, "%x", buf[writelen] );				
		}
		else
		{
			fprintf( writefile, "%x", buf[writelen] );
			
		}			
		writelen++;
		
	}
	return SUCCESS;
}

INT32 file_enc( FILE *readfile, FILE *writefile,ULONG8 *key)
{
	INT32 filelen = 0,readlen = 0,writelen = 0;
	ULONG32 totalfilelen = 0 ;/*ͳ��ʵ�ʵ��ļ��ĳ���*/
	INT32 i;
	ULONG8 readbuf[READFILESIZE] = { 0 };		
	filelen = fread( readbuf, sizeof( INT8 ), READFILESIZE, readfile );
	while( filelen == READFILESIZE )
	{	
		totalfilelen += READFILESIZE;
		for ( i = 0 ; i < READFILESIZE ; i += 8)
		{
			gost_enc( (ULONG32*)&readbuf[i] ,(ULONG32*) key);/*����*/			
		}				
		hextofile( readbuf, writefile, READFILESIZE );/*��16������ʽд���ļ�*/
		memset(readbuf,0,READFILESIZE);	
		filelen = fread( readbuf, sizeof( INT8 ), READFILESIZE, readfile );
	}	
	/*���Ǵ��ļ��ж��������һ������,���ȿ��ܻ����0,����Ҫ���ж�*/

	if ( filelen > 0 )
	{
		/*������ļ��ж����ĳ��Ȳ�����0,��ô�϶���8���ֽ����ϵĿռ�
		�ļ����ȴ������8���ֽ���*/
		totalfilelen += filelen;
		memcpy( &readbuf[READFILESIZE-8], (ULONG8*)&totalfilelen,4);	
		for ( i = 0 ; i < READFILESIZE ; i += 8)
		{
			gost_enc( (ULONG32*)&readbuf[i] ,(ULONG32*) key);/*����*/
		}		
		hextofile( readbuf, writefile,READFILESIZE );/*��16������ʽд���ļ�*/		
		memset(readbuf,0 ,READFILESIZE);
	}
	else /*filelen == 0*/
	{
		memcpy( &readbuf[0], (ULONG8*)&totalfilelen,4);			
		gost_enc( (ULONG32*)&readbuf[0] ,(ULONG32*) key);/*����*/		
		hextofile( readbuf, writefile, 8);/*��16������ʽд���ļ�*/		
	}
	return SUCCESS;
}


INT32 file_dec( FILE *readfile, FILE *writefile,ULONG8 *key)
{
	INT32 filelen = 0,readlen = 0,writelen = 0;
	ULONG32 totalfilelen = 0 ;/*ͳ��ʵ�ʵ��ļ��ĳ���*/
	INT32 i,num;
	ULONG8 readbuf[READFILESIZE] = { 0 };
	ULONG8 sendbuf[READFILESIZE*2] = { 0 };	
	
	fseek(readfile,-16,SEEK_END);/*���16���ֽڵı�ʾ�ļ����ȵĿռ�*/			
	filelen = fread( sendbuf, sizeof( INT8 ), 16, readfile );	
	encodehex( readbuf,sendbuf,8);	
	gost_dec( (ULONG32*)&readbuf[0] ,(ULONG32*) key);/*����*/
	memcpy((ULONG8*)&totalfilelen, &readbuf[0],4);/*�õ��ļ��ܳ�*/
	memset(readbuf,0 ,8);
	memset(sendbuf,0 ,16);			
    
	num = totalfilelen/READFILESIZE;/*�м���READFILESIZE��*/
	totalfilelen %= READFILESIZE;

	fseek(readfile,0,SEEK_SET);/*�����ļ�ͷ*/	
	while(num--)
	{
		filelen = fread( sendbuf, sizeof( INT8 ), READFILESIZE*2, readfile );			
		encodehex( readbuf,sendbuf,READFILESIZE);
		for ( i = 0 ; i < READFILESIZE ; i += 8)
		{
			gost_dec( (ULONG32*)&readbuf[i] ,(ULONG32*) key);/*����*/
		}				
		writelen = fwrite(readbuf, sizeof( INT8 ), READFILESIZE, writefile);				  		
		memset(readbuf,0 ,READFILESIZE);
		memset(sendbuf,0 ,READFILESIZE*2);			
	}
	if ( totalfilelen > 0 )/*���һ���ж����Ԫ��*/
	{
		filelen = fread( sendbuf, sizeof( INT8 ), READFILESIZE*2, readfile );	
		encodehex( readbuf,sendbuf,READFILESIZE);
		for ( i = 0 ; i < READFILESIZE ; i += 8)
		{
			gost_dec( (ULONG32*)&readbuf[i] ,(ULONG32*) key);/*����*/
		}		
		writelen = fwrite(readbuf, sizeof( INT8 ), totalfilelen, writefile);	
		memset(readbuf,0 ,READFILESIZE);
		memset(sendbuf,0 ,READFILESIZE*2);						
		
	}	
	return SUCCESS;
}

void wz_printhelp()
{
	INT32 i ;
	printf("\t");
	for (  i = 0 ; i < 22 ; i++)
	{
		printf("%c ",5);
	}
	printf("\n");
	for( i = 0 ; i < WZ_COMMEND_NUM ; i++)
	{
		printf("\t%c\t%s %c\n",5,WZ_Commend_Help[i],5);	
	}
	printf("\t");
	for (  i = 0 ; i < 22 ; i++)
	{
		printf("%c ",5);
	}	
	printf("\n");
	for( i = 0 ; i < WZUSEHELPNUM ; i++)
	{
		printf("\t%s\n",WZ_USE_HELP[i]);
	}
	return ;
}


void main(INT32 argc,INT8 *argv[])
{
INT8 *WZ_Commend_Help[] =
{
	"����GOST�ļ��ܽ��ܹ���v1.0	 ",/*0*/	
	"׷��׿Խ,���ڴ���		 ",	
	"----���� :  ����---		 ",
	"			  	 "	
};

INT8 *WZ_USE_HELP[]={
	"����5������:",
	"\t1.��ִ���ļ��� *.exe",
	"\t2.�������� 1:����;2:����;",
	"\t3.�������ݵ��ļ���*.txt",
	"\t4.д�����ݵ��ļ���*.txt",	
	"\t5.��Կ(32�ֽ�)",	
	"******************************"
};


	INT8 *FILENAME1,*FILENAME2;    
	FILE *fp, *fp2;		
	ULONG8 key[33] = { 0 };	/*��Կ����*/		
	if ( argc != 5 )	
	{
		wz_printhelp();
	}
	FILENAME1 = argv[2];
	FILENAME2 = argv[3];
	if ((fp= fopen(FILENAME1,"rb")) == NULL	|| (fp2 = fopen(FILENAME2,"wb"))==NULL)
	{			
		printf("Can't open file\n");		
		return ;
	}  	
    memcpy( key, argv[4] , strlen( argv[4]) );/*ȡ����Կ*/	
	switch( atoi(argv[1] ))
	{
	case 1:/*���ܲ���*/			
		file_enc(fp,fp2,key);				
		printf("\n \tGost �������,���Ĵ���%s�ļ�\n",FILENAME2);
		break;
	case 2:
		/*����*/
		file_dec(fp,fp2,key);	
		printf("\n\t Gost�������,���Ĵ���%s�ļ�\n",FILENAME2);
		break;
	default:	
		printf("��ѡ���Ǽ���|���� plese choose encrypt|deencrypt\n");		
		break;
	}
	
	fclose(fp);
	fclose(fp2);
}

#endif

/*�㷨ʵ�ִ������*/
