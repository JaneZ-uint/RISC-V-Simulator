# RISC-V Simulator Note

## verilog 语法

### 时钟周期
计算机内部会以一定频率发出时钟信号，其决定了各个部件的工作节奏。
时钟周期即为一次时钟信号的完整周期。

clk：时钟信号；rstn：复位信号（由true变为false时执行复位）
![时钟周期](/img/时钟周期.png)

### 组合逻辑
- 输出信号只由输入信号决定
- 一般不能存储状态
- 立即响应，当输入信号改变时，输出立即开始变化
- 一般用于运算器等
- 当电路极其复杂时，会带来很大**时延**
- 存在竞争冒险

### 时序逻辑
- 输出信号取决于输入信号和当前状态
- 一般包含存储元件，可以存储状态
- 由时钟信号控制响应（因此各元件可以同步执行，避免产生竞争冒险）
- 一般用于**状态机**、**存储器**等

小作业里的某个**状态机**：
```
/*
    Problem:
    https://acm.sjtu.edu.cn/OnlineJudge/problem?problem_id=1616

    任务：掌握时序逻辑，了解状态机，完成给定状态转移图对应的状态机。

    PS：下方的代码注释仅为建议，可以使用任意方式解决问题（以下代码均可以改动）。
*/

module seq_circuit(
    input           C,
    input           clk,
    input           rst,
    output wire     Y
);
    reg [1:0] now_state, next_state;
    
    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            now_state <= 2'b00;
        end
        else begin

            // TODO
            // now_state = ?
            now_state <= next_state;

        end
    end
    
    always @ (*) begin

        // TODO
        // next_state = ?
        if (now_state == 2'b00) begin
            if (C) begin 
                next_state = 2'b01;
            end else begin
                next_state = 2'b00; 
            end
        end else if(now_state == 2'b01) begin
            if (C) begin
                next_state = 2'b01; 
            end else begin 
                next_state = 2'b11;
            end 
        end else if(now_state == 2'b10) begin
            if (C) begin
                next_state = 2'b10;
            end else begin
                next_state = 2'b00;
            end
        end else begin 
            if (C) begin 
                next_state = 2'b10;
            end else begin 
                next_state = 2'b11;
            end
        end
    end
    // TODO
    // Y = ?
    assign Y = (now_state == 2'b11) ? 1 : ((now_state == 2'b10 && C ? 1 : 0));

endmodule
```

### wire
**wire 类型表示硬件单元之间的物理连线，由其连接的器件输出端连续驱动**

1. wire 可简单地理解为电线，连接的过程中可以夹杂逻辑门（对数据进行运算）
2. wire 只能用 assign 赋值，并且只能被赋值⼀次（因为电线不可能随意变动）
3. wire 中数值的变化是实时的

### 阻塞赋值与非阻塞赋值
- 阻塞赋值：顺序执行，= 号赋值
- 非阻塞赋值：并行执行语句，不会阻塞位于同一个语句块中后面语句的执行，<= 号赋值

### reg
寄存器用来表示存储单元，它会保持数据原有的值，直到被改写

2种赋值方式：
```
@always(*)
```
只能采用阻塞赋值 = 给reg 赋值，等价于wire 用assign 赋值

```
@always(posedge clk)
```
只使用非阻塞赋值 <= 给reg 赋值，等价于**只会使用上一周期的数据进行赋值**

### 向量
位宽大于1 时，wire 和reg 可声明为向量形式。
```
reg [3:0] counter;
wire [31:0] gpio_data;
```

## 指令集架构 ISA
### 冯诺依曼架构
**计算机的基础设计范式** \
指令和数据都存储在内存中 \
CPU = 计算单元 + 控制单元

![冯诺伊曼架构](/img/冯诺依曼架构.png)

### RISC-V RV32I
RISC-V是一个基于精简指令集（RISC）原则的开源指令集架构。

RV32I 支持32位寻址空间，支持字节地址访问。其寄存器也都是32位。

1. 指令地址寄存器PC
2. 32个通用寄存器
![32个通用寄存器](/img/32个通用寄存器.png)

