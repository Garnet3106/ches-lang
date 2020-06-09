#pragma once

#undef LN
#undef TK
#undef CH
#define LN(i)       (lines.at(i))
#define TK(i, j)    (lines.at(i).at(j))
#define CH(i, j, k) (lines.at(i).at(j).at(k))



class Interpreter {

public:

    Options options;
    HeaderInfo headerInfo;
    InstList instList;

    Interpreter(Options opt);

    void run();

private:

    std::vector<std::stack<void*>> stacks;
    std::vector<FuncData> funcData;

    void runProgram(Bytecode src);

    void setFuncData(Bytecode src);

    void runInst(Instruction instruction);

    std::string joinCode(ByteSeq src);
};
