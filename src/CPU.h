#pragma once
#include "ALUs.h"
#include "Decoder.h"
#include "LSB.h"
#include "Memory.h"
#include "Predictor.h"
#include "RF.h"
#include "RoB.h"
#include "RS.h"
#include "Type.h"
#include <cstdint>
#include <cstdio>

namespace JaneZ{

class CPU{
private:
    void WireFromDecoderToRoBRS(){
        DecodeRes res = decoder.getType(memory.mem[pc],memory.mem[pc + 1] , 
            memory.mem[pc + 2], memory.mem[pc + 3]);
        if (res.op == JALR) {
            isStop = true;
        }
        rob.input.serial = res.serial;
        rob.input.des = res.rd;
        rob.input.imm = res.imm;
        rob.input.pc = pc;
        rob.input.op = res.op;
        rob.input.imm = res.imm;
        rob.input.isAdd = true;
        rob.input.rs1 = res.rs1;
        rs.input.serial = res.serial;
        rs.input.op = res.op;
        rs.input.des = res.rd;
        rs.input.imm = res.imm;
        rs.input.Qj = rf.dependency[res.rs1];
        rs.input.Qk = rf.dependency[res.rs2];
        if(rf.dependency[res.rs1] == 0){
            rs.input.Vj = rf.value[res.rs1];
        }
        if(rf.dependency[res.rs2] == 0){
            rs.input.Vk = rf.value[res.rs2];
        }
        rs.input.isAdd = true;
        if(res.op == BEQ || res.op == BGE || res.op == BGEU || res.op == BLT || res.op == BLTU || res.op == BNE){
            predictor.input.offset = res.imm;
            predictor.input.op = res.op;
            predictor.input.pc = pc;
            predictor.input.serial = res.serial;
            predictor.input.flag = true;
        }
        if(res.op == JAL){
            pc = pc + res.imm - 4;
        }
    }

    /*void WireFromDecoderToRS(){
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
    }*/

    void WireFromRoBToCPU(){
        if(rob.robpc.flag){
            if (rob.robpc.isJALR) {
                pc = rob.robpc.v1 + rob.robpc.offset;
            }else {
                pc = rob.robpc.offset + rob.robpc.pc;
            }          
            rob.robpc.flag = false;
        }
    }

    //Special check
    void WireFromRoBToRS(){
        if(rob.output.flag1){
            rs.specialcheckOutput = rob.output;
            // rs.specialcheckOutput.des1 = rob.output.des1;
            // rs.specialcheckOutput.value = rob.output.value;
        }
    }

    void WireFromRoBToRF(){
        if(rob.output.flag2){
            rf.dependency[rob.output.des2] = rob.output.serial2;
        }
        if(rob.output.flag1){
            rf.value[rob.output.des1] = rob.output.value;
            if(rf.dependency[rob.output.des1] == rob.output.serial1){
                rf.dependency[rob.output.des1] = 0;
            }
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
            lsb.input.isAdd = true;
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

    void WireFromLSBToRoB(){
        if(lsb.lsboutput.flag){
            rob.InfoFromLSB = lsb.lsboutput;
        }
    }

    void WireFromLSBToRS(){
        if(lsb.lsboutput.flag){
            rs.lsboutput = lsb.lsboutput;
            lsb.lsboutput.flag = false;
        }
    }

    void WireFromRoBToLSB(){
        lsb.infofromrob.serial = rob.robtomem.serial;
    }

    void WireFromPredictorToCPU(){
        if(rob.isFlush.flag){
            pc = rob.isFlush.new_pc;
        }
    }

    void WireFromPredictorToLSB(){
        if(rob.isFlush.flag){
            lsb.info = rob.isFlush;
        }
    }

    void WireFromPredictorToRoB(){
        if(rob.isFlush.flag){
            rob.info = rob.isFlush;
            isStop = false;
        }
    }

    void WireFromPredictorToRS(){
        if(rob.isFlush.flag){
            rs.info = rob.isFlush;
        }
        rob.isFlush.flag = false;
    }

    void WireFromRSToPredictor(){
        if(rs.toPredictor.flag){
            predictor.inputALU = rs.toPredictor;
            rs.toPredictor.flag = false;
        }
    }

    void WireIsControl(){
        if(rob.isFlush.flag){
            for(int i = 0;i < 32;i ++){
                rf.dependency[i] = 0;
            }
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
    bool isStop = false;

    //总接口
    unsigned int run(){
        while (!isTerminal) {
            /*if(rob.head == 40){
                printf("Here!");
            }*/
            ++ clk;
            tick();
            Wire();
            Run();
            JALRCheck();

            pc += 4;
        }
        return rf.value[10] & 0b011111111;
    }

    void tick(){
        lsb.tick();
        predictor.tick();
        rob.tick();
        rs.tick();
    }

    void Wire(){
        WireFromRoBToRS();
        WireFromRoBToRF();
        WireIsControl();
        WireFromRoBToCPU();
        WireFromPredictorToCPU();
        if(!isStop){
            WireFromDecoderToRoBRS();
        }else{
            pc -= 4;
        }
        //!!!TODO 暂时性注释掉for debugging
        WireFromRSToRoB();
        WireFromRSToLSB();
        WireFromLSBToMem();
        WireFromMemToLSB();
        WireFromLSBToRoB();
        WireFromLSBToRS();
        WireFromRoBToLSB();
        WireFromRSToPredictor();
        WireFromPredictorToLSB();
        WireFromPredictorToRoB();
        WireFromPredictorToRS();
    }

    void Run(){
        lsb.run();
        memory.run();
        predictor.run();
        rob.run();
        rs.run();
    }

    void JALRCheck(){
        if(rob.jalrsignal.isOK){
            isStop = false;
            rob.jalrsignal.isOK = false;
        }
    }
    
};
}