### 指令地址寄存器PC
计算机要执行的指令在内存中。为了获取指令，需要知道指令在内存中的位置，通过一次访存操作读取到下一条指令。PC就是一个指针，指向下一条指令的地址。RV32I每条指令长度都是4个byte，**因此在读取完一条非跳转指令后，PC应当+4**，以指向下一条指令。当读到跳转类指令时，根据跳转类指令的执行结果修改PC的值。

### 指令类型
- 跳转指令：**修改PC值**
- 运算指令：用两个寄存器中的数，或者一个寄存器中的数和一个立即数进行一些运算，包括加减、比较、位运算。计算结果存在目标寄存器中。
- 访存指令：将寄存器中的值存到某个内存地址，或将内存地址中的数据读到某个寄存器中。

### 一些指令例子
![Add](/img/加法.png)

![跳转指令](/img/跳转指令.png)

![beq](/img/beq.png)

![字加载](/img/字加载.png)

![存字节](/img/存字节.png)

## 高效的CPU指令类型
### 效率低原因
指令执行过程：读取指令（IF）解析指令（ID）运算（EX）访存（MEM）写回寄存器（WB）

![CPU元件](/img/CPU元件.png)

当一条指令执行到后面阶段时，负责前面阶段的元件都会空闲（因为本条指令还没执行完，不能执行下一条指令）

解决方案：流水线！
![五级流水](/img/五级流水.png)

### 冒险
**一旦指令的执行周期存在重叠，就会遇到冒险**

- 数据冒险：WAW WAR RAW
- 结构冒险：一个元件不能同时处理两条指令的要求
- 控制冒险：跳转指令引发的问题

### Bubble 结构冒险
Insert Stall

![Insert Stall](/img/bubble.png)

### Solution to 数据冒险
暂停流水线、forward

## Tomasulo 算法
### 想法
**指令不必等到前面的指令都执行完才能执行；只要所需数据准备好，就可以执行**

### 算法的五个阶段
指令获取(fetch) -> 指令发射(issue) -> 指令执行(exec) -> 指令结果写入(write&broadcast) ->指令提交(commit)

### 算法的几个部件
- Decoder 
- ReorderBuffer(RoB) 
- ReservationStation(RS)
- LoadStoreBuffer(LSB) 
- ALU
- RegisterFile(RF) 
- Predictor

### 寄存器重命名
- 只要保证最后寄存器的状态是正确的，新值旧值暂存在哪里都无所谓。
- 通过**增加一些寄存器**，使得不影响新值立刻写入的同时，仍允许获取旧值
- 更宽泛地说，只需要保证**覆盖前的指令能正确获取到旧值就行**

### 顺序提交-RoB
只需要用一个循环队列暂存一下正在执行的所有指令和对应的目标寄存器、指令执行结果。一旦队头指令执行完毕，就用其结果更新目标寄存器。

**指令发射（被读取的暂存于CPU中的指令开始被执行）时入队，提交时出队。**

### Reservation Station
**在指令发射后，会进入RS中。RS会维护这条指令的数据依赖关系。**

- Vj，Vk：真正参与该指令运算的值（操作数，指令要操作的数据）
- Qj，Qk：记录该指令的操作数（即寄存器/内存地址）需要靠哪个正在执行的指令算出来（用RoB中的编号来表示）若为空，则说明对应的值已准备好。
- Dest：该指令在RoB中的编号。
- A：记录立即数或地址。
- Op：指令类型。

RS连接着ALU，当Qj，Qk为空时，将其送入ALU中计算

在一条指令的发射过程中，会向Register File询问其源寄存器（i.e.向哪个寄存器获取所需值）是否有依赖。若有，则将对应位置的Q更新为该寄存器的依赖；否则直接获取寄存器的值，填入对应的V中。

### Register File
CPU的通用寄存器们在Register File中。Register File还可以存储并维护一些有关通用寄存器的信息。

在Tomasulo架构中，为每个通用寄存器记录并维护其依赖（i.e.其最新值将由哪条正在执行的指令算出）可以用RoB中的编号来代表指令。

- 当一条指令被发射时，将其目标寄存器的依赖修改为其在RoB中的编号
- 当一条指令被提交时，如果目标寄存器的依赖仍是该指令，则将其设置为无依赖，并更新目标寄存器的值

### LSB
**用来专门存访存指令的**

结构和功能与RS差不多，但连的不是ALU，是RAM
