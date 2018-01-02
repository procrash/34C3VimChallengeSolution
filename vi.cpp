#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <cctype>
#include <map>

#include <vector>

#include <boost/lexical_cast.hpp>
#include <commands.h>

#include "Vimparser.h"



std::string flag = "34C3_012345678901234567890123456789012";


void checkVimCodeValidities(std::vector<std::string> strs) {
    std::vector<std::string> lineCommands;
    for (std::size_t i=9;i<260;i++) {
        std::string line = strs[i];
        std::vector<std::string> splitted;
        boost::split(splitted,line,boost::is_any_of("."));

        for (auto entry:splitted) {
            std::cout << entry << std::endl;
        }

    }

    exit(1);
}









int main(int argc, char**argv) {
    VimParser vimParser;

    std::map<std::string, std::string> stateLengths;


    std::ifstream is;
    // is.open("vim-5ca46d1e8afdc0b30b25fdf8f69f868b33a16241.txt.1", std::ios::binary);
    is.open("dummy2", std::ios::binary);


    std::vector<std::string> strs;



    std::string line;

    while(getline(is, line)) {
        strs.push_back(line);
    }

    is.close();

    vimParser.setLines(strs);


    //checkVimCodeValidities(strs);


    std::vector<std::string> todo;
    std::vector<std::string> tmpTodo;
    std::vector<std::string> tmpTodo2;

    todo.push_back("34C3_");

    // Execute last line..

    bool shorterCombiantionFound;
    do
    {

        shorterCombiantionFound = false;


        for (uint64_t i=0; i<alphabet.size();i++) {

            for (auto entry: todo) {

                flag =entry;
                if (flag.length()<5 ||
                        flag.substr(0, 5)!="34C3_" ||
                        flag.size()>37)
                    continue;

                vimParser.resetFlagCounter();



                // for (std::size_t flagIdx=0; flagIdx<flag.size();flagIdx++){
                //    strs[3][flagIdx]=flag[flagIdx];
                //}

                std::stringstream fss;
                fss << alphabet[i];

                flag += fss.str();

                if (flag.size()<=37) tmpTodo2.push_back(flag);

                strs[3] = flag;
                vimParser.setFlag(flag);


                // std::cout << flag << std::endl;


                std::string currentLine = strs[strs.size()-1];
                std::string currentLineUnmodified = strs[strs.size()-1];



                vimParser.setCursorAndLinePos(0, strs.size()-1);

                while (currentLine.size()>0) {

                    currentLine = vimParser.parseNextCommand(currentLine);
                }


                // Analyze shortest paths to individual states...

                /*
                for (std::map<std::string,
                     std::vector<std::tuple<uint64_t, uint64_t, std::string>>>::iterator it= vimParser.stateMachinePairs.begin();
                     it!=vimParser.stateMachinePairs.end(); ++it) {
                    std::cout << it->first << std::endl;
                }*/

                std::vector<std::tuple<uint64_t, uint64_t, std::string>> stateMachinePair = vimParser.stateMachinePairs[flag];

                for (std::size_t idxStates = 0; idxStates<stateMachinePair.size(); idxStates++) {
                    std::string stateName = std::to_string(std::get<0>(stateMachinePair[idxStates]))+" "
                            +std::to_string(std::get<1>(stateMachinePair[idxStates]));

                    std::string characterCombination = std::get<2>(stateMachinePair[idxStates]);


                    if (stateLengths.find(stateName)==stateLengths.end()
                            || stateLengths[stateName].length()>characterCombination.length()
                            )
                    {
                        stateLengths[stateName] = characterCombination;
                        std::cout << "Found shorter combination to " << stateName << " with " << characterCombination << " "  << std::endl;
                        shorterCombiantionFound = true;

                        if (std::find(tmpTodo.begin(), tmpTodo.end(),characterCombination)==tmpTodo.end())
                          tmpTodo.push_back(characterCombination);
                    }
                }
            }
            //                for (int idxStates = 0; idxStates<stateMachinePairs[flag].size(); idxStates++) {
            //                    std::cout << "(" << std::get<0>(stateMachinePairs[flag][idxStates])
            //                              << " " << std::get<1>(stateMachinePairs[flag][idxStates]) << "["
            //                              << std::get<2>(stateMachinePairs[flag][idxStates])<< "]" << ") ";
            //                }
            //                std::cout << std::endl;
            // exit(1);
        }


        todo.clear();
        todo = tmpTodo;

        for (auto entry : tmpTodo2) {
            if (std::find(todo.begin(), todo.end(), entry) == todo.end()) todo.push_back(entry);
        }

        tmpTodo.clear();
        tmpTodo2.clear();

    } while (todo.size()>0);


}
