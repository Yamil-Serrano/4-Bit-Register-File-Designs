# 4-Bit Register Bank Designs

A collection of 4-bit register bank implementations
exploring architectural trade-offs between
hardware minimalism and full register flexibility.
These designs demonstrate how storage, decoding,
and multiplexing interact inside a CPU register file
using discrete logic ICs and explicit wiring.

This repository includes two complete register bank
architectures: a minimalist split-read design using
74HC157 multiplexers, and a fully flexible design
using 74HC153 multiplexers. Together, they show how
architectural decisions directly affect hardware
complexity, wiring density, and instruction freedom.


# Minimalist 4-Bit Register Bank (RB1574B – Split Read Architecture)

RB157 represents a hardware-minimalist approach
to register file design. It is built entirely
from discrete logic ICs and focuses on reducing
the number of multiplexers required for register
selection while maintaining full write capability.

The architecture uses a split read model, where
each read port accesses a different subset of
registers. This limits some register-to-register
operations but significantly reduces hardware
complexity and control logic.


## Technical Specifications

### Core Features

* **Register Count**: 4 general-purpose registers
* **Register Width**: 4-bit parallel storage
* **Storage Type**: Edge-triggered D-type latches
* **Read Ports**:

  * RS1: Register 1 or Register 2
  * RS2: Register 3 or Register 4
* **Write Port**: Single global RegWrite signal
* **Clocked Write**: Yes


## Integrated Circuits Used

| IC          | Function              | Quantity | Purpose                 |
| ----------- | --------------------- | -------- | ----------------------- |
| **74HC173** | 4-bit D-type Register | 4        | Register storage        |
| **74HC157** | 2:1 Multiplexer       | 2        | Read path selection     |
| **74HC238** | 3-to-8 Decoder        | 1        | Register write decoding |
| **74HC00**  | Quad NAND Gate        | 1        | RegWrite gating logic   |

## Hardware Implementation

### Circuit Design
<img width="6178" height="4630" alt="Main" src="https://github.com/user-attachments/assets/b122f54f-9ce8-4d05-9413-44a941efb2c1" />
*Complete circuit schematic designed in CircuitVerse*

## Architecture Details

### Read Operation

* RS1 selects between Register 1 and Register 2
* RS2 selects between Register 3 and Register 4
* Registers within the same group cannot be read together
* Cross-group operations are supported

This split read structure mirrors constrained or
early CPU designs where hardware limitations
shape instruction-level flexibility.


### Write Operation

* A 74HC238 decoder selects one register
* NAND gates combine decoder outputs with RegWrite
* Only the selected register is enabled for writing
* Write access is fully flexible across all registers


## Design Philosophy

RB157 prioritizes simplicity and hardware efficiency.
By limiting read flexibility, the design reduces
chip count, wiring density, and control complexity.
This architecture demonstrates how physical hardware
constraints directly influence CPU behavior.


## Known Limitations

1. Not all register-to-register combinations are possible
2. RS1 and RS2 access disjoint register groups
3. Additional data movement instructions may be required


# Full 4-Bit Register Bank (RB1534B – Complete Read Architecture)

RB1534B represents a fully flexible register file
implementation. Both read ports can independently
access any register, enabling all possible register
to-register operations in hardware. This completeness comes at the cost of increased
multiplexing logic, dense wiring, and higher visual
complexity, especially in discrete IC designs.


## Technical Specifications

### Core Features

* **Register Count**: 4 general-purpose registers
* **Register Width**: 4-bit parallel storage
* **Storage Type**: Edge-triggered D-type latches
* **Read Ports**: Two fully independent read outputs
* **Write Port**: Single global RegWrite signal
* **Clocked Write**: Yes


## Integrated Circuits Used

| IC          | Function              | Quantity | Purpose                 |
| ----------- | --------------------- | -------- | ----------------------- |
| **74HC173** | 4-bit D-type Register | 4        | Register storage        |
| **74HC153** | Dual 4:1 Multiplexer  | 4        | Full read selection     |
| **74HC238** | 3-to-8 Decoder        | 1        | Register write decoding |
| **74HC00**  | Quad NAND Gate        | 1        | RegWrite gating logic   |

## Hardware Implementation

### Circuit Design
<img width="6516" height="4908" alt="Main_1" src="https://github.com/user-attachments/assets/7fae47a0-3860-42c4-972d-d7781ad67d49" />
*Complete circuit schematic designed in CircuitVerse*

## Architecture Details

### Read Operation

* RS1 can select Register 1 through Register 4
* RS2 can select Register 1 through Register 4
* All register combinations are supported
* Both read ports operate simultaneously

The explicit multiplexing structure makes all
data paths visible and scalable, at the cost of
extensive wiring and schematic complexity.

### Write Operation

* Identical to RB1574B
* Fully flexible write access
* One register written per clock cycle


## Design Philosophy

RB1534B prioritizes architectural completeness over
hardware simplicity. Every register selection is
explicitly decoded and multiplexed, making this
design ideal for studying scalable CPU register
files and data-path construction.

## Comparative Summary

| Feature           | RB1574B        | RB1534B        |
| ----------------- | ------------- | ------------ |
| Read Flexibility  | Limited (2×2) | Full (4×4)   |
| Write Flexibility | Full          | Full         |
| Chip Count        | Low           | High         |
| Wiring Density    | Low           | Very High    |
| Educational Focus | Constraints   | Completeness |

### Historical Note: 
While working on this design, I later discovered the 74HC670, a single IC that could implement a full 4-bit register bank with all read/write flexibility. At that point, I had already spent around $35 on multiple ICs (157s, 153s, 173s, etc.) to build the RB157 and RB153 banks, the 74HC670 costs around $10. However, building the banks with discrete components first gave me deep insight into MUX cascading, RegWrite logic, and architectural limitations.

Reflection: If I had bought the 74HC670 from the start:

- I would not have understood MUX cascades
- I would not have understood RegWrite
- I would not have seen real limitations
- The chip would have been a black box

With $10 you could buy a fully functional chip. With $35, you bought deep understanding and hands-on experience.
