#pragma once
namespace JaneZ {
const int MEMSize = 1e6;
class Memory{
struct Unit{
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;
};

public:
    unsigned int mem[MEMSize] = {};

    Unit read(int addr){
        return Unit{mem[addr],mem[addr + 1],mem[addr + 2],mem[addr + 3]};
    }

    void write(Unit current,int addr){
        mem[addr] = current.first;
        mem[addr + 1] = current.second;
        mem[addr + 2] = current.third;
        mem[addr + 3] = current.fourth;
    }
};
}