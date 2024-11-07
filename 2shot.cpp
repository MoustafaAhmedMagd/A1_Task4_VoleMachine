#include "volemachine.h"
ProgramCounter pc;
Memory memory;
Cache cache;
CPU cpu(&memory, &cache);
Input input;
Output output;
void ControlUnit::execute_instruction(const string& instruction) {
    if (processInstruction(instruction, pc)) {
        if (instruction.substr(0, 1) == "B")
        {
            set_now_instrucrion(pc);
            increment(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second,pc);
        }
        else
        {
            set_now_instrucrion(pc);
            set_next_instrucrion(pc);
            increment(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second, pc);
        }
    }
}
void jump(int row, int column, ProgramCounter& pc) {
    pc.Next_instruction_position.first = row;
    pc.Next_instruction_position.second = column;
}
bool Input::load_from_file_to_memory(const string& filename, Memory& memory) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    int row = 0;
    int column = 0;
    string line;
    while (getline(file, line)) {
        if (line.length() == 4) { // Basic validation
            if (row + 1 > 15 && column + 1 > 15)
            {
                cout << "Memory is full\n";
                break;
            }
            memory.write(row, column, line.substr(0, 2));
            memory.write(row, ++column, line.substr(2, 2));
            if (column <= 15) { column++; }
            else { column = 0; row++; }
        }
        else {
            cout << "Invalid instruction format. Must be 4 hexadecimal digits." << "what you write is: " << line << "\n";
        }
    }
    return true;
}
bool Input::load_program_to_memory(Memory& memory) {
    cout << "Enter instructions (type 'End' to finish):\n";
    int row = 0;
    int column = 0;
    string line;
    while (line != "End") {
        getline(cin, line);
        if (line.length() == 4) { // Basic validation
            if (row + 1 > 15 && column + 1 > 15)
            {
                cout << "Memory is full\n";
                break;
            }
            memory.write(row, column, line.substr(0, 2));
            memory.write(row, ++column, line.substr(2, 2));
            if (column <= 15)column++;
            else
            {
                column = 0;
                row++;
            }
        }
        else {
            cout << "Invalid instruction format. Must be 4 hexadecimal digits." << "what you write is: " << line << "\n";
        }
    }
    return true;
}
bool Input::load_instruction_to_memory(Memory& memory) {
    cout << "Enter instructions (type 'End' to finish):\n";
    static int row = 0;
    static int column = 0;
    string line;
    getline(cin, line);
    if (line.length() == 4) { // Basic validation
        if (row + 1 > 15 && column + 1 > 15)
        {
            cout << "Memory is full\n";
            return false;
        }
        memory.write(row, column, line.substr(0, 2));
        memory.write(row, ++column, line.substr(2, 2));
        if (column <= 15)column++;
        else
        {
            column = 0;
            row++;
        }
    }
    else {
        cout << "Invalid instruction format. Must be 4 hexadecimal digits." << "what you write is: " << line << "\n";
        Input::load_instruction_to_memory(memory);
    }
    return true;
}

void Output::displayMemory(const Memory& memory) const {
    cout << "Memory Contents:\n  ";
    cout << " ";
    for (int i = 0; i < 16; i++) {
        cout << hex << uppercase << i << "  ";
    }
    cout << "\n";

    for (int i = 0; i < 16; i++) {
        cout << hex << uppercase << i << " ";
        for (int j = 0; j < 16; j++) {
            cout << memory.read(i, j) << " ";
        }
        cout << "\n";
    }
}

void Output::displayRegisters(const RegisterFile& regFile) const {
    cout << "Register Contents:\n";
    for (int i = 0; i < 16; i++) {
        cout << "R" << hex << uppercase << i << ": "
            << regFile.get_register_value(i) << "\n";
    }
}

void Output::displayCache(const Cache& cache) const {
    cout << "Cache Contents:\n";
    for (int i = 0; i < 16; i++) {
        cout << hex << uppercase << i << ": "
            << cache.read(i) << "\n";
    }
}

void Output::displayValue(const string& value) const {
    cout << value << endl;
}

