#include <iostream>
#include <string>
#include <vector>
 
using namespace std;

#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;


namespace {
	class MyCallGraphPass:public ModulePass,public CallGraph{
		private:
			CallGraphNode *root;
			CallGraphNode *externalCallingNode;
			CallGraphNode *callsExternalNode;
			int functionCount;
			int edgeCount ;

			//Private Methods
			void addToCallGraph (Function *f);
			CallGraphNode* getOrInsertFunction (Function *f);
			int traverseDominatorTree (DominatorTree*,Function *);

			//Debug Methods
			void showAllInstructionsInBasicBlock(BasicBlock *bBlock);

			//Basic Block Counter
			int maxBasicBlocks;
			int minBasicBlocks;
			
			//CFG Edges perfunction
			int maxCFGEdge;
			int minCFGEdge;

		public:
			static char ID;
			MyCallGraphPass();
			~MyCallGraphPass();
			virtual bool runOnModule(Module &m);
			virtual void getAnalysisUsage ( AnalysisUsage &au)const;
		       // virtual void destroy();
			void result();

	};

	MyCallGraphPass::MyCallGraphPass():ModulePass(ID) {
		functionCount = 0;
		edgeCount = 0;
		root =0;
		externalCallingNode =0;
		callsExternalNode = 0;

                maxBasicBlocks = 0;
		minBasicBlocks = 0;

		maxCFGEdge = 0;
		minCFGEdge = 0;

		initializeBasicCallGraphPass(*PassRegistry::getPassRegistry());
	}


	MyCallGraphPass::~MyCallGraphPass() {
	       // destroy();
	}
 	
	void itr(BasicBlock *head, BasicBlock *tail) {
	      if((head->getTerminator()->getNumSuccessors() == 0) || head == tail ) {
	      	return ;
	      }
	      for (int  i = 0 ; i < head->getTerminator()->getNumSuccessors() ; ++i) {
	      	itr(head->getTerminator()->getSuccessor(i),tail) ;
	      }
	}
        
	bool MyCallGraphPass::runOnModule(Module &m) {
		for(Module::iterator func = m.begin(); func != m.end(); ++func) {
			if(func->size() != 0 ) {
				DominatorTree &DT = getAnalysis<DominatorTree>(*func) ;
				for(Function::iterator bBlock = func->begin() ; bBlock != func->end();  ++bBlock) {
					TerminatorInst * termInst = bBlock->getTerminator();
					//errs()<<"Function Name : "<<func->getName()<<"\n";
					for(unsigned int i= 0 ; i < termInst->getNumSuccessors() ; ++i)  {
						BasicBlock *successorBBlock = termInst->getSuccessor(i);
						if(DT.dominates(successorBBlock,bBlock)) {
							BasicBlock *loopHead = bBlock;
							BasicBlock *loopEnd = successorBBlock;
							itr(loopHead,loopEnd);
							errs()<<"Back edge detected\n";
						}

					}
				}

			}
		}
		return false;
	}

	/*

	bool MyCallGraphPass::runOnModule(Module &m) {
		for(Module::iterator func = m.begin(); func != m.end(); ++func) {
			for(Function::iterator bBlock = func->begin() ; bBlock != func->end();  ++bBlock) {
				TerminatorInst * termInst = bBlock->getTerminator();
				if(func->size() != 0 ) {
					DominatorTree &DT = getAnalysis<DominatorTree>(*func) ;
					//errs()<<"Function Name : "<<func->getName()<<"\n";
					for(unsigned int i= 0 ; i < termInst->getNumSuccessors() ; ++i)  {
						BasicBlock *successorBBlock = termInst->getSuccessor(i);
						if(DT.dominates(successorBBlock,bBlock)) {
							BasicBlock *loopHead = bBlock;
							BasicBlock *loopEnd = successorBBlock;
						    errs()<<"Back edge detected\n";
						}

					}
				}

			}
		}
		return false;
	}
	*/

