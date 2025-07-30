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
    void enQueue(){
        if(input.isAdd){
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
                tail = (tail + 1)%1000;
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
                tail = (tail + 1)%1000;
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
                tail = (tail + 1)%1000;
            } else if (input.op == EXIT) {
                rob[tail] = input;
                rob[tail].isReady = true;
                tail = (tail + 1)%1000;
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
                tail = (tail + 1)%1000;
            }
            input.isAdd = false;
        }
    }

    void deQue(){
        if(rob[head].op == JALR ){
            rob[head].isReady = true;
            jalrsignal.isOK = true;
        }
        if(!rob[head].isBusy && rob[head].isReady  && head != tail){
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
            output.des1 = rob[head].des;
            output.serial1 = rob[head].serial;
            if(rob[head].des == 0){
                output.value = 0;
            }else{
                output.value = rob[head].value;
            }
            output.flag1 = true;
            head = (head + 1)%1000;
        }
    }

    void update(){
        if(InfoFromRS.flag){
            for(int i = 0;i < 1000;i ++){
                if(rob[i].serial == InfoFromRS.serial){
                    rob[i].value = InfoFromRS.res;
                    rob[i].isReady = true;
                    break;
                }
            }
        }
        if(InfoFromLSB.flag){
            for(int i = 0;i < 1000;i ++){
                if(rob[i].serial == InfoFromLSB.serial){
                    rob[i].value = InfoFromLSB.value;
                    rob[i].isReady = true;
                    break;
                }
            }
        }
        
    }

public:
    RoBInfo rob[1000];
    int head = 0;
    int tail = 0;
    RoBInfo input;
    RoBOutput output;
    
    RSOutput InfoFromRS;
    RoBPC robpc;
    RoBToMEM robtomem;
    PredictorOutput info, isFlush;

    LSBOutput InfoFromLSB;
    JALRSignal jalrsignal;
    bool isControl = false;

    void run(){
        flush();
        enQueue();
        deQue();
        update();
        robtomem.serial = rob[head].serial;
    }

    void tick(){
        for(int i = 0;i < 1000;i ++){
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
                        break;
                    }
                }
            }else if(head > tail){
                bool signal = false;
                for(int i = head;i < 1000;i ++){
                    if(rob[i].serial > info.serial){
                        tail = i;
                        signal = true;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail;i ++){
                        if(rob[i].serial > info.serial){
                            tail = i;
                            signal = true;
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