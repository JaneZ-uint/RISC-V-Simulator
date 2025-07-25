#pragma once
#include "Type.h"

namespace JaneZ{
struct RSInfo{
    int serial;
    bool isBusy;
    Operation op;    
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int des;
    int imm = 0;
};

class RS{

};
}