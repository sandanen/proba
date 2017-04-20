#ifndef __RC5SIMPLE_H__
#define __RC5SIMPLE_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include <iostream>
#include <vector>

using namespace std;



#define RC5_FORMAT_VERSION_1 1
#define RC5_FORMAT_VERSION_2 2
#define RC5_FORMAT_VERSION_3 3
#define RC5_FORMAT_VERSION_CURRENT RC5_FORMAT_VERSION_3

#define RC5_SIMPLE_SIGNATURE "RC5SIMP" 




#ifndef LONG_BIT

typedef unsigned long int RC5_TWORD;
#define RC5_ARCHITECTURE 32

#else


#if LONG_BIT==32
typedef unsigned long int RC5_TWORD;
#define RC5_ARCHITECTURE 32
#endif

#if LONG_BIT==64
typedef unsigned int RC5_TWORD;
#define RC5_ARCHITECTURE 64
#endif

#endif

#ifndef RC5_ARCHITECTURE
#error CAN NOT DETECT MICROPROCESSOR ARCHITECTURE
#endif


#define RC5_W    32           
#define RC5_R    12          
#define RC5_B    16            
#define RC5_C     4             
#define RC5_T    26             

#define RC5_WORDS_IN_BLOCK 2                           
#define RC5_BLOCK_LEN     (RC5_W*RC5_WORDS_IN_BLOCK/8) 
#define RC5_WORD_LEN      (RC5_W/8)                    


#define RC5_ROTL(x,y) (((x)<<(y&(RC5_W-1))) | ((x)>>(RC5_W-(y&(RC5_W-1)))))
#define RC5_ROTR(x,y) (((x)>>(y&(RC5_W-1))) | ((x)<<(RC5_W-(y&(RC5_W-1)))))

#define RC5_MODE_ENCODE 0
#define RC5_MODE_DECODE 1

#define RC5_ERROR_CODE_1 1 
#define RC5_ERROR_CODE_2 2 
#define RC5_ERROR_CODE_3 3
#define RC5_ERROR_CODE_4 4 
#define RC5_ERROR_CODE_5 5 
#define RC5_ERROR_CODE_6 6 
#define RC5_ERROR_CODE_7 7 


#define RC5_ENABLE_DEBUG_PRINT 0

#if RC5_ENABLE_DEBUG_PRINT==1
#define RC5_LOG(X) RC5_LogPrint X
#else
#define RC5_LOG(X)
#endif


class RC5Simple
{

public:
 RC5Simple(bool enableRandomInit=false);
 virtual ~RC5Simple();

 const char *RC5_GetVersion(void);

 void RC5_EncryptBlock(RC5_TWORD *pt, RC5_TWORD *ct);
 void RC5_DecryptBlock(RC5_TWORD *ct, RC5_TWORD *pt);
 void RC5_Setup(unsigned char *key);

 void RC5_SetKey(vector<unsigned char> &key);
 void RC5_SetFormatVersionForce(unsigned char formatVersion);
 void RC5_Encrypt(vector<unsigned char> &in, vector<unsigned char> &out);
 void RC5_Decrypt(vector<unsigned char> &in, vector<unsigned char> &out);

 void RC5_EncryptFile(unsigned char *in_name, unsigned char *out_name);
 void RC5_EncryptFile(const char *in_name, const char *out_name);

 void RC5_DecryptFile(unsigned char *in_name, unsigned char *out_name);
 void RC5_DecryptFile(const char *in_name, const char *out_name);

 void RC5_EncDecFile(unsigned char *in_name, unsigned char *out_name, int mode);

 unsigned int RC5_GetErrorCode();

private:

 RC5_TWORD rc5_s[RC5_T]; 
 RC5_TWORD rc5_p; 
 RC5_TWORD rc5_q; 

 unsigned char rc5_key[RC5_B];
 unsigned char rc5_formatVersion;
 bool rc5_isSetFormatVersionForce;

 unsigned int errorCode;

