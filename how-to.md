
## HOW TO COMPILE A BASIC PROGRAM

```bash
# Compile the app using quastra-compiler
./build/bin/quastra-compiler example.q > output.cpp
# Build you output file into executable
g++ -std=c++17 output.cpp -o my_program
```

