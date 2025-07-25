#pragma once
#include "Type.h"
#include "reg.h"
#include "Queue.h"

namespace JaneZ{
struct LSBInfo{
    int serial;
    Reg<bool> isBusy;
    int Vj;
    int Vk;
    Operation op;
    int des;
    int imm;
    void tick(){
        isBusy.flush();
    }
};

struct LSBOutput{
    LSBInfo res;
    bool isRead;
};
    
class LSB{
private:
public:
    LSBInfo lsb[1000];
    Reg<int> head = {0,0};
    Reg<int> tail = {0,0};
    Reg<int> length = {0,0};
    int cnt = 0;

    LSBOutput run(bool isAdd,LSBInfo toAdd) {
        if(isAdd){
            lsb[tail.current] = toAdd;
            tail.current ++;
            tail.update(tail.current);
        }
        LSBInfo b = lsb[head.current];
        LSBOutput res;
        res.isRead = false;
        if(cnt == 0){
            if(!b.isBusy.current){
                LSBInfo output;
                if(b.op == LB){
                    //TODO
                    res.res = output;
                    res.isRead = true;
                }else if(b.op == LBU){
                    //TODO
                    res.res = output;
                    res.isRead = true;
                }else if(b.op == LH){
                    //TODO
                    res.res = output;
                    res.isRead = true;
                }else if(b.op == LHU){
                    //TODO
                    res.res = output;
                    res.isRead = true;
                }else if(b.op == LW){
                    //TODO
                    res.res = output;
                    res.isRead = true;
                }else if(b.op == SB){

                }else if(b.op == SH){

                }else if(b.op == SW){

                }
                b.isBusy.current = true;
                b.isBusy.update(b.isBusy.current);
            }
        }
        cnt ++;
        return res;
    }

    void tick() {
        for(int i = 0;i < 100;i ++){
            lsb[i].tick();
        }
    }
};
}