void Output::machine_status(const Memory& memory, const Cache& cache, const CPU& cpu, const ProgramCounter& programcounter) const
{
    displayMemory(memory);
    displayRegisters(cpu.getRegFile());
    displayCache(cache);
    programcounter.getCurrentAddress();
}
static int operating_system() {
    while (true) {
        cout << "\nVon Neumann Machine Simulator\n";
        cout << "My dear user this is Vole machine that you can build your Own prgram with using only specific instruction and your creativity"<<'\n'
        << " Please choose any of these Instruction by choosing the number beside each instruction\n";
        cout << "1. Load instructions from file\n";
        cout << "2. Enter instructions manually\n";
        cout << "3. Step by Step Execution\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        string choice;
        getline(cin,choice);
        if (choice=="1") {
            string filename;
            cout << "Enter filename: ";
            getline(cin, filename);
            if (input.load_from_file_to_memory(filename, memory)) {
                output.machine_status( memory, cache, cpu, pc);
                for (int cell_in_memory = 0; cell_in_memory < 256; cell_in_memory++) {
                    if (((pc.get_now_instrucrion().first == pc.get_next_instrucrion().first) &&
                        ((pc.get_now_instrucrion().second == pc.get_next_instrucrion().second) ||
                            (pc.get_now_instrucrion().second + 2 == pc.get_next_instrucrion().second))) ||
                        ((pc.get_now_instrucrion().first + 1 == pc.get_next_instrucrion().first) &&
                            (pc.get_next_instrucrion().second == 0))) {

                        if ((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)) == "C000") break;
                        cpu.direct_instruction((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)));
                    }
                    else
                    {
                        if ((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)) == "C000") break;
                        cpu.direct_instruction((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)));

                    }
                }
                cout << "Instructions executed successfully.\n";
            }
            else {
                cout << "Error loading file.\n";
            }
        }
        else if(choice=="2") {
            //load to memory all instructions
            cout << "\nExecution completed. Results:\n";
            if(input.load_program_to_memory(memory)){
                output.machine_status(memory, cache, cpu, pc);
            }
            for (int cell_in_memory = 0; cell_in_memory < 256; cell_in_memory++) {
                if(((pc.get_now_instrucrion().first== pc.get_next_instrucrion().first)&& 
                    ((pc.get_now_instrucrion().second == pc.get_next_instrucrion().second)|| 
                        (pc.get_now_instrucrion().second+2 == pc.get_next_instrucrion().second)))||
                    ((pc.get_now_instrucrion().first + 1 == pc.get_next_instrucrion().first)&& 
                        (pc.get_next_instrucrion().second==0))) {

                    if ((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)) == "C000") break;
                    cpu.direct_instruction((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)));
                }
                else
                {
                    if ((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)) == "C000") break;
                    cpu.direct_instruction((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)));
                
                }
            }
            break;
        }
        else if (choice == "3") {
            while (true) {
                if (input.load_instruction_to_memory(memory)) {
                    if (((pc.get_now_instrucrion().first == pc.get_next_instrucrion().first) &&
                        ((pc.get_now_instrucrion().second == pc.get_next_instrucrion().second) ||
                            (pc.get_now_instrucrion().second + 2 == pc.get_next_instrucrion().second))) ||
                        ((pc.get_now_instrucrion().first + 1 == pc.get_next_instrucrion().first) &&
                            (pc.get_next_instrucrion().second == 0))) {
                        //pc.set_part2_instrucrion(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second);
                        if ((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)) == "C000") break;
                        cpu.direct_instruction((memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second) + memory.read(pc.get_now_instrucrion().first, pc.get_now_instrucrion().second + 1)));
                        cout << "\nExecution completed. Results:\n";
                        output.machine_status(memory, cache, cpu, pc);
                    }
                    else
                    {
                        if ((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)) == "C000") break;
                        cpu.direct_instruction((memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second) + memory.read(pc.get_next_instrucrion().first, pc.get_next_instrucrion().second + 1)));
                        cout << "\nExecution completed. Results:\n";
                        output.machine_status(memory, cache, cpu, pc);

                    }

                }
                }

        }
        else if (choice == "4") {
            cout << "Exiting...\n";
            return 0;
        }
        else{
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
int main(){
    operating_system();
    return 0; 
}