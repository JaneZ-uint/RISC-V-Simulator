#pragma once
#include "Type.h"
namespace JaneZ {
const int MEMSize = 1e6;
struct MemInput{
    int addr = 0;// mem address
    int value = 0;// store what
    Operation op;
    int serial;
    bool flag;
};

struct MemOutput{
    int value = 0;
    int serial;
    bool flag;
};

class Memory{
public:
    unsigned char mem[MEMSize] = {};
    MemInput input;
    MemOutput output;
    bool isWork = false;

    void run(){
        if(input.flag){
            switch (input.op) {
                case LB: {
                    output.serial = input.serial;
                    if((mem[input.addr] >> 7) & 1){
                        output.value = mem[input.addr] & 0xffffff00;
                    }else{
                        output.value = mem[input.addr];
                    }
                    break;
                }
                case LBU: {
                    output.serial = input.serial;
                    output.value = mem[input.addr];
                    break;
                }
                case LH: {
                    output.serial = input.serial;
                    int reverse = (mem[input.addr + 1] << 8) + mem[input.addr];
                    if((reverse >> 15) & 1){
                        output.value = reverse & 0xffff0000;
                    }else{
                        output.value = reverse;
                    }
                    break;
                }
                case LHU: {
                    output.serial = input.serial;
                    int reverse = (mem[input.addr + 1] << 8) + mem[input.addr];
                    output.value = reverse;
                    break;
                }
                case LW: {
                    output.serial = input.serial;
                    int reverse = (mem[input.addr + 3] << 24) + (mem[input.addr + 2] << 16) + (mem[input.addr + 1] << 8) + mem[input.addr];
                    output.value = reverse;
                    break;
                }
                case SB: {
                    output.serial = input.serial;
                    mem[input.addr] = input.value;
                    break;
                }
                case SH: {
                    output.serial = input.serial;
                    mem[input.addr] = input.value & 0b01111111;
                    mem[input.addr + 1] = (input.value >> 8) & 0b011111111;
                    break;
                }
                case SW: {
                    output.serial = input.serial;
                    mem[input.addr] = input.value & 0b011111111;
                    mem[input.addr + 1] = (input.value >> 8) & 0b011111111;
                    mem[input.addr + 2] = (input.value >> 16) & 0b011111111;
                    mem[input.addr + 3] = (input.value >> 24) & 0b011111111;
                    break;
                }
                default:
                    break;

            }
            output.flag = true;
        }
    }
};
}