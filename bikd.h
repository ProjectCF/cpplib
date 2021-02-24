#ifndef BIKD_H
#define BIKD_H
#pragma pack(1)
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
typedef long long ll;
typedef unsigned long long ull;
using namespace std;
ull kerr,_safe32=1<<30;
struct KFILE{
    FILE *fp;
    ull len,pos;
};
void kcreate(char *fn){
    FILE* fp=fopen(fn,"wb");
    if(fp==NULL){kerr=1;return;}
    for(int i=0;i<32;i++)fputc(0,fp);
    fclose(fp);
}
KFILE* kopen(char *fn){
    KFILE *kf=new KFILE;
    kf->fp=fopen(fn,"rb+");
    if(kf->fp==NULL){kerr=1;return NULL;}
    kf->len=0;
    kf->pos=0;
    for(int i=0;i<24;i++)fgetc(kf->fp);
    for(int i=0;i<8;i++)kf->len=kf->len*256+fgetc(kf->fp);
    return kf;
}
void kclose(KFILE *kf){
    rewind(kf->fp);
    for(int i=0;i<24;i++)fputc(0,kf->fp);
    for(int i=0;i<8;i++)fputc((kf->len>>(8*(7-i)))%256,kf->fp);
    fclose(kf->fp);
    delete kf;
}
ull ktell(KFILE *kf){return kf->pos;}
ull klength(KFILE *kf){return kf->len;}
void kseek(ull pos,KFILE *kf){
    ull bytepos=pos/8;
    if(pos>kf->len){kerr=2;return;}
    fseek(kf->fp,32,0);
    for(ull i=0;i<bytepos/_safe32;i++)fseek(kf->fp,_safe32,1);
    bytepos%=_safe32;
    fseek(kf->fp,bytepos,1);
    kf->pos=pos;
}
ull kread(ull cnt,KFILE *kf){
    if(kf->pos+cnt>kf->len){kerr=2;return 0;}
    ull ans=0;
    ans=fgetc(kf->fp)%(1ull<<(8-kf->pos%8));
    ll r=cnt-(8-kf->pos%8);
    if(r<=0){kf->pos+=cnt;return ans>>(-r);}
    for(int i=0;i<r/8;i++)ans=ans*256+fgetc(kf->fp);
    r%=8;
    if(r==0){kf->pos+=cnt;return ans;}
    ull tmp=fgetc(kf->fp)>>(8-r);
    fseek(kf->fp,-1,1);
    kf->pos+=cnt;
    return (ans<<r)+tmp;
}
void kwrite(ull d,ull cnt,KFILE *kf){
    if(kf->pos+cnt>kf->len){kerr=2;return;}
    ull b1=8-kf->pos%8;
    ull cur=fgetc(kf->fp);
    fseek(kf->fp,-1,1);
    ull b=(cnt<b1?cnt:b1);
    fputc(cur/(1ull<<b)*(1ull<<b)+d>>(cnt-b),kf->fp);
    ull r=cnt-b1;
    if(r<=0){kf->pos+=cnt;return;}
    d%=(1ull<<r);
    for(int i=0;i<r/8;i++)fputc(d%(1ull<<(r-8*i))/(1ull<<(r-8*i-8)),kf->fp);
    d%=(1ull<<(r%8));
    r%=8;
    if(r==0){kf->pos+=cnt;return;}
    cur=fgetc(kf->fp);
    fseek(kf->fp,-1,1);
    fputc(cur%(1<<(8-r))+d<<(8-r),kf->fp);
    kf->pos+=cnt;
}
void ktrunc(ull len,KFILE *kf){
    if(kf->len>len){kf->len=len;kseek(len,kf);return;}
    kseek(kf->len,kf);
    ull t=8-kf->pos%8;
    if(t!=8){
        kf->len+=t;
        kwrite(0,t,kf);
    }
    if(kf->len>len){kf->len=len;kseek(len,kf);return;}
    ull diff=(len+7)/8-(kf->len+7)/8;
    for(ull i=0;i<diff;i++)fputc(0,kf->fp);
    kf->len=len;
    kf->pos=len;
}
#endif
