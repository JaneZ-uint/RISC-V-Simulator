#pragma once
#include "Queue.h"
namespace JaneZ{
struct RoBInfo{
    int serial;
    bool isBusy;
    bool isReady;
    int des;
    int value;
};

class RoB{
public:
    Queue<RoBInfo> rob;

};
}