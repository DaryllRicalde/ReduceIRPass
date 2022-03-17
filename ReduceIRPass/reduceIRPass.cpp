#include <string>
#include <cstddef>
#include <cxxabi.h>
#include <iostream>
#include <exception>
#include <stdlib.h>
#include <unordered_map> 
#include <vector>

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// $LLVM_DIR/bin/opt -enable-new-pm=0 -load ./ReduceIRPass/libReduceIRPass.so -legacy-reducer-pass -analyze ../basic_cpp_tests_BC/list-1.cpp.bc -o new.ll
// $LLVM_DIR/bin/opt -enable-new-pm=0 -load ./ReduceIRPass/libReduceIRPass.so -legacy-reducer-pass -S ../basic_cpp_tests_BC/list-1.cpp.bc -o out.ll

/*

1. Create hashmap 
      hmap[Function] = all the function which it calls 
       
*/

namespace {
    
    struct ReducerPass : public ModulePass{
        static char ID;
        ReducerPass() : ModulePass(ID) {}

        virtual bool runOnModule(Module &M) override {
          // iterate through every Function in the Module
          std::unordered_map<Function*, std::vector<Function*>> calls;
          std::vector<Function*> vec;
          std::string prefix = "std::"    // for finding STL functions
          for(auto &Func : M){
            StringRef name = Func.getName();
            // name.str() -> get contents of StringRef object as a string
            if(!name.str().empty()){
                // errs() << "Mangled name: " << name << "\n";
                std::string demangled_name = demangle(name.str());
                if(demangled_name == "") { errs() << "Empty demangled name returned!" << "\n"; }
                if(demangled_name.starts_with(prefix)){
                  vec.push_back(&Func);
                }
            //     errs() << "Demangled name: " << demangled_name << "\n";
            //     // Func.setName(demangled_name);
            //     // errs() << "Check if the function's name has been set to demangled version: " << Func.getName() << "\n";
            //     // errs() << "-------------------------------------------------" << "\n";
            // }

            // for(auto &BB : Func){
            //   for(auto &Ins : BB){

            //   }
            // }
          }
          delete_chain(vec);
            return true;
        }

        // Demangle function names
        std::string demangle(const std::string &name){
            int status = -1;        // some arbitrary value to eliminate the compiler warning

            // abi::__cxa_demangle(const char* mangled_name,
            //                     char* output_buffer, size_t* length,                     
            //                                           int* status)
            char buf[1024];
            //unsigned int size = 1024;
            size_t size = 1024;
            if(name.c_str() != nullptr){
                char* result = abi::__cxa_demangle(name.c_str(),buf,&size,&status);
                if(result != nullptr){
                    return result;
                }
            }
            return "";
        }   // end demangle()

        void delete_chain(std::vector<Function*> vec){
          errs() << "Calling delete_chain()" << "\n";
          for(auto F : vec){
            F->replaceAllUsesWith(UndefValue::get(F->getType()));
            F->eraseFromParent();
          }

        }

        /* ------------------------------------------------------------------------------------
          HELPER FUNCTIONS
        -------------------------------------------------------------------------------------*/ 
        void print(std::vector<Function*> vec){
          errs() << "Calling print() " << "\n";
          for(auto F : vec){
            errs() << "Function: " << F->getName() << "\n";
          }
        }
    }; // end struct
} // end anonymous namespace


char ReducerPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerReducerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new ReducerPass());
}

//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------

// #1 REGISTRATION FOR "opt -analyze -legacy-opcode-counter"
static RegisterPass<ReducerPass> X(/*PassArg=*/"legacy-reducer-pass",
                                           /*Name=*/"Legacy Reducer Pass",
                                           /*CFGOnly=*/true,
                                           /*is_analysis=*/false);


static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerReducerPass);