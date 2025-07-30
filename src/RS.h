#pragma once
#include "output.h"
#include "Type.h"
#include "ALUs.h"
#include "LSB.h"
#include "Predictor.h"
#include <cstdio>

namespace JaneZ{
struct RSInfo{
    int serial;
    int signal;
    Operation op;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int des;
    int imm = 0;
    int res;
    bool isAdd = false;

    // void tick(){
    //     signal.flush();
    //     Vj.flush();
    //     Vk.flush();
    //     Qj.flush();
    //     Qk.flush();
    // }
};

class RS{
public:
    RSInfo rsALU[1000];
    RSInfo rsLSB[1000];
    int head = 0;
    int tail = 0;
    RSInfo input;
    RSOutput output;
    
    class ALU alu;
    class LSB lsb;
    LSBInfo lsbinfo;
    LSBOutput lsboutput;
    RoBOutput specialcheckOutput;
    PredictorOutput info;
    ALUInfo toPredictor;

    void Enqueue(){
        if(input.isAdd){
            switch (input.op) {
            case ADD:
            case SUB:
            case AND:
            case OR:
            case XOR:
            case SLL:
            case SRL:
            case SRA:
            case SLT:
            case SLTU:
            case ADDI:
            case ANDI:
            case ORI:
            case XORI:
            case SLLI:
            case SRLI:
            case SRAI:
            case SLTI:
            case SLTIU:
            case BEQ:
            case BGE:
            case BGEU:
            case BLT:
            case BLTU:
            case BNE: {
                for(int i = 0;i < 1000;i ++){
                    if(rsALU[i].signal == 0){
                        rsALU[i] = input;
                        rsALU[i].signal = 1;
                        break;
                    }
                }
                break;
            }
            case LB:
            case LBU:
            case LH:
            case LHU:
            case LW:
            case SB:
            case SH:
            case SW: {
                rsLSB[tail] = input;
                rsLSB[tail].signal = 1;
                tail = (tail + 1) % 1000;
                break;
            }
            case EXIT: {
                break;
            }
            case JAL:
            case JALR:
            case AUIPC:
            case LUI:
                // printf("Never reach here!");
                break;
            }
            input.isAdd = false;
        }
    }

    void Check(){
        //rsALU中 第一个 可送去ALU的order 
        for(int i = 0;i < 1000;i ++){
            if(rsALU[i].Qj == 0 && rsALU[i].Qk == 0){
                if(rsALU[i].signal == 1){
                    rsALU[i].res = alu.execute(ALUInput{rsALU[i].op, rsALU[i].Vj,
                        rsALU[i].Vk,rsALU[i].imm});
                    if(rsALU[i].op == BEQ || rsALU[i].op == BGE || rsALU[i].op == BGEU || rsALU[i].op == BLT || rsALU[i].op == BLTU || rsALU[i].op == BNE){
                        toPredictor.serial = rsALU[i].serial;
                        toPredictor.res = rsALU[i].res;
                        toPredictor.flag = true;
                    }
                    rsALU[i].signal = 0;
                    //broadcast
                    //output.des = rsALU[i].des;
                    output.res = rsALU[i].res;
                    output.flag = true;
                    output.serial = rsALU[i].serial;
                    for(int j = 0;j < 1000;j ++){
                        if(rsALU[j].signal == 1){
                            if(rsALU[j].Qj == rsALU[i].serial){
                                rsALU[j].Qj = 0;
                                rsALU[j].Vj = output.res;
                            }
                            if(rsALU[j].Qk == rsALU[i].serial){
                                rsALU[j].Qk = 0;
                                rsALU[j].Vk = output.res;
                            }
                        }
                    }
                    for(int j = 0;j < 1000;j ++){
                        if(rsLSB[j].signal == 1){
                            if(rsLSB[j].Qj == rsALU[i].serial){
                                rsLSB[j].Qj = 0;
                                rsLSB[j].Vj = output.res;
                            }
                            if(rsLSB[j].Qk == rsALU[i].serial){
                                rsLSB[j].Qk = 0;
                                rsLSB[j].Vk = output.res;
                            }
                        }
                    }
                    break;
                }
            }
        }
        //rsLSB 
        /*for(int i = 0;i < 1000;i ++){
            if(rsLSB[i].Qj == 0 && rsLSB[i].Qk == 0){
                if(rsLSB[i].signal == 1){
                    lsbinfo.des = rsLSB[i].des;
                    lsbinfo.imm = rsLSB[i].imm;
                    lsbinfo.op = rsLSB[i].op;
                    lsbinfo.serial = rsLSB[i].serial;
                    lsbinfo.Vj = rsLSB[i].Vj;
                    lsbinfo.Vk = rsLSB[i].Vk;
                    lsbinfo.isAdd = true;
                    rsLSB[i].signal = 2;
                    break;
                }
            }
        }*/
        if(rsLSB[head].Qj == 0 && rsLSB[head].Qk == 0){
            if(rsLSB[head].signal == 1){
                lsbinfo.des = rsLSB[head].des;
                lsbinfo.imm = rsLSB[head].imm;
                lsbinfo.op = rsLSB[head].op;
                lsbinfo.serial = rsLSB[head].serial;
                lsbinfo.Vj = rsLSB[head].Vj;
                lsbinfo.Vk = rsLSB[head].Vk;
                lsbinfo.isAdd = true;
                rsLSB[head].signal = 2;
            }
        }
    }

