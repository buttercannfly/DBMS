#include<iostream>
#include<stdio.h>
#include<fstream>
#include<vector>
#include<typeinfo>
#include<stdlib.h>
#include<cstdio>
// #include"BCB.h"
#include"BMgr.h"
// #include"DSMgr.h"
using namespace std;
FILE* create_file();
void init();
#define N 50000
int main(){
    //创建文件
    vector<int> cmds;
    vector<int> page_ids;
    FILE* fp=create_file();
    Bmgr bmgr;
    char b[] = "q";//q代表未写前
    for(int a=0;a<N;a++){
        fwrite(b, FRAMESIZE, 1, fp);
    }
    FILE* target=fopen("data-5w-50w-zipf.txt","r+");//输入
    cout<<ftell(target)<<endl;
    int i=0;
    while(!feof(target)){
        int a=-1,b=-1;
        fscanf(target,"%d,%d\n",&a,&b);
        i++;
        if(i%10000==0){
            int tmp=i/10000;
            cout<<"page already read:"<< i <<"  hit_num:"<<bmgr.hit_num<<"  hit_rate:"<<float(bmgr.hit_num/(i*1.0))<<" IO_num:"<<bmgr.dsmgr.IO_nums<<endl;
        }
        //读
        if(a==0){
            bmgr.FixPage(b);
        }
        //写
        else if (a==1)
        {
            bmgr.Writeinbuffer(b);
        }
    }
    cout<<"****************************"<<endl;
    cout<<"final_hit_num:"<<bmgr.hit_num<<endl;
    cout<<"final_IO_num:"<<bmgr.dsmgr.IO_nums<<endl;
    bmgr.dsmgr.CloseFile();
    fclose(fp);
    fclose(target);
    return 0;
}
FILE* create_file(){
    FILE *fp;
    fp = fopen("./data.txt","wb+");
    return fp;
}
