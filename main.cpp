#include <iostream>
#include "src/CPU.h"

int main(){
    std::string op;
    std::string pos;
    JaneZ::CPU cpu;
    while(true){
        std::cin >> op;
        if(op[0] == '@'){
            for(int i = 1;i <= 8;i ++){
                pos += op[i];
            }

        }else{

        }
    }
}