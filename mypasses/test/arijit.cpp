#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"

using namespace llvm;

#include <iostream>
#include <string>
using namespace std;


namespace {
	class Arijit : public LoopPass {

		private:
			void printInstructions(Function &f);
			int counter;
		public:
			static char ID ;	
			Arijit():LoopPass(ID){
				counter = 0;
			}
			virtual bool runOnLoop(Loop *loop, LPPassManager &lpm)  {
				counter++;
				errs()<<"Loop Count :"<<counter<<"\n";
				return false;
			}

	};

}

char Arijit::ID=0;
static RegisterPass<Arijit> X("arijit" ,"This is test pass",false,false);

/*
namespace {
	class Arijit : public FunctionPass {

		private:
			void printInstructions(Function &f);
		public:
			static char ID ;
			Arijit():FunctionPass(ID){}
			virtual bool runOnFunction(Function &f ) {
				LoopInfo& li = getAnalysis<LoopInfo>(f);
				int loopCount = 0;
                                for(Function::iterator bBlock = f.begin(); bBlock != f.end() ; ++bBlock ) {
					Loop *loop = li.getLoopFor(bBlock);
					if(loop)
						loopCount++;					
				}
				errs()<<"Loop Count : " << loopCount<<"\n";
				return false;
			}


	};

	void Arijit::printInstructions(Function &f) {

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
	}
}

char Arijit::ID=0;
static RegisterPass<Arijit> X("arijit" ,"This is test pass",false,false);

*/