 inline unsigned char RC5_GetByteFromWord(RC5_TWORD w, int n);
 inline unsigned char RC5_GetByteFromInt(unsigned int l, int n);
 inline unsigned int RC5_GetIntFromByte(unsigned char b0,
                                        unsigned char b1,
                                        unsigned char b2,
                                        unsigned char b3);
 inline RC5_TWORD RC5_GetWordFromByte(unsigned char b0,
                                      unsigned char b1,
                                      unsigned char b2,
                                      unsigned char b3);

 void RC5_EncDec(vector<unsigned char> &in, vector<unsigned char> &out, int mode);

 unsigned int RC5_Rand(unsigned int from, unsigned int to);

 void RC5_LogPrint(char *lpszText, ...);
};

#endif 


RC5Simple::RC5Simple(bool enableRandomInit)
{
 
 rc5_p = 0xb7e15163;
 rc5_q = 0x9e3779b9;

 
 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=0;

 rc5_formatVersion=RC5_FORMAT_VERSION_CURRENT;
 rc5_isSetFormatVersionForce=false;

 errorCode=0;

 
 if(enableRandomInit)
  srand( time(NULL) );
}


RC5Simple::~RC5Simple(void)
{
 
 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=0;
}


const char *RC5Simple::RC5_GetVersion(void)
{
 static const char *lib_version=RC5_SIMPLE_VERSION;
 return(lib_version);
}


void RC5Simple::RC5_SetFormatVersionForce(unsigned char formatVersionForce)
{
 rc5_formatVersion=formatVersionForce;
 rc5_isSetFormatVersionForce=true;
}



void RC5Simple::RC5_EncryptBlock(RC5_TWORD *pt, RC5_TWORD *ct)
{
  RC5_TWORD i;
  RC5_TWORD a=pt[0]+rc5_s[0];
  RC5_TWORD b=pt[1]+rc5_s[1];

  for(i=1; i<=RC5_R; i++)
    {
      a = RC5_ROTL(a^b, b)+rc5_s[2*i];
      b = RC5_ROTL(b^a, a)+rc5_s[2*i+1];
    }

  ct[0] = a;
  ct[1] = b;
}



void RC5Simple::RC5_DecryptBlock(RC5_TWORD *ct, RC5_TWORD *pt)
{
  RC5_TWORD i;
  RC5_TWORD b=ct[1];
  RC5_TWORD a=ct[0];

  for(i=RC5_R; i>0; i--)
    {
      b = RC5_ROTR(b-rc5_s[2*i+1], a)^a;
      a = RC5_ROTR(a-rc5_s[2*i], b)^b;
    }

  pt[1] = b-rc5_s[1];
  pt[0] = a-rc5_s[0];
}



void RC5Simple::RC5_Setup(unsigned char *key)
{
   RC5_LOG(( "RC5_Setup, set key to: " ));
   for(int i=0; i<RC5_B; i++)
    RC5_LOG(( "%.2X", key[i] ));
   RC5_LOG(( "\n" ));


   int i, j, k;
   RC5_TWORD u=RC5_W/8, a, b, l[RC5_C];

   
   for(i=RC5_B-1, l[RC5_C-1]=0; i!=-1; i--)
    l[i/u] = (l[i/u]<<8)+key[i];

   RC5_LOG(( "RC5_Setup, l[]: " ));
   for(int i=0; i<RC5_C; i++)
    RC5_LOG(( "%.2X", l[i] ));
   RC5_LOG(( "\n" ));


   for(rc5_s[0]=rc5_p,i=1; i<RC5_T; i++)
    rc5_s[i] = rc5_s[i-1]+rc5_q;

   RC5_LOG(( "RC5_Setup, rc5_s[]: " ));
   for(int i=0; i<RC5_T; i++)
    RC5_LOG(( "%.2X", rc5_s[i] ));
   RC5_LOG(( "\n" ));


   
   for(a=b=i=j=k=0; k<3*RC5_T; k++, i=(i+1)%RC5_T, j=(j+1)%RC5_C)
     {
       a = rc5_s[i] = RC5_ROTL(rc5_s[i]+(a+b),3);
       b = l[j] = RC5_ROTL(l[j]+(a+b),(a+b));
     }

   RC5_LOG(( "RC5_Setup, mix rc5_s[]: " ));
   for(int i=0; i<RC5_T; i++)
    RC5_LOG(( "%.2X", rc5_s[i] ));
   RC5_LOG(( "\n" ));
}



