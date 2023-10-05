# VAssembler
Compiler for simplified assembler-like language. Translates the following
commands to the bytecode compatible with my [VM_CPU](https://github.com/victorbaldin56/VM_CPU).

## Usage
### Build
1. `git clone https://github.com/victorbaldin56/VAssembler`
2. `make`

### Supported commands
This virtual CPU currently
supports the entire list of the following:
| Command code | Command name | What command does |
| :----------: | :----------: | :---------------: |
| -1           | hlt          | Halt; stop the program |
| 0            | in           | Take a value from standart input and push it to stack |
| 1            | out          | Pop a last value from stack and print it |
| 2            | push         | Push an argument following to the command code to stack |
| 3            | add          | Pop 2 last values from stack and push the sum to stack |
| 4            | sub          | Substract 2 last numbers from stack |
| 5            | mult         | Multiply |
| 6            | div          | Divide; raise signal if denominator is zero |
| 7            | sqrt         | Take a square root |


## Support
Created by [victorbaldin56](https://github.com/victorbaldin56)
