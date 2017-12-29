#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <cctype>
#include <map>

#include <boost/lexical_cast.hpp>

bool tryReadNumber(std::string& line, uint64_t& number) {
    int nrOfDigits =-1;
    while (isdigit(line[++nrOfDigits]));

    number = -1;
    bool numberFound = false;

    if (nrOfDigits>0) {
        std::string numberStr = line.substr(0, nrOfDigits);
        number = boost::lexical_cast<uint64_t>(numberStr);
        line = line.substr(nrOfDigits, line.size());
        numberFound = true;
    }

    return numberFound;
}

std::map<std::string, std::string> registerMap;


std::string parseNextCommand(int& currentCursorPos,
                             int& currentLinePos,
                             std::string line,
                             std::string lineUnmodified,
                             std::vector<std::string> lines) {



    std::vector<std::string> commands = {
        // ----------------------
        // Navigation Commands...
        // ----------------------

        // Gotos...
        "1G","4G","6G","9G",
        "260G","261G",
        "G",

        // Cursor
        "h", "j", "k", "l",

        // Beginning, end of Line
        "^", "$",

        // From Top/Bottom
        "0H",

        // ----------------------
        // Finds...
        // ----------------------

        // Forward finds...
        "f0",
        "f2",
        "fh", "fj", "fk", "fl",

        "fG",
        "fB",
        "ff",

        // Backward finds...
        "Fh",
        "F@",
        "F0",
        "F1",
        "FY",
        "\"G",

        // ----------------------
        // Register access...
        // ----------------------

        // Single character...
        "\"ayl",
        "\"byl",
        "\"cyl",
        "\"dyl",
        "\"eyl",
        "\"fyl",
        "\"gyl",
        "\"hyl",

        // Register access appends...
        "\"Ayl",
        "\"Byl",
        "\"Cyl",
        "\"Dyl",
        "\"Eyl",
        "\"Fyl",
        "\"Gyl",
        "\"Hyl",

        // Multi character...
        "\"Ayt."


        "y3l",
        "yt.",

        // Execute code commands...
        "@a",
        "@b",
        "@c",

        "@d",
        "@e",
        "@f",

        /*
            "Y@\"",
            ,"Y@\"", "3c0", "443Bh4Hf5#",
            ,"19l","\"B",,
            "19l","40|",
            "l" ,"Y@""" ,"@1",
            ,"ya5B^"
            */
    };

    // Test if line starts with number...
    uint64_t number = -1;
    bool numberFound = tryReadNumber(line, number);

    char c,d,e,f;
    std::stringstream ss;
    std::string regName;

    // Parse first charcter of command
    // std::cout << line << std::endl;

    switch (line[0]) {
    case 'a' : // append mode
        std::cout << "Switching to append mode" << std::endl;
        std::cout << "Appending " << line << " @" << currentLinePos << ":"<<currentCursorPos << std::endl;
        line = "";
        break;

     case '|' : // to screen column "

        if (numberFound) {
            std::cout << "Moving cursor to screen column" << number <<std::endl;
            line = line.substr(1,line.size());
            currentCursorPos = number;
        }

        break;
    case 'y' : // Yank current character to register "
        std::cout << "Yanking current character" << std::endl;

        ss << lines[currentLinePos][currentCursorPos];

        line = line.substr(1,line.size());

        registerMap["\""] = ss.str();

        break;

    case 'Y' : // Yank whole line to register "
        std::cout << "Yanking current line" << std::endl;

        line = line.substr(1,line.size());

        registerMap["\""] = lines[currentLinePos];

        break;
    case '@' : // Execute

        c = line[1];
        ss << c;
        regName=ss.str();

        std::cout << "Executing code in register " << regName << " " << registerMap[regName]<<" " << std::endl;

        line = line.substr(2,line.size()); // e.g @"

        line = registerMap[regName]+ line;
        break;

    case '^' : // Go to beginning of line
        std::cout << "Go to begining of line" << std::endl;

        currentCursorPos = 0;
        line = line.substr(1,line.size()); // e.g "Ayt.
        break;
    case '$' : // Go to the end of line
        std::cout << "Go to end of line" << std::endl;

        currentCursorPos = lines[currentLinePos].size()-1;
        line = line.substr(1,line.size()); // e.g "Ayt.
        break;
    case 'l' : // Go one to the right
        std::cout << "Go one to the right" << std::endl;

        currentCursorPos++;

        if (currentCursorPos>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

        line = line.substr(1,line.size()); // e.g "Ayt.
        break;

    case 'h' : // Go one to the left
        std::cout << "Go one left" << std::endl;

        if (currentCursorPos>0) currentCursorPos--;
        line = line.substr(1,line.size());
        break;
    case 'j' : // Go one down, if down line is less strlen then go to the end,
        // Not implemented yet, if one goes up again, go to the oldposition...

        std::cout << "Go one down" << std::endl;

        currentLinePos++;
        if (currentLinePos>lines.size()-1) currentLinePos = lines.size()-1;

        if (currentCursorPos>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

        line = line.substr(1,line.size());
        break;

    case 'k' : // Go one up, if up line is less strlen then go to the end,
        // Not implemented yet, if one down up again, go to the oldposition...

        std::cout << "Go one up" << std::endl;

        if (currentLinePos>0) currentLinePos--;
        if (currentCursorPos>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

        line = line.substr(1,line.size());
        break;
    case 'G' : // Goto Line found
        std::cout << "Goto";
        if (numberFound) {
            currentLinePos = number-1;
            std::cout << " line " << number;
        } else {
            currentLinePos = lines.size()-1;
            std::cout << " eof";
        }

        std::cout << std::endl;
        if (number==260) {
            std::cout << "Next character from FLag" << std::endl;

        }

        currentCursorPos = 0;

        line = line.substr(1,line.size());
        break;
    case 'f' : // find command found
        c = line[1];
        std::cout << "Trying to find " << c << "...";

        for (int i=currentCursorPos; i<lines[currentLinePos].size(); ++i) {
            // std::cout << "Looking at " << i << " " << lines[currentLinePos][i] << std::endl;
            if (lines[currentLinePos][i]==c) {
                std::cout << "found at " << i;
                currentCursorPos = i;
                break;
            }
        }

        std::cout << std::endl;

        line = line.substr(2,line.size());
        break;
    case 'F' : // reverse find command found
        c = line[0];
        std::cout << "Trying to reverse find " << c << "...";

        for (int i=currentCursorPos; i>=0; --i) {
            if (lines[currentLinePos][i]==c) {
                std::cout << "found at " << i;
                currentCursorPos = i;
                break;
            }
        }

        std::cout << std::endl;

        line = line.substr(2,line.size());
        break;
    case '\"' : // Register command found

        c = line[1];
        ss << c;

        regName=ss.str();

        d = line[2];
        e = line[3];

        f = line[4];


        std::string numberStr = line.substr(3, line.size());
        uint64_t lenNumberStr =numberStr.size();
        uint64_t number;


        bool numberFound = false;

        if (tryReadNumber(numberStr, number)) {
            numberFound = true;
            lenNumberStr = lenNumberStr-numberStr.length();
            f = numberStr[0];
        }




        bool singleCharacter = false;
        bool multiCharacterWithCharacter = false;
        bool multiCharacterWithNr = false;

        if (d=='y' && e=='l') {
            singleCharacter = true;
        } else if (d=='y' && e=='t' && f=='.') {
            multiCharacterWithCharacter = true;
        } else if (d=='y' && numberFound && f=='l'){
            multiCharacterWithNr = true;
        } else {
            std::cout << "Error" << std::endl;
        }


        // Check if we have a digit
        // Or a lowercase charcter
        if (isdigit(c) || islower(c)) {
            std::cout << "Setting register " << c << " to ";

            if (singleCharacter) {
                registerMap[regName] = lines[currentLinePos][currentCursorPos];

                line = line.substr(4,line.size()); // e.g "ayl

                std::cout << registerMap[regName]  << std::endl;
            } else
                std::cout << "Setting register " << c << " to ";
            if (multiCharacterWithCharacter) {
                std::string val;
                for (int i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                    if (lines[currentLinePos][i]==f) break;
                    val += lines[currentLinePos][i];
                }

                registerMap[regName] = val;

                std::cout << val << std::endl;

                line = line.substr(5,line.size()); // e.g "ayt.

            } else if (multiCharacterWithNr) {

                std::cout << "Setting register " << c << " to ";


                if (numberFound) {
                    std::string val = "";
                    for (int i=0; i<number; i++) {
                        val += lines[currentLinePos][currentCursorPos+i];
                    }

                    std::cout << val << std::endl;
                    registerMap[regName] = val;

                }

                line = line.substr(4+lenNumberStr,line.size()); // "Gy3l


            }

            std::cout << std::endl;



        } else {
            // Or an uppercase
            std::cout << "Appending to register " << c << " ";

            boost::algorithm::to_lower(regName);

            /*
            if (c=='d') {
                std::cout << "Debug" << std::endl;
            }*/

            if (singleCharacter) {
                registerMap[regName] = registerMap[regName] + lines[currentLinePos][currentCursorPos];
                line = line.substr(4,line.size()); // e.g "Ayl

                std::cout << lines[currentLinePos][currentCursorPos] << " Newval is:" << registerMap[regName] << std::endl;

            } else
                if (multiCharacterWithCharacter) {
                    std::string val;
                    for (int i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                        if (lines[currentLinePos][i]==f) break;
                        val += lines[currentLinePos][i];
                    }

                    registerMap[regName] = registerMap[regName]+val;
                    line = line.substr(5,line.size()); // e.g "Ayt.

                    std::cout << val << " Newval is:" << registerMap[regName] <<  std::endl;


                }

                else if (multiCharacterWithNr) {

                    std::cout << "Appending to register " << c << " to ";


                    if (numberFound) {
                        std::string val = "";
                        for (int i=0; i<number; i++) {
                            val += lines[currentLinePos][currentCursorPos+i];
                        }

                        std::cout << val;
                        registerMap[regName] += val;

                    }


                    line = line.substr(4+lenNumberStr,line.size()); // "Gy3l


                    std::cout << " Newval is:" << registerMap[regName];
                    std::cout << std::endl;

                }


            break;


        }
    }


        return line;


    }

    int main(int argc, char**argv) {
        std::ifstream is;
        is.open("vim-5ca46d1e8afdc0b30b25fdf8f69f868b33a16241.txt.1", std::ios::binary);


        std::vector<std::string> strs;



        std::string line;

        while(getline(is, line)) {
            strs.push_back(line);
        }

        int lineNr = 1;

        /*
    for (std::size_t i=0; i<strs.size(); i++) {
        std::cout << (lineNr++) << " " <<  strs[i] << std::endl;
    }
    */

        // Execute last line..

        std::string currentLine = strs[strs.size()-1];
        std::string currentLineUnmodified = strs[strs.size()-1];
        int currentCursorPos = 0;
        int currentLinePos = strs.size()-1 ;

        while (currentLine.size()>0) {
            currentLine = parseNextCommand(currentCursorPos,
                                           currentLinePos,
                                           currentLine,
                                           currentLineUnmodified,
                                           strs);
        }


        is.close();

    }
