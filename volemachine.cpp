// VonNeumannMachine.cpp
#include "volemachine.h"

// RegisterFile Implementation
RegisterFile::RegisterFile() {
    // Initialize all registers with "00"
    for (auto& reg : registers) {
        reg = "00";
    }
}

string RegisterFile::get_register_value(int register_id) const {
    if (register_id >= 0 && register_id < 16) {
        return registers[register_id];
    }
    return "00"; // Default return for invalid register
}

void RegisterFile::set_register(int register_id, const string& value) {
    if (register_id >= 0 && register_id < 16) {
        registers[register_id] = value;
    }
}

// ALU Implementation
ALU::ALU() {}

string ALU::addTwosComplement(const string& a, const string& b) {
    // Convert hex strings to integers
    int val1 = stoi(a, nullptr, 16);
    int val2 = stoi(b, nullptr, 16);

    // Convert to 8-bit two's complement
    if (val1 > 127) val1 -= 256;
    if (val2 > 127) val2 -= 256;

    // Add and handle overflow
    int result = val1 + val2;
    if (result > 127) result = 127;
    if (result < -128) result = -128;

    // Convert back to hex
    stringstream ss;
    ss << uppercase << setfill('0') << setw(2) << hex << (result & 0xFF);
    return ss.str();
}

// Helper struct for our 8-bit float format
struct CustomFloat {
    bool sign;           // 1 bit for sign
    int exponent;        // 3 bits for exponent (biased by 4)
    uint8_t mantissa;    // 4 bits for mantissa

    // Convert from hex string to components
    static CustomFloat fromHex(const string& hex) {
        uint8_t value = stoi(hex, nullptr, 16);
        CustomFloat result;
        result.sign = (value & 0x80) != 0;        // Get bit 7
        result.exponent = (value >> 4) & 0x07;    // Get bits 6-4
        result.mantissa = value & 0x0F;           // Get bits 3-0
        return result;
    }

    // Convert to actual floating-point value
    double toDouble() const {
        double value = 1.0 + (mantissa / 16.0);  // Convert mantissa (1.xxxx)
        int unbias_exp = exponent - 4;           // Remove bias
        value *= pow(2.0, unbias_exp);      // Apply exponent
        return sign ? -value : value;            // Apply sign
    }

    // Convert from double to our format
    static CustomFloat fromDouble(double value) {
        CustomFloat result;
        result.sign = value < 0;
        value = abs(value);

        // Get exponent
        int exp = 0;
        while (value >= 2.0) { value /= 2.0; exp++; }
        while (value < 1.0 && exp > -4) { value *= 2.0; exp--; }

        // Clamp to our range
        if (exp < -4) {
            return { false, 0, 0 }; // Return 0 if too small
        }
        if (exp > 3) {
            result.sign = result.sign;
            result.exponent = 7;
            result.mantissa = 0x0F;
            return result; // Return max value with correct sign
        }

        result.exponent = exp + 4; // Add bias
        value -= 1.0; // Remove implied 1
        result.mantissa = static_cast<uint8_t>(value * 16.0 + 0.5); // Convert to 4-bit mantissa
        return result;
    }

    // Convert to hex string
    string toHex() const {
        uint8_t value = 0;
        value |= sign ? 0x80 : 0;           // Set sign bit
        value |= (exponent & 0x07) << 4;    // Set exponent bits
        value |= mantissa & 0x0F;           // Set mantissa bits

        stringstream ss;
        ss << uppercase << setfill('0') << setw(2) << hex << static_cast<int>(value);
        return ss.str();
    }
};

string ALU::addFloatingPoint(const string& a, const string& b) {
    // Convert hex strings to CustomFloat
    CustomFloat num1 = CustomFloat::fromHex(a);
    CustomFloat num2 = CustomFloat::fromHex(b);

    // Convert to double, add, and convert back
    double sum = num1.toDouble() + num2.toDouble();

    // Convert result back to our format
    CustomFloat result = CustomFloat::fromDouble(sum);

    // Return hex string
    return result.toHex();
}

// Memory Implementation
Memory::Memory() {
    // Initialize all memory cells with "00"
    for (auto& row_of_the_memory : memory) {
        for (auto& cell : row_of_the_memory) {
            cell = "00";
        }
    }
}