void RC5Simple::RC5_SetKey(vector<unsigned char> &key)
{
 if(key.size()!=RC5_B)
  {
   errorCode=RC5_ERROR_CODE_1;
   return;
  }

 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=key[i];
}



void RC5Simple::RC5_Encrypt(vector<unsigned char> &in, vector<unsigned char> &out)
{

 out.clear();

 if(in.size()==0)
  {
   errorCode=RC5_ERROR_CODE_5;
   return;
  }



 unsigned int clean_data_size=in.size();
 RC5_LOG(( "Input data size: %d\n", clean_data_size ));



 unsigned char iv[RC5_BLOCK_LEN];
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=(unsigned char)RC5_Rand(0, 0xFF);


 
 unsigned char data_size[RC5_BLOCK_LEN];

 for(int i=0; i<RC5_BLOCK_LEN; i++)
  {
   data_size[i]=RC5_GetByteFromInt( clean_data_size, i );
   RC5_LOG(( "Data size byte %d: %.2X\n", i, data_size[i] ));
  }



 in.insert( in.begin(), RC5_BLOCK_LEN, 0);
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  in[i]=data_size[i];

 
 unsigned int firsRandomDataBlocks=0;
 if(rc5_formatVersion==RC5_FORMAT_VERSION_2)
  firsRandomDataBlocks=1; 

 if(rc5_formatVersion==RC5_FORMAT_VERSION_3)
  firsRandomDataBlocks=2;  

 if(firsRandomDataBlocks>0)
  for(unsigned int n=1; n<=firsRandomDataBlocks; n++)
   {
    in.insert( in.begin(), RC5_BLOCK_LEN, 0);
    for(int i=0; i<RC5_BLOCK_LEN; i++)
     in[i]=(unsigned char)RC5_Rand(0, 0xFF);
   }


 
 int last_unalign_len=clean_data_size%RC5_BLOCK_LEN;
 RC5_LOG(( "Last unalign len: %d\n", last_unalign_len ));

 if(last_unalign_len>0)
  {
   
   for(int i=0; i<(RC5_BLOCK_LEN-last_unalign_len); i++)
    {
     RC5_LOG(( "Add byte: %d\n", i ));
     in.push_back( (unsigned char)RC5_Rand(0, 0xFF) );
    }
  }

 #if RC5_ENABLE_DEBUG_PRINT==1
  RC5_LOG(( "Data size after crypt setup: %d\n", in.size() ));
  RC5_LOG(( "Plain byte after crypt setup: " ));
  for(int i=0; i<in.size(); i++)
   RC5_LOG(( "%.2X ", in[i] ));
  RC5_LOG(( "\n" ));
 #endif


 

 unsigned int notCryptDataSize=0;

 
 if(rc5_formatVersion==RC5_FORMAT_VERSION_1)
  {
   out.resize(in.size()+RC5_BLOCK_LEN, 0);

   
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    out[i]=iv[i];

   notCryptDataSize=RC5_BLOCK_LEN;
  }

 
 if(rc5_formatVersion>=RC5_FORMAT_VERSION_2)
  {
   out.resize(in.size()+RC5_BLOCK_LEN*2, 0);

  
   const char *signature=RC5_SIMPLE_SIGNATURE;
   for(int i=0; i<(RC5_BLOCK_LEN-1); i++)
    out[i]=signature[i];

   if(rc5_isSetFormatVersionForce)
    out[RC5_BLOCK_LEN-1]=rc5_formatVersion;
   else
    out[RC5_BLOCK_LEN-1]=RC5_FORMAT_VERSION_CURRENT;

   
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    out[i+RC5_BLOCK_LEN]=iv[i];

   notCryptDataSize=RC5_BLOCK_LEN*2;
  }



 RC5_Setup(rc5_key);


 
 unsigned int block=0;
 while( (RC5_BLOCK_LEN*(block+1))<=in.size() )
  {
   unsigned int shift=block*RC5_BLOCK_LEN;

  
   unsigned char temp_in[RC5_BLOCK_LEN];

  
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    temp_in[i]=in[shift+i] ^ iv[i];

   RC5_TWORD temp_word_1;
   RC5_TWORD temp_word_2;
   RC5_TWORD pt[RC5_WORDS_IN_BLOCK];
   RC5_TWORD ct[RC5_WORDS_IN_BLOCK];

   RC5_LOG(( "Block num %d, shift %d\n", block, shift ));

   temp_word_1=RC5_GetWordFromByte(temp_in[0],
                                   temp_in[1],
                                   temp_in[2],
                                   temp_in[3]);

   temp_word_2=RC5_GetWordFromByte(temp_in[RC5_WORD_LEN+0],
                                   temp_in[RC5_WORD_LEN+1],
                                   temp_in[RC5_WORD_LEN+2],
                                   temp_in[RC5_WORD_LEN+3]);

   pt[0]=temp_word_1;
   pt[1]=temp_word_2;


   
   RC5_EncryptBlock(pt, ct);


   #if RC5_ENABLE_DEBUG_PRINT==1
    RC5_LOG(( "Pt %.8X, %.8X\n", pt[0], pt[1] ));
    RC5_LOG(( "Ct %.8X, %.8X\n", ct[0], ct[1] ));
   #endif

   for(int i=0; i<RC5_WORD_LEN; i++)
    {
     
     out[notCryptDataSize+shift+i]=RC5_GetByteFromWord(ct[0], i);
     out[notCryptDataSize+shift+RC5_WORD_LEN+i]=RC5_GetByteFromWord(ct[1], i);
    }


   
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    iv[i]=out[notCryptDataSize+shift+i];


   block++;
  }


 
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=0;


 
 in.erase(in.begin(), in.begin()+RC5_BLOCK_LEN);

 if(firsRandomDataBlocks>0)
  for(unsigned int n=1; n<=firsRandomDataBlocks; n++)
   in.erase(in.begin(), in.begin()+RC5_BLOCK_LEN);

 if(in.size()>clean_data_size)
  in.erase(in.begin()+clean_data_size, in.end());

}



