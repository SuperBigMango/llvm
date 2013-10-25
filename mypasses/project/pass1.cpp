#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/BasicBlock.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/Constant.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {

	class IRModification:public FunctionPass {
		public:
			static char ID;
			IRModification();
			virtual bool runOnFunction(Function&);
	};

	IRModification::IRModification():FunctionPass(ID) {}
	
	bool IRModification::runOnFunction(Function& func) {
		errs()<<"Function name is  "<<func.getName()<<"\n";
	        Module* module = func.getParent();
	        Constant* hook = module->getOrInsertFunction("hook",
	        			Type::getVoidTy(module->getContext()),
	        			Type::getVoidTy(module->getContext()),
	        			NULL
	        		);
                Function* hookFunction = cast<Function>(hook);
                Instruction* hookInstruction = CallInst::Create(hookFunction);
                for(BasicBlock *bBlock = func.begin(); bBlock != func.end() ; ++bBlock) {
	        	Instruction* termInst = bBlock->getTerminator();
	        	hookInstruction->insertBefore(termInst);
                }
		return true;
	}

}


char IRModification::ID=0;
static RegisterPass<IRModification> M("modify","This is a test pass",false,false);
