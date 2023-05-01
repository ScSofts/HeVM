#include <hevm.hh>
#include <functional>

namespace HeVM{
    std::unique_ptr<VirtualMachine>
    CreateVirtualMachine(const std::vector<Instruction> &programs, const std::vector<uint8_t> &constant_pool) {
        std::unique_ptr<VirtualMachine> vm(new VirtualMachine{});

        vm->stack = {};
        vm->pc = 0;
        vm->programs = programs;
        vm->status = VirtualMachine::Paused;
        vm->thread = std::thread([vm = vm.get()] { vm->thread_work(); });
        vm->constant_pool = constant_pool;

        vm->bind_function("alloc", VirtualMachine::malloc);
        vm->bind_function("free", VirtualMachine::free);

        return std::move(vm);
    }

    void VirtualMachine::run() {
        this->status = Running;
    }

    VirtualMachine::~VirtualMachine()
    {
        if(this->status != Paused && this->thread.joinable())
            this->thread.join();
        else{
            this->thread.detach();
        }
    }

    void VirtualMachine::pause() {
        this->status = Paused;
    }

    void VirtualMachine::thread_work() {
        while (this->status != Crashed && this->status != Stopped){
            if(this->status == Step){
                this->status = Paused;
            }
            else if(this->status == Paused){
                std::this_thread::yield();
            }
            else if(this->programs.size() > this->pc) {
                const Instruction &now = this->programs[this->pc];
                this->pc++;
                RunInstruction(*this, now);
            } else{
                this->status = Stopped;
            }
        }
    }

