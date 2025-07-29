#pragma once
#include "Type.h"
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
    int head = 0;
    int tail = 0;
    int cnt = 0;
    LSBInfo input;
    MemInput output;
    MemOutput message;
    LSBOutput lsboutput;
    PredictorOutput info;
    InfoFromRoB infofromrob;

    //interface
    void Queue() {
        if(input.isAdd){
            lsb[tail] = input;
            tail = (tail + 1)%1000;
            input.isAdd = false;
        }
        LSBInfo b = lsb[head];
        if(cnt == 0){
            if(head != tail){
                if(b.op == SB || b.op == SH || b.op == SW){
                    if(b.serial != infofromrob.serial){
                        return;
                    }
                }
                cnt = (cnt + 1) % 3;
                head = (head + 1) % 1000;
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
        message.flag = false;
    }

    void run(){
        Queue();
        flush();
        RMB();
    }
    
    void tick() {
        // head.flush();
        // tail.flush();
    }

    void flush(){
        if(info.flag){
            if(head < tail){
                for(int i = head;i < tail;i ++){
                    if(lsb[i].serial >= info.serial){
                        tail = i;
                        break;
                    }
                }
            }else if(head > tail){
                bool signal = false;
                for(int i = head;i < 1000;i ++){
                    if(lsb[i].serial >= info.serial){
                        tail = i;
                        signal = true;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail;i ++){
                        if(lsb[i].serial >= info.serial){
                            tail = i;
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