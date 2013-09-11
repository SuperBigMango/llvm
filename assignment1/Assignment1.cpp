#include <iostream>
#include <string>
 
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
			int callGraphEdgeCount ;

			//Private Methods
			void addToCallGraph (Function *f);
			CallGraphNode* getOrInsertFunction (Function *f);
			void  traverseDominatorTree (DominatorTree*,Function *, double *, double *);

			//Debug Methods
			void showAllInstructionsInBasicBlock(BasicBlock *bBlock);

			//Basic Block Counter
			int maxBasicBlocks;
			int minBasicBlocks;
			int totalBasicBlocks;
			
			//CFG Edges perfunction
			int maxCFGEdge;
			int minCFGEdge;
			int totalCFGEdges;

			//Loop Count per function
			int minLoopCountPerFunction;
			int maxLoopCountPerFunction;
			int totalLoopCount;

			double totalAvgDominatorCount;
			double totalAvgDominatedCount;

		public:
			static char ID;
			MyCallGraphPass();
			~MyCallGraphPass();
			virtual bool runOnModule(Module &m);
			virtual void getAnalysisUsage ( AnalysisUsage &au)const;
		       // virtual void destroy();
		       
			void loopCountPerFunction (Function *,DominatorTree *dt);
			void result();

	};

	MyCallGraphPass::MyCallGraphPass():ModulePass(ID) {
		functionCount = 0;
		callGraphEdgeCount = 0;
		root =0;
		externalCallingNode =0;
		callsExternalNode = 0;

                maxBasicBlocks = 0;
		minBasicBlocks = 0;
		totalBasicBlocks = 0;

		maxCFGEdge = 0;
		minCFGEdge = 0;
		totalCFGEdges = 0;

		minLoopCountPerFunction = 0;
		maxLoopCountPerFunction = 0;
		totalLoopCount = 0 ;

		totalAvgDominatedCount = 0.0;
		totalAvgDominatorCount = 0.0;

		initializeBasicCallGraphPass(*PassRegistry::getPassRegistry());
	}


	MyCallGraphPass::~MyCallGraphPass() {
	       // destroy();
	}

	bool MyCallGraphPass::runOnModule(Module &m) {
		CallGraph::initialize(m);
		externalCallingNode = getOrInsertFunction (0);
                callsExternalNode = new CallGraphNode(0);
		
		for(Module::iterator func = m.begin(); func != m.end() ; ++func) {
                        //errs()<<"The name of the function is : "<<func->getName()<<"\n";
			if(func->size() != 0) {
				DominatorTree &DT = getAnalysis<DominatorTree>(*func) ;
				double avgDominatorCount = 0;
				double avgDominatedCount = 0 ;
				traverseDominatorTree(&DT,func, &avgDominatorCount, &avgDominatedCount);
				totalAvgDominatorCount += avgDominatorCount;
				totalAvgDominatedCount += avgDominatedCount;
				loopCountPerFunction(func,&DT);
				LoopInfo &lp = getAnalysis<LoopInfo>(*func);
			}


			functionCount++;
			int bBlockSize = func->size();
			totalBasicBlocks += bBlockSize;
			if(bBlockSize > maxBasicBlocks) {
				maxBasicBlocks = bBlockSize;
			}
			if(bBlockSize < minBasicBlocks) {
				minBasicBlocks = bBlockSize;
			}
			addToCallGraph(func);
		}

		if(root == 0 ) {
			root = externalCallingNode;
		}
		result();
		return false;
	}		

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
					callGraphEdgeCount++;
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
			//errs()<<"Basic block name "<<bBlock->getName()<<" Terminator instructions " << numOfEdges<<" \n";
		}

		totalCFGEdges += edgeCountPerFunction;
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

	void MyCallGraphPass::loopCountPerFunction (Function *func,DominatorTree *dt)  {
		int loopCounter = 0;
		for(Function::iterator bBlock = func->begin() ; bBlock != func->end();  ++bBlock) {
			TerminatorInst * termInst = bBlock->getTerminator();
			for(unsigned int i= 0 ; i < termInst->getNumSuccessors() ; ++i)  {
				BasicBlock *successorBBlock = termInst->getSuccessor(i);
				if(dt->dominates(successorBBlock,bBlock)) {
					++loopCounter;
				}

			}
		}


		totalLoopCount += loopCounter;

		if( loopCounter > maxLoopCountPerFunction ) {
			maxLoopCountPerFunction = loopCounter;
		}
		if(loopCounter < minLoopCountPerFunction ) {
			minLoopCountPerFunction = loopCounter;
		}
	}

	void MyCallGraphPass::traverseDominatorTree (DominatorTree *dt , Function *f, double *avgDominatorCount, double *avgDominatedCount) {

	    	int dCount = 0;
		for (Function::iterator bb =  f->begin(); bb != f->end(); ++bb) {
			dCount = 0 ;
			for (Function::iterator bbInt =  f->begin() ; bbInt != f->end(); ++bbInt) {
				if(dt->dominates(bb,bbInt)){
				  	dCount ++;
				}
			}
		       // errs()<<"Dominator count : "<<dCount<<"\n";
		} 
		int basicBlockCount = f->size();
		*avgDominatedCount = dCount/basicBlockCount;

		int dominatorCount =0;

		for (Function::iterator bb =  f->begin(); bb != f->end(); ++bb) {
			dCount = 0 ;
			for (Function::iterator bbInt =  f->begin() ; bbInt != f->end(); ++bbInt) {
				if(dt->dominates(bbInt,bb)){
				  	dominatorCount++;
				}
			}
		       // errs()<<"Dominator count : "<<dCount<<"\n";
		} 

		*avgDominatorCount = dominatorCount/basicBlockCount;
	}

       /* void MyCallGraphPass::destroy() {
		if(callsExternalNode) {
			callsExternalNode->allReferencesDropped();
			//delete callsExternalNode;
		}
	        CallGraph::destroy();
	}*/

	
	void MyCallGraphPass::getAnalysisUsage ( AnalysisUsage &au) const{
		au.addRequired<DominatorTree>();
	} 

	void MyCallGraphPass::result() {
		errs()<<"Function count : "<<functionCount<<"\n";
		errs()<<"Call Graph Edge Count : "<<callGraphEdgeCount<<"\n";
		errs()<<"Max BasicBlocks : "<<maxBasicBlocks<<"\n";
		errs()<<"Min BasicBlocks : "<<minBasicBlocks<<"\n";
		errs()<<"Max CFG Edge Count per Function "<<maxCFGEdge<<"\n";
		errs()<<"Min CFG Edge Count per Function " <<minCFGEdge<<"\n";
		errs()<<"Max Loop Count per function "<<maxLoopCountPerFunction<<"\n";
		errs()<<"Min Loop Count per function "<<minLoopCountPerFunction<<"\n";
		errs()<<"Total avg dominator count " <<totalAvgDominatorCount<<"\n";
		errs()<<"Total avg dominated count "<<totalAvgDominatedCount<<"\n";
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

