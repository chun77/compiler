# Compiler Project

This project implements a basic compiler using C++, Flex, and Yacc. The compiler performs lexical analysis, syntax analysis, semantic analysis, and code generation with various optimization techniques.

## Features

- **Lexical Analysis**: Tokenizes input source code using Flex.
- **Syntax Analysis**: Parses tokens using Yacc to generate syntax trees.
- **Semantic Analysis**: Ensures the correctness and reliability of the code.
- **Code Generation**: Generates assembly code with optimizations like live variable analysis and linear scan register allocation.
- **Intermediate Code Representation**: Utilizes abstract syntax trees (AST) and three-address code (TAC) for efficient code optimization and translation.
- **Modular Architecture**: Designed for maintainability and scalability, handling complex expressions and control structures.

## Technologies Used

- **C++**
- **Flex** (Lexical Analysis)
- **Yacc** (Syntax Analysis)
- **Makefile** (Build Automation)

## Project Structure

```
.
├── include         # Header files
│   ├── ...         # Other header files
├── src             # Source files
│   ├── ...         # Other source files
├── Makefile        # Build script
└── README.md       # Project description
```

## Installation and Usage

1. **Clone the repository**:
   ```bash
   git clone https://github.com/chun77/compiler.git
   cd compiler
   ```

2. **Build the project**:
   ```bash
   make
   ```

3. **Run the compiler**:
   ```bash
   ./compiler <input-file>
   ```

## Examples

To compile a source file:

```bash
./compiler examples/test.c
```
