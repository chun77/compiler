#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MUL:
        op = "mul";
        break;
    case SREM:
        op = "srem";
        break;
    case AND:
        op= "and";
        break;
    case OR:
        op= "or";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UnaryInstruction::UnaryInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb):Instruction(UNARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

UnaryInstruction::~UnaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void UnaryInstruction::output() const
{
    std::string s1, s2,op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case NOT:
        op = "not";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb,int num) : Instruction(STORE, insert_bb)
{
    this->paramnum=num;
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

GlobalDefInstruction::GlobalDefInstruction(Operand *dst_addr, Operand *src,bool isConst, BasicBlock *insert_bb) : Instruction(GLOBALDEF, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
    this->isConst=isConst;
}

GlobalDefInstruction::~GlobalDefInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void GlobalDefInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    if(!isConst)
    {
        fprintf(yyout, "%s = global %s %s, align 4\n", dst.c_str(),src_type.c_str(), src.c_str());
    }
    else
    {
        fprintf(yyout, "%s = constant %s %s, align 4\n", dst.c_str(),src_type.c_str(), src.c_str());
    }
}

GlobalDeclInstruction::GlobalDeclInstruction(Operand *dst_addr, BasicBlock *insert_bb) : Instruction(GLOBALDECL, insert_bb)
{
    operands.push_back(dst_addr);
    dst_addr->addUse(this);
}

GlobalDeclInstruction::~GlobalDeclInstruction()
{
    operands[0]->removeUse(this);
}

void GlobalDeclInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    fprintf(yyout, "%s = common global %s %s, align 4\n", dst.c_str(),"i32", "0");
}

FuncCallInstruction::FuncCallInstruction(Operand* dst,vector<Operand *> params, SymbolEntry *se, BasicBlock *insert_bb): Instruction(FUNCCALL, insert_bb),dst(dst),se(se)
{
    if(dst!=NULL){
        dst->setDef(this);
    }
    operands.push_back(dst);
    for(auto it:params)
    {
        operands.push_back(it);
        it->addUse(this);
    }
}

void FuncCallInstruction::output() const
{
    if (operands[0])
        fprintf(yyout, "  %s = ", operands[0]->toStr().c_str());
    FunctionType* type = (FunctionType*)(se->getType());
    fprintf(yyout, "  call %s %s(", type->getRetType()->toStr().c_str(),se->toStr().c_str());

    for(int i=1;i<operands.size();i++)
    {
        if(i>1){
            fprintf(yyout,", ");
        }
        fprintf(yyout, "%s %s",operands[i]->getType()->toStr().c_str(),operands[i]->toStr().c_str());
    
    }
    fprintf(yyout,")\n");
}

FuncCallInstruction::~FuncCallInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    for (long unsigned int i = 1; i < operands.size(); i++)
        operands[i]->removeUse(this);
}

