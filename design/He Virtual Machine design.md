

# Instruction set

Every instruction is a quadruple, it's definition is like
```cpp
std::tuple<uint8_t, uint8_t, uint8_t, int64_t > instruction;
```

| Instruction |  Operator 1   |  Operator 2  | Immediate |                                 Comment                                  |
|:-----------:|:-------------:|:------------:|:---------:|:------------------------------------------------------------------------:|
|     nop     |     None      |     None     |   None    |                                Do nothing                                |
|    move     | register_dst  | register_src |   None    |                       register_dst = register_src                        |
|  load_val   | register_dst  |     None     |  Number   |                   Load a number or address to register                   |
|   read_N    | register_dst  | register_src |   None    |  Read ` N ` bytes signed int from address register_src to register_dst   |
|    read     | register_dst  | register_src |     N     |  Read ` N ` bytes signed int from address register_src to register_dst   |
|   write_N   | register_dst  | register_src |   None    | Write ` N ` bytes unsigned int to address register_dst from register_src |
|    write    | register_dst  | register_src |     N     | Write ` N ` bytes unsigned int to address register_dst from register_src |
|     add     | register_dst  | register_src |   None    |                register_dst = register_dst + register_src                |
|     sub     | register_dst  | register_src |   None    |                register_dst = register_dst - register_src                |
|     mul     | register_dst  | register_src |   None    |                register_dst = register_dst * register_src                |
|     div     | register_dst  | register_src |   None    |                register_dst = register_dst / register_src                |
|     rem     | register_dst  | register_src |   None    |                register_dst = register_dst % register_src                |
|    call     | register_func |     None     |   None    |                  Call function in address register_func                  |
|  call_ext   | register_func |     None     |   None    |             Call external function in register_func(string)              |
|     ret     |     None      |     None     |   None    |                        Return to caller function                         |
|    jmpr     |   register    |     None     |   None    |                 Jump to the ` register ` -th instruction                 |
|     jmp     |     None      |     None     |    Num    |                      Jump to the Num-th instruction                      |
|     jez     |   register    |     None     |    Num    |               if( register == 0 ) goto Num-th instruction                |
|     jeo     |   register    |     None     |    Num    |               if( register == 1 ) goto Num-th instruction                |
|    jezr     |   register    | register_dst |   None    |         if( register == 0 ) goto `register_dst` -th instruction          |
|    jeor     |   register    | register_dst |   None    |          if( register == 1 ) goto `register_dst`-th instruction          |
|     cl      | register_dst  | register_src |   None    |               register_dst = (register_dst < register_src)               |
|     cle     | register_dst  | register_src |   None    |              register_dst = (register_dst <= register_src)               |
|     cg      | register_dst  | register_src |   None    |               register_dst = (register_dst > register_src)               |
|     cge     | register_dst  | register_src |   None    |              register_dst = (register_dst >= register_src)               |
|     ce      | register_dst  | register_src |   None    |              register_dst = (register_dst == register_src)               |
|     cne     | register_dst  | register_src |   None    |              register_dst != (register_dst == register_src)              |
|     xor     | register_dst  | register_src |   None    |                register_dst = register_src ^ register_dst                |
|     or      | register_dst  | register_src |   None    |              register_dst = (register_src \| register_dst)               |
|     neg     | register_dst  |     None     |   None    |                      register_dst = ~ register_dst                       |
|     and     | register_dst  | register_src |   None    |               register_dst = (register_src & register_dst)               |
|    push     |     None      |     None     |   Value   |                     push a immediate value to stack                      |
|    pushr    |   register    |     None     |   None    |                         push a register to stack                         |
|     pop     |   register    |     None     |   None    |                   pop a value  from stack  to register                   | 
|    crash    |     None      |     None     |   None    |                   Cause a error and exit the program.                    |
