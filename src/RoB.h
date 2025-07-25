#pragma once
#include "Queue.h"
namespace JaneZ{
struct RoBInfo{
    int serial;
    bool isBusy;
    bool isReady;
    int des;
    int value;
};

class RoB{
public:
    Queue<RoBInfo> rob;
    RoBInfo toAdd;//待加入指令
    RoBInfo toLeave;//出队指令

    void run(){
        if(!rob.isFull() && toAdd.isBusy){
            rob.push(toAdd);
        }
        if(toLeave.isBusy){
            return;
        }
        RoBInfo head = rob.front();
        if(head.isBusy && head.isReady){
            rob.pop();
            toLeave = head;
        }
    }
};
}