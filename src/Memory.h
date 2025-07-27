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
    MemOutput outputbuffer;
    bool isWork = false;
    int timer = 0;

    void run(){
        if(input.flag){
            switch (input.op) {
                case LB: {
                    outputbuffer.serial = input.serial;
                    if((mem[input.addr] >> 7) & 1){
                        outputbuffer.value = mem[input.addr] & 0xffffff00;
                    }else{
                        outputbuffer.value = mem[input.addr];
                    }
                    break;
                }
                case LBU: {
                    outputbuffer.serial = input.serial;
                    outputbuffer.value = mem[input.addr];
                    break;
                }
                case LH: {
                    outputbuffer.serial = input.serial;
                    int reverse = (mem[input.addr + 1] << 8) + mem[input.addr];
                    if((reverse >> 15) & 1){
                        outputbuffer.value = reverse & 0xffff0000;
                    }else{
                        outputbuffer.value = reverse;
                    }
                    break;
                }
                case LHU: {
                    outputbuffer.serial = input.serial;
                    int reverse = (mem[input.addr + 1] << 8) + mem[input.addr];
                    outputbuffer.value = reverse;
                    break;
                }
                case LW: {
                    outputbuffer.serial = input.serial;
                    int reverse = (mem[input.addr + 3] << 24) + (mem[input.addr + 2] << 16) + (mem[input.addr + 1] << 8) + mem[input.addr];
                    outputbuffer.value = reverse;
                    break;
                }
                case SB: {
                    outputbuffer.serial = input.serial;
                    mem[input.addr] = input.value;
                    break;
                }
                case SH: {
                    outputbuffer.serial = input.serial;
                    mem[input.addr] = input.value & 0b01111111;
                    mem[input.addr + 1] = (input.value >> 8) & 0b011111111;
                    break;
                }
                case SW: {
                    outputbuffer.serial = input.serial;
                    mem[input.addr] = input.value & 0b011111111;
                    mem[input.addr + 1] = (input.value >> 8) & 0b011111111;
                    mem[input.addr + 2] = (input.value >> 16) & 0b011111111;
                    mem[input.addr + 3] = (input.value >> 24) & 0b011111111;
                    break;
                }
                default:
                    break;
            }
            timer = 0;
        }
        if (timer == 3) {
            output.flag = true;
            output = outputbuffer;
        } else {
            output.flag = false;
            timer++;
        }
    }
};
}