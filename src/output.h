#pragma once

namespace JaneZ {
    struct RoBOutput{
        int des1;
        int serial1;
        int value;
        bool flag1 = false;
    
        int des2;
        int serial2;
        bool flag2 = false;
    };
    
    struct RSOutput{
        int serial;
        int res;
        bool flag = false;
    };
}