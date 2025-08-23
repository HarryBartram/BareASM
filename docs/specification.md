# Bare Assembler (BareASM) #

Bare assembler or BareASM is an assembler designed by [The Routinely Interrupts](https://theroutinelyinterrupts.substack.com/welcome).

## Output Formats ##

Output formats include: binary and ELF. Only targets x86-64 machines but can produce 16-bit, 32-bit and 64-bit code.

## Preprocessor ##

Macros must exist on their own line and may be preceded by whitespace, which in this case is strictly defined as ' ' and '\t' characters.

The preprocessor uses a separate lexical analyser than the assembler, such that any non valid symbol characters as well as strings character are interpreted as whitespace. This means that '#define,()SYMBOL+-"value"' is technically a valid statement in the preprocessor although using it this way is discouraged.

Comments can still exist on lines that contain macros.

### Macros ###

Macros are instructions for the preprocessor.

Macros begin with a '#' character for example: '#include "file.asm"'.

Full list:
 - #include "file" - Replaces the macro with the full contents of the specified file.
 - #define symbol "value" - Replaces all occurances of the symbol with the value.
 - #repeat n "value" - Copies the string line after line n times. 

## Syntax ##

### Statement Breaks ###

All statements (apart from directives, label blocks and preprocessor macros) must end with a statement break: ':'.

### Symbols ###

Symbols may include any alphanumeric character upper or lowercase and the following characters: '_', '?', '!', '#'. However they cannot begin with a number.

Essentially anything that isn't a symbol character or operator is considered whitespace.

BareASM includes predefined symbols that cannot be overwritten these all begin with '__', symbols also cannot be directive names, instructions, or register names (all of which are case insensitive).

Full list of predefined symbols:
 - __HERE - Refers to the address of the beginning of the current line.
 - __SECTION_BEGIN - Refers to the address at the beginning of the current section used in non binary formats.
 - __ORG - Refers to the address set in the org directive for binary formats.

### Constants ###

Numerical formats:
 - 200 - Decimal.
 - 0x200 - Hexadecimal.

Other formats:
 - "string" - Resolves to ASCII encoding, use backslash escape sequences.

### Expressions ###

Integer expressions in BareASM are evaluated as 64-bit signed integers, there size will then be adjusted accordingly.

Full list of operators(in order of precedence):
 - '+','-' - Carries out addition and subtraction respectively.
 - '*','/' - Carries out multiplication and division respectively

### Effective Addresses and Segment Overrides ###

Effective addresses refers to any operand that references a memory address. In order to use an effective address the operand should be wrapped in angle brackets.

For example the following instruction moves the value in the eax register into the memory address at 0x500: 'mov <0x500>, eax'.

Segment overrides refers to operands that refer to a memory address, however they use an offset and a base part. The base is stored in a segment register, refered to after an '@' character, and the will be wrapped in angle brackets.

The following instruction shows a segment override, where the value in the eax register is moved into the memory address at an offset of 0x500 from the value stored in the ds regsiter: 'mov <0x500>@ds, eax'.

### Directives ###

Directives are special instructions that give information on the context in which the rest of the program or parts of it are to be assembled.

Directives apply only after they occur in the file and may be overwritten later on, i.e. '[bits 64]' and '[bits 32]' later on would create both 64-bit code and then 32-bit code.

Directives are encased with square brackets for example: '[bits 64]'

Full list:
 - [bits 16/32/64] - Creates machine code for the stated number of bits after the directive.
 - [section symbol] - Creates a section in an object file including the following code.
 - [import symbol] - Used the declare a symbol not yet defined in the program so it can be resolved later on in linking.
 - [export symbol] - Used to export a symbol defined in the current program so other object files can find it in linking.
 - [org address] - Used in binary formats where basm can assume where the program is to be loaded in memory.

### Labels ###

Labels are used to refer to a point in the program and will resolve to the address at which they are located, there are two types of labels: master and slave.

Master labels are universally accessible throughout the program, they are defined by stating the name of the label (these cannot contain a '.' character) then they are followed by a '{' character which begins the so called block that contains all the labels contents this is ended by the '}'.

Slave labels can only exist within the block of a master label, within this block they can be refered to using just their name. They must begin with a '.' character and are defined by stating the name of the label with a statement break character. Outside of their master label's block they can only be refered to by using the master label's name and then the slave label's name, including the dot and no whitespace between.

The illustration below shows a master label (label) and two slave labels (.slavelabel1 and .slavelabel2):
````
label {
    jmp .slavelabel1:
.slavelabel2:
    mov eax, 1:
    ret:
.slavelabel1:
    jmp .slavelabel2:
}

jmp label.slavelabel2:
````

### Comments ###

Single line comments begin with ';' and continue until the end of the line.

## Instructions ##

The following is a full list of the instructions supported by BareASM.

Instructions are not case sensitive.

### Conventional ###

 - ADD
 - AND
 - CALL
 - CLI
 - CMP
 - DIV
 - HLT
 - IN
 - INC
 - INT
 - JA
 - JAE
 - JB
 - JBE
 - JC
 - JCXZ
 - JECXZ
 - JRCXZ
 - JE
 - JG
 - JGE
 - JL
 - JLE
 - JNA
 - JNAE
 - JNB
 - JNBE
 - JNC
 - JNE
 - JNG
 - JNGE
 - JNL
 - JNLE
 - JNO
 - JNP
 - JNS
 - JNZ
 - JO
 - JP
 - JPE
 - JPO
 - JS
 - JZ
 - JMP
 - LGDT
 - LIDT
 - LOOP
 - MOV
 - MUL
 - OR
 - OUT
 - POP
 - PUSH
 - RDMSR
 - REP
 - RET
 - SHL
 - SHR
 - STI
 - TEST
 - WRMSR
 - XOR


### Pseudo-instructions ###

These instructions do not translate directly to any specific machine code but will still be translated into binary.

 - DB - Declare bytes.
 - DW - Declare words, end is padded to 2 bytes for ascii.
 - DD - Declare double words, end is padded to 4 bytes for ascii.
 - DQ - Declare quad words, end is padded to 8 bytes for ascii.
 - TIMES - Will repeat a statement 'n' times, unlike repeat macro this has access to symbols like: __HERE, __SECTION_BEGIN and __ORG.