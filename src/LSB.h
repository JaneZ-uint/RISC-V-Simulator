#pragma once
#include "Type.h"
#include "reg.h"
#include "Queue.h"
#include "Memory.h"

namespace JaneZ{
struct LSBInfo{
    int serial;
    int Vj;
    int Vk;
    Operation op;
    int des;
    int imm;
};

struct LSBOutput{
    int value = 0;
    int serial;
    bool flag;
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
    MemInput output;
    MemOutput message;
    LSBOutput lsboutput;

    //interface
    void Queue() {
        if(isAdd){
            lsb[tail.current] = input;
            tail.update((tail.current + 1)%1000);
        }
        LSBInfo b = lsb[head.current];
        if(cnt == 0){
            if(head.current != tail.current){
                cnt = (cnt + 1) % 3;
                head.update((head.current + 1)%1000);
                output.addr = b.Vj + b.imm;
                output.op = b.op;
                output.flag = true;
                output.serial = b.serial;
                output.value = b.Vk;
            }else{
                cnt = 0;
            }
        }else{
            cnt = (cnt + 1) % 3;
        }
    }

    void RMB(){
        lsboutput.serial = message.serial;
        lsboutput.value = message.value;
        lsboutput.flag = message.flag;
    }

    void run(){
        Queue();
        RMB();
    }
    

    void tick() {
        head.flush();
        tail.flush();
    }
};
}