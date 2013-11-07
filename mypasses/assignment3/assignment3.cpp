#include "llvm/Pass.h"
#include "llvm/Module.h"
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
		Module *module = func.getParent();
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
						
						Constant* mallocHook = module->getOrInsertFunction("malloc_hook",
								Type::getVoidTy(module->getContext()),
								IntegerType::get(module->getContext(),64),
								NULL) ;

						Function* mallocHookFunction = cast<Function>(mallocHook);
						vector<Value*> argList;
						argList.push_back(static_cast<Value*>(size));
						Instruction *callInst = CallInst::Create(mallocHookFunction,argList);
						callInst->insertAfter(inst);
					}else if(targetFunction->getName() == "free") {
						errs()<<"Number of operands "<<callInst->getNumOperands()<<"\n";
						errs()<<"Sec : "<<*(callInst->getArgOperand(1))<<"\n";
						Value *op = callInst->getArgOperand(0);
						//LoadInst *ldInst = static_cast<LoadInst*>(callInst->getArgOperand(0));
						//errs()<<"Load inst  " <<*(ldInst->getPointerOperand())<<"\n";
				                // op  = ldInst->getPointerOperand();
						//Constant* freeHook = module->getOrInsertFunction("free_hook",
						//		Type::getVoidTy(module->getContext()),
						//		IntegerType::get(module->getContext(),64),
						//		NULL) ;
					        Constant* freeHook = module->getOrInsertFunction("free_hook",
					        		Type::getVoidTy(module->getContext()),
					        		PointerType::get(IntegerType::get(module->getContext(), 8), 0),
					        		NULL);		

					        Function* freeHookFunction = cast<Function>(freeHook);
					        vector<Value*> freeArgList;
						Type *pTy = PointerType::get(IntegerType::get(module->getContext(),8),0);
						freeArgList.push_back(ConstantExpr::getIntToPtr(dyn_cast<Constant>(op), pTy));
					       // freeArgList.push_back(CastInst::CreatePointerCast(op,
					        //		PointerType::get(IntegerType::get(module->getContext(), 8), 0),"Ari",inst));
					        //Instruction *freeCallInst = CallInst::Create(freeHookFunction,freeArgList);
					        //callInst->insertAfter(freeCallInst);
					}
			        }
				errs()<<"\n\n\n";
			}
		}
		return true;
	}
}

char Ass3::ID=0;
RegisterPass<Ass3> X("mem","This calculates the memory foot print ", false ,false);
