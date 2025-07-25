#pragma once
#include "Queue.h"
#include "reg.h"

namespace JaneZ{
struct RoBInfo{
    int serial;
    bool isBusy = false;
    bool isReady = false;
    int des;// target reg
    int value;
};

class RoB{
public:
    Queue<RoBInfo> rob;
    RoBInfo toAdd; //待加入指令
    RoBInfo toLeave; //出队指令
    Reg<RoBInfo> buffer[100];

    void run(){
        if(!rob.isFull() && toAdd.isBusy){
            rob.push(toAdd);
        }
        if(toLeave.isBusy){
            return;
        }
        RoBInfo head = rob.front();
        if(head.isBusy && head.isReady){
            head.isBusy = false;
            rob.pop();
            toLeave = head;
        }
    }
};
}