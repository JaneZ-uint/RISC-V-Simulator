#pragma once
#include "Queue.h"
#include "RS.h"
#include "Type.h"
#include "reg.h"

namespace JaneZ{
struct RoBInfo{
    int serial;
    Reg<bool> isBusy = {false,false};
    Reg<bool> isReady = {false,false};
    int des;// target reg
    Reg<int> value;
    Operation op;
    int pc;
    int imm;

    void tick(){
        isBusy.flush();
        isReady.flush();
        value.flush();
    }
};
struct RoBOutput{
    int des1;
    int serial1;
    int des2;
    int serial2;
    int value;
};

class RoB{
private:
    void enQueue(){
        if(isAdd){
            if(input.op == JAL){
                output.des2 = input.des;
                output.serial2 = input.serial;
                input.value.update(input.pc + 4);
                rob[tail.current] = input;
                tail.update((tail.current + 1)%1000);
            }else if(input.op == AUIPC){
                output.des2 = input.des;
                output.serial2 = input.serial;
                int res = input.imm << 12;
                input.value.update(input.pc + res);
                rob[tail.current] = input;
                tail.update((tail.current + 1)%1000);
            }else if(input.op == LUI){
                output.des2 = input.des;
                output.serial2 = input.serial;
                int res = input.imm << 12;
                input.value.update(res);
                rob[tail.current] = input;
                tail.update((tail.current + 1)%1000);
            }else{
                output.des2 = input.des;
                output.serial2 = input.serial;
                rob[tail.current] = input;
                tail.update((tail.current + 1)%1000);
            }
        }
    }

    void deQue(){
        if(!rob[head.current].isBusy.current && rob[head.current].isReady.current){
            output.des1 = rob[head.current].des;
            output.serial1 = rob[head.current].serial;
            output.value = rob[head.current].value.current;
            head.update((head.current + 1)%1000);
        }
    }

    void update(){
        if(InfoFromRS.flag){
            for(int i = 0;i < 1000;i ++){
                if(rob[i].serial == InfoFromRS.serial){
                    rob[i].value.update(InfoFromRS.res);
                    rob[i].isReady.update(true);
                    break;
                }
            }
        }
    }

public:
    RoBInfo rob[1000];
    Reg<int> head = {0,0};
    Reg<int> tail = {0,0};
    RoBInfo input;
    RoBOutput output;
    bool isAdd;
    RSOutput InfoFromRS;


    void run(){
        enQueue();
        deQue();
        update();
    }

    void tick(){
        for(int i = 0;i < 1000;i ++){
            rob[i].tick();
        }
        head.flush();
        tail.flush();
    }

    void flush(){
        //TODO
    }
};
}