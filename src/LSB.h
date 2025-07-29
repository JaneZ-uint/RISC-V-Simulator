#pragma once
#include "Type.h"
#include "reg.h"
#include "Memory.h"
#include "Predictor.h"

namespace JaneZ{
struct LSBInfo{
    int serial;
    int Vj;
    int Vk;
    Operation op;
    int des;
    int imm;
    bool isAdd = false;
};

struct LSBOutput{
    int value = 0;
    int serial;
    bool flag = false;
};

class LSB{
private:
public:
    LSBInfo lsb[1000];
    Reg<int> head = {0,0};
    Reg<int> tail = {0,0};
    int cnt = 0;
    LSBInfo input;
    MemInput output;
    MemOutput message;
    LSBOutput lsboutput;
    PredictorOutput info;

    //interface
    void Queue() {
        if(input.isAdd){
            lsb[tail.current] = input;
            tail.update((tail.current + 1)%1000);
            input.isAdd = false;
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
        flush();
        RMB();
    }
    
    void tick() {
        head.flush();
        tail.flush();
    }

    void flush(){
        if(info.flag){
            if(head.current < tail.current){
                for(int i = head.current;i < tail.current;i ++){
                    if(lsb[i].serial >= info.serial){
                        tail.update(i);
                        break;
                    }
                }
            }else if(head.current > tail.current){
                bool signal = false;
                for(int i = head.current;i < 1000;i ++){
                    if(lsb[i].serial >= info.serial){
                        tail.update(i);
                        signal = true;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail.current;i ++){
                        if(lsb[i].serial >= info.serial){
                            tail.update(i);
                            signal = true;
                            break;
                        }
                    }
                }
            }
        }
    }
};
}