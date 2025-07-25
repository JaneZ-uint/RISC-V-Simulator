#pragma once
#include "ALUs.h"
#include "Decoder.h"
#include "LSB.h"
#include "Memory.h"
#include "Predictor.h"
#include "RF.h"
#include "RoB.h"
#include "RS.h"
#include <cstdint>

namespace JaneZ{
class CPU{
private:

public:
    ALU alu;
    Decoder decoder;
    LSB lsb;
    Memory memory;
    Predictor predictor;
    RF rf;
    RoB rob;
    RS rs;
    uint32_t pc;
    int clk = 0;

    unsigned int run(){
        
    }

    
};
}