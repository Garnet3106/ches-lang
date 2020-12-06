#pragma once

#undef  INST

#define BYTE                        (BYTE_AT(this->index))
#define BYTE_AT(index)              (this->bytes.at(index))
#define BYTE_LEN                    (this->bytes.size())
#define BYTES_TO_BOOL(value)        (Interpreter::toBool(value))
#define BYTE_TO_HEX(value)          (Interpreter::toHexString(value))
#define BYTE_TO_HEX_SEP(value, sep) (Interpreter::toHexString(value, sep))
#define BYTES_TO_INT(value)         (Interpreter::toInt(value))

#define CONSOLE_OUT(value)  Console::writeln("* print\n    " + BYTE_TO_HEX(value));

#define STACK_EMPTY         (this->stack.empty() || this->stack.top().empty())
#define STACK_POP()         do { if(this->stack.empty() || this->stack.top().empty()) { Console::write(" <err:pop> "); break; } this->stack.top().pop(); } while(0);
#define STACK_PUSH(value)   do { if(this->stack.empty()) { Console::write(" <err:push> "); break; } this->stack.top().push(value); } while(0);
#define STACK_TOP           (this->stack.top().top())

#define OPESTACK_LEN        (this->opeStack.top().size())
#define OPESTACK_LOAD()     do { if(this->opeStack.empty() || this->stack.empty() || this->stack.top().empty()) { Console::write(" <err:load> "); break; } this->opeStack.top().push(STACK_TOP); } while(0);
#define OPESTACK_STORE()    do { if(this->opeStack.empty() || this->opeStack.top().empty()) { Console::write(" <err:store> "); break; } this->opeStack.top().pop(); } while(0);
#define OPESTACK_TOP        (this->opeStack.top().top())

#define L_AT(lineIndex)                     (lines.at(lineIndex))
#define L_TOKEN_AT(lineIndex, tokenIndex)   (lines.at(lineIndex).at(tokenIndex))

typedef std::stack<ches::ByteVec>   Stack;

#include "interpreter.cpp"


ches::Block::Block() {}

ches::Block::Block(ByteVec name, int beginIndex) {
    this->name = name;
    this->beginIndex = beginIndex;
}


ches::Interpreter::Interpreter(std::string filePath) {
    try {
        this->loadCompiledFile(filePath);

        for(auto [ id, block ] : this->blockList) {
            Console::writeln("- block -");
            Console::writeln("  * id:\t" + BYTE_TO_HEX(id));
            Console::writeln("  * name:\t" + BYTE_TO_HEX(block.name));
            Console::writeln("  * begin:\t" + std::to_string(block.beginIndex));
            Console::writeln();
        }

        // マジックナンバーをチェック
        if(this->magicNum != MAGIC_NUM)
            Console::log(LogType_Error, 8732, { { "Path", filePath } }, true);

    } catch(std::out_of_range ignored) {
        std::cout << "EXCEPTION" << std::endl;
    }
}

ches::ByteVec ches::Interpreter::copyBytesUntilDiv(int &indexer) {
    ByteVec result;

    try {
        for(; indexer < BYTE_LEN; indexer++) {
            if(BYTE_AT(indexer) == IT_InstDiv) {
                if(indexer + 1 < BYTE_LEN && BYTE_AT(indexer + 1) == IT_InstDiv) {
                    result.push_back(IT_InstDiv);
                    indexer++;
                } else {
                    break;
                }
            } else {
                result.push_back(BYTE_AT(indexer));
            }
        }
    } catch(std::out_of_range ignored) {
        std::cout << "EXCEPTION on Interpreter::copyBytesUntilDiv()" << std::endl;
    }

    return result;
}

