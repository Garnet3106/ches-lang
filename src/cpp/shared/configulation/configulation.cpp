/*
 * Chestnut 0.0.0
 *
 * GitHub Repository:
 *     https://github.com/Garnet3106/chestnut/
 *
 * Copyright © 2020-2021 Garnet3106 All rights reserved.
 */


#pragma once


namespace ches::shared {
    enum ConfigulationExceptionType {
        ConfigulationException_Unknown,
        ConfigulationException_DuplicatedPropName,
        ConfigulationException_InvalidPropName,
        ConfigulationException_InvalidPropValue,
        ConfigulationException_InvalidSyntax,
        ConfigulationException_UnknownPropName
    };


    class ConfigulationException {
    public:
        ConfigulationExceptionType type;
        std::string target = "";

        ConfigulationException();

        ConfigulationException(ConfigulationExceptionType type);

        ConfigulationException(ConfigulationExceptionType type, std::string target);
    };


    class Configulation {
    protected:
        std::string path = "";
        std::unordered_map<std::string, std::string> dataMap;

    public:
        static std::string homeDirEnvName;
        static std::string homeDirPath;

        static Configulation settings;
        static Configulation langPack;

        Configulation();

        /*
         * arg: path: configデータのホームディレクトリから見た相対的なディレクトリパスまたはファイルパス
         */
        Configulation(std::string relPath);

        /*
         * arg: editedOptionMap: 設定値を編集したオプションマップ
         * excep: FileManager::getLines(std::string) と同様 / Configulation::toPropPair(std::string) と同様 / ConfigulationException [InvalidPropValue, UnknownPropName]
         */
        void edit(std::unordered_map<std::string, std::string> editedOptionMap);

        /*
         * ret: プロパティ名が存在するかどうか
         */
        bool exists(std::string key);

        /*
         * ret: プロパティ名が見つかれば対応するプロパティ値、見つからなければ propName を返す
         */
        std::string get(std::string key);

        /*
         * ret: 環境変数が存在する場合は設定値、存在しない場合は空文字を返す
         */
        inline static std::string getEnvironmentVariable(std::string envName) {
            const char *value = std::getenv(envName.c_str());

            if(value == NULL)
                return "";

            return value;
        }

        void print();

        /*
         * note: プロパティ名の重複検査は行われない
         * ret: プロパティ形式の行であればプロパティ名とプロパティ値のペア、そうでなければ null_ptr を返す
         * excep: ConfigulationException [InvalidSyntax, InvalidPropName]
         */
        static std::pair<std::string, std::string> toPropPair(std::string line) {
            if(line.size() == 0)
                return {};

            if(line.at(0) == '#')
                return {};

            int separatorIndex = 0;

            for(; separatorIndex < line.size(); separatorIndex++)
                if(line.at(separatorIndex) == '=')
                    break;

            if(line.size() == separatorIndex)
                throw ConfigulationException(ConfigulationException_InvalidSyntax);

            std::string propName = line.substr(0, separatorIndex);
            std::string propValue = line.substr(separatorIndex + 1);

            Configulation::removeBothSideSpaces(propName);
            Configulation::removeBothSideSpaces(propValue);

            if(propName == "")
                throw ConfigulationException(ConfigulationException_InvalidPropName);

            return std::make_pair(propName, propValue);
        }

    protected:
        void loadData();

    private:
        static void removeBothSideSpaces(std::string &text) {
            try {
                int beforeSpaceLen = 0;
                int behindSpaceLen = 0;

                for(; beforeSpaceLen < text.size(); beforeSpaceLen++)
                    if(text.at(beforeSpaceLen) != ' ')
                        break;

                for(; behindSpaceLen < text.size(); behindSpaceLen++)
                    if(text.at(text.size() - behindSpaceLen - 1) != ' ')
                        break;

                if(beforeSpaceLen != 0 && beforeSpaceLen == behindSpaceLen) {
                    text = "";
                    return;
                }

                text = text.substr(beforeSpaceLen, text.size() - behindSpaceLen - beforeSpaceLen);
            } catch(std::out_of_range excep) {
                std::cout << "String Parse Error (std::out_of_range) \"" + text + "\"" << std::endl;
                exit(-1);
            }
        }
    };
}