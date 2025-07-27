#pragma once
#include "output.h"
#include "Type.h"
#include "reg.h"
#include "ALUs.h"
#include "LSB.h"
#include <cstdio>

namespace JaneZ{
struct RSInfo{
    int serial;
    Reg<int> signal;
    Operation op;
    Reg<int> Vj;
    Reg<int> Vk;
    Reg<int> Qj;
    Reg<int> Qk;
    int des;
    int imm = 0;
    Reg<int> res;
    bool isAdd;

    void tick(){
        signal.flush();
        Vj.flush();
        Vk.flush();
        Qj.flush();
        Qk.flush();
    }
};

class RS{
public:
    RSInfo rsALU[1000];
    RSInfo rsLSB[1000];
    Reg<int> head = {0,0};
    Reg<int> tail = {0,0};
    RSInfo input;
    RSOutput output;
    
    class ALU alu;
    class LSB lsb;
    LSBInfo lsbinfo;
    LSBOutput lsboutput;
    RoBOutput specialcheckOutput;

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
                    if(rsALU[i].signal.current == 0){
                        rsALU[i] = input;
                        rsALU[i].signal.update(1);
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
                rsLSB[tail.current] = input;
                tail.update((tail.current + 1)%1000);
                break;
            }
            case JAL:
            case JALR:
            case AUIPC:
            case LUI:
            case EXIT:
                printf("Never reach here!");
                break;
            }
            input.isAdd = false;
        }
    }

    void Check(){
        //rsALU中 第一个 可送去ALU的order 
        for(int i = 0;i < 1000;i ++){
            if(rsALU[i].Qj.current == 0 && rsALU[i].Qk.current == 0){
                if(rsALU[i].signal.current == 1){
                    rsALU[i].res.update(alu.execute(ALUInput{rsALU[i].op,(unsigned) rsALU[i].Vj.current,
                        (unsigned) rsALU[i].Vk.current}));
                    rsALU[i].signal.update(0);
                    //broadcast
                    //output.des = rsALU[i].des;
                    output.res = rsALU[i].res.next;
                    output.flag = true;
                    output.serial = rsALU[i].serial;
                    for(int j = 0;j < 1000;j ++){
                        if(rsALU[j].signal.current == 1){
                            if(rsALU[j].Qj.current == rsALU[i].serial){
                                rsALU[j].Qj.update(0);
                                rsALU[j].Vj.update(output.res);
                            }
                            if(rsALU[j].Qk.current == rsALU[i].serial){
                                rsALU[j].Qk.update(0);
                                rsALU[j].Vk.update(output.res);
                            }
                        }
                    }
                    for(int j = 0;j < 1000;j ++){
                        if(rsLSB[j].signal.current == 1){
                            if(rsLSB[j].Qj.current == rsALU[i].serial){
                                rsLSB[j].Qj.update(0);
                                rsLSB[j].Vj.update(output.res);
                            }
                            if(rsLSB[j].Qk.current == rsALU[i].serial){
                                rsLSB[j].Qk.update(0);
                                rsLSB[j].Vk.update(output.res);
                            }
                        }
                    }
                    break;
                }
            }
        }
        //rsLSB 
        for(int i = 0;i < 1000;i ++){
            if(rsLSB[i].Qj.current == 0 && rsLSB[i].Qk.current == 0){
                if(rsLSB[i].signal.current == 1){
                    lsbinfo.des = rsLSB[i].des;
                    lsbinfo.imm = rsLSB[i].imm;
                    lsbinfo.op = rsLSB[i].op;
                    lsbinfo.serial = rsLSB[i].serial;
                    lsbinfo.Vj = rsLSB[i].Vj.current;
                    lsbinfo.Vk = rsLSB[i].Vk.current;
                    lsbinfo.isAdd = true;
                    rsLSB[i].signal.update(2);
                    break;
                }
            }
        }
    }

    void Deque(){
        if(lsboutput.flag){
            for(int i = 0;i < 1000;i ++){
                if(lsboutput.serial == rsLSB[i].serial){
                    for(int j = 0;j < 1000;j ++){
                        if(rsLSB[j].signal.current == 1){
                            if(rsLSB[j].Qj.current == rsLSB[i].serial){
                                rsLSB[j].Qj.update(0);
                                rsLSB[j].Vj.update(lsboutput.value);
                            }
                            if(rsLSB[j].Qk.current == rsLSB[j].serial){
                                rsLSB[j].Qk.update(0);
                                rsLSB[j].Vk.update(lsboutput.value);
                            }
                        }
                    }
                    for(int j = 0;j < 1000;j ++){
                        if(rsALU[j].signal.current == 1){
                            if(rsALU[j].Qj.current == rsLSB[i].serial){
                                rsALU[j].Qj.update(0);
                                rsALU[j].Vj.update(lsboutput.value);
                            }
                            if(rsALU[j].Qk.current == rsLSB[j].serial){
                                rsALU[j].Qk.update(0);
                                rsALU[j].Vk.update(lsboutput.value);
                            }
                        }
                    }
                    head.update((head.current + 1)%1000);
                    break;
                }
            }
        }
    }

    void specialCheck(){
        if(specialcheckOutput.flag1){
            for(int i = 0;i < 1000;i ++){
                if(rsALU[i].signal.current == 1){
                    if(rsALU[i].Qj.current == specialcheckOutput.serial1){
                        rsALU[i].Qj.update(0);
                        rsALU[i].Vj.update(specialcheckOutput.value);
                    }
                    if(rsALU[i].Qk.current == specialcheckOutput.serial1){
                        rsALU[i].Qk.update(0);
                        rsALU[i].Vk.update(specialcheckOutput.value);
                    }
                }
            }
            for(int i = 0;i < 1000;i ++){
                if(rsLSB[i].signal.current == 1){
                    if(rsLSB[i].Qj.current == specialcheckOutput.serial1){
                        rsLSB[i].Qj.update(0);
                        rsLSB[i].Vj.update(specialcheckOutput.value);
                    }
                    if(rsLSB[i].Qk.current == specialcheckOutput.serial1){
                        rsLSB[i].Qk.update(0);
                        rsLSB[i].Vk.update(specialcheckOutput.value);
                    }
                }
            }    
        }
    }

    //interface
    void run(){
        Enqueue();
        Check();
        specialCheck();
        Deque();
    }

    void tick(){
        for(int i = 0;i < 1000;i ++){
            rsALU[i].tick();
            rsLSB[i].tick();
        }
    }
};
}