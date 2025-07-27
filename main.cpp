#include <iostream>
#include <string>
#include "src/CPU.h"
#include "src/RF.h"
JaneZ::RF rf;
bool isTerminal = false;

int main(){
    std::string op;
    unsigned int currentPos = 0;
    JaneZ::CPU cpu;
    while(std::cin >> op){
        if(op[0] == '@'){
            std::string addr;
            for(int i = 1;i <= 8;i ++){
                addr += op[i];
            }
            currentPos = std::stoi(addr,nullptr,16);
        }else{
            std::string currentOp = op;
            unsigned int current = std::stoi(currentOp,nullptr,16);
            cpu.memory.mem[currentPos] = current;
            currentPos ++;
        }
    }
    std::cout << cpu.run() << '\n';
}