    void RunInstruction(VirtualMachine &vm, const Instruction &instruction) {
        switch (instruction.instruction) {

            case InstructionSet::NOP:
                break;
            case InstructionSet::MOVE:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = src;
                break;
            }
            case InstructionSet::LOAD_VAL:
                vm.registers[instruction.operand1] = instruction.immediate;
                break;
            case InstructionSet::READ_1: {
                auto &dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    dst = (int64_t)(*(const char *)src);
                else
                    vm.crash("Invalid instruction READ_1: invalid operand");
                break;
            }
            case InstructionSet::READ_2: {
                auto &dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if (dst && src)
                    dst = *( const int16_t*)src;
                else
                    vm.crash("Invalid instruction READ_2: invalid operand");
                break;
            }
            case InstructionSet::READ_4:{
                auto &dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    dst = *( const int32_t*)src;
                else
                    vm.crash("Invalid instruction READ_4: invalid operand");
                break;
            }
            case InstructionSet::READ_8:{
                auto &dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    dst = *( const int64_t*)src;
                else
                    vm.crash("Invalid instruction READ_8: invalid operand");
                break;
            }
            case InstructionSet::READ:{
                auto &dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                auto N = instruction.immediate;
                if(dst && src && N)
                    memcpy((void*)src, (void*)&dst, N);
                else
                    vm.crash("Invalid instruction READ: invalid operand");
                break;
            }
            case InstructionSet::WRITE_1:{
                auto dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    *(char*)dst = *(const char*)&src;
                else
                    vm.crash("Invalid instruction WRITE_1: invalid operand");
                break;
            }
            case InstructionSet::WRITE_2:{
                auto dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    memcpy((void*)dst, (void*)&src, 2);
                else
                    vm.crash("Invalid instruction WRITE_2: invalid operand");
                break;
            }
            case InstructionSet::WRITE_4:{
                auto dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    memcpy((void*)dst, (void*)&src, 4);
                else
                    vm.crash("Invalid instruction WRITE_4: invalid operand");
                break;
            }
            case InstructionSet::WRITE_8:{
                auto dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                if(dst && src)
                    memcpy((void*)dst, (void*)&src, 8);
                else
                    vm.crash("Invalid instruction WRITE_8: invalid operand");
                break;
            }
            case InstructionSet::WRITE:{
                auto dst = vm.registers[instruction.operand1];
                auto src = vm.registers[instruction.operand2];
                auto N = instruction.immediate;
                if(dst && src && N)
                    memcpy((void*)dst, (void*)&src, N);
                else
                    vm.crash("Invalid instruction WRITE: invalid operand");
                break;
            }
            case InstructionSet::ADD:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst + src;
                break;
            }
            case InstructionSet::SUB:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst - src;
                break;
            }
            case InstructionSet::MUL:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst * src;
                break;
            }
            case InstructionSet::DIV:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst / src;
                break;
            }
            case InstructionSet::REM:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst % src;
                break;
            }
            case InstructionSet::CALL:
                vm.call_stack.push( ++vm.pc );
                vm.pc = vm.registers[instruction.operand1];
                break;
            case InstructionSet::CALL_EXT: {
                auto addr = vm.registers[instruction.operand1];
                if(addr >= vm.constant_pool.size()){
                    vm.crash("Invalid instruction CALL_EXT: Unknown function name addr \'" + std::to_string(addr) + "\'");
                    break;
                }
                const char * name = (const char*)(vm.constant_pool.data() + addr);
                if(!vm.external_functions.count(name)){
                    vm.crash(std::string("Invalid instruction CALL_EXT: Unknown function \'") + name + "\'");
                    break;
                }
                vm.external_functions[name](vm);
                break;
            }
            case InstructionSet::RET: {
                if(vm.call_stack.empty()){
                    vm.crash("Invalid instruction RET: Callstack is empty!");
                    break;
                }
                vm.pc = vm.call_stack.top();
                break;
            }
            case InstructionSet::JMPR:{
                vm.pc = vm.registers[instruction.operand1];
                break;
            }

            case InstructionSet::JMP:{
                vm.pc = instruction.immediate;
                break;
            }

            case InstructionSet::JEZ:{
                auto op1 = vm.registers[instruction.operand1];
                if( op1 == 0){
                    vm.pc = instruction.immediate;
                }
                break;
            }
            case InstructionSet::JEO:{
                auto op1 = vm.registers[instruction.operand1];
                if( op1 == 1){
                    vm.pc = instruction.immediate;
                }
                break;
            }

            case InstructionSet::JEZR:{
                auto op1 = vm.registers[instruction.operand1];
                auto op2 = vm.registers[instruction.operand1];
                if( op1 == 0){
                    vm.pc = op2;
                }
                break;
            }
            case InstructionSet::JEOR:{
                auto op1 = vm.registers[instruction.operand1];
                auto op2 = vm.registers[instruction.operand1];
                if( op1 == 1){
                    vm.pc = op2;
                }
                break;
            }
            case InstructionSet::CL:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst < src;
                break;
            }
            case InstructionSet::CLE:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst <= src;
                break;
            }
            case InstructionSet::CG:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst > src;
                break;
            }
            case InstructionSet::CGE:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst >= src;
                break;
            }
            case InstructionSet::CE:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst == src;
                break;
            }
            case InstructionSet::CNE:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = dst != src;
                break;
            }
            case InstructionSet::XOR:{
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = src ^ dst;
                break;
            }
            case InstructionSet::OR: {
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = src | dst;
                break;
            }
            case InstructionSet::NEG:{
                auto &dst = vm.registers[ instruction.operand1 ];
                dst = ~dst;
                break;
            }
            case InstructionSet::AND: {
                auto &dst = vm.registers[instruction.operand1];
                auto &src = vm.registers[instruction.operand2];
                dst = src & dst;
                break;
            }
            case InstructionSet::PUSH:
                vm.stack.push( instruction.immediate );
                break;
            case InstructionSet::PUSHR:
                vm.stack.push( vm.registers[instruction.operand1] ) ;
                break;
            case InstructionSet::POP:
                if(vm.stack.empty()){
                    vm.crash("Invalid pop instruction: stack is empty!");
                    break;
                }
                vm.registers[instruction.operand1] = vm.stack.top();
                vm.stack.pop();
                break;
            default:
                vm.crash("Invalid instruction!");
            case InstructionSet::CRASH:
                vm.crash("CRASH instruction called!");
                break;
        }
    }

    std::stack<int64_t> VirtualMachine::stack_trace() {
        if(this->status == Crashed || this->status == Paused)
            return this->call_stack;
        return {};
    }

    void VirtualMachine::step() {
        this->status = Step;
    }

    void VirtualMachine::crash(const std::string &reason) {
        this->status = Crashed;
        this->reason = reason;
    }

    void VirtualMachine::bind_function(const std::string& name, ExternalFunction function) {
        this->external_functions[name] = std::move(function);
    }

    void VirtualMachine::malloc(VirtualMachine &vm) {
        if(vm.stack.empty()){
            vm.crash("Function malloc: invalid arguments, stack is empty!");
        }
    }

    void VirtualMachine::free(VirtualMachine &vm) {
    }

    std::string VirtualMachine::crash_reason() {
        return this->reason;
    }


}