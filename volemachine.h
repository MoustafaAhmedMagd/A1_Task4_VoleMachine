// VonNeumannMachine.h
#ifndef VON_NEUMANN_MACHINE_H
#define VON_NEUMANN_MACHINE_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <string>
#include <array>
#include <vector>
#include <utility>
#include <cmath> // Added for pow() function

using namespace std;

// Forward declarations
class RegisterFile;
class ALU;
class Memory;
class Cache;
class ProgramCounter;
class ControlUnit;
class CPU;
class Input;
class Output;

class RegisterFile {
private:
    array<string, 16> registers;  // 16 registers (0-F)
public:
    RegisterFile();
    string get_register_value(int regId) const;
    void set_register(int regId, const string& value);
};

class ALU {
public:
    ALU();
    string addTwosComplement(const string& a, const string& b);
    string addFloatingPoint(const string& a, const string& b);
};

class Memory {
private:
    array<array<string, 16>, 16> memory;  // 16x16 memory array
public:
    Memory();
    string read(int row, int col) const;
    void write(int row, int col, const string& value);
    void clear(int row, int col);
};

class Cache {
private:
    array<string, 16> cache;  // 16 cache entries
public:
    Cache();
    string read(int address) const;
    void write(int address, const string& value);
    void clear();
};

class ProgramCounter {
private:
    int currentAddress;
    pair<int, int> now_instruction_position;
    pair<int, int> Next_instruction_position;
public:
    ProgramCounter();
    friend void set_now_instrucrion(ProgramCounter& programcounter);
    friend void set_next_instrucrion(ProgramCounter& programcounter);
    pair<int, int> get_now_instrucrion() const;
    pair<int, int> get_next_instrucrion() const;
    void getCurrentAddress() const;
    friend void increment(int row, int column, ProgramCounter& programcounter);
    friend void jump(int row, int column, ProgramCounter& programcounter);
};

class CPU {
private:
    RegisterFile regFile;
    ALU alu;
    ControlUnit* cu_pointer;
    ProgramCounter pc;
public:
    CPU(Memory* memory, Cache* cache);
    ~CPU();
    void direct_instruction(const string& instruction);
    const RegisterFile& getRegFile() const { return regFile; }
};

class ControlUnit {
private:
    RegisterFile* regFile;
    ALU* alu;
    Memory* memory;
    Cache* cache;
    ProgramCounter* pc_pointer;
public:
    ControlUnit(RegisterFile* rf, ALU* alu, Memory* mem, Cache* cache);
    void execute_instruction(const string& instruction);
    bool processInstruction(const string& instruction, ProgramCounter& programcounter);
};

class Input {
private:
    string fileName;
public:
    Input();
    bool load_instruction_to_memory(Memory& memory);
    bool load_program_to_memory(Memory& memory);
    bool load_from_file_to_memory(const string& filename, Memory& memory);
};

class Output {
public:
    Output();
    void displayMemory(const Memory& memory) const;
    void displayRegisters(const RegisterFile& regFile) const;
    void displayCache(const Cache& cache) const;
    void displayValue(const string& value) const;
    void machine_status(const Memory& memory, const Cache& cache, const CPU& cpu, const ProgramCounter& programcounter) const;
};

#endif // VON_NEUMANN_MACHINE_H