void RC5Simple::RC5_Decrypt(vector<unsigned char> &in, vector<unsigned char> &out)
{
 RC5_LOG(( "\nDecrypt\n" ));

 RC5_LOG(( "\nInput data size: %d\n", in.size() ));

 
 if(in.size()==0)
  {
   errorCode=RC5_ERROR_CODE_6;
   return;
  }



 unsigned int formatVersion=0;
 if(rc5_isSetFormatVersionForce==true)
  formatVersion=rc5_formatVersion; // If format set force, format not autodetected
 else
  {
  
   const char *signature=RC5_SIMPLE_SIGNATURE;

   
   bool isSignatureCorrect=true;
   for(int i=0; i<(RC5_BLOCK_LEN-1); i++)
    if(in[i]!=signature[i])
     isSignatureCorrect=false;

   if(isSignatureCorrect==false)
    formatVersion=RC5_FORMAT_VERSION_1; 
   else
    {
     
     unsigned char readFormatVersion=in[RC5_BLOCK_LEN-1];

    
     if(readFormatVersion>=RC5_FORMAT_VERSION_2 && readFormatVersion<=RC5_FORMAT_VERSION_CURRENT)
      formatVersion=readFormatVersion;
     else
      formatVersion<=1; 
    }
  }


 unsigned int ivShift=0;
 unsigned int firstDataBlock=0;
 unsigned int removeBlocksFromOutput=0;
 unsigned int blockWithDataSize=0;


 if(formatVersion==RC5_FORMAT_VERSION_1)
  {
   ivShift=0;
   firstDataBlock=1; 
   removeBlocksFromOutput=1;
   blockWithDataSize=1;
  }

 if(formatVersion==RC5_FORMAT_VERSION_2)
  {
   ivShift=RC5_BLOCK_LEN;
   firstDataBlock=2; 
   removeBlocksFromOutput=2; 
   blockWithDataSize=3;
  }

 if(formatVersion==RC5_FORMAT_VERSION_3)
  {
   ivShift=RC5_BLOCK_LEN;
   firstDataBlock=2; 
   removeBlocksFromOutput=3; 
   blockWithDataSize=4;

  }


 
 unsigned char iv[RC5_BLOCK_LEN];
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=in[i+ivShift];


 
 RC5_Setup(rc5_key);


 
 out.clear();
 out.resize(in.size(), 0);


 
 unsigned int data_size=0;
 unsigned int block=firstDataBlock;
 while( (RC5_BLOCK_LEN*(block+1))<=in.size() )
  {
   unsigned int shift=block*RC5_BLOCK_LEN;

   RC5_TWORD temp_word_1;
   RC5_TWORD temp_word_2;
   RC5_TWORD pt[RC5_WORDS_IN_BLOCK];
   RC5_TWORD ct[RC5_WORDS_IN_BLOCK];

   RC5_LOG(( "Block num %d, shift %d\n", block, shift ));

   temp_word_1=RC5_GetWordFromByte(in[shift],
                                   in[shift+1],
                                   in[shift+2],
                                   in[shift+3]);

   temp_word_2=RC5_GetWordFromByte(in[shift+RC5_WORD_LEN],
                                   in[shift+RC5_WORD_LEN+1],
                                   in[shift+RC5_WORD_LEN+2],
                                   in[shift+RC5_WORD_LEN+3]);

   pt[0]=temp_word_1;
   pt[1]=temp_word_2;

   RC5_LOG(( "Block data. Word 1: %.2X, Word 2: %.2X\n", temp_word_1, temp_word_2 ));


  
   RC5_DecryptBlock(pt, ct);


   

   
   unsigned char ct_part[RC5_BLOCK_LEN];

   for(int i=0; i<RC5_WORD_LEN; i++)
    {
     ct_part[i]=RC5_GetByteFromWord(ct[0], i);
     ct_part[i+RC5_WORD_LEN]=RC5_GetByteFromWord(ct[1], i);
    }

  
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    ct_part[i]^=iv[i];

   if(block==blockWithDataSize)
    {
     data_size=RC5_GetIntFromByte(ct_part[0], ct_part[1], ct_part[2], ct_part[3]);

     RC5_LOG(( "Decrypt data size: %d\n", data_size ));

     
     if((unsigned int)data_size>(unsigned int)in.size())
      {
       
       errorCode=RC5_ERROR_CODE_7;
       return;
      }
    }

   #if RC5_ENABLE_DEBUG_PRINT==1
    RC5_LOG(( "Pt %.8X, %.8X\n", pt[0], pt[1] ));
    RC5_LOG(( "Ct %.8X, %.8X\n", ct[0], ct[1] ));
   #endif


   
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    iv[i]=in[shift+i];


  
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    {
     RC5_LOG(( "Put decrypt data to vector out[%d] = %.2X\n", shift-(removeBlocksFromOutput*RC5_BLOCK_LEN)+i, ct_part[i] ));
     out[shift-(firstDataBlock*RC5_BLOCK_LEN)+i]=ct_part[i];
    }

   block++;
  }


 
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=0;

 
 out.erase(out.begin(), out.begin()+removeBlocksFromOutput*RC5_BLOCK_LEN);

 
 if(out.size()>data_size)
  out.erase(out.begin()+data_size, out.end());
}


