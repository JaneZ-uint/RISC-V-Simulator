# pragma once

namespace JaneZ{ 
template<class T,const int length = 100>
class Queue{
public:
    T data[length];
    int head = 0;
    int tail = 0;
    int l = 0;
    void push(T &current){
        data[tail] = current;
        tail = (tail + 1) % length;
        ++ l;
    }

    void pop() {
        head = (head + 1) % length;
        -- l;
    }

    void clear(){
        head = tail = l = 0;
    }

    T front(){
        return data[head];
    }

    T back(){
        return data[(tail - 1 + length) % length];
    }

    int Size(){
        return l;
    }

    bool isFull(){
        return l == length;
    }

    /*void tick() {
        for () {
            element.tick();
        }
    }*/
};
}