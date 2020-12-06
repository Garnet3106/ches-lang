#pragma once


namespace ches {
    struct Block {
        ByteVec name;
        int beginIndex = -1;

        Block();

        Block(ByteVec name, int beginIndex);

        inline bool empty() {
            // nameが空かどうかで判断する
            return (this->name.empty() || this->beginIndex == -1);
        }
    };


    class Interpreter {
    public:
        ByteVec bytes;

        ByteVec magicNum;
        int idAreaIndex = -1;

        Interpreter(std::string filePath);

        void runProgram();

    private:
        int index = HEADER_LEN;

        std::stack<Stack> stack;
        std::stack<Stack> opeStack;

        // <id, Block>
        std::map<ByteVec, Block> blockList;

        // indexerは参照渡しなので注意
        ByteVec copyBytesUntilDiv(int &indexer);

        void loadCompiledFile(std::string filePath);

        void runNextInst();

        std::string joinCode(ByteVec src);

        static bool toBool(ByteVec source) {
            // note: 0x00 以外の値はすべて true と判定されます
            return source != ByteVec { 0x00 };
        }

        static std::string toHexString(ByteVec source, std::string separator = "") {
            std::string result;

            for(int src : source) {
                std::stringstream ss;
                ss << std::hex << src;
                result += (src < 16 ? "0" : "") + ss.str() + separator;
            }

            if(source.size() > 0) {
                for(int i = 0; i < separator.length(); i++)
                    result.pop_back();
            } else {
                return "nobytes";
            }

            return result;
        }

        static int toInt(ByteVec source) {
            long result = std::stol(BYTE_TO_HEX(source), nullptr, 16);

            if((result >> 31) & 1 == 1)
                result = (~result + 1) * -1;

            return (int)result;
        }
    };
}