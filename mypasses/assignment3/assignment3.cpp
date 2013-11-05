#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/ADT/ArrayRef.h"

using namespace llvm;

#include <vector>
using namespace std;

namespace {


	class Ass3:public FunctionPass {
		private:
			vector<StringRef> instrFunctions;
                        bool find(StringRef str) {
				for(vector<StringRef>::iterator itr = instrFunctions.begin() ; itr != instrFunctions.end(); ++itr) {
					if(*itr == str) {
						return true;
					}
				}	
				return false;
			}

		public:
			static char ID;
			Ass3();
			virtual bool runOnFunction(Function&);
	};

	Ass3::Ass3():FunctionPass(ID) {
		instrFunctions.push_back(StringRef("malloc"));
		instrFunctions.push_back(StringRef("free"));
	}

	bool Ass3::runOnFunction(Function &func) {
		for(inst_iterator I = inst_begin(func), E = inst_end(func); I != E ; ++I ) {
			if(isa<CallInst>(*I)){
				Instruction *inst = &*I;
				CallInst *callInst = static_cast<CallInst*>(inst);
				Function *targetFunction = callInst->getCalledFunction();
			        if(find(targetFunction->getName())) {
			        	errs() <<"Found the function"<<"\n";
					errs()<<"Instruction is " <<*callInst<<"\n";
					errs()<<"Tall : "<<*(callInst->getArgOperand(0))<<"\n";
					if(targetFunction->getName() == "malloc") {
						ConstantInt*size = dyn_cast<ConstantInt>(callInst->getArgOperand(0));
						errs()<<"Size  is " <<*(size->getValue().getRawData())<<"\n";
					}else if(targetFunction->getName() == "free") {
					 	if(isa<LoadInst>(callInst->getArgOperand(0))) {
							LoadInst *ldInst = cast<LoadInst>(callInst->getOperand(0));
							errs()<<"This is load instruciton "<<ldInst->getPointerAddressSpace()<<"\n";

						}       
					}
			        }
				errs()<<"\n\n\n";
				

			}
		}
		return false;
	}
}

char Ass3::ID=0;
RegisterPass<Ass3> X("mem","This calculates the memory foot print ", false ,false);
