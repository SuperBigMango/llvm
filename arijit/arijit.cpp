#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#include <iostream>
#include <string>
using namespace std;


namespace {
	struct Arijit : public FunctionPass {
		static char ID ;
		Arijit():FunctionPass(ID){}
		virtual bool runOnFunction(Function &f ) {
			errs()<<"\n\n";
			errs().write_escaped(f.getName())<<"\n";
			int count = 0;
			for(Function::iterator b = f.begin() ; b != f.end() ; ++b) {
				count ++;		
                                //string output = "Basic block namea " +b->getName()+"  having " +f->size() +" number of instructions\n\n";
				errs().write_escaped(b->getName()) << ": " <<b->size()<<"\n";
				for(BasicBlock::iterator instr = b->begin(); instr != b->end(); instr++) {
					errs()<<*instr<<"\n";
				}

			} 

			errs()<<"Number of basic blocks" <<count<<"\n";
			errs()<<"\n\n";
			return false;
		}
	};
}

char Arijit::ID=0;
static RegisterPass<Arijit> X("arijit" ,"This is test pass",false,false);
