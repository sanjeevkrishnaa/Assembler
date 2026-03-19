# SIC Assembler Implementations

This folder contains two implementations of a SIC assembler:

- `one_pass/` - a one-pass SIC assembler that builds the symbol table and generates object code in a single scan.
- `two_pass/` - a two-pass SIC assembler where Pass 1 builds the symbol table and computes addresses, and Pass 2 generates the final object program.

Both implementations target the SIC instruction set and produce object code in standard H / T / E record format.

## Folder Structure

```text
Assignment_2_Assembler/
|-- one_pass/
`-- two_pass/
```

## Notes

- See `one_pass/README.md` for details about the one-pass implementation.
- See `two_pass/README.md` for details about the two-pass implementation.