void ches::Interpreter::loadCompiledFile(std::string filePath) {
    try {
        std::ifstream ifs(filePath);

        if(!ifs.is_open())
            Console::log(LogType_Error, 327, { { "Path", filePath } }, true);

        if(ifs.fail())
            Console::log(LogType_Error, 6845, { { "Path", filePath } }, true);

        Byte byte;

        do {
            ifs.read((char*)&byte, sizeof(char));
            this->bytes.push_back(byte);
        } while(!ifs.eof());

        if(this->bytes.size() > 0)
            this->bytes.pop_back();

        ifs.close();

        // ヘッダ情報をロード

        for(int i = 0; i < MAGIC_NUM.size(); i++)
            this->magicNum.push_back(BYTE_AT(i));

        if(magicNum != MAGIC_NUM)
            Console::log(LogType_Error, 8732, { { "Path", filePath } }, true);

        int idAreaIndexer = MAGIC_NUM.size();
        ByteVec idAreaIndexBytes = this->copyBytesUntilDiv(idAreaIndexer);
        this->idAreaIndex = BYTES_TO_INT(idAreaIndexBytes);

        // ID情報をロード

        try {
            for(int i = this->idAreaIndex; i < BYTE_LEN; ) {
                ByteVec id;

                for(int j = 0; j < 16; i++, j++)
                    id.push_back(BYTE_AT(i));

                ByteVec name = this->copyBytesUntilDiv(i);
                i++;

                ByteVec beginIndexBytes = this->copyBytesUntilDiv(i);
                int beginIndex = BYTES_TO_INT(beginIndexBytes);
                i++;

                this->blockList[id] = Block(name, beginIndex);
            }
        } catch(std::exception excep) {
            // todo: ID情報のエラーを追加
            Console::writeln("ID area data is invalid.");
        }
    } catch(std::exception excep) {
        Console::log(LogType_Error, 6845, { { "Path", filePath } }, true);
    }
}

void ches::Interpreter::runProgram() {
    try {
        // スタックの初期化

        this->stack.push(Stack());
        this->opeStack.push(Stack());

        // // エントリポイントの呼び出し

        Block entryBlock;
        ByteVec entryBlockName = { 0x63, 0x6F, 0x6E, 0x73, 0x74 };

        // エントリポイントのラベルを取得
        for(auto [ id, block ] : this->blockList)
            if(block.name == entryBlockName)
                entryBlock = block;

        if(entryBlock.empty()) {
            Console::writeln("Entry point was not found.");
            Console::writeln("( name: " + BYTE_TO_HEX_SEP(entryBlockName, " ") + " )");
            return;
        }

        this->index = entryBlock.beginIndex;

        Console::writeln();
        Console::writeln("byteLen: " + std::to_string(BYTE_LEN));
        Console::writeln("headerLen: " + std::to_string(HEADER_LEN));
        Console::writeln();

        Console::writeln("idAreaIndex: " + std::to_string(this->idAreaIndex));
        Console::writeln("entryPoint: " + std::to_string(this->index));
        Console::writeln();

        Console::writeln("magicNum: " + BYTE_TO_HEX_SEP(this->magicNum, " "));
        Console::writeln();
        Console::writeln();

        if(this->index < HEADER_LEN || this->index >= this->idAreaIndex) {
            Console::writeln("Index is invalid.");
            return;
        }

        Console::writeln("index\tbyte\topcode\t\tstacktop\toslen\tinst (raw)");
        Console::writeln();

        while(this->index < this->idAreaIndex)
            this->runNextInst();

        std::string hexStackTop = (this->stack.empty() || this->stack.top().empty()) ? "noelem" : BYTE_TO_HEX_SEP(STACK_TOP, " ");
        Console::writeln("<end>\t\t\t\t" + hexStackTop);

    } catch(std::out_of_range ignored) {
        std::cout << "EXCEPTION" << std::endl;
    }
}