XorInstruction::XorInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb):Instruction(XOR, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void XorInstruction::output() const
{
    Operand* dst=operands[0];
    Operand* src=operands[1];
    fprintf(yyout, "  %s = xor %s %s, true\n",dst->toStr().c_str(),src->getType()->toStr().c_str(), src->toStr().c_str());
}

ZextInstruction::ZextInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb):Instruction(ZEXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ZextInstruction::output() const
{
    Operand* dst=operands[0];
    Operand* src=operands[1];
    fprintf(yyout, "  %s = zext %s %s to i32\n",dst->toStr().c_str(),src->getType()->toStr().c_str(), src->toStr().c_str());
}

MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    else if(se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    else if(se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if(id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int offset = cur_func->AllocSpace(4);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[0]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
            cur_block->InsertInst(cur_inst);
            //src = new MachineOperand(*internal_reg);
            src=internal_reg;
        }
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst);
        cur_block->InsertInst(cur_inst);
        // example: store r1, [r0]
        cur_inst = new StoreMInstruction(cur_block, src, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    else if (paramnum != -1)
    {  
        auto dst1 = genMachineReg(11);
        auto dst2 = genMachineImm(dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset());
        if(paramnum>3)
        {
            auto reg1=genMachineVReg();
            cur_inst=new LoadMInstruction(cur_block,reg1,genMachineReg(11),genMachineImm((paramnum-4)*4));
            cur_block->InsertInst(cur_inst);
            cur_inst=new StoreMInstruction(cur_block,new MachineOperand(*reg1),dst1,dst2);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            auto src = genMachineReg(paramnum);
            cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2);
            cur_block->InsertInst(cur_inst);
        }
    }
    else if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc())
    {
        // example: store r1, [r0, #4]
        auto src = genMachineOperand(operands[1]);
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
            cur_block->InsertInst(cur_inst);
            src = new MachineOperand(*internal_reg);
        }
        auto dst1 = genMachineReg(11);
        auto dst2 = genMachineImm(dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset());
        cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2);
        cur_block->InsertInst(cur_inst);
    }
    // store operand from temporary variable
    else
    {
        // example: store r1, [r0]
        auto src = genMachineOperand(operands[1]);
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
            cur_block->InsertInst(cur_inst);
            src = new MachineOperand(*internal_reg);
        }
        auto dst = genMachineOperand(operands[0]);
        cur_inst = new StoreMInstruction(cur_block, src, dst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
    * The source operands of ADD instruction in ir code both can be immediate num.
    * However, it's not allowed in assembly code.
    * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
    * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
    MachineInstruction* cur_inst = nullptr;
    if(src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if(src2->isImm()){
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg);
    }
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case DIV:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    case MUL:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case SREM:{
        auto internal_res1 = genMachineVReg();  // div res1
        MachineInstruction* mid_inst1 = nullptr;
        MachineInstruction* mid_inst2 = nullptr;
        mid_inst1 = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, internal_res1, src1, src2);
        cur_block->InsertInst(mid_inst1);
        auto internal_res2 = genMachineVReg();  // mul res2
        mid_inst2 = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, internal_res2, internal_res1,src2);
        cur_block->InsertInst(mid_inst2);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, internal_res2);
        cur_block->InsertInst(cur_inst);
        break;}
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    MachineInstruction* cur_inst = nullptr;
    if(src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if(src2->isImm()){
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg);
    }

    cur_inst=new CmpMInstruction(cur_block, src1, src2);
    cur_block->InsertInst(cur_inst);
    CondBrInstruction *next_inst=dynamic_cast<CondBrInstruction* >(getNext());
    if(next_inst!=0)  // cond branch
    {
        int cur_op;
        switch (opcode)
        {
        case E:
            /* code */
            cur_op=BranchMInstruction::BEQ;
            next_inst->setOp(CondBrInstruction::E);
            break;
        case NE:
            cur_op=BranchMInstruction::BNE;
            next_inst->setOp(CondBrInstruction::NE);
            break;
        case L:
            cur_op=BranchMInstruction::BLT;
            next_inst->setOp(CondBrInstruction::L);
            break;
        case GE:
            cur_op=BranchMInstruction::BGE;
            next_inst->setOp(CondBrInstruction::GE);
            break;
        case G:
            cur_op=BranchMInstruction::BGT;
            next_inst->setOp(CondBrInstruction::G);
            break;
        case LE:
            cur_op=BranchMInstruction::BLE;
            next_inst->setOp(CondBrInstruction::LE);
            break;
        default:
            break;
        }
        string true_label=".L";
        true_label+=to_string(next_inst->getTrueBranch()->getNo());

        auto dst=new MachineOperand(true_label);
        cur_inst = new BranchMInstruction(cur_block, cur_op,dst);
        cur_block->InsertInst(cur_inst);
    }else{  // uncond compare

    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    string label=".L";        
    label+=to_string(getBranch()->getNo());
    auto dst=new MachineOperand(label);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B,dst);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    string false_label=".L";        
    false_label+=to_string(getFalseBranch()->getNo());
    auto dst=new MachineOperand(false_label);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B,dst);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();
    auto cur_fun = builder->getFunction();

    //如果有返回值的话，生成MOV 指令，将返回值保存在 R0 寄存器中
    if(!operands.empty())
    {
        auto reg=new MachineOperand(MachineOperand::REG,0);
        auto src=genMachineOperand(operands[0]);
        cur_block->InsertInst(new MovMInstruction(cur_block,MovMInstruction::MOV,reg,src));
    }

    //生成add指令恢复sp
    auto cur_func = builder->getFunction();
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    //获得current frame offset
    auto offset =new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    auto cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, offset);
    cur_block->InsertInst(cur_inst);

    // 如果该函数有 Callee saved 寄存器，我们还需要生成 POP 指令恢复这些寄存器
    //在MachineCode.cpp的MachineBlock::output()里实现

    //生成跳转指令回到Caller
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto cur_inst2 =new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst2);
    
    // cur_block->InsertInst(new BinaryMInstruction(cur_block,BinaryMInstruction::SUB,new MachineOperand(MachineOperand::REG,13),new MachineOperand(MachineOperand::REG,11),new MachineOperand(MachineOperand::IMM,0)));
    
    
    
    
    // 13:sp 11:fp sub sp, fp, offset
    // 14:lr link register
    // pop fp
    // cur_block->InsertInst(new StackMInstrcuton(cur_block,StackMInstrcuton::POP,new MachineOperand(MachineOperand::REG,11)));
    // if(cur_fun->isLeaf())
    // {
    //     // bx lr
    //     cur_block->InsertInst(new BranchMInstruction(cur_block,BranchMInstruction::BX,new MachineOperand(MachineOperand::REG,14)));
    // }else{
    //     // pop pc
    //     cur_block->InsertInst(new StackMInstrcuton(cur_block,StackMInstrcuton::POP, new MachineOperand(MachineOperand::REG,15)));
    // }
}

void XorInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    // MachineInstruction *cur_inst=0;
    // auto dst = genMachineOperand(operands[0]);
    // auto src = genMachineOperand(operands[1]);
    // cur_inst = new EorMInstruction(cur_block,dst,src);
    // cur_block->InsertInst(cur_inst);
    auto dst = genMachineOperand(operands[0]);
    auto trueOperand = genMachineImm(1);
    auto falseOperand = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                        trueOperand, MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                    falseOperand, MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);
}

void ZextInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst=0;
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
    cur_block->InsertInst(cur_inst);
}

void FuncCallInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineOperand*passOperand;
    MachineInstruction*inst;

    // 要使用 R0-R3 寄存器传递前四个参数，和callee saved的关系？
    int i = 0;
    for (auto it = operands.begin(); it != operands.end(); it++, i++) {
        // 第一个operand是fp不处理
        if (i == 0)
            continue;
        if (i == 5)
            break;
        //物理寄存器R(i-1)
       passOperand=genMachineReg(i-1);
       auto src=genMachineOperand(operands[i]);
       //从内存加载到寄存器？
       if(src->isImm()&&src->getVal()>255)
       {
           inst=new LoadMInstruction(cur_block,passOperand,src);
       }
       //直接寄存器赋值
       else
       {
           inst=new MovMInstruction(cur_block,MovMInstruction::MOV,passOperand,src);
       }
        cur_block->InsertInst(inst);
    }
    //参数超过4个的时候，通过压栈的方式传递参数
    for (int i = operands.size() - 1; i > 4; i--) {
        passOperand = genMachineOperand(operands[i]);
        if (passOperand->isImm()) {
            auto dst = genMachineVReg();
            if (passOperand->getVal() < 256)
                inst = new MovMInstruction(cur_block, MovMInstruction::MOV,dst, passOperand);
            else
                inst = new LoadMInstruction(cur_block, dst, passOperand);
            cur_block->InsertInst(inst);
            passOperand = dst;
        }
        std::vector<MachineOperand*> vec;
        inst = new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH, vec,passOperand);
        cur_block->InsertInst(inst);
    }
    // std::string str1="addr_";
    // std::string labelStr=str1.append(se->toStr().c_str());
    auto label = new MachineOperand(se->toStr().c_str());
    label->setFunc(true);
    inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(inst);

    if (operands.size() > 5) {
        auto off = genMachineImm((operands.size() - 5) * 4);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, off);
        cur_block->InsertInst(inst);
    }
    if (dst) {
        passOperand = genMachineOperand(dst);
        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        inst =new MovMInstruction(cur_block, MovMInstruction::MOV, passOperand, r0);
        cur_block->InsertInst(inst);
    }
}

void GlobalDeclInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    auto cur_unit = builder->getUnit();
    MachineInstruction *cur_inst = 0;
    auto dst = genMachineOperand(operands[0]);
    SymbolEntry* se=new ConstantSymbolEntry(TypeSystem::intType,0);
    auto src = genMachineOperand(new Operand(se));
    cur_inst = new GlobalMInstruction(cur_block, dst, src);
    cur_unit->InsertGlobal(dynamic_cast<GlobalMInstruction *>(cur_inst));
}

void GlobalDefInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    auto cur_unit = builder->getUnit();
    MachineInstruction *cur_inst = 0;
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    cur_inst = new GlobalMInstruction(cur_block, dst, src);
    cur_unit->InsertGlobal(dynamic_cast<GlobalMInstruction *>(cur_inst));
}

void UnaryInstruction::genMachineCode(AsmBuilder* builder)
{

}
