#pragma once
#include "Type.h"
namespace JaneZ {
struct ALUInput{
    Operation op;
    unsigned int left;
    unsigned int right;
};

class ALU {
public:
    unsigned int execute(ALUInput current){
        unsigned int value = 0;
        if(current.op == ADD){
            value = current.left + current.right;
        }else if(current.op == SUB){
            value = current.left - current.right;
        }else if(current.op == AND){
            value = current.left & current.right;
        }else if(current.op == OR){
            value = current.left | current.right;
        }else if(current.op == XOR){
            value = current.left ^ current.right;
        }else if(current.op == SLL){
            value = current.left << current.right;
        }else if(current.op == SRL){
            value = current.left >> current.right;
        }else if(current.op == SRA){
            value = static_cast<int>(current.left) >> current.right;
        }else if(current.op == SLT){
            value = static_cast<int>(current.left) < static_cast<int>(current.right);
        }else if(current.op == SLTU){
            value = current.left < current.right;
        }else if(current.op == ADDI){
            value = current.left + current.right;
        }else if(current.op == ANDI){
            value = current.left & current.right;
        }else if(current.op == ORI){
            value = current.left | current.right;
        }else if(current.op == XORI){
            value = current.left ^ current.right;
        }else if(current.op == SLLI){
            value = current.left << current.right;
        }else if(current.op == SRLI){
            value = current.left >> current.right;
        }else if(current.op == SRAI){
            value = static_cast<int>(current.left) >> current.right;
        }else if(current.op == SLTI){
            value = static_cast<int>(current.left) < static_cast<int>(current.right);
        }else if(current.op == SLTIU){
            value = current.left < current.right;
        }else if(current.op == LB){
            value = current.left + current.right;
        }else if(current.op == LBU){
            value = current.left + current.right;
        }else if(current.op == LH){
            value = current.left + current.right;
        }else if(current.op == LHU){
            value = current.left + current.right;
        }else if(current.op == LW){
            value = current.left + current.right;
        }else if(current.op == SB){
            value = current.left + current.right;
        }else if(current.op == SH){
            value = current.left + current.right;
        }else if(current.op == SW){
            value = current.left + current.right;
        }else if(current.op == BEQ){
            value = (current.left == current.right);
        }else if(current.op == BGE){
            value = static_cast<int>(current.left) >= static_cast<int>(current.right);
        }else if(current.op == BGEU){
            value = current.left >= current.right;
        }else if(current.op == BLT){
            value = static_cast<int>(current.left) < static_cast<int>(current.right);
        }else if(current.op == BLTU){
            value = current.left < current.right;
        }else if(current.op == BNE){
            value = current.left != current.right;
        }else if(current.op == JAL){
            value = current.left + current.right;
        }else if(current.op == JALR){
            value = current.left + current.right;
        }else if(current.op == AUIPC){
            value = current.left + current.right;
        }else if(current.op == LUI){
            value = current.left + current.right;
        }
        return value;
    }
};
}
