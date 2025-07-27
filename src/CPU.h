#pragma once
#include "ALUs.h"
#include "Decoder.h"
#include "LSB.h"
#include "Memory.h"
#include "Predictor.h"
#include "RF.h"
#include "RoB.h"
#include "RS.h"
#include <cstdint>

namespace JaneZ{

class CPU{
private:
    void WireFromDecoderToRoB(){
        DecodeRes res = decoder.getType(memory.mem[pc],memory.mem[pc + 1] , 
            memory.mem[pc + 2], memory.mem[pc + 3]);
        rob.input.serial = res.serial;
        rob.input.des = res.rd;
        rob.input.imm = res.imm;
        rob.input.pc = pc;
        rob.input.op = res.op;
        rob.input.imm = res.imm;
        rob.input.isAdd = true;
    }

    void WireFromDecoderToRS(){
        DecodeRes res = decoder.getType(memory.mem[pc],memory.mem[pc + 1] , 
            memory.mem[pc + 2], memory.mem[pc + 3]);
        rs.input.serial = res.serial;
        rs.input.op = res.op;
        rs.input.des = res.rd;
        rs.input.imm = res.imm;
        rs.input.Qj.update(rf.dependency[res.rs1]);
        rs.input.Qk.update(rf.dependency[res.rs2]);
        if(rf.dependency[res.rs1] == 0){
            rs.input.Vj.update(rf.value[res.rs1]);
        }
        if(rf.dependency[res.rs2] == 0){
            rs.input.Vk.update(rf.value[res.rs2]);
        }
        rs.input.isAdd = true;
    }

    //Special check
    void WireFromRoBToRS(){
        if(rob.output.flag1){
            rs.specialcheckOutput.serial1 = rob.output.serial1;
            rs.specialcheckOutput.des1 = rob.output.des1;
            rs.specialcheckOutput.value = rob.output.value;
        }
    }

    void WireFromRoBToRF(){
        if(rob.output.flag1){
            rf.value[rob.output.des1] = rob.output.value;
            if(rf.dependency[rob.output.des1] == rob.output.serial1){
                rf.dependency[rob.output.des1] = 0;
            }
        }
        if(rob.output.flag2){
            rf.dependency[rob.output.des2] = rob.output.serial2;
        }
        rob.output.flag1 = rob.output.flag2 = false;
    }

    void WireFromRSToRoB(){
        if(rs.output.flag){
            rob.InfoFromRS.flag = true;
            rob.InfoFromRS.res = rs.output.res;
            rob.InfoFromRS.serial = rs.output.serial;
        }
        rs.output.flag = false;
    }

    void WireFromRSToLSB(){
        if(rs.lsbinfo.isAdd){
            lsb.input.des = rs.lsbinfo.des;
            lsb.input.imm = rs.lsbinfo.imm;
            lsb.input.op = rs.lsbinfo.op;
            lsb.input.serial = rs.lsbinfo.serial;
            lsb.input.Vj = rs.lsbinfo.Vj;
            lsb.input.Vk = rs.lsbinfo.Vk;
            rs.lsbinfo.isAdd = false;
        }
    }

    void WireFromLSBToMem(){
        if(lsb.output.flag){
            memory.input = lsb.output;
            lsb.output.flag = false;
        }
    }

    void WireFromMemToLSB(){
        if(memory.output.flag){
            lsb.message = memory.output;
            memory.output.flag = false;
        }
    }

    void WireFromLSBToRS(){
        if(lsb.lsboutput.flag){
            rs.lsboutput = lsb.lsboutput;
            lsb.lsboutput.flag = false;
        }
    }

public:
    CPU() = default;

    class ALU alu;
    Decoder decoder;
    LSB lsb;
    Memory memory;
    Predictor predictor;
    RoB rob;
    RS rs;
    uint32_t pc;
    int clk = 0;

    //总接口
    unsigned int run(){
        while (!isTerminal) {
            ++ clk;
            tick();
            Wire();
            Run();
            pc += 4;
        }
        return rf.value[0] & 0b011111111;
    }

    void tick(){
        lsb.tick();
        predictor.tick();
        rob.tick();
        rs.tick();
    }

    void Wire(){
        WireFromDecoderToRoB();
        WireFromDecoderToRS();
        WireFromRoBToRS();
        WireFromRoBToRF();
        WireFromRSToRoB();
        WireFromRSToLSB();
        WireFromLSBToMem();
        WireFromMemToLSB();
        WireFromLSBToRS();
    }

    void Run(){
        lsb.run();
        memory.run();
        predictor.run();
        rob.run();
        rs.run();
    }
};
}