#include<iostream>
#include<map>
#include<unordered_map>
#include<vector>
#include"DSMgr.h"
#include"BCB.h"
#define DEFBUFSIZE 1024
using namespace std;
class Bmgr{
    public:
        BCB hTable[DEFBUFSIZE];
        unordered_map<int,int> pagetoframe_id;
        DSMgr dsmgr;
        FILE* tmp=dsmgr.OpenFile();    
        vector<bFrame> buf=vector<bFrame>(DEFBUFSIZE);
        int hit_num=0;
        int IO_nums=0;
        Bmgr(){
            for(int i=0;i<DEFBUFSIZE;i++){
                hTable[i].page_id=-1;hTable[i].count=0;hTable[i].dirty=-1;hTable[i].frame_id=-1;
                memset(buf[i].field,'\0',sizeof(buf[i].field));
            }
            cout<<"init ended"<<endl;
        }
        int FixPage(int page_id){//判断页面是否在缓冲区，并返回对应的frame_id；如果不在缓冲区，选择一个页victim page，有必要则载入
            //找到了此page_id,返回frame_id
            int frame_id=-1;
            if(pagetoframe_id.find(page_id)!=pagetoframe_id.end()){
                frame_id=pagetoframe_id[page_id];
                hit_num++;
                int full_count=0;
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].frame_id!=-1){
                        full_count++;
                    }
                }
                if(full_count==DEFBUFSIZE){
                    for(int i=0;i<DEFBUFSIZE;i++){
                        if(i!=frame_id){
                            hTable[i].count++;
                        }
                    }
                }
                hTable[frame_id].count=0;
            }
            //未找到page_id,载入
            else{
                frame_id=SelectVictim();
                if(hTable[frame_id].dirty==1){
                    memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                    strcpy(buf[frame_id].field, "p");
                    // buf[frame_id].field='q';
                    int page_id=hTable[frame_id].page_id;
                    dsmgr.WritePage(page_id,buf[frame_id]);
                    IO_nums++;
                    //写回内存
                }
                pagetoframe_id.insert(make_pair(page_id, frame_id));//更新pagetoframe哈希表
                hTable[frame_id].frame_id=frame_id;//更新BCB表
                hTable[frame_id].count=0;
                hTable[frame_id].dirty=1;
                hTable[frame_id].page_id=page_id;
                dsmgr.ReadPage(page_id);
                IO_nums++;
            }
            return frame_id;
        }
        void Writeinbuffer(int page_id){//写page，若在缓冲区中，直接写入磁盘，更改BCB相关值；若不在缓冲区，分配一个frame_id,并且修改BCB(dirty位置为1)
            if(pagetoframe_id.find(page_id)!=pagetoframe_id.end()){
                int frame_id=pagetoframe_id[page_id];
                // dsmgr.WritePage(frame_id,buf[frame_id]);
                hTable[frame_id].count=0;hTable[frame_id].dirty=1;
                hTable[frame_id].frame_id=frame_id;hTable[frame_id].page_id=page_id;
                hit_num++;
                int full_count=0;
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].frame_id!=-1){
                        full_count++;
                    }
                }
                if(full_count==DEFBUFSIZE){
                    for(int i=0;i<DEFBUFSIZE;i++){
                        if(i!=frame_id){
                            hTable[i].count++;
                        }
                    }
                }
                memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                strcpy(buf[frame_id].field, "q");
            }else{
                int frame_id=SelectVictim();
                if(hTable[frame_id].dirty==1){
                    memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                    strcpy(buf[frame_id].field, "q");
                    dsmgr.WritePage(frame_id,buf[frame_id]);
                    IO_nums++;
                }
                dsmgr.ReadPage(page_id);
                IO_nums++;
                pagetoframe_id.insert(make_pair(page_id,frame_id));
                hTable[frame_id].frame_id=frame_id;
                hTable[frame_id].count=0;
                hTable[frame_id].dirty=1;
                hTable[frame_id].page_id=page_id;
                memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                strcpy(buf[frame_id].field, "q");
            }
        }
        int UnfixPage(int page_id);
        void SetDirty(int frame_id){
            hTable[frame_id].dirty=1;
        }
        void Unsetdirty(int frame_id){
            hTable[frame_id].dirty=0;
        }
        int NumFreeFrames(){
            int count=0;
            for(int i=0;i<DEFBUFSIZE;i++){
                if(hTable[i].frame_id!=-1){
                    count++;
                }
            }
            return count;
        }
        int SelectVictim(){
            int res=-1;
            int full_count=0;
            for(int i=0;i<DEFBUFSIZE;i++){
                if(hTable[i].frame_id!=-1){
                    full_count++;
                }
            }
            if(full_count!=DEFBUFSIZE){
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].frame_id==-1){
                        res=i;
                        return res;
                    }
                }
            }
            else{//full already
                int max=-1;
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].count>=max){
                        max=hTable[i].count;
                    }
                }
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].count==max){
                        res=i;
                        break;
                    }
                }
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(i!=res){
                        hTable[i].count++;
                    }
                }
                return res;
            }
            return res;
        }//选择frame将page载入
        void PrintFrame(int frame_id){
            char* str;
            str=dsmgr.ReadPage(hTable[frame_id].page_id);
            cout<<"frame:"<<str<<endl;
        }
};