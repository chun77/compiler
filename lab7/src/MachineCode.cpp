#include "MachineCode.h"
#include "Type.h"
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM)
        this->val = val;
    else 
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->getName().c_str());
        else if (this->label.substr(0, 1) == "@"&&this->IsFunc())
            fprintf(yyout, "%s", this->getLabel().c_str() + 1);
        else
            fprintf(yyout, "addr_%s", this->getName().c_str());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
    default:
        break;
    }
}

BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    // TODO: 
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tsdiv ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::SREM:
        // no mod instruction
        break;
    default:
        break;
    }
}

UxtbMInstruction::UxtbMInstruction(MachineBlock* p,MachineOperand* dst, MachineOperand* src, int cond)
{
    this->parent=p;
    this->type=MachineInstruction::UXTB;
    this->op=-1;
    this->cond=cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void UxtbMInstruction::output()
{
    fprintf(yyout, "\tuxtb ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");

}

EorMInstruction::EorMInstruction(MachineBlock* p,MachineOperand* dst, MachineOperand* src, int cond)
{
    this->parent=p;
    this->type=MachineInstruction::EOR;
    this->op=-1;
    this->cond=cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void EorMInstruction::output()
{
    fprintf(yyout, "\teor ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");

}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output()
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void StoreMInstruction::output()
{
    // TODO
    fprintf(yyout, "\tstr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: str r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    // TODO
    this->parent=p;
    this->op=op;
    this->cond=cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void MovMInstruction::output() 
{
    // TODO
    switch (op)
    {
    case MOV:
        fprintf(yyout,"\tmov ");
        break;
    case MVN:
        fprintf(yyout,"\tmvn ");
        break;
    case MOVS:
        fprintf(yyout,"\tmovs ");
        break;
    default:
        break;
    }
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    // TODO
    this->parent=p;
    this->op=op;
    this->cond=cond;
    this->type=MachineInstruction::BRANCH;
    this->def_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output()
{
    // TODO
    switch (op)
    {
    case BX:
        fprintf(yyout,"\tbx ");
        break;
    case B:
        fprintf(yyout,"\tb .");
        break;
    case BL:
        fprintf(yyout,"\tbl ");
        break;
    case BEQ:
        fprintf(yyout,"\tbeq .");
        break;
    case BNE:
        fprintf(yyout,"\tbne .");
        break;
    case BGE:
        fprintf(yyout,"\tbge .");
        break;
    case BGT:
        fprintf(yyout,"\tbgt .");
        break;
    case BLT:
        fprintf(yyout,"\tblt .");
        break;
    case BLE:
        fprintf(yyout,"\tble .");
        break;
    default:
        break;
    }
    //fprintf(yyout, ".");
    this->def_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO
    this->parent=p;
    this->op=op;
    this->cond=cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test
    fprintf(yyout, "\tcmp ");
    use_list[0]->output();
    fprintf(yyout, ", ");
    use_list[1]->output();
    fprintf(yyout, "\n");
}

//srcs:callee savedRegs
StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, std::vector<MachineOperand*> srcs,
    MachineOperand* fpSrc,MachineOperand* lrSrc,
    int cond)
{
    // TODO
    this->parent=p;
    this->type = MachineInstruction::STACK;
    this->op=op;
    this->cond=cond;
    if(srcs.size())
    {
            for(auto it = srcs.begin(); it != srcs.end(); it++)  
            {
                this->use_list.push_back(*it);
                // (*it)->setParent(this);
            }
    }
    this->use_list.push_back(fpSrc);
    fpSrc->setParent(this);
    //如果当前函数调用了其他函数才需要保存lr
    if(lrSrc)
    {
        this->use_list.push_back(lrSrc);
        lrSrc->setParent(this);
    }
}

void StackMInstrcuton::output()
{
    // TODO
    switch (op)
    {
    case POP:
        fprintf(yyout, "\tpop {");
        break;
    case PUSH:
        fprintf(yyout, "\tpush {");
        break;
    default:
        break;
    }
    this->use_list[0]->output();
    for (long unsigned int i = 1; i < use_list.size(); i++) {
        fprintf(yyout, ", ");
        this->use_list[i]->output();
    }
    fprintf(yyout, "}\n");
}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
    this->paramsNum=((FunctionType*)(sym_ptr->getType()))->getParamSe().size();
    
};

std::vector<MachineOperand*> MachineFunction::getSavedRegs() {
    std::vector<MachineOperand*> regs;
    for (auto it = saved_regs.begin(); it != saved_regs.end(); it++) {
        auto reg = new MachineOperand(MachineOperand::REG, *it);
        regs.push_back(reg);
    }
    return regs;
}


void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);
    // bool first=true;
    //目前还avalible的寄存器
    // int offset=(parent->getSavedRegs().size() + 2) * 4;
    int num = parent->getParamsNum();
    for(auto iter : inst_list)
    {
        //在bx前加pop指令
        if(iter->isBX())
        {
            auto fp = new MachineOperand(MachineOperand::REG, 11);
            auto lr = new MachineOperand(MachineOperand::REG, 14);
            //将之前push保存的信息pop恢复
            auto cur_inst =new StackMInstrcuton(this, StackMInstrcuton::POP,parent->getSavedRegs(), fp, lr);
            cur_inst->output();
        }
        iter->output();
        // if (iter->isAdd()) {
        //         auto dst = iter->getDef()[0];
        //         auto src1 = iter->getUse()[0];
        //         if (dst->isReg() && dst->getReg() == 13 && src1->isReg() &&
        //             src1->getReg() == 13 && (iter+1)->isBX()) {
        //             int size = parent->AllocSpace(0);
        //             if (size < -255 || size > 255) {
        //                 auto r1 = new MachineOperand(MachineOperand::REG, 1);
        //                 auto off =new MachineOperand(MachineOperand::IMM, size);
        //                 (new LoadMInstruction(nullptr, r1, off))->output();
        //                 iter->getUse()[1]->setReg(1);
        //             } else
        //                 iter->getUse()[1]->setVal(size);
        //         }
        //     }
        // if(num>4&&iter->isStore())
        // {
        //     MachineOperand* operand=iter->getUse()[0];
        //     if(operand->isReg()&&operand->getReg()==3)
        //     {
                
        //     }
        // }
    }
}


void MachineFunction::output()
{
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);
    // TODO
    /* Hint:
    *  1. Save fp
    *  2. fp = sp
    *  3. Save callee saved register
    *  4. Allocate stack space for local variable */
    // fprintf(yyout, "\tpush {");
    // if(isLeaf()){
    //     fprintf(yyout, "fp}\n");
    // }else{
    //     fprintf(yyout, "fp, lr}\n");
    // }
    //在刚进入一个新的函数开始执行的时候，它们保存的是上个函数的信息，需要将它们入栈保存起来
    auto fp = new MachineOperand(MachineOperand::REG, 11);
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    (new StackMInstrcuton(nullptr, StackMInstrcuton::PUSH, getSavedRegs(), fp,lr))->output();
    (new MovMInstruction(nullptr, MovMInstruction::MOV, fp, sp))->output();
    // fprintf(yyout, "\tmov fp,sp\n");
    // fprintf(yyout, "\tsub sp, sp, #%d\n", this->stack_size);
    // Traverse all the block in block_list to print assembly code.
    // for(auto iter : block_list)
    // {
    //     if((*iter).empty()==false)
    //         iter->output();
    // }

    //之后需要生成 SUB 指令为局部变量分配栈内空间,此时已经知道实际的栈内空间大小
    int off = AllocSpace(0);
    auto size = new MachineOperand(MachineOperand::IMM, off);
    if (off < -255 || off > 255) 
    {
        auto r4 = new MachineOperand(MachineOperand::REG, 4);
        (new LoadMInstruction(nullptr, r4, size))->output();
        (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, r4))->output();
    } 
    else 
    {
        (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, size))->output();
    }
    for (auto iter : block_list) {
        if(iter->empty()){
            continue;
        }
        iter->output();
    }
    fprintf(yyout, "\n");
}

