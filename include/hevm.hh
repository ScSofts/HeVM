#pragma once
#include <cstdint>
#include <stack>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <map>
#include <string>
#include <set>


namespace HeVM{
    enum InstructionSet : uint8_t {
        NOP,
        MOVE,
        LOAD_VAL,
        READ_1,
        READ_2,
        READ_4,
        READ_8,
        READ,
        WRITE_1,
        WRITE_2,
        WRITE_4,
        WRITE_8,
        WRITE,
        ADD,
        SUB,
        MUL,
        DIV,
        REM,
        CALL,
        CALL_EXT,
        RET,
        JMPR,
        JMP,
        JEZ,
        JEO,
        JEZR,
        JEOR,
        CL,
        CLE,
        CG,
        CGE,
        CE,
        CNE,
        XOR,
        OR,
        NEG,
        AND,
        PUSH,
        PUSHR,
        POP,
        CRASH
    };

    struct Instruction{
        InstructionSet instruction;
        uint8_t operand1;
        uint8_t operand2;
        int64_t immediate;
    };

    class VirtualMachine;

    using ExternalFunction = std::function< void(VirtualMachine& vm)>;

    class VirtualMachine{
    protected:
        std::stack<int64_t> call_stack;       // call stack
        std::vector<Instruction> programs;    // instructions
        std::thread thread;
        std::string reason;
    public:
        int64_t registers[256]{};             // registers
        int64_t pc{};                         // program counter
        std::stack<int64_t> stack;            // stack
        std::vector<uint8_t> constant_pool;
        std::map<std::string, ExternalFunction> external_functions{};

        void run();
        void step();
        void pause();
        void crash(const std::string &reason);

        void bind_function(const std::string& name, ExternalFunction function);

        std::stack<int64_t> stack_trace();
        std::string crash_reason();

        enum Status{
            Running,
            Step,
            Paused,
            Stopped,
            Crashed
        };

        std::atomic<Status> status{};


        ~VirtualMachine();

    protected:
        VirtualMachine() = default;
        friend std::unique_ptr<VirtualMachine>
        CreateVirtualMachine(const std::vector<Instruction> &programs, const std::vector<uint8_t> &constant_pool);
        friend void RunInstruction(VirtualMachine &vm, const Instruction & instruction);
        void thread_work();

        static void malloc(VirtualMachine &vm);
        static void free(VirtualMachine &vm);
    };

    std::unique_ptr<VirtualMachine>
    CreateVirtualMachine(const std::vector<Instruction> &programs, const std::vector<uint8_t> &constant_pool);
};