string Memory::read(int row_of_the_memory, int column_of_the_memory) const {
    if (row_of_the_memory >= 0 && row_of_the_memory < 16 && column_of_the_memory >= 0 && column_of_the_memory < 16) {
        return memory[row_of_the_memory][column_of_the_memory];
    }
    return "00";
}

void Memory::write(int row_of_the_memory, int column_of_the_memory, const string& value) {
    if (row_of_the_memory >= 0 && row_of_the_memory < 16 && column_of_the_memory >= 0 && column_of_the_memory < 16) {
        memory[row_of_the_memory][column_of_the_memory] = value;
    }
}

void Memory::clear(int row_of_the_memory, int column_of_the_memory) {
    if (row_of_the_memory >= 0 && row_of_the_memory < 16 && column_of_the_memory >= 0 && column_of_the_memory < 16) {
        memory[row_of_the_memory][column_of_the_memory] = "00";
    }
}

// Cache Implementation
Cache::Cache() {
    // Initialize all cache entries with "00"
    for (auto& entry : cache) {
        entry = "00";
    }
}

string Cache::read(int address) const {
    if (address >= 0 && address < 16) {
        return cache[address];
    }
    return "00";
}

void Cache::write(int address, const string& value) {
    if (address >= 0 && address < 16) {
        cache[address] = value;
    }
}

void Cache::clear() {
    for (auto& entry : cache) {
        entry = "00";
    }
}

// ProgramCounter Implementation
ProgramCounter::ProgramCounter() : currentAddress(0), now_instruction_position(0,0), Next_instruction_position(0,0){}

void set_now_instrucrion(ProgramCounter& programcounter)
{
    //first == row
    //second == column
    if (programcounter.now_instruction_position.second+1>15 && programcounter.now_instruction_position.first + 1 > 15)
    {
    }
    else if (programcounter.now_instruction_position.second + 1 > 15)
    {
        programcounter.now_instruction_position.second = 0;
        programcounter.now_instruction_position.first+=1;

    }
    else
    {
        programcounter.now_instruction_position.second+=2;

    }

}

void set_next_instrucrion(ProgramCounter& programcounter)
{
    if (programcounter.now_instruction_position.second + 2 < 16)
    {
        programcounter.Next_instruction_position.second = programcounter.now_instruction_position.second + 2;

    }
    else if (programcounter.now_instruction_position.second + 2  > 16)
    {
        programcounter.Next_instruction_position.second = 0;
        programcounter.Next_instruction_position.first++;

    }

}
pair<int, int> ProgramCounter::get_now_instrucrion() const
{
    return now_instruction_position;
}

pair<int, int> ProgramCounter::get_next_instrucrion() const
{
    return Next_instruction_position;
}

void ProgramCounter::getCurrentAddress() const {
    cout << "Program Counter Position :  " << "cell : " << currentAddress << " \n\n\n";
}

void increment(int row, int column, ProgramCounter& programcounter) {
    programcounter.currentAddress =(row*16)+1+column ; // Wrap around at 256
}



// ControlUnit Implementation
ControlUnit::ControlUnit(RegisterFile* rf, ALU* alu, Memory* mem, Cache* cache)
    : regFile(rf), alu(alu), memory(mem), cache(cache) {}

