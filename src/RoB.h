#pragma once
#include "output.h"
#include "Type.h"
#include "RF.h"
#include "reg.h"
#include "LSB.h"
#include "Predictor.h"
extern bool isTerminal;
extern JaneZ::RF rf;

namespace JaneZ{
struct RoBInfo{
    int serial;
    bool isBusy = false;
    bool isReady = false;
    int des;// target reg
    int value;
    Operation op;
    int pc;
    int imm;
    bool isAdd = false;
    int rs1;

    void tick(){
        // isBusy.flush();
        // isReady.flush();
        // value.flush();
    }
};

struct RoBPC{
    bool flag = false;
    int v1;
    int offset;
    int pc;
    bool isJALR = false;
};

struct RoBToMEM{
    int serial;
};

struct JALRSignal{
    bool isOK = false;
};

class RoB{
private:
static const int SIZE=10;

    void enQueue(){
        if(input.isAdd){
            if(isfull()){
                isFull = true;
                return;
            }
            isFull = false;
            length ++;
            output.flag2 = true;
            if(input.op == JAL){
                output.des2 = input.des;
                if(input.des == 0){
                    output.serial2 = 0;
                }else{
                    output.serial2 = input.serial;
                }
                input.value = input.pc + 4;
                rob[tail] = input;
                rob[tail].isReady = true;
                tail = (tail + 1)%SIZE;
            }else if(input.op == AUIPC){
                output.des2 = input.des;
                //output.serial2 = input.serial;
                if(input.des == 0){
                    output.serial2 = 0;
                }else{
                    output.serial2 = input.serial;
                }
                int res = input.imm << 12;
                input.value = input.pc + res;
                rob[tail] = input;
                rob[tail].isReady = true;
                tail = (tail + 1)%SIZE;
            }else if(input.op == LUI){
                output.des2 = input.des;
                //output.serial2 = input.serial;
                if(input.des == 0){
                    output.serial2 = 0;
                }else{
                    output.serial2 = input.serial;
                }
                int res = input.imm << 12;
                input.value = res;
                rob[tail] = input;
                rob[tail].isReady = true;
                tail = (tail + 1)%SIZE;
            } else if (input.op == EXIT) {
                rob[tail] = input;
                rob[tail].isReady = true;
                tail = (tail + 1)%SIZE;
            } else{
                if(input.op != SB && input.op != SH && input.op != SW && input.op != BEQ && input.op != BGE && input.op != BGEU && input.op != BLT && input.op != BLTU && input.op != BNE){
                    output.des2= input.des;
                    //output.serial2 = input.serial;
                    if(input.des == 0){
                        output.serial2 = 0;
                    }else{
                        output.serial2 = input.serial;
                    }
                }else{
                    output.flag2 = false;
                }
                rob[tail] = input;
                tail = (tail + 1)%SIZE;
            }
            input.isAdd = false;
        }
    }

    void deQue(){
        if(rob[head].op == JALR ){
            rob[head].isReady = true;
            jalrsignal.isOK = true;
        }
        if(!rob[head].isBusy && rob[head].isReady  && length != 0){
            length --;
            if(rob[head].op == EXIT){
                isTerminal = true;
                return;
            }
            switch (rob[head].op) {
                case BEQ:
                case BGE:
                case BGEU:
                case BLT:
                case BLTU: 
                case BNE:{
                    isFlush.flag = rob[head].value;
                    isFlush.new_pc = rob[head].pc + rob[head].imm;
                    isFlush.serial = rob[head].serial;
                    break;
                };
                case JAL:{
                    // robpc.flag = true;
                    // robpc.offset = rob[head].imm;
                    // robpc.isJALR = false;
                    // robpc.pc = rob[head].pc;
                    break;
                }
                case JALR: {
                    robpc.flag = true;
                    robpc.isJALR = true;
                    robpc.offset = rob[head].imm;
                    robpc.pc = rob[head].pc;
                    robpc.v1 = rf.value[rob[head].rs1];
                    break;
                }
                default: {
                    robpc.flag = false;
                    break;
                }
            }
            if(rob[head].op == BEQ || rob[head].op == BGE || rob[head].op == BGEU || rob[head].op == BLT || rob[head].op == BLTU || rob[head].op == BNE || rob[head].op == SB || rob[head].op == SH || rob[head].op == SW){
                output.flag1 = false;
                head = (head + 1)%SIZE;
                return;
            }
            output.des1 = rob[head].des;
            output.serial1 = rob[head].serial;
            if(rob[head].des == 0){
                output.value = 0;
            }else{
                output.value = rob[head].value;
            }
            output.flag1 = true;
            head = (head + 1)%SIZE;
        }
    }

    void update(){
        if(InfoFromRS.flag){
            for(int i = 0;i < SIZE;i ++){
                if(rob[i].serial == InfoFromRS.serial){
                    rob[i].value = InfoFromRS.res;
                    rob[i].isReady = true;
                    break;
                }
            }
        }
        if(InfoFromLSB.flag){
            for(int i = 0;i < SIZE;i ++){
                if(rob[i].serial == InfoFromLSB.serial){
                    rob[i].value = InfoFromLSB.value;
                    rob[i].isReady = true;
                    break;
                }
            }
        }
        
    }

public:
    RoBInfo rob[SIZE];
    int head = 0;
    int tail = 0;
    int length = 0;
    RoBInfo input;
    RoBOutput output;
    
    RSOutput InfoFromRS;
    RoBPC robpc;
    RoBToMEM robtomem;
    PredictorOutput info, isFlush;

    LSBOutput InfoFromLSB;
    JALRSignal jalrsignal;
    bool isControl = false;

    bool isFull = false;

    bool isfull(){
        return length == SIZE;
    }

    void run(){
        robtomem.serial = rob[head].serial;
        flush();
        enQueue();
        deQue();
        update();
    }

    void tick(){
        for(int i = 0;i < SIZE;i ++){
            rob[i].tick();
        }
        // head.flush();
        // tail.flush();
    }

    void flush(){
        if(info.flag){
            if(head < tail){
                for(int i = head;i < tail;i ++){
                    if(rob[i].serial > info.serial){
                        tail = i;
                        length = i - head;
                        break;
                    }
                }
            }else if(head > tail){
                bool signal = false;
                for(int i = head;i < SIZE;i ++){
                    if(rob[i].serial > info.serial){
                        tail = i;
                        signal = true;
                        length = i - head;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail;i ++){
                        if(rob[i].serial > info.serial){
                            tail = i;
                            signal = true;
                            length = SIZE - head + i;
                            break;
                        }
                    }
                }
            }else if(head == tail && length != 0){
                bool signal = false;
                for(int i = head;i < SIZE;i ++){
                    if(rob[i].serial > info.serial){
                        tail = i;
                        signal = true;
                        length = i - head;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail;i ++){
                        if(rob[i].serial > info.serial){
                            tail = i;
                            signal = true;
                            length = SIZE - head + i;
                            break;
                        }
                    }
                }
            }
            info.flag = false;
        }
    }
};
}