void RC5Simple::RC5_EncryptFile(unsigned char *in_name, unsigned char *out_name)
{
 RC5_EncDecFile(in_name, out_name, RC5_MODE_ENCODE);
}


void RC5Simple::RC5_EncryptFile(const char *in_name, const char *out_name)
{
 RC5_EncDecFile((unsigned char *)in_name, (unsigned char *)out_name, RC5_MODE_ENCODE);
}


void RC5Simple::RC5_DecryptFile(unsigned char *in_name, unsigned char *out_name)
{
 RC5_EncDecFile(in_name, out_name, RC5_MODE_DECODE);
}


void RC5Simple::RC5_DecryptFile(const char *in_name, const char *out_name)
{
 RC5_EncDecFile((unsigned char *)in_name, (unsigned char *)out_name, RC5_MODE_DECODE);
}


void RC5Simple::RC5_EncDecFile(unsigned char *in_name, unsigned char *out_name, int mode)
{
 FILE *in_file;
 FILE *out_file;

 if((in_file=fopen( (const char*)in_name, "rb") )==NULL)
  {
   errorCode=RC5_ERROR_CODE_2;
   return;
  }

 fseek(in_file, 0, SEEK_END);
 unsigned int in_file_length=ftell(in_file);
 RC5_LOG(( "Input file name: %s, size: %d\n", in_name, in_file_length));

 if(in_file_length==0)
  {
   errorCode=RC5_ERROR_CODE_3;
   return;
  }

 fseek(in_file, 0, SEEK_SET);


 vector<unsigned char> in(in_file_length);
 vector<unsigned char> out;

 unsigned int byte=0;
 while(byte<in_file_length)
  in[byte++]=fgetc(in_file);

 fclose(in_file);

 if(mode==RC5_MODE_ENCODE)
  RC5_Encrypt(in, out); 
 else
  RC5_Decrypt(in, out); 

 
 if((out_file=fopen( (const char*)out_name, "wb") )==NULL)
  {
   errorCode=RC5_ERROR_CODE_4;
   return;
  }

 
 for(unsigned int i=0; i<out.size(); i++)
  {
   RC5_LOG(( "File byte %d : %.2X \n", i, out[i] ));
   fputc(out[i], out_file);
  }

 fclose(out_file);

}


