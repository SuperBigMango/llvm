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
		public:
			static char ID;
			Ass3();
			virtual bool runOnFunction(Function&);
	};

	Ass3::Ass3():FunctionPass(ID) {
	}

	bool Ass3::runOnFunction(Function &func) {
		if(func.getName() == "malloc_hook" || func.getName() == "free_hook") {
			return true;
		}

		Module *module = func.getParent();
		for(inst_iterator I = inst_begin(func), E = inst_end(func); I != E ; ++I ) {
			if(isa<CallInst>(*I)){
				if(func.getName() == "jpeg_get_small")  {
					bool deb = true;
				}
				Instruction *inst = &*I;
				CallInst *callInst = static_cast<CallInst*>(inst);
				Function *targetFunction = callInst->getCalledFunction();
				if(targetFunction && targetFunction->getName() == "malloc") {
					Value  *size = callInst->getArgOperand(0);
					Constant* mallocHook = module->getOrInsertFunction("malloc_hook",
							Type::getVoidTy(module->getContext()),
							PointerType::get(IntegerType::get(module->getContext(), 8), 0),
							IntegerType::get(module->getContext(),64),
							NULL) ;

					Function* mallocHookFunction = cast<Function>(mallocHook);
					vector<Value*> argList;
					Instruction *ii = &*(++I);
					--I;
					argList.push_back(CastInst::CreatePointerCast(callInst,
								PointerType::get(IntegerType::get(module->getContext(), 8), 0),
								"mallocHookCast",ii));
					//argList.push_back(static_cast<Value*>(size));
					argList.push_back(size);
					Instruction *mallocCallInst = CallInst::Create(mallocHookFunction,argList);
					mallocCallInst->insertBefore(ii);
				}else if(targetFunction && targetFunction->getName() == "free") {
					Value *op = callInst->getArgOperand(0);
					Constant* freeHook = module->getOrInsertFunction("free_hook",
							Type::getVoidTy(module->getContext()),
							PointerType::get(IntegerType::get(module->getContext(), 8), 0),
							NULL);		

					Function* freeHookFunction = cast<Function>(freeHook);
					vector<Value*> freeArgList;
					freeArgList.push_back(CastInst::CreatePointerCast(op,
								PointerType::get(IntegerType::get(module->getContext(), 8), 0),
								"freeHookCast",inst));
					Instruction *freeCallInst = CallInst::Create(freeHookFunction,freeArgList);
					freeCallInst->insertBefore(inst);
				}
			}
		}
		return true;
	}
}

char Ass3::ID=0;
RegisterPass<Ass3> X("mem","This calculates the memory foot print ", false ,false);
