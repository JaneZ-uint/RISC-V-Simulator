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
     res;
    bool isRead;
};
    
class LSB{
private:
public:
    LSBInfo lsb[1000];
    Reg<int> head = {0,0};
    Reg<int> tail = {0,0};
    int cnt = 0;
    bool isAdd;
    LSBInfo input;
    LSBInfo output;

    void run() {
        if(isAdd){
            lsb[tail.current] = input;
            tail.update(tail.current + 1);
        }
        LSBInfo b = lsb[head.current];
        if(cnt == 0){
            if(!b.isBusy.current){
                cnt = (cnt + 1) % 3;
                head.update(head.current + 1);
                LSBInfo res;
                if(b.op == LB){
                    //TODO
                    output = res;
                   
                }else if(b.op == LBU){
                    //TODO
                    output = res;
                    
                }else if(b.op == LH){
                    //TODO
                    output = res;
                    
                }else if(b.op == LHU){
                    //TODO
                    output = res;
                    
                }else if(b.op == LW){
                    //TODO
                    output = res;
                    
                }else if(b.op == SB){

                }else if(b.op == SH){

                }else if(b.op == SW){

                }
                b.isBusy.current = true;
                b.isBusy.update(b.isBusy.current);
            }else{
                cnt = 0;
            }
        }else{
            cnt = (cnt + 1) % 3;
        }
        if(cnt == 3){
            //TODO
        }else{
            //TODO
        }
    }

    void tick() {
        for(int i = 0;i < 1000;i ++){
            lsb[i].tick();
        }
    }
};
}