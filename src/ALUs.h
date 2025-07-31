#pragma once
#include "Type.h"
namespace JaneZ {
struct ALUInput{
    Operation op;
    int rs1;
    int rs2;
    int imm;
};

class ALU {
public:
    unsigned int execute(ALUInput current){
        unsigned int value = 0;
        if(current.op == ADD){
            value = current.rs1 + current.rs2;
        }else if(current.op == SUB){
            value = current.rs1 - current.rs2;
        }else if(current.op == AND){
            value = current.rs1 & current.rs2;
        }else if(current.op == OR){
            value = current.rs1 | current.rs2;
        }else if(current.op == XOR){
            value = current.rs1 ^ current.rs2;
        }else if(current.op == SLL){
            value = current.rs1 << current.rs2;
        }else if(current.op == SRL){
            value = static_cast<unsigned int>(current.rs1) >> current.rs2;
        }else if(current.op == SRA){
            value = current.rs1 >> current.rs2;
        }else if(current.op == SLT){
            value = current.rs1 < current.rs2;
        }else if(current.op == SLTU){
            value = static_cast<unsigned int>(current.rs1) < static_cast<unsigned int>(current.rs2);
        }else if(current.op == ADDI){
            value = current.rs1 + current.imm;
        }else if(current.op == ANDI){
            value = current.rs1 & current.imm;
        }else if(current.op == ORI){
            value = current.rs1 | current.imm;
        }else if(current.op == XORI){
            value = current.rs1 ^ current.imm;
        }else if(current.op == SLLI){
            value = current.rs1 << current.imm;
        }else if(current.op == SRLI){
            value = static_cast<unsigned int>(current.rs1) >> current.imm;
        }else if(current.op == SRAI){
            value = current.rs1 >> current.imm;
        }else if(current.op == SLTI){
            value = current.rs1 < current.imm;
        }else if(current.op == SLTIU){
            value = static_cast<unsigned int>(current.rs1) < static_cast<unsigned int>(current.imm);
        }else if(current.op == BEQ){
            value = (current.rs1 == current.rs2);
        }else if(current.op == BGE){
            value = current.rs1 >= current.rs2;
        }else if(current.op == BGEU){
            value = static_cast<unsigned int>(current.rs1) >= static_cast<unsigned int>(current.rs2);
        }else if(current.op == BLT){
            value = current.rs1 < current.rs2;
        }else if(current.op == BLTU){
            value = static_cast<unsigned int>(current.rs1) < static_cast<unsigned int>(current.rs2);
        }else if(current.op == BNE){
            value = current.rs1 != current.rs2;
        }
        return value;
    }
};
}
