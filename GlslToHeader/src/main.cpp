#include <fstream>
#include <string>
#include <iostream>

//Input: Input file path
int main(int argc, char **argv) {

    if(argc > 1) {
        std::string filePath(argv[1]);

        std::ifstream in;
        in.open(filePath);

        if(!in.is_open()) {
            std::cout << "GlslToHeader: Error opening " << filePath << ". Specify a valid GLSL in." << std::endl;
            return 1;
        }

        std::string path;
        std::string name;
        std::string ext;

        size_t lastBar = filePath.find_last_of("/");
        if(lastBar != std::string::npos) {
            path = filePath.substr(0, lastBar);
            name = filePath.substr(lastBar + 1, filePath.size());

            size_t dot = name.find_last_of(".");
            if(dot != std::string::npos) {
                ext = name.substr(dot + 1, name.size());
                name = name.substr(0, dot);
            }
        }

        std::string outFilePath(path + "/generated/" + name + "." + ext + ".h");
        std::ofstream out;
        out.open(outFilePath);

        out << "#pragma once" << std::endl << std::endl;
        out << "/*--------------------------------------------------------*/" << std::endl;
        out << "/*--------------AUTOMATICALLY GENERATED FILE--------------*/" << std::endl;
        out << "/*-----------------------DO NOT EDIT----------------------*/" << std::endl;
        out << "/*--------------------------------------------------------*/" << std::endl << std::endl;

        out << "constexpr const char* " << name << "_" << ext << " = R\"(" << std::endl;
        out << in.rdbuf();
        out << ")\";";

        in.close();
        out.close();

        std::cout << "GlslToHeader: Successfully generated header " << outFilePath << "." << std::endl;
    }
    else {
        std::cout << "GlslToHeader: Not enough arguments. Specify an input GLSL." << std::endl;
        return 1;
    }
    return 0;
}