    void Deque(){
        if(lsboutput.flag){
            for(int i = 0;i < 1000;i ++){ 
                if(lsboutput.serial == rsLSB[i].serial){
                    for(int j = 0;j < 1000;j ++){
                        if(rsLSB[j].signal == 1){
                            if(rsLSB[j].Qj == rsLSB[i].serial){
                                rsLSB[j].Qj = 0;
                                rsLSB[j].Vj = lsboutput.value;
                            }
                            if(rsLSB[j].Qk == rsLSB[j].serial){
                                rsLSB[j].Qk = 0;
                                rsLSB[j].Vk = lsboutput.value;
                            }
                        }
                    }
                    for(int j = 0;j < 1000;j ++){
                        if(rsALU[j].signal == 1){
                            if(rsALU[j].Qj == rsLSB[i].serial){
                                rsALU[j].Qj = 0;
                                rsALU[j].Vj = lsboutput.value;
                            }
                            if(rsALU[j].Qk == rsLSB[j].serial){
                                rsALU[j].Qk = 0;
                                rsALU[j].Vk = lsboutput.value;
                            }
                        }
                    }
                    head = (head + 1)%1000;
                    break;
                }
            }
            lsboutput.flag = false;
        }
    }

    void specialCheck(){
        if(specialcheckOutput.flag1){
            for(int i = 0;i < 1000;i ++){
                if(rsALU[i].signal == 1){
                    if(rsALU[i].Qj == specialcheckOutput.serial1){
                        rsALU[i].Qj = 0;
                        rsALU[i].Vj = specialcheckOutput.value;
                    }
                    if(rsALU[i].Qk == specialcheckOutput.serial1){
                        rsALU[i].Qk = 0;
                        rsALU[i].Vk = specialcheckOutput.value;
                    }
                }
            }
            for(int i = 0;i < 1000;i ++){
                if(rsLSB[i].signal == 1){
                    if(rsLSB[i].Qj == specialcheckOutput.serial1){
                        rsLSB[i].Qj = 0;
                        rsLSB[i].Vj = specialcheckOutput.value;
                    }
                    if(rsLSB[i].Qk == specialcheckOutput.serial1){
                        rsLSB[i].Qk = 0;
                        rsLSB[i].Vk = specialcheckOutput.value;
                    }
                }
            }    
        }
    }

    //interface
    void run(){
        flush();
        Enqueue();
        Check();
        specialCheck();
        Deque();
    }

    void tick(){
        // for(int i = 0;i < 1000;i ++){
        //     rsALU[i].tick();
        //     rsLSB[i].tick();
        // }
    }

    void flush(){
        if(info.flag){
            if(head < tail){
                for(int i = head;i < tail;i ++){
                    if(rsLSB[i].serial > info.serial){
                        tail = i;
                        break;
                    }
                }
            }else if(head > tail){
                bool signal = false;
                for(int i = head;i < 1000;i ++){
                    if(rsLSB[i].serial > info.serial){
                        tail = i;
                        signal = true;
                        break;
                    }
                }
                if(!signal){
                    for(int i = 0;i < tail;i ++){
                        if(rsLSB[i].serial > info.serial){
                            tail = i;
                            signal = true;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < 1000; i ++) {
                if(rsALU[i].serial > info.serial){
                    rsALU[i].signal = 0;
                }
            }
            info.flag = false;
        }
    }
};
}