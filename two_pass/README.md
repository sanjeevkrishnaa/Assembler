# Two-Pass Assembler (SIC)

This project implements a two-pass assembler for a subset of the SIC architecture.  
It translates SIC assembly language into an object program using the standard H / T / E record format.

---

## Build & Run

$ gcc main.c optab.c symtab.c tokenize.c -o assembler
$ ./assembler.exe

---

## Overview

The assembler works in two distinct passes:

### Pass 1 — Analysis
- Scans the source program line by line
- Assigns addresses using LOCCTR
- Builds the SYMTAB (symbol table)
- Handles assembler directives (`START`, `WORD`, `RESW`, `RESB`, `BYTE`, `END`)
- Detects basic errors (e.g., duplicate symbols)
- Computes total program length

### Pass 2 — Synthesis
- Uses OPTAB and SYMTAB to generate object code
- Resolves all symbol addresses
- Produces the final HTE object program
- Packs object codes into text records (max 30 bytes per record)

---

## Project Structure

├── main.c # Pass 1 + Pass 2 control logic, HTE generation
├── optab.c/.h # Opcode table and lookup
├── symtab.c/.h # Symbol table implementation
├── tokenize.c/.h # Line parsing and token utilities