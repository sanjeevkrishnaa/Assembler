# One-Pass Assembler (SIC)

A minimal single pass assembler for a subset of the SIC instruction set.  
It reads an assembly program, builds/updates the SYMTAB on the fly, resolves forward references using backpatching, and emits an object program in standard H / T / E record format.

---

## Build & run

$ gcc main.c optab.c symtab.c tokenize.c -o assembler
$ ./assembler.exe

---

## What this does

- Single pass over the input file (no separate intermediate file / second pass)
- Maintains:
  - OPTAB (mnemonic → opcode)
  - SYMTAB (symbol → address) with forward-reference chains
- Produces object program records:
  - H: Header record
  - T: Text records (max 60 hex chars = 30 bytes of object code per record)
  - E: End record
- Supports indexed addressing via `LABEL,X` (sets the high bit of address)

---

## Project structure

- `main.c`  
  Core assembler logic: parsing lines, managing LOCCTR, generating H/T/E records, flushing text records, and final program length patch.

- `optab.c/.h`  
  Opcode lookup table (`optab_lookup()`).

- `symtab.c/.h`  
  Symbol table + forward reference fixups:
  - Undefined symbol → store a pointer to the address field inside generated object code
  - When symbol is later defined → backpatch all stored locations

- `tokenize.c/.h`  
  Helpers for trimming and tokenization (`trim()`, `next_token()`, `is_mnemonic()`).

- `buffer.h`  
  Data structures for buffering object code into text records (`buffer_node`, `line_data`).

---
