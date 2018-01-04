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
#include <deque>

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


 std::map<std::tuple<uint64_t, uint64_t>, std::string> stateLengths;


void printCombinations() {

    std::ofstream os;
    os.open("table.txt", std::ios::binary);


    std::map<uint64_t, std::vector<uint64_t>> sortedColumns;

    for (auto entry : stateLengths) {
        std::pair<uint64_t, uint64_t> key = entry.first;
        std::string value = entry.second;
        uint64_t row = key.first;
        uint64_t col = key.second;


        if (sortedColumns.find(row)==sortedColumns.end()) {
            sortedColumns[row] = std::vector<uint64_t>();
        }

        sortedColumns[row].push_back(col);
    }

    // Sort Columns

    for (auto entry : sortedColumns) {
        std::sort(entry.second.begin(), entry.second.end());
    }

    for (auto entry : sortedColumns) {
        std::stringstream ss;

        ss << std::to_string(entry.first+1) << ": ";

        for (std::size_t i=0; i<10-ss.str().size(); i++) {
            os << " ";
        }

        os << ss.str();
        std::vector<uint64_t> cols = entry.second;

        for (auto col : cols) {
            std::stringstream ss;
            std::pair<uint64_t, uint64_t> key = std::pair<uint64_t, uint64_t>(entry.first, col);

            ss << std::to_string(col+1);

            for (std::size_t i=0; i<40-ss.str().size(); i++) {
                os << " ";
            }


            os << ss.str();
        }
        os << std::endl;
    }

    os << std::endl;

    for (auto entry : sortedColumns) {
        std::stringstream ss;

        ss << std::to_string(entry.first+1) << ": ";

        for (std::size_t i=0; i<10-ss.str().size(); i++) {
            os << " ";
        }

        os << ss.str();
        std::vector<uint64_t> cols = entry.second;

        for (auto col : cols) {
            std::stringstream ss;
            std::pair<uint64_t, uint64_t> key = std::pair<uint64_t, uint64_t>(entry.first, col);

            ss << std::to_string(col+1) << ":" << stateLengths[key];

            for (std::size_t i=0; i<60-ss.str().size(); i++) {
                os << " ";
            }


            os << ss.str();
        }
        os << std::endl;
    }

    os << std::endl;


    os.close();

}

auto lastWrittenTS = std::chrono::system_clock::now();

int main(int argc, char**argv) {
    VimParser vimParser;




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


    std::deque<std::string> todo;
    std::deque<std::string> tmpTodo;
    std::vector<std::string> tmpTodo2;

    std::map<std::string, bool> strsDoneFull;

    todo.push_back("34C3_");

    // Execute last line..


    while (todo.size()>0) {

            for (uint64_t i=0; i<alphabet.size();i++) {

                flag =todo[0];
                if (flag.length()<5 ||
                        flag.substr(0, 5)!="34C3_" ||
                        flag.size()>37 ||
                        strsDoneFull[todo[0]] == true)
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

                    std::string characterCombination = std::get<2>(stateMachinePair[idxStates]);

                    uint64_t row = std::get<0>(stateMachinePair[idxStates]);
                    uint64_t col = std::get<1>(stateMachinePair[idxStates]);


                    std::pair<uint64_t, uint64_t> pair(row,col);


                    if (stateLengths.find(pair)==stateLengths.end()
                            )
                    {
                        stateLengths[pair] = characterCombination;
                        std::cout << "Found New combination to " << std::to_string(row) << " " << std::to_string(col) << " with " << characterCombination << " States:" <<  std::to_string(stateLengths.size()) << " Todo:" <<  std::to_string(todo.size()) << std::endl;

                        if (std::find(tmpTodo.begin(), tmpTodo.end(),characterCombination)==tmpTodo.end())
                            tmpTodo.push_front(characterCombination);
                    } else
                    if (stateLengths[pair].length()>characterCombination.length()
                            )
                    {
                        stateLengths[pair] = characterCombination;
                        std::cout << "Found shorter combination to " << std::to_string(row) << " " << std::to_string(col) << " with " << characterCombination << " States:" <<  std::to_string(stateLengths.size()) << " Todo:" << std::to_string(todo.size()) << std::endl;

                        if (std::find(tmpTodo.begin(), tmpTodo.end(),characterCombination)==tmpTodo.end())
                            tmpTodo.push_back(characterCombination);
                    }
                }
            }


            vimParser.clearStates();

            strsDoneFull[todo[0]] = true;
            todo.pop_front();

            //                for (int idxStates = 0; idxStates<stateMachinePairs[flag].size(); idxStates++) {
            //                    std::cout << "(" << std::get<0>(stateMachinePairs[flag][idxStates])
            //                              << " " << std::get<1>(stateMachinePairs[flag][idxStates]) << "["
            //                              << std::get<2>(stateMachinePairs[flag][idxStates])<< "]" << ") ";
            //                }
            //                std::cout << std::endl;
            // exit(1);

        bool dataAmended = false;
        for (auto entry:tmpTodo) {
            if (std::find(todo.begin(), todo.end(), entry) == todo.end() &&
                strsDoneFull.find(entry)==strsDoneFull.end())
                todo.push_front(entry);
            dataAmended=true;
        }

        auto now = std::chrono::system_clock::now();

        auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now-lastWrittenTS);

        if (dataAmended && seconds.count()>300 || todo.size()==0) {
            printCombinations();
            lastWrittenTS = std::chrono::system_clock::now();
        }

        /*
        for (auto entry : tmpTodo2) {
            if (std::find(todo.begin(), todo.end(), entry) == todo.end() &&
                    strsDoneFull.find(entry)==strsDoneFull.end()) todo.push_back(entry);
        }
        */

        tmpTodo.clear();
        tmpTodo2.clear();

        //if (stateLengths.size()>4000) {
        //    printCombinations();
        //}

    }


}