bool ControlUnit::processInstruction(const string& instruction,ProgramCounter& programcounter) {
    if (instruction.length() != 4) return false;

    char process = instruction[0];
    int register_id = stoi(instruction.substr(1, 1), nullptr, 16);
    int row_of_the_memory = stoi(instruction.substr(2, 1), nullptr, 16);
    int column_of_the_memory = stoi(instruction.substr(3, 1), nullptr, 16);
    int index_for_first_register_in_the_register_file = stoi(instruction.substr(2, 1), nullptr, 16);
    int index_for_second_register_in_the_register_file = stoi(instruction.substr(3, 1), nullptr, 16);

    switch (process) {
    case '1': { // Transfer memory to register
        string value_in_the_memory_cell = memory->read(row_of_the_memory, column_of_the_memory);
        regFile->set_register(register_id, value_in_the_memory_cell);
        break;
    }
    case '2': { // Load address to register
        stringstream ss;
        ss << uppercase << setfill('0') << setw(2) << hex << (row_of_the_memory * 16 + column_of_the_memory);
        regFile->set_register(register_id, ss.str());
        break;
    }
    case '3': { // Transfer register to memory/output
        string value_in_the_register = regFile->get_register_value(register_id);
        if (row_of_the_memory == 0 && column_of_the_memory == 0) {
            cout << "Output: " << value_in_the_register << endl;
        }
        else {
            memory->write(row_of_the_memory, column_of_the_memory, value_in_the_register);
        }
        break;
    }
    case '4': { // copying between registers
            string value_in_the_register_1 = regFile->get_register_value(index_for_first_register_in_the_register_file);
            regFile->set_register(index_for_second_register_in_the_register_file, value_in_the_register_1);
        break;
    }
    case '5': { // Two's complement addition
        string value_in_the_register_1 = regFile->get_register_value(index_for_first_register_in_the_register_file);
        string value_in_the_register_2 = regFile->get_register_value(index_for_second_register_in_the_register_file);
        string Twos_complement_addition_result = alu->addTwosComplement(value_in_the_register_1, value_in_the_register_2);
        regFile->set_register(register_id, Twos_complement_addition_result);
        break;
    }
    case '6': { // Floating-point addition
        string value_in_the_register_1 = regFile->get_register_value(index_for_first_register_in_the_register_file);
        string value_in_the_register_2 = regFile->get_register_value(index_for_second_register_in_the_register_file);
        string Floating_point_addition_result = alu->addFloatingPoint(value_in_the_register_1, value_in_the_register_2);
        regFile->set_register(register_id, Floating_point_addition_result);
        break;
    }
    case '7': { // Bitwise OR (V) the content of register S and T, and put the result in register R
        string valS = regFile->get_register_value(row_of_the_memory);
        string valT = regFile->get_register_value(column_of_the_memory);
        bitset<8> bitsS(valS);
        bitset<8> bitsT(valT);
        bitset<8> result = bitsS | bitsT;
        regFile->set_register(register_id, result.to_string());
        break;
    }
    case '8': { // Bitwise AND (^) the content of register S and T, and put the result in register R
        string valS = regFile->get_register_value(row_of_the_memory);
        string valT = regFile->get_register_value(column_of_the_memory);
        bitset<8> bitsS(valS);
        bitset<8> bitsT(valT);
        bitset<8> result = bitsS & bitsT;
        regFile->set_register(register_id, result.to_string());
        break;
    }
    case '9': { // Bitwise XOR (?) the content of register S and T, and put the result in register R
        string valS = regFile->get_register_value(row_of_the_memory);
        string valT = regFile->get_register_value(column_of_the_memory);
        bitset<8> bitsS(valS);
        bitset<8> bitsT(valT);
        bitset<8> result = bitsS ^ bitsT;
        regFile->set_register(register_id, result.to_string());
        break;
    }
    case 'A': { // Rotate the content of register R cyclically right X steps
        string valR = regFile->get_register_value(register_id);
        bitset<8> bits(valR);
        bits >>= row_of_the_memory;
        bits |= (bits << (16 - row_of_the_memory));
        regFile->set_register(register_id, bits.to_string());
        break;
    }
    case 'B': { // Conditional jump
        if (regFile->get_register_value(register_id) == regFile->get_register_value(0)) {
            jump(row_of_the_memory, column_of_the_memory, programcounter);
            return true; // Special handling needed
        }
        break;
    }
    case 'D': { // Jump to instruction in RAM cell XY if the content of register R is greater than (>) the content of register 0. Data is interpreted as integers in two's-complement notation.
        string valR = regFile->get_register_value(register_id);
        string val0 = regFile->get_register_value(0);
        int intR = stoi(valR, nullptr, 16);
        int int0 = stoi(val0, nullptr, 16);
        if (intR > int0) {
            jump(row_of_the_memory , column_of_the_memory,programcounter);
            return true; // Special handling needed
        }
        break;
    }
    }
    return true;
}


// Input Implementation
Input::Input() {}

// Output Implementation
Output::Output() {}


// CPU Implementation
CPU::CPU(Memory* memory, Cache* cache) {
    cu_pointer = new ControlUnit(&regFile, &alu, memory, cache);
}

CPU::~CPU() {
    delete cu_pointer;
}

void CPU::direct_instruction(const string& instruction) {
    cu_pointer->execute_instruction(instruction);
}

