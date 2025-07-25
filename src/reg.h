#pragma once

namespace JaneZ{
template<class T>
class Reg{
public:    
    T current;
    T next;

    void update(T data){
        next = data;
    }

    void flush(){
        current = next;
    }
};

}