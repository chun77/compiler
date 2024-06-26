#include <algorithm>
#include "LinearScan.h"
#include "MachineCode.h"
#include "LiveVariableAnalysis.h"

LinearScan::LinearScan(MachineUnit *unit)
{
    this->unit = unit;
    for (int i = 4; i < 11; i++)
    {
        regs.push_back(i);
        interval_reg[i]=nullptr;
    }
}

void LinearScan::allocateRegisters()
{
    for (auto &f : unit->getFuncs())
    {
        func = f;
        bool success;
        success = false;
        while (!success)        // repeat until all vregs can be mapped
        {
            computeLiveIntervals();
            success = linearScanRegisterAllocation();
            if (success)        // all vregs can be mapped to real regs
                modifyCode();
            else                // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

void LinearScan::makeDuChains()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand *>> liveVar;
    for (auto &bb : func->getBlocks())
    {
        liveVar.clear();
        for (auto &t : bb->getLiveOut())
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend(); inst++)
        {
            (*inst)->setNo(no--);
            for (auto &def : (*inst)->getDef())
            {
                if (def->isVReg())
                {
                    auto &uses = liveVar[*def];
                    du_chains[def].insert(uses.begin(), uses.end());
                    auto &kill = lva.getAllUses()[*def];
                    std::set<MachineOperand *> res;
                    set_difference(uses.begin(), uses.end(), kill.begin(), kill.end(), inserter(res, res.end()));
                    liveVar[*def] = res;
                }
            }
            for (auto &use : (*inst)->getUse())
            {
                if (use->isVReg())
                    liveVar[*use].insert(use);
            }
        }
    }
}

void LinearScan::computeLiveIntervals()
{
    makeDuChains();
    intervals.clear();
    for (auto &du_chain : du_chains)
    {
        int t = -1;
        for (auto &use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval *interval = new Interval({du_chain.first->getParent()->getNo(), t, false, 0, 0, {du_chain.first}, du_chain.second});
        intervals.push_back(interval);
    }
    bool change;
    change = true;
    while (change)
    {
        change = false;
        std::vector<Interval *> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++)
            {
                Interval *w1 = t[i];
                Interval *w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin())
                {
                    std::set<MachineOperand *> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(), w2->uses.begin(), w2->uses.end(), inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        w1->start = std::min(w1->start, w2->start);
                        w1->end = std::max(w1->end, w2->end);
                        auto it = std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

bool LinearScan::linearScanRegisterAllocation()
{
    // Todo 
    bool helper=true;
    for (auto &interval : intervals)
    {
        for (auto &active : actives)
        {
            if(active->end < interval->start)
            // have time ealier than unhandled interval
            {
                // erase for reuse register
                int regno=active->rreg;
                for (auto i = actives.begin(); i != actives.end(); i++)
                {
                    if (*i == active)
                    {
                        actives.erase(i);
                        break;
                    }
                }
                allocReg(nullptr,regno);
                // }
                /*f(actives.empty()){
                    break;
                }*/
            }
        }
        if(actives.size()==regs.size())
        {
            Interval* last=actives[actives.size()-1];  // the last active interval
            if(last->end < interval->end)
            {
                interval->spill=true;
            }else{
                last->spill=true;
                allocReg(interval,last->rreg);
                actives.pop_back();   // erase the last active
                insertActive(interval);
                //break;
            }
            helper=false;
        }else{
            int free=getFreeReg();
            allocReg(interval,free);
            insertActive(interval);
        }
        
    }
    return helper;
}

void LinearScan::modifyCode()
{
    for (auto &interval : intervals)
    {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode()
{
    for(auto &interval:intervals)
    {
        if(!interval->spill)
            continue;
        // TODO
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */ 
        spillAtInterval(interval);
    }
}

void LinearScan::expireOldIntervals(Interval *interval)
{
    // Todo
}

void LinearScan::spillAtInterval(Interval *interval)
{
    // Todo:insert ldr and str
    auto cur_func=func;
    MachineInstruction* cur_inst=nullptr;
    MachineBlock* cur_block;
    int offset = cur_func->AllocSpace(4);
    for(auto use : interval->uses)
    {
        // add ldr
        cur_block=use->getParent()->getParent();
        auto parent = use->getParent();
        MachineOperand *src1= new MachineOperand(MachineOperand::REG,11);
        MachineOperand *src2= new MachineOperand(MachineOperand::IMM, -offset);
        cur_inst=new LoadMInstruction(cur_block,new MachineOperand(*use),src1,src2);
        for(auto it=cur_block->getInsts().begin();it!=cur_block->getInsts().end();it++)
        {
            if(*it==parent){
                cur_block->getInsts().insert(it,1,cur_inst);
                break;
            }
        }
    }
    for(auto def : interval->defs)
    {
        // add str
        cur_block=def->getParent()->getParent();
        auto parent=def->getParent();
        MachineOperand *src1=new MachineOperand(MachineOperand::REG,11);
        MachineOperand *src2= new MachineOperand(MachineOperand::IMM, -offset);
        cur_inst=new StoreMInstruction(cur_block,new MachineOperand(*def),src1,src2);
        for(auto it=cur_block->getInsts().begin();it!=cur_block->getInsts().end();it++)
        {
            if(*it==parent){
                cur_block->getInsts().insert(it,1,cur_inst);
                break;
            }
        }
    }
}

bool LinearScan::compareStart(Interval *a, Interval *b)
{
    return a->start < b->start;
}

void LinearScan::allocReg(Interval* interval, int regno)
{
    interval_reg[regno]=interval;
    if(interval!=NULL)
    {
        interval->rreg=regno;
    }
}

int LinearScan::getFreeReg()
{
    for (int i = 4; i < 11; i++)
    {
        if(interval_reg[i]==nullptr)
        {
            return i;
        }
    }
    return 0;
}

void LinearScan::insertActive(Interval* interval)
{
    if(actives.size()==0)
    {
        actives.push_back(interval);
        return;
    }
    for (vector<Interval*>::iterator it=actives.begin();it!=actives.end();it++)
    {
        if((*it)->end>interval->end)
        {
            actives.insert(it,1,interval);
            return;
        }
    }
    actives.push_back(interval);
}