GlobalMInstruction::GlobalMInstruction(MachineBlock* p,MachineOperand* dst, MachineOperand* src, int cond)
{
    this->parent=p;
    def_list.push_back(dst);
    use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void GlobalMInstruction::outputAddr()
{
    fprintf(yyout, "addr_%s:\n", def_list[0]->getName().c_str());
    fprintf(yyout, "\t.word %s\n",def_list[0]->getName().c_str());
}

void GlobalMInstruction::output()
{
    fprintf(yyout,"\t.global %s\n",def_list[0]->getName().c_str());
    fprintf(yyout,"\t.align 4\n");
    fprintf(yyout,"\t.size %s, 4\n",def_list[0]->getName().c_str());
    fprintf(yyout,"%s:\n",def_list[0]->getName().c_str());
    fprintf(yyout,"\t.word %d\n",use_list[0]->getVal());
}

void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;
    if(!global_inst.empty())
    {
        fprintf(yyout, "\t.data\n");
    }
    for( auto it= global_inst.begin();it!=global_inst.end();it++)
    {
        (*it)->output();
    }
    // fprintf(yyout,"output head");
}

void MachineUnit::PrintBridge()
{
    for( auto it= global_inst.begin();it!=global_inst.end();it++)
    {
        dynamic_cast<GlobalMInstruction* >(*it)->outputAddr();
    }
}

void MachineUnit::output()
{
    // TODO
    /* Hint:
    * 1. You need to print global variable/const declarition code;
    * 2. Traverse all the function in func_list to print assembly code;
    * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    for(auto iter : func_list)
        iter->output();
    PrintBridge();
}
