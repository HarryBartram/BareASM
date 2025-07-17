# Bare Assembler (BareASM) #

Bare assembler or BareASM is an assembler designed by [The Routinely Interrupts](https://theroutinelyinterrupts.substack.com/).

## Output Formats ##

Output formats include: binary and ELF. Only targets x86-64 machines but can produce 16-bit, 32-bit and 64-bit code.

## Syntax ##

### Symbols ###

Symbols may include any alphanumeric character upper or lowercase and the following characters: '_', '?', '!'. However they cannot begin with a number.

BareASM includes predefined symbols that cannot be overwritten these all begin with '__'.

Full list of predefined symbols:
 - __HERE - Refers to the address of the beginning of the current line.
 - __SECTION_BEGIN - Refers to the address at the beginning of the current section used in non binary formats.
 - __ORG - Refers to the address set in the org directive for binary formats.

### Constants ###

Numerical formats:
 - 200/200d/0200 - decimal.
 - 0x200/200h - hexadecimal.
 - 0b100/100b - binary.

Other formats:
 - "string" - Resolves to ASCII encoding, use backslash escape sequences.

### Expressions ###

Numeric expressions in BareASM are evaluated as 64-bit signed integers, there size will then be adjusted accordingly.

Full list of operators(in order of precedence):
 - '+','-' - Carries out addition and subtraction respectively.
 - '*','/' - Carries out multiplication and division respectively

### Effective Addresses ###

This refers to any operand that references a memory address. In order to use an effective address the operand should be wrapped in angle brackets.

For example the following instruction moves the value in the eax register into the memory address at 0x500: 'mov <0x500>, eax'.

### Macros ###

Macros are instructions for the preprocessor.

Macros begin with a '#' character for example: '#include "file.asm"'.

Full list:
 - #include "file" - Replaces the macro with the full contents of the specified file.
 - #define symbol "value" - Replaces all occurances of the symbol with the value.
 - #repeat n "value" - Copies the string line after line n times. 

### Directives ###

Directives are special instructions that give information on the context in which the rest of the program or parts of it are to be assembled.

Directives apply only after they occur in the file and may be overwritten later on, i.e. '[bits 64]' and '[bits 32]' later on would create both 64-bit code and then 32-bit code.

Directives are encased with square brackets for example: '[bits 64]'

Full list:
 - [bits (16/32/64)] - Creates machine code for the stated number of bits after the directive.
 - [section symbol] - Creates a section in an object file including the following code.
 - [import symbol] - Used the declare a symbol not yet defined in the program so it can be resolved later on in linking.
 - [export symbol] - Used to export a symbol defined in the current program so other object files can find it in linking.
 - [org address] - Used in binary formats where basm can assume where the program is to be loaded in memory.

### Labels and Sublabels ###

Labels are used to refer to a point in the program and will resolve to the address at which they are located.

Labels and sublabels have the same syntactic requirements as symbols.

Code after labels should be indented and may include sublabels, for example:
````
label:
    jmp .sublabel1
.sublabel2:
    mov eax, 1
    ret
.sublabel1:
    jmp .sublabel2
````

Sublabels as shown previously, inside a label sublabels can be refered to using only the . and symbol name. However outside of the label the entire label must be used, i.e. 'label.sublabel1'.

### Comments ###

Single line comments begin with ';' and continue until the end of the line.

## Instructions ##

The following is a full list of the instructions supported by BareASM.

Instructions are not case sensitive.

### Pseudo-instructions ###

These instructions do not translate directly to any specific machine code but will still be translated into binary.

Full list:
 - DB - Declare bytes.
 - DW - Declare words, end is padded to 2 bytes for ascii.
 - DD - Declare double words, end is padded to 4 bytes for ascii.
 - DQ - Declare quad words, end is padded to 8 bytes for ascii.

### Conventional Instructions ###

Full list:
 - ADD
 - CALL
 - HLT