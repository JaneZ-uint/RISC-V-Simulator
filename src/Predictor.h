#pragma once
#include "Type.h"
#include "reg.h"

namespace JaneZ{
struct PredictorOutput{
    int new_pc;
    int serial;
    bool flag = false;
};

struct ALUInfo{
    int serial;
    int res;
    bool flag = false;
};

struct BranchInfo{
    int pc;
    int offset;
    Operation op;
    int serial;
    bool flag = false;

    void tick(){
        //flag.flush();
    }
};


class Predictor{
public:
    BranchInfo predictor[100000];
    BranchInfo input;
    ALUInfo inputALU;
    PredictorOutput output;
    
    void run(){
        if(input.flag){
            predictor[input.serial] = input;
            input.flag = false;
        }
        if(inputALU.flag){
            if(inputALU.res == 1){
                output.new_pc = predictor[inputALU.serial].pc + predictor[inputALU.serial].offset;
                output.serial = inputALU.serial;
                output.flag = true;
                flush();
            }
            inputALU.flag = false;
        }
    }

    void flush(){
        // if(output.flag){
        //     for(int i = output.serial;i < 10000;i ++){
        //         if(predictor[i].flag){
        //             predictor[i].flag = false;
        //         }
        //     }
        // }
    }

    void tick(){
        // for(int i = 0;i < 1000;i ++){
        //     predictor[i].tick();
        // }
    }
};
}