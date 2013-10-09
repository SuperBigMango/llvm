

#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Analysis/Trace.h"
#include "llvm/Analysis/ConstantsScanner.h"
#include "llvm/Instructions.h"
#include "llvm/Constants.h"
#include "llvm/LLVMContext.h"

using namespace llvm;

#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
using namespace std;



#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/BasicBlock.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/CFG.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/InstrTypes.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"

using namespace llvm;


namespace {
	vector<BasicBlock*> basicBlockVector;
	struct Assignment2 :public FunctionPass {
		Trace *t;
		static char ID ;
		Assignment2():FunctionPass(ID){}

		void showAllInstructions(BasicBlock *bBlock) {
			errs()<<"Curren Basic Block " <<bBlock->getName()<<"\n";
			for(BasicBlock::iterator inst = bBlock->begin() ; inst!= bBlock->end(); ++inst) {
				errs()<<"Instruction : "<<*inst<<"\n";
			}

			errs()<<"\n***********************\n";
		}




		virtual bool runOnFunction(Function &func) {
		      if(func.getName() != "compare") {
			for(inst_iterator I = inst_begin(func), E = inst_end(func); I != E ; ++I) {
				if(isa<GetElementPtrInst>(*I)) {
					Instruction *inst = &*I;
					GetElementPtrInst* ptrInstr = cast<GetElementPtrInst>(inst);
					int NumElements = 0 ;
					Module *module = func.getParent();

					gep_type_iterator ptrInstrItr = gep_type_begin(ptrInstr);
					while(ptrInstrItr != gep_type_end(ptrInstr)) {
						if(const ArrayType *array = dyn_cast<ArrayType>(*ptrInstrItr))	{
							NumElements = array->getNumElements();
						}
						++ptrInstrItr;
					}


					Value *value;
					if(ptrInstr->getNumIndices() >= 2) {
						value  = ptrInstr->getOperand(2);
					}else if(ptrInstr->getNumIndices() == 1){
						value = ptrInstr->getOperand(1);
						continue;
					}

					if(value==NULL || NumElements==0)
						continue;

					if(value->getType() != IntegerType::get(module->getContext(),64)) {
						continue;
					}

					ConstantInt* upperBound = ConstantInt::get(module->getContext(),APInt(64,NumElements)) ;

					Constant* hook = module->getOrInsertFunction("compare",
							Type::getVoidTy(module->getContext()),IntegerType::get(module->getContext(),64),
							IntegerType::get(module->getContext(),64),
							NULL) ;

					Function* hookFunction = cast<Function>(hook);
					vector<Value*> argList;
					argList.push_back(value);
					argList.push_back(upperBound);
					Instruction *callInst = CallInst::Create(hookFunction,argList);
					callInst->insertBefore(inst);
				}
			}
		}
			return true;
		}
	};
} 

char Assignment2::ID = 0;
static RegisterPass<Assignment2> X("arijit","This is a test pass",false,false);
