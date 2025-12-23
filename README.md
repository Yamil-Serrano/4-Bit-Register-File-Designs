# 4-Bit Register File Designs

A collection of 4-bit register file implementations
exploring architectural trade-offs between
hardware minimalism and full register flexibility.
These designs demonstrate how storage, decoding,
and multiplexing interact inside a CPU register file
using discrete logic ICs and explicit wiring.

This repository includes two complete register file
architectures: a minimalist split-read design using
74HC157 multiplexers, and a fully flexible design
using 74HC153 multiplexers. Together, they show how
architectural decisions directly affect hardware
complexity, wiring density, and instruction freedom.

# Minimalist 4-Bit Register File (RB1574B – Split Read Architecture)

RB1574B represents a hardware-minimalist approach
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
* **Storage Type**: Edge-triggered D-type flip-flops
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

RB1574B prioritizes simplicity and hardware efficiency.
By limiting read flexibility, the design reduces
chip count, wiring density, and control complexity.
This architecture demonstrates how physical hardware
constraints directly influence CPU behavior.

## Known Limitations

1. Not all register-to-register combinations are possible
2. RS1 and RS2 access disjoint register groups
3. Additional data movement instructions may be required

# Full 4-Bit Register File (RB1534B – Complete Read Architecture)

RB1534B represents a fully flexible register file
implementation. Both read ports can independently
access any register, enabling all possible register
to-register operations in hardware. This completeness
comes at the cost of increased multiplexing logic,
dense wiring, and higher visual complexity,
especially in discrete IC designs.

## Technical Specifications

### Core Features

* **Register Count**: 4 general-purpose registers
* **Register Width**: 4-bit parallel storage
* **Storage Type**: Edge-triggered D-type flip-flops
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

| Feature           | RB1574B       | RB1534B      |
| ----------------- | ------------- | ------------ |
| Read Flexibility  | Limited (2×2) | Full (4×4)   |
| Write Flexibility | Full          | Full         |
| Chip Count        | Low           | High         |
| Wiring Density    | Low           | Very High    |
| Educational Focus | Constraints   | Completeness |

# FPGA Implementation: 4-Bit Register File (Tang Nano 9K)

This module implements a 4-bit register file on the Sipeed Tang Nano 9K FPGA, translating a discrete, flip-flop–based register file into a compact HDL description. The design faithfully recreates a classic hardware register file built from D flip-flops, a 2-to-4 decoder, and external multiplexers, preserving the original architectural intent while leveraging FPGA integration. The register file serves as a foundational storage element for a CPU-style datapath, providing controlled write access and multiple read paths suitable for ALU operand sourcing.

## Physical Prototype & Interfacing

To maintain a hands-on and educational workflow, the FPGA is interfaced with external switches and LEDs, closely mirroring the behavior and observability of the original discrete implementation.

**Input (Switches):**

* A 4-bit switch group provides *speculative ALU data*, allowing manual injection of values into the register file.
* Additional switches select the write register address and control the global write enable (`RegWrite`).
* All inputs use pull-down configurations to guarantee stable logic levels and avoid floating inputs.

**Output (LEDs):**

* Two independent 4-bit LED groups display the contents of the selected source registers (`rs1` and `rs2`).
* LEDs operate in an active-high configuration, where illumination represents a logic high (`1`).

This interface allows direct visual confirmation of register contents and write/read behavior in real time.

![20251222\_233541](https://github.com/user-attachments/assets/e4640e7c-0cc6-416e-96e9-a8ac34387e20)
*Tang Nano 9K demonstrating manual register writes and LED-based readout.*

## Design Philosophy & Notes

* **Discrete-to-HDL Fidelity**: The register file was intentionally described using explicit registers, a decoder, and multiplexers to mirror the original TTL-based design (e.g., 74HC238 for decoding and 74HC157 for selection), rather than relying on abstract HDL shortcuts.
* **Educational Transparency**: Each register is represented explicitly, making the data flow and control logic easy to trace and reason about, especially for learners transitioning from breadboard-based designs to HDL.
* **Controlled Write Semantics**: A global write enable combined with a decoded register select ensures that exactly one register captures data on a rising clock edge, replicating real-world register file behavior.
* **Modular Expansion**: Exposing individual register outputs enables flexible experimentation, such as external multiplexing, debugging, or future ALU integration.

## Comparative Summary: Discrete vs. FPGA Register File

| Aspect               | Discrete IC Implementation                          | FPGA Implementation                                      |
| :------------------- | :-------------------------------------------------- | :------------------------------------------------------- |
| **Component Count**  | Multiple flip-flops, decoder, and multiplexers.     | **1** FPGA device.                                       |
| **Storage Elements** | Individual 74HC173 wired by hand.                   | Synthesized registers from HDL.                          |
| **Address Decoding** | Physical 74HC238 decoder IC.                        | Behavioral decoder logic.                                |
| **Read Selection**   | External multiplexers (e.g., 74HC157).              | Combinational HDL multiplexing.                          |
| **Flexibility**      | Fixed wiring; modifications require rewiring.       | **Reconfigurable** via HDL changes.                      |
| **Learning Focus**   | Understanding register-level hardware construction. | Bridging discrete logic concepts into FPGA-based design. |

### Historical Note:

While working on this design, I later discovered the 74HC670, a single IC that could implement a full 4-bit register file with all read/write flexibility. At that point, I had already spent around $35 on multiple ICs (157s, 153s, 173s, etc.) to build the RB157 and RB153 designs, while the 74HC670 costs around $10. However, building the register files with discrete components first gave me deep insight into MUX cascading, RegWrite logic, and architectural limitations.

Reflection: If I had bought the 74HC670 from the start:

* I would not have understood MUX cascades
* I would not have understood RegWrite
* I would not have seen real limitations
* The chip would have been a black box

With $10 you could buy a fully functional chip. With $35, you bought deep understanding and hands-on experience.

## License

Licensed under the **Solderpad Hardware License v2.1**.
See the [LICENSE](LICENSE-HARDWARE) file for full terms.

## Contact

If you have any questions or suggestions, feel free to reach out:

* **GitHub:** [Neowizen](https://github.com/Yamil-Serrano)