inline unsigned char RC5Simple::RC5_GetByteFromWord(RC5_TWORD w, int n)
{
 unsigned char b=0;

 switch (n) {
   case 0:
       b=(w & 0x000000FF);
       break;
   case 1:
       b=(w & 0x0000FF00) >> 8;
       break;
   case 2:
       b=(w & 0x00FF0000) >> 16;
       break;
   case 3:
       b=(w & 0xFF000000) >> 24;
 }

 

 return b;
}


inline unsigned char RC5Simple::RC5_GetByteFromInt(unsigned int w, int n)
{
 unsigned char b=0;

 switch (n) {
   case 0:
       b=(w & 0x000000FF);
       break;
   case 1:
       b=(w & 0x0000FF00) >> 8;
       break;
   case 2:
       b=(w & 0x00FF0000) >> 16;
       break;
   case 3:
       b=(w & 0xFF000000) >> 24;
 }

 

 return b;
}


inline unsigned int RC5Simple::RC5_GetIntFromByte(unsigned char b0,
                                                  unsigned char b1,
                                                  unsigned char b2,
                                                  unsigned char b3)
{
 return b0+
        (b1 << 8)+
        (b2 << 16)+
        (b3 << 24);
}


inline RC5_TWORD RC5Simple::RC5_GetWordFromByte(unsigned char b0,
                                                unsigned char b1,
                                                unsigned char b2,
                                                unsigned char b3)
{
 return b0+
        (b1 << 8)+
        (b2 << 16)+
        (b3 << 24);
}



unsigned int RC5Simple::RC5_Rand(unsigned int from, unsigned int to)
{
 unsigned int len=to-from+1;

 return (rand()%len)+from;
}


unsigned int RC5Simple::RC5_GetErrorCode()
{
 return errorCode;
}


void RC5Simple::RC5_LogPrint(char *lpszText, ...)
{
 va_list argList;

 va_start(argList, lpszText);

 vprintf(lpszText, argList);

 va_end(argList);

 return;
}

