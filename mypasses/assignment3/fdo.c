#include "llvm/Pass.h"
#include <stdio.h> 
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Constants.h"
#include "llvm/Support/CallSite.h"
#include "llvm/InstrTypes.h"
#include <string>

using namespace std;
using namespace llvm;


namespace {
	struct fdo :public FunctionPass {
		static char ID ;
		fdo():FunctionPass(ID){};
		virtual bool runOnFunction(Function &F);
	};
 
}

bool fdo::runOnFunction(Function &F) {
	if (F.getName()=="nabanita_profile") return false;
	const char * from=F.getName().data();
	const char * to;
	Module *M =F.getParent();
	
	for (Function::iterator BB = F.begin(), BBE = F.end(); BB != BBE; ++BB){
      		for (BasicBlock::iterator II = BB->begin(), IE = BB->end() ; II!= IE ; ++II){
        		CallSite CS(cast<Value>(II));
        		if (CS) {
				 Function *Fn = CS.getCalledFunction();
          			 if(Fn && Fn->getName().size()!=0)
					to=CS.getCalledFunction()->getName().data();
				 else 
					continue;
				
				Constant * Init_to = ConstantDataArray::getString (II->getContext(),to);
				Constant * Init_from = ConstantDataArray::getString (II->getContext(),from);
				Value * To=new GlobalVariable (*M,Init_to->getType(),true,GlobalValue::InternalLinkage,Init_to,"callee");
				Value * From=new GlobalVariable (*M,Init_from->getType(),true,GlobalValue::InternalLinkage,Init_from,"caller");
				Constant* profile = M->getOrInsertFunction("nabanita_profile",Type::getVoidTy(M->getContext()),PointerType::get(IntegerType::get(M->getContext(), 8), 0),PointerType::get(IntegerType::get(M->getContext(), 8), 0),NULL);		
				Function *CallFn=cast<Function>(profile);	
				vector<Value*> Args;
				Args.push_back(CastInst::CreatePointerCast(From,PointerType::get(IntegerType::get(M->getContext(), 8), 0) ,"fromptr",II));
				Args.push_back(CastInst::CreatePointerCast(To,PointerType::get(IntegerType::get(M->getContext(), 8), 0) ,"toptr",II));
				Instruction *callinst=CallInst::Create(CallFn,Args);
				callinst->insertBefore(II);
        		}
      		}
	}
	
	return false;
}



char fdo::ID = 0;
static RegisterPass<fdo> X("gen","This is a profile generator",false,false);
