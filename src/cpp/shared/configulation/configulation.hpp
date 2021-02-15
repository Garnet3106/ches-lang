/*
 * Chestnut 0.0.0
 *
 * GitHub Repository:
 *     https://github.com/Garnet3106/chestnut/
 *
 * Copyright © 2020-2021 Garnet3106 All rights reserved.
 */


#pragma once


#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../filemanager/filemanager.hpp"

#include "./configulation.cpp"


using namespace ches::shared;


ConfigulationException::ConfigulationException() {}

ConfigulationException::ConfigulationException(ConfigulationExceptionType type) {
    this->type = type;
}

ConfigulationException::ConfigulationException(ConfigulationExceptionType type, std::string target) {
    this->type = type;
    this->target = target;
}


std::string Configulation::homeDirEnvName = "CHES_HOME";
std::string Configulation::homeDirPath = Configulation::getEnvironmentVariable(Configulation::homeDirEnvName);

Configulation Configulation::settings = Configulation("/0.0.0/settings/chesc.cnf");
Configulation Configulation::langPack = Configulation("/0.0.0/langpack/ja-jp/");

Configulation::Configulation() {}

Configulation::Configulation(std::string path) {
    if(Configulation::homeDirPath == "") {
        std::cout << "configulation error: Environment variable '" << Configulation::homeDirEnvName << "' is not set or empty." << std::endl;
        exit(-1);
    }

    this->path = Configulation::homeDirPath + path;
    this->loadData();
}

void Configulation::edit(std::unordered_map<std::string, std::string> editedOptionMap) {
    for(const auto [ key, value ] : editedOptionMap)
        if(!this->exists(key))
            throw ConfigulationException(ConfigulationException_UnknownPropName);

    std::vector<std::string> lineVec;

    try {
        lineVec = FileManager::getLines(this->path);
    } catch(FileManagerException excep) {
        throw excep;
    }

    for(int i = 0; i < lineVec.size(); i++) {
        std::pair<std::string, std::string> prop;

        try {
            prop = Configulation::toPropPair(lineVec.at(i));
        } catch(ConfigulationException excep) {
            throw excep;
        }

        if(prop == (std::pair<std::string, std::string>){})
            continue;

        if(editedOptionMap.count(prop.first) == 1)
            lineVec.at(i) = prop.first + "=" + editedOptionMap.at(prop.first);
    }

    try {
        FileManager::writeLines(this->path, lineVec);
    } catch(FileManagerException excep) {
        throw excep;
    }
}

bool Configulation::exists(std::string key) {
    return this->dataMap.count(key);
}

std::string Configulation::get(std::string key) {
    if(!this->exists(key))
        return key;

    return this->dataMap.at(key);
}

void Configulation::print() {
    std::cout << "[debug] Configulation Properties ( whole size: " << this->dataMap.size() << " )" << std::endl;

    for(auto [ key, value ] : this->dataMap)
        std::cout << "\t" << key << ": " << value << "," << std::endl;

    if(this->dataMap.size() > 0)
        std::cout << std::endl;
}

void Configulation::loadData() {
    std::vector<std::string> lineVec;

    try {
        if(!FileManager::exists(this->path))
            throw FileManagerException(FileManagerException_PathNotFound, this->path);

        if(FileManager::isDirectory(this->path)) {
            std::vector<std::string> filePathVec = FileManager::getFilePathsInDirectory(this->path);

            for(std::string filePath : filePathVec) {
                std::vector<std::string> lineVec_tmp = FileManager::getLines(filePath);

                for(std::string line : lineVec_tmp)
                    lineVec.push_back(line);
            }
        } else {
            lineVec = FileManager::getLines(this->path);
        }
    } catch(FileManagerException excep) {
        // note: 言語データが利用できないため、switchを使ってエラー名を取得する

        std::string errorName;

        switch(excep.type) {
            case FileManagerException_FileUnopenable:
            errorName = "FileUnopenable";
            break;

            case FileManagerException_NotFilePath:
            errorName = "NotFilePath";
            break;

            case FileManagerException_PathNotFound:
            errorName = "PathNotFound";
            break;

            default:
            errorName = "UnknownFileManagerError";
            break;
        }

        std::cout << "error (FileError: " << errorName << "; " << excep.target << ") on Configulation::loadData()" << std::endl;
        exit(-1);
    }

    try {
        for(std::string line : lineVec) {
            std::pair<std::string, std::string> prop = Configulation::toPropPair(line);

            if(prop == (std::pair<std::string, std::string>){})
                continue;

            if(this->exists(prop.first))
                throw ConfigulationException(ConfigulationException_DuplicatedPropName);

            this->dataMap[prop.first] = prop.second;
        }
    } catch(ConfigulationException excep) {
        // note: 言語データが利用できないため、switchを使ってエラー名を取得する

        std::string errorName;

        switch(excep.type) {
            case ConfigulationException_DuplicatedPropName:
            errorName = "DuplicatedPropName";
            break;

            case ConfigulationException_InvalidPropName:
            errorName = "InvalidPropName";
            break;

            case ConfigulationException_InvalidSyntax:
            errorName = "InvalidSyntax";
            break;

            default:
            errorName = "UnknownConfigulationError";
            break;
        }

        std::cout << "error (" << errorName << "; " << excep.target << ") on Configulation::loadConfigulation" << std::endl;
        exit(-1);
    }
}
