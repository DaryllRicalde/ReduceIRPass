# ReduceIRPass

We have also developed an LLVM Pass to obtain a simplified LLVM IR. The LLVM Pass does the following:
 - Loops through the module and demangles the name of the functions, identifying those with an "stl" prefix which indicates these functions are from the C++ Standard Library
 - Removes the call chain that this function dumps

To build the pass:
```
mkdir build
cd build
cmake ..
make
export $LLVM_DIR=<llvm path>
$LLVM_DIR/bin/opt -enable-new-pm=0 -load ./ReduceIRPass/libReduceIRPass.so -legacy-reducer-pass -S <path to test file> -o out.ll

The pass is registered through the LLVM legacy pass manager
```
Users can run main.py to obtain the whole program LLVM that they want to feed into the ReduceIRPass
