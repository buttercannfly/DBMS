#include<iostream>
#include<cstring>
#include<cstdio>
#include<string>
#include"frame_buffer.h"
using namespace std;
class DSMgr{
    public:
        FILE *fp=NULL;
        int IO_nums;
        DSMgr(){
            fp=NULL;
            IO_nums=0;
        }
        FILE* OpenFile(){
            fp = fopen("./data.txt","wb+");
            return fp;
        }
        void CloseFile(){
            fclose(fp);
        }
        char* ReadPage(int page_id){
            fseek(fp, page_id*FRAMESIZE, SEEK_SET);
            char buffer[FRAMESIZE];
            fread(buffer, FRAMESIZE, 1, fp);
            IO_nums++;
            return buffer;
        }
        void WritePage(int page_id, bFrame frm){
            fseek(fp,page_id*FRAMESIZE,SEEK_SET);
            fwrite(frm.field,FRAMESIZE,1,fp);
            IO_nums++;
        }
        int Seek(int offset, int pos){
            int res = offset + FRAMESIZE*pos;
            return res;
        }
        FILE * GetFile(){
            return fp;
        }
        // int Pages[MAXPAGES];
};