# Getting Started
Compile with the following command
```bash
gcc -Wall -Wpedantic -std=iso9899:1999 -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion main.c parse_request.c && ./a.out
```

# Coding Guidelines
This server is designed to loosely follow JPL's "Powers of 10" guidelines. Notable exceptions to JPL's rules are as follows:
- Removal of -Wtraditional
- This code uses the heap
