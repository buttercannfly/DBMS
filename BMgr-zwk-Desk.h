#include<iostream>
#include<map>
#include<unordered_map>
#include"DSMgr.h"
#include"BCB.h"
#define DEFBUFSIZE 1024
using namespace std;
class Bmgr{
    BCB hTable[DEFBUFSIZE];
    unordered_map<int,int> pagetoframe_id;
    DSMgr dsmgr;
    // bFrame buf[DEFBUFSIZE];
    public:
        vector<bFrame> buf = vector<bFrame>(DEFBUFSIZE);
        int hit_num = 0;
        Bmgr(){
            for(int i=0;i<DEFBUFSIZE;i++){
                pagetoframe_id[i]=-1;
                // buf[i].field='\0';
                hTable[i].page_id=-1;hTable[i].count=0;hTable[i].dirty=-1;hTable[i].frame_id=-1;hTable[i].next=NULL;
                // buf[i].field = ;
                memset(buf[i].field,'\0',sizeof(buf[i].field));
            }
            cout<<"init ended"<<endl;
        }
        int FixPage(int page_id){//判断页面是否在缓冲区，并返回对应的frame_id；如果不在缓冲区，选择一个页victim page，有必要则载入
            //找到了此page_id,返回frame_id
            int frame_id = -1;
            if(pagetoframe_id.find(page_id)!=pagetoframe_id.end()){
                frame_id=pagetoframe_id[page_id];
                hit_num++;
                hTable[frame_id].count=0;
                dsmgr.ReadPage(page_id);
            }
            //未找到page_id,载入
            else{
                frame_id=SelectVictim();
                if(hTable[frame_id].dirty==1){
                    memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                    strcpy(buf[frame_id].field, "q");
                    // buf[frame_id].field='q';
                    dsmgr.WritePage(frame_id,buf[frame_id]);
                    //写回内存
                }
                dsmgr.ReadPage(page_id);
                pagetoframe_id.insert(make_pair(page_id, frame_id));//更新pagetoframe哈希表
                hTable[frame_id].frame_id=frame_id;//更新BCB表
                hTable[frame_id].count=0;
                hTable[frame_id].dirty=1;
                hTable[frame_id].page_id=page_id;
            }
            for(int i=0;i<DEFBUFSIZE;i++){
                if(i!=frame_id){
                    hTable[i].count++;
                }
            }
            return frame_id;
        }
        void Writeinbuffer(int page_id){//写page，若在缓冲区中，直接写入磁盘，更改BCB相关值；若不在缓冲区，分配一个frame_id,并且修改BCB(dirty位置为1)
            int frame_id = -1;
            if(pagetoframe_id.find(page_id)!=pagetoframe_id.end()){
                frame_id=pagetoframe_id[page_id];
                // dsmgr.WritePage(frame_id,buf[frame_id]);
                hTable[frame_id].count=0;hTable[frame_id].dirty=1;
                hTable[frame_id].frame_id=frame_id;hTable[frame_id].page_id=page_id;
                hit_num++;
            }else{
                frame_id=SelectVictim();
                if(hTable[frame_id].dirty==1){
                    memset(buf[frame_id].field,'\0',sizeof(buf[frame_id].field));
                    strcpy(buf[frame_id].field, "q");
                    dsmgr.WritePage(frame_id,buf[frame_id]);
                }
                pagetoframe_id.insert(make_pair(page_id,frame_id));
                hTable[frame_id].frame_id=frame_id;
                hTable[frame_id].count=0;
                hTable[frame_id].dirty=1;
                hTable[frame_id].page_id=page_id;
            }
            for(int i=0;i<DEFBUFSIZE;i++){
                if(i!=frame_id){
                    hTable[i].count++;
                }
            }
        }
        // void FixNewPage();
        int UnfixPage(int page_id);
        int NumFreeFrames();
        int SelectVictim(){
            int full_amount=0;
            int flag = -1;
            for(int i=0;i<DEFBUFSIZE;i++){
                if(hTable[i].frame_id!=-1){
                    full_amount++;
                }
            }
            if(full_amount!=DEFBUFSIZE){
                for(int i=0;i<DEFBUFSIZE;i++){
                    if(hTable[i].frame_id==-1){
                        flag=i;
                        return flag;
                    }
                }
            }
            else{
                int max_count=0;
            for (int i=0;i<DEFBUFSIZE;i++){
                if(hTable[i].count>=max_count){
                    max_count=hTable[i].count;
                }
            }
            for(int i=0;i<DEFBUFSIZE;i++){
                if(hTable[i].count==max_count){
                    flag = i;
                    break;
                }
            }
            }
            return flag;
        }//选择frame将page载入

        // int Hash(int page_id);
        // void RemoveBCB(BCB *ptr,int page_id);
        // void RemoveLRU
};