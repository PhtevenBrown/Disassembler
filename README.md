# Disassembler

This disassembler operates in the context of a conceptual computer that simulates many of the basic features found in modern computers. More detailed info in pdf.

Three possible arguments when loading the combiled program: `-d` loads the default instructions, `-c` allows manual console input of instructions, `-f` loads our set of written instructions.

Instructions are to be placed in a folder named `tmp` at the root of the directory. This was hardcoded in due to privileges on university machines.

Instruction format:
4-bit opcode followed by 12-bit address.

|Binary|Mnemonic|Description|
|:---:|:---:|:---|
|0001	|LDA	|Load the contents at address into AC.|
|0010	|STA	|Store the contents of AC to address.|
|0011	|SUB	|Subtract the contents at address from AC and store the result in AC.|
|0100	|ADD	|Add the contents at address to AC and store the result in AC.|
|0101	|PUT	|Input a value from the keyboard into AC.|
|0110	|OUT	|Output the value in AC to the display.|
|0000	|HLT	|Terminate the program.|
|0111	|SKP	|Skip the next instruction on condition.|
|1000	|JMP	|Load the address into PC.|
|1001	|JZE	|If AC = 0 jump to the address.|
|1010	|SHL	|Shifts the AC left by 1.|
|1011	|SHR	|Shifts the AC right by 1.|
