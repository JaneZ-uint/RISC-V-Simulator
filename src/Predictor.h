#pragma once
#include "Type.h"
#include "reg.h"

namespace JaneZ{
class Predictor{
struct BranchInfo{
    int pc;
    int offset;
    Operation op;
    int serial;
    Reg<bool> flag;

    void tick(){
        flag.flush();
    }
};

struct ALUInfo{
    int serial;
    int res;
};

struct PredictorOutput{
    int new_pc;
    int serial;
};

public:
    BranchInfo predictor[1000];
    BranchInfo input;
    ALUInfo inputALU;
    PredictorOutput output;
    bool isAdd;
    bool isALU;
    
    void run(){
        if(isAdd){
            predictor[input.serial] = input;
        }
        if(isALU){
            if(inputALU.res == 1){
                //wrong
                output.new_pc = input.pc + input.offset;
                output.serial = input.serial;
                flush();
            }
        }
    }

    void flush(){
        for(int i = output.serial;i < 1000;i ++){
            if(predictor[i].flag.current){
                predictor[i].flag.update(false);
            }
        }
    }

    void tick(){
        for(int i = 0;i < 1000;i ++){
            predictor[i].tick();
        }
    }
};
}