void ches::Interpreter::runNextInst() {
    try {
        // ByteVec bytes = {(Byte)-2};
        // Console::writeln(BYTES_TO_INT(bytes));
        if(this->index < HEADER_LEN || this->index >= this->idAreaIndex) {
            Console::writeln("Index is invalid. / runNextInst()");
            return;
        }

        Byte opcode = BYTE;
        Byte opcodeIndex = this->index;

        // opcode分をindexに追加する
        this->index++;

        std::string hexOpcode = instTypeMap.count(opcode) == 1 ? instTypeMap.at(opcode) : "OOR";
        std::string hexStackTop = (this->stack.empty() || this->stack.top().empty()) ? "noelem" : BYTE_TO_HEX_SEP(STACK_TOP, " ");
        Console::write(std::to_string(this->index + 1) + "\t0x" + BYTE_TO_HEX(ByteVec { opcode }) + "\t" + hexOpcode + "\t\t" + hexStackTop + "\t\t" + std::to_string(OPESTACK_LEN) + "\t");

        switch(opcode) {
            case IT_InstDiv: {
                // 表示調節のために改行
                Console::writeln();
                Console::writeln();
                Console::log(LogType_Error, 7316, { { "Index", std::to_string(this->index - 1) } }, true);
            } break;

            case IT_Equal: {
                ByteVec value1 = OPESTACK_TOP;
                OPESTACK_STORE();

                ByteVec value2 = OPESTACK_TOP;
                OPESTACK_STORE();

                STACK_PUSH(value1 == value2 ? ByteVec { 0x01 } : ByteVec { 0x00 });
            } break;

            case IT_Jump: {
                ByteVec indexVec = this->copyBytesUntilDiv(this->index);

                Console::write("<");
                Console::write(BYTES_TO_INT(indexVec));
                Console::write("> ");

                if(indexVec.size() == 16 && this->blockList.count(indexVec) == 1) {
                    Block block = this->blockList.at(indexVec);
                    this->index = block.beginIndex;
                } else {
                    this->index += BYTES_TO_INT(indexVec);

                    if(this->index + 1 > this->idAreaIndex || this->index + 1 < HEADER_LEN) {
                        // 表示調節のために改行
                        Console::writeln();
                        Console::writeln();
                        Console::log(LogType_Error, 2947, { { "Index", std::to_string(this->index + 1) } }, true);
                    }
                }
            } break;

            case IT_JumpIf: {
                ByteVec value = OPESTACK_TOP;
                OPESTACK_STORE();

                ByteVec indexVec = this->copyBytesUntilDiv(this->index);

                if(!BYTES_TO_BOOL(value))
                    break;

                if(indexVec.size() == 16 && this->blockList.count(indexVec) == 1) {
                    Block block = this->blockList.at(indexVec);
                    this->index = block.beginIndex;
                } else {
                    Console::write("<");
                    Console::write(BYTES_TO_INT(indexVec));
                    Console::write("> ");

                    this->index += BYTES_TO_INT(indexVec);

                    if(this->index + 1 > this->idAreaIndex || this->index + 1 < HEADER_LEN) {
                        // 表示調節のために改行
                        Console::writeln();
                        Console::writeln();
                        Console::log(LogType_Error, 2947, { { "Index", std::to_string(this->index + 1) } }, true);
                    }
                }
            } break;

            case IT_Load: {
                ByteVec countBytes = this->copyBytesUntilDiv(this->index);
                int count = BYTES_TO_INT(countBytes);

                Stack stackCache;

                for(int i = 0; i < count; i++) {
                    if(STACK_EMPTY) {
                        // 表示調節のために改行
                        Console::writeln();
                        Console::writeln();
                        Console::log(LogType_Error, 7317, { { "Index", std::to_string(this->index - 1) } }, true);
                    }

                    stackCache.push(STACK_TOP);
                    OPESTACK_LOAD();
                    STACK_POP();
                }

                while(!stackCache.empty()) {
                    STACK_PUSH(stackCache.top());
                    stackCache.pop();
                }
            } break;

            case IT_Push: {
                // 範囲チェック入れる？
                DataSizeType size = static_cast<DataSizeType>(BYTE);
                this->index++;

                ByteVec value = this->copyBytesUntilDiv(this->index);

                STACK_PUSH(value);
            } break;

            case IT_Pop: {
                STACK_POP();
            } break;
        }

        /* 旧コード
        switch(opcode) {
            case IT_Unknown: {
            } break;

            case IT_SysCall: {
                Byte sysOpe = STACK_TOP.at(0);
                STACK_POP();

                switch(sysOpe) {
                    // enumを作る
                    case 0x00:
                    CONSOLE_OUT(STACK_TOP);
                    STACK_POP();
                    break;
                }
            } break;

            case IT_Block:
            return;

            case IT_Ret:
            return;

            case IT_Equal: {
                ByteVec value1 = STACK_TOP;
                STORE();
                ByteVec value2 = STACK_TOP;
                STORE();
                // Console::writeln(value1.toInt());
                // Console::writeln(value2.toInt());
                // Console::writeln(ByteSeq(value1 == value2).toHexString());
                STACK_PUSH(value1 == value2 ? ByteVec { 0x01 } : ByteVec { 0x00 });
            } break;

            case IT_Greater: {
                ByteVec value1 = STACK_TOP;
                STACK_POP();
                ByteVec value2 = STACK_TOP;
                STACK_POP();
                // Console::writeln(value1.toHexString());
                // Console::writeln(value2.toHexString());
                // Console::writeln(ByteVec(value1 < value2).toHexString());
                STACK_PUSH(value1 < value2 ? ByteVec { 0x01 } : ByteVec { 0x00 });
            } break;

            case IT_Jump: {
                ByteVec id = STACK_TOP;
                STACK_POP();

                if(id.size() == 16) {
                    Block block = this->blockList[id];

                    // if(id == this->blockList.findByName(ByteSeq("println")).id) {
                    //     CONSOLE_OUT(STACK_TOP);
                    //     STACK_POP();
                    //     break;
                    // }

                    this->index = block.beginIndex;
                } else {
                    this->index += BYTES_TO_INT(id);
                    return;
                }
            } break;

            case IT_JumpIf: {
                ByteVec value = STACK_TOP;
                STACK_POP();

                ByteVec indexVec = this->copyBytesUntilDiv(this->index);

                // false (0x00) 以外の値はすべて通す
                if(value == ByteVec { 0x00 })
                    break;

                if(indexVec.size() == 16 && this->blockList.count(indexVec) == 1) {
                    Block block = this->blockList.at(indexVec);
                    this->index = block.beginIndex;
                } else {
                    this->index += BYTES_TO_INT(indexVec);
                }
            } break;

            case IT_Load: {
                ByteVec countBytes = this->copyBytesUntilDiv(this->index);
                int count = BYTES_TO_INT(countBytes);

                for(int i = 0; i < count; i++)
                    LOAD();
            } break;

            case IT_Push: {
                // 範囲チェック入れる？
                DataSizeType size = static_cast<DataSizeType>(BYTE);
                this->index++;

                ByteVec value = this->copyBytesUntilDiv(this->index);

                STACK_PUSH(value);
            } break;

            case IT_Pop: {
                STACK_POP();
            } break;

            default:
            break;
        }
        */

        ByteVec inst;

        // インデックスはINST_DIVの直前に合わせるようにすること
        this->index++;

        for(int i = opcodeIndex; i < this->index - 1; i++)
            inst.push_back(BYTE_AT(i));

        std::string hexInst = BYTE_TO_HEX_SEP(inst, " ");
        Console::writeln((hexInst == "" ? "[nobytes]" : hexInst));

    } catch(std::out_of_range ignored) {
        // 表示調節のために改行
        Console::writeln();
        Console::writeln();
        Console::log(LogType_Error, 0, { { "At", "Interpreter::runNextInst()" } }, true);
    }
}