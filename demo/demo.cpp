#include <hevm.hh>


void he_puts(HeVM::VirtualMachine &vm){
    if(vm.stack.empty()){
        vm.crash("Function he_puts: invalid arguments, stack is empty!");
        return;
    }
    int64_t offset = vm.stack.top(); vm.stack.pop();
    puts((const char*)vm.constant_pool.data() + offset);
}

int main(){
    std::vector<HeVM::Instruction> program = {
        {HeVM::PUSH, 0, 0, 5},
        {HeVM::LOAD_VAL, 1, 0, 0},
        {HeVM::CALL_EXT, 1, 0, 0},
        { HeVM::JMP, 0, 0, 0},
    };

    const char data[] = "puts\0Hello World!";
    std::vector<uint8_t> constants = { data, data + sizeof(data)};

    auto vm = HeVM::CreateVirtualMachine(program, constants);
    vm->bind_function("puts", he_puts);
    vm->run();
    return 0;
}