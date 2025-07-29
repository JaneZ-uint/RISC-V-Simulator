#pragma once
#include "RF.h"
#include "Type.h"
#include <exception>
extern JaneZ::RF rf;

namespace JaneZ{
struct DecodeRes{
    Operation op;
    int rs1 = 0;
    int q1 = -1;
    int v1;
    int rs2 = 0;
    int q2 = -1;
    int v2;
    int imm = 0;
    int rd = 0;
    int serial;
};

class Decoder{
public:
    int cnt = 0;

    DecodeRes getType(unsigned char first,unsigned char second,unsigned char third,unsigned char fourth){
        DecodeRes current;
        current.serial = ++ cnt;
        unsigned char order[4];
        order[0] = fourth;
        order[1] = third;
        order[2] = second;
        order[3] = first;
        //37 01 02 00
        //0x 00 02 01 37
        int opcode = order[3] & 0b01111111;
        if(opcode == 0b0110011){  //Type R 33 B3
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            int func = 0;
            func = (order[2] >> 4 ) & 0b0111;
            if(func == 0){
                int funct7 = 0;
                funct7 = order[0] >> 1;
                if(funct7 == 0){
                    current.op = ADD;
                }else if(funct7 == 0b0100000){
                    current.op = SUB;
                }
            }else if(func == 1){
                current.op = SLL;
            }else if(func == 2){
                current.op = SLT;
            }else if(func == 3){
                current.op = SLTU;
            }else if(func == 4){
                current.op = XOR;
            }else if(func == 5){
                int funct7 = 0;
                funct7 = order[0] >> 1;
                if(funct7 == 0){
                    current.op = SRL;
                }else if(funct7 == 0b0100000){
                    current.op = SRA;
                }
            }else if(func == 6){
                current.op = OR;
            }else if(func == 7){
                current.op = AND;
            }
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.rs2 = (order[0] & 1) << 4;
            current.rs2 += (order[1] >> 4) & 0b01111;
        }else if(opcode == 0b0010011){  //Type I & I* 93 13
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            int func = 0;
            func = (order[2] >> 4 ) & 0b0111;
            if(func == 0){
                current.op = ADDI;
            }else if(func == 1){
                current.op = SLLI;
            }else if(func == 2){
                current.op = SLTI;
            }else if(func == 3){
                current.op = SLTIU;
            }else if(func == 4){
                current.op = XORI;
            }else if(func == 5){
                int funct7 = 0;
                funct7 = order[0] >> 1;
                if(funct7 == 0){
                    current.op = SRLI;
                }else if(funct7 == 0b0100000){
                    current.op = SRAI;
                }
            }else if(func == 6){
                current.op = ORI;
            }else if(func == 7){
                current.op = ANDI;
            }
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.imm = order[0] << 4;
            current.imm += order[1] >> 4;
            if(current.op == ADDI && current.rs1 == 0 && current.rd == 10 && current.imm == 255){
                current.op = EXIT;
            }
            if(current.op == ADDI || current.op == ANDI || current.op == ORI || current.op == XORI || current.op == SLTI){
                if(current.imm >> 11){
                    current.imm = current.imm | 0xfffff000;
                }
            }
            if(func == 5 || func == 1){  //Type I*
                current.imm = current.imm & 0b011111;
            }
        }else if(opcode == 0b0000011){  //Type I 83
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            int func = 0;
            func = (order[2] >> 4 ) & 0b0111;
            if(func == 0){
                current.op = LB;
            }else if(func == 1){
                current.op = LH;
            }else if(func == 2){
                current.op = LW;
            }else if(func == 4){
                current.op = LBU;
            }else if(func == 5){
                current.op = LHU;
            }
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.imm = order[0] << 4;
            current.imm += order[1] >> 4;
            if(current.imm >> 11){
                current.imm = current.imm | 0xfffff000;
            }
        }else if(opcode == 0b1100111){  //Type I jalr 67
            current.op = JALR;
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.imm = order[0] << 4;
            current.imm += order[1] >> 4;
        }else if(opcode == 0b0100011){  //Type S 23
            int func = 0;
            func = (order[2] >> 4 ) & 0b0111;
            if(func == 0){
                current.op = SB;
            }else if(func == 1){
                current.op = SH;
            }else if(func == 2){
                current.op = SW;
            }
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.rs2 = (order[0] & 1) << 4;
            current.rs2 += (order[1] >> 4) & 0b01111;
            current.rd = current.rs2;
            current.imm = order[0] >> 1;
            current.imm = current.imm << 5;
            current.imm += (order[2] & 0b01111) << 1;
            current.imm += (order[3] >> 7) & 1;
            if(current.imm >> 11){
                current.imm = current.imm | 0xfffff000;
            }
        }else if(opcode == 0b1100011){  //Type B 63 E3
            int func = 0;
            func = (order[2] >> 4 ) & 0b0111;
            if(func == 0){
                current.op = BEQ;
            }else if(func == 1){
                current.op = BNE;
            }else if(func == 4){
                current.op = BLT;
            }else if(func == 5){
                current.op = BGE;
            }else if(func == 6){
                current.op = BLTU;
            }else if(func == 7){
                current.op = BGEU;
            }
            current.rs1 = (order[1] & 0b01111) << 1;
            current.rs1 += (order[2] >> 7) & 1;
            current.rs2 = (order[0] & 1) << 4;
            current.rs2 += (order[1] >> 4) & 0b01111;
            current.imm = (order[0] >> 7) & 1;
            current.imm = current.imm << 1;
            current.imm += (order[3] >> 7) & 1;
            current.imm = current.imm << 6;
            current.imm += (order[0] >> 1) & 0b0111111;
            current.imm = current.imm << 4;
            current.imm += order[2] & 0b01111;
            current.imm = current.imm << 1;
        }else if(opcode == 0b1101111){  //Type J 6F EF
            current.op = JAL;
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            current.imm = (order[0] >> 7) & 1;
            current.imm = current.imm << 8;
            current.imm += (order[1] & 0b01111) << 4;
            current.imm += (order[2] >> 4) & 0b01111;
            current.imm = current.imm << 1;
            current.imm += (order[1] >> 4) & 1;
            current.imm = current.imm << 7;
            current.imm += order[0] & 0b01111111;
            current.imm = current.imm << 3;
            current.imm += (order[1] >> 5) & 0b0111;
            current.imm = current.imm << 1;
        }else if(opcode == 0b0110111){  //Type U lui 37 
            current.op = LUI;
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            current.imm = order[0];
            current.imm = current.imm << 8;
            current.imm += order[1];
            current.imm = current.imm << 4;
            current.imm += (order[2] >> 4) & 0b01111;
            if(current.imm >> 19){
                current.imm = current.imm | 0xfff00000;
            }
            // current.imm = current.imm << 12;
        }else if(opcode == 0b0010111){  //Type U auipc
            current.op = AUIPC;
            current.rd = order[2] & 0b01111;
            current.rd = current.rd << 1;
            current.rd += (order[3] >> 7) & 1;
            current.imm = order[0];
            current.imm = current.imm << 8;
            current.imm += order[1];
            current.imm = current.imm << 4;
            current.imm += (order[2] >> 4) & 0b01111;
            if(current.imm >> 19){
                current.imm = current.imm | 0xfff00000;
            }
            //current.imm = current.imm << 12;
        }
        current.q1 = rf.dependency[current.rs1];
        if(rf.dependency[current.rs1] == 0){
            current.v1 = rf.value[current.rs1];
        }
        current.q2 = rf.dependency[current.rs2];
        if(rf.dependency[current.rs2] == 0){
            current.v2 = rf.value[current.rs2];
        }
        return current;
    }
};
}