	void MyCallGraphPass::addToCallGraph(Function *f) {
		CallGraphNode *node = getOrInsertFunction(f);
		if(!f->hasLocalLinkage()) {
			externalCallingNode->addCalledFunction(CallSite(),node);    
			if(f->getName() == "main") {
				if(root) 
			         	root = externalCallingNode;
				else
					root = node;
			}
		}

		if(f->hasAddressTaken()) {
			errs().write_escaped(f->getName());
			externalCallingNode->addCalledFunction(CallSite(),node);
		}

		if(f->isDeclaration() && !f->isIntrinsic()) {
			node->addCalledFunction(CallSite(),callsExternalNode);
		}

		int edgeCountPerFunction = 0;


		for(Function::iterator bBlock = f->begin();bBlock != f->end() ; ++bBlock ) {
			for(BasicBlock::iterator instr = bBlock->begin(); instr!= bBlock->end(); instr++)  {
				CallSite cs (cast<Value>(instr));
				if(cs) {
					edgeCount++;
					Function *callee =  cs.getCalledFunction();
					if(!callee) {
						node->addCalledFunction(cs,callsExternalNode);    
					} else if(!callee->isIntrinsic()) {
						node->addCalledFunction(cs,getOrInsertFunction(callee));
					}
				}

			}
			TerminatorInst* terminatorInst =  static_cast<TerminatorInst*>(bBlock->getTerminator());
			int numOfEdges = terminatorInst->getNumSuccessors();
			edgeCountPerFunction += numOfEdges;
			errs()<<"Basic block name "<<bBlock->getName()<<" Terminator instructions " << numOfEdges<<" \n";
		}


		if(edgeCountPerFunction > maxCFGEdge) {
			maxCFGEdge = edgeCountPerFunction;
		}
		if(edgeCountPerFunction < minCFGEdge) {
			minCFGEdge = edgeCountPerFunction;
		}
	}

	CallGraphNode* MyCallGraphPass::getOrInsertFunction(Function *f) {
		CallGraphNode *&node = FunctionMap[f];
		if(node)
			return node;
		return new CallGraphNode(const_cast<Function*>(f));
	}

	int MyCallGraphPass::traverseDominatorTree (DominatorTree *dt , Function *f) {
	        
	    	int dCount = 0;
		for (Function::iterator bb =  f->begin(); bb != f->end(); ++bb) {
			dCount = 0 ;
			for (Function::iterator bbInt =  f->begin() ; bbInt != f->end(); ++bbInt) {
				if(dt->dominates(bb,bbInt)){
				  	dCount ++;
				}
			}
		        errs()<<"Dominator count : "<<dCount<<"\n";
		} 
		return dCount;
	}

	void MyCallGraphPass::getAnalysisUsage ( AnalysisUsage &au) const{
		au.addRequired<DominatorTree>();
	} 

	void MyCallGraphPass::result() {
		errs()<<"Function count : "<<functionCount<<"\n";
		errs()<<"Edge Count : "<<edgeCount<<"\n";
		errs()<<"Max BasicBlocks : "<<maxBasicBlocks<<"\n";
		errs()<<"Min BasicBlocks : "<<minBasicBlocks<<"\n";
		errs()<<"Max CFG Edge Count per Function "<<maxCFGEdge<<"\n";
		errs()<<"Min CFG Edge Count per Function " <<minCFGEdge<<"\n";
	}


	void MyCallGraphPass::showAllInstructionsInBasicBlock(BasicBlock *bBlock) {
		errs()<<"Instructions in the basic Block : " <<"\n"; 
		for(BasicBlock::iterator i = bBlock->begin() ; i != bBlock->end(); ++i) {
			errs()<<*i<<"\n";				
		}
	}
}


char MyCallGraphPass::ID= 0;
static RegisterPass<MyCallGraphPass> X("ass1","This is assignment 1" ,false,false);

