

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
#include "llvm/Support/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Bitcode/ReaderWriter.h"

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
#include "llvm/ADT/StringExtras.h"
//#include "llvm/Support/Streams.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/InstrTypes.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <set>
#include <vector>

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

		void insertFunction(Function& func) {
		      /*  Module *module = func.getParent();
			Constant *c =  module->getOrInsertFunction("test",NULL);
			Function *f = cast<Function>(c);
			f->setCallingConv(CallingConv::C);
			BasicBlock* block = BasicBlock::Create(getGlobalContext(),"entry",f);
			IRBuilder<> builder(block);
			*/
		}



		virtual bool runOnFunction(Function &func) {
			if(func.getName()=="main") {
				for(inst_iterator I = inst_begin(func), E = inst_end(func); I != E ; ++I) {
					if(isa<GetElementPtrInst>(*I)) {
						Instruction *inst = &*I;
						GetElementPtrInst* ptrInstr = cast<GetElementPtrInst>(inst);
						int NumElements = 0 ;
						gep_type_iterator ptrInstrItr = gep_type_begin(ptrInstr);
						while(ptrInstrItr != gep_type_end(ptrInstr)) {
							if(const ArrayType *array = dyn_cast<ArrayType>(*ptrInstrItr))	{
								NumElements = array->getNumElements();
							}
							++ptrInstrItr;
						}


						Value *value = ptrInstr->getOperand(2);
						
						if(value==NULL || NumElements==0)
							continue;

						const IntegerType *intType = IntegerType::get(func.getContext(), 64);
						Constant *cons = ConstantInt::get(const_cast<IntegerType*>(intType), NumElements);
						Value *upperbound = cast<Value>(cons);

						BasicBlock *currBlock = inst->getParent();

						ICmpInst *cmpinst = new ICmpInst(inst, ICmpInst::ICMP_SLT, value, upperbound);
						BasicBlock *newBlock = currBlock->splitBasicBlock(inst, "bounds");
						TerminatorInst *termInst = currBlock->getTerminator();
						termInst->eraseFromParent();


						Twine t2("endBlock");
						BasicBlock *endBlock = BasicBlock::Create(func.getContext(),t2,&func);
						const IntegerType *intType2 = IntegerType::get(func.getContext(), 32);
						Constant *retVal2 = ConstantInt::get(const_cast<IntegerType*>(intType2),0);
						ReturnInst *retInst2 = ReturnInst::Create(func.getContext(),cast<Value>(retVal2));
						endBlock->getInstList().push_back(retInst2);
						BranchInst* finalJmp = BranchInst::Create(endBlock);

						// BranchInst* finalJmp = builder.CreateBr(endBlock);
						/** 
						 * Attach a terminator instructions
						 const IntegerType *intType1 = IntegerType::get(func.getContext(), 32);
						 Constant *retVal = ConstantInt::get(const_cast<IntegerType*>(intType1),0);
						 ReturnInst *retInst = ReturnInst::Create(func.getContext(),cast<Value>(retVal));
						 currBlock->getInstList().push_back(retInst);
						 */
						//  showAllInstructions(currBlock);

						Twine t("elseblock");
						BasicBlock *elseBlock = BasicBlock::Create(func.getContext(),t,&func);
						Module *module = func.getParent();
						Constant* hook = module->getOrInsertFunction("print",Type::getVoidTy(module->getContext()),(Type*)0) ;
						Function* hookFunction = cast<Function>(hook);
						Instruction *callInst = CallInst::Create(hookFunction, "");
						elseBlock->getInstList().push_back(callInst);
						elseBlock->getInstList().push_back(finalJmp);

						/*
						const IntegerType *intType1 = IntegerType::get(func.getContext(), 32);
						Constant *retVal = ConstantInt::get(const_cast<IntegerType*>(intType1),1);
						ReturnInst *retInst = ReturnInst::Create(func.getContext(),cast<Value>(retVal));
						elseBlock->getInstList().push_back(retInst);
						// showAllInstructions(elseBlock);
						*/

						IRBuilder<> builder(elseBlock);
						BranchInst *branchInst = BranchInst::Create(newBlock, elseBlock,cmpinst);
						currBlock->getInstList().push_back(branchInst);
						 
						showAllInstructions(currBlock);
						showAllInstructions(newBlock);
						showAllInstructions(elseBlock);
						break;
					}
				}
			}
			return true;
		}
	};
} 

char Assignment2::ID = 0;
static RegisterPass<Assignment2> X("arijit","This is a test pass",false,false);
