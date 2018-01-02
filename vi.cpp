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

void initDotFile() {
    std::ofstream os;
    os.open("dotFile.txt", std::ios::binary);
    os << "digraph graphname" << std::endl;
    os << "{" << std::endl;


    os.close();
}

uint64_t flagNr = 0;

std::vector<std::string> colors = {
    "red",
    "blue",
    "green",
    "orange",
    "pink"
};

std::string edgeColor = "black";

std::string lastPositionString = "";

uint64_t locationsWritten = 0;

std::string currentCharacter = "";
std::stringstream ccSS;

std::map<std::string, std::vector<std::tuple<uint64_t, uint64_t, std::string>>> stateMachinePairs;

std::map<std::string, bool> writtenStrings;

std::string flag = "34C3_012345678901234567890123456789012";

void addDotLocation(uint64_t row, uint64_t col) {
    if (row<9 || row>258) return;

    if (row==107 && col==0) {
        std::cout << "Found destination with " << flag << std::endl;
        exit(1);
    }
    stateMachinePairs[flag].push_back(std::tuple<uint64_t, uint64_t,std::string>(row,col, currentCharacter));


    /*
    if (row==0 && col==11) {
        std::cout << "Nächstes zeichen" << flagNr << std::endl;
    }*/


    locationsWritten++;

   // if (locationsWritten>300) return;

    std::ofstream os;
    os.open("dotFile.txt", std::ios::app);

    std::stringstream iss;
    iss << "\"" << row << "," << col << "\"";


    if (lastPositionString!="") {
        std::string currentPosString = iss.str();

        std::string stringToWrite =  lastPositionString + " -> " + currentPosString ;

        if (writtenStrings.find(stringToWrite)==writtenStrings.end()) {
            os << "edge [color=" << edgeColor << "]" << std::endl;
            os << stringToWrite << std::endl;
            writtenStrings[stringToWrite]=true;
            std::cout << flag << std::endl;
        }

        lastPositionString = currentPosString;
    } else {
        std::string currentPosString = iss.str();
        writtenStrings[iss.str()]=true;
        os <<  currentPosString << std::endl;
        lastPositionString = currentPosString;
    }


    os.close();

    /*
    if (row==3 && col==16) {
        exit(1);
    }*/
}


void printCommand(std::string command, std::string comment) {
    /*
    std::cout << command;

    for (int i=0; i<20-command.length();i++) {
        std::cout << " ";
    }
    std::cout << comment << std::endl;
    */
}

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

    static int gotoCounter =0;


    std::string debugBefore = line;

    // Test if line starts with number...
    uint64_t number = -1;
    bool numberFound = tryReadNumber(line, number);

    char c,d,e,f;
    std::stringstream ss;
    std::string regName;
    bool found = false;

    // Parse first charcter of command
    // std::cout << line << std::endl;

    switch (line[0]) {
    /*
    case 'a' : // append mode
        std::cout << "Switching to append mode" << std::endl;
        std::cout << "Appending " << line << " @" << currentLinePos << ":"<<currentCursorPos << std::endl;
        line = "";
        break;
    */

     case '|' : // to screen column "

        if (numberFound) {
            line = line.substr(1,line.size());
            currentCursorPos = number;
            printCommand("|","Moving cursor to screen column "+std::to_string(number));

            addDotLocation(currentLinePos, currentCursorPos);
        }

        break;
    case 'y' :

        ss << lines[currentLinePos][currentCursorPos];

        line = line.substr(1,line.size());

        registerMap["\""] = ss.str();

        printCommand("y","Yank current character "+ss.str()+" to register \" ");


        break;

    case 'Y' :

        //std::cout << "Yanking current line" << std::endl;

        line = line.substr(1,line.size());

        printCommand("Y","Y Yank whole line "+lines[currentLinePos]+" to register \" ");

        registerMap["\""] = lines[currentLinePos];

        break;
    case '@' : // Execute



        c = line[1];
        ss << c;
        regName=ss.str();

        line = line.substr(2,line.size()); // e.g @"

        line = registerMap[regName]+ line;

        // if (regName=="a" || regName=="b") {
        //    std::cout << std::endl;
        //}

        printCommand("@"+regName,"Executing code in register " + regName + " " + registerMap[regName]);
        break;

    case '^' : // Go to beginning of line

        printCommand("^","Go to begining of line "+std::to_string(currentLinePos+1));

        currentCursorPos = 0;

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size()); // e.g "Ayt.
        break;
    case '$' : // Go to the end of line

        printCommand("$","Go to end of line "+std::to_string(currentLinePos+1));

        currentCursorPos = lines[currentLinePos].size()-1;

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size()); // e.g "Ayt.
        break;
    case 'l' : // Go one to the right
        if (numberFound) {
            currentCursorPos+=number;
        } else {
            currentCursorPos++;
        }

        if (currentCursorPos>lines[currentLinePos].size()-1) {
            currentCursorPos = lines[currentLinePos].size()-1;

            if (!numberFound) {
                printCommand("l","Go one to the right "+std::to_string(currentCursorPos+1));
            } else {
                printCommand(std::to_string(number)+"l","Go "+std::to_string(number)+" to the right "+std::to_string(currentCursorPos+1));
            }
            // Exit if you can't go further to the right
            line = "";
            return line;
        }

        if (!numberFound) {
            printCommand("l","Go one to the right "+std::to_string(currentCursorPos+1));
        } else {
            printCommand(std::to_string(number)+"l","Go "+std::to_string(number)+" to the right "+std::to_string(currentCursorPos+1));
        }

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size()); // e.g "Ayt.
        break;

    case 'h' : // Go one to the left
        if (numberFound) currentCursorPos-=number; else currentCursorPos--;

        if (currentCursorPos<0)  {
            currentCursorPos=0;

            if (!numberFound) {
                printCommand("h","Go one to the left "+std::to_string(currentCursorPos+1));
                line = "";
                return line;
            }
        }

        if (!numberFound) {
            printCommand("h","Go one to the left "+std::to_string(currentCursorPos+1));
        } else {
            printCommand(std::to_string(number)+"h","Go "+std::to_string(number)+" to the left "+std::to_string(currentCursorPos+1));
        }

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size());
        break;
    case 'j' : // Go one down, if down line is less strlen then go to the end,
        // Not implemented yet, if one goes up again, go to the oldposition...
        if (numberFound) {
            currentLinePos += number;
        } else {
            currentLinePos++;
        }

        if (currentLinePos>lines.size()-1) {
            currentLinePos = lines.size()-1;

            if (!numberFound) {
                printCommand("j","Go one line down "+std::to_string(currentLinePos+1));
                line = "";
                return line;
            }
        }

        if (currentCursorPos>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

        if (!numberFound) {
            printCommand("j","Go one line down "+std::to_string(currentLinePos+1));
        } else {
            printCommand(std::to_string(number)+"j","Go "+std::to_string(number)+" line down "+std::to_string(currentLinePos+1));
        }

        addDotLocation(currentLinePos, currentCursorPos);


        line = line.substr(1,line.size());
        break;

    case 'k' : // Go one up, if up line is less strlen then go to the end,
        // Not implemented yet, if one down up again, go to the oldposition...
        if (numberFound) {
            currentLinePos-=number;
        } else {
            currentLinePos--;
        }

        if (currentLinePos<0) {
            currentLinePos = 0;

            if (!numberFound) {
                printCommand("k","Go one line up "+std::to_string(currentLinePos+1));
                line = "";
                return line;
            }
        }

        if (currentCursorPos>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

        if (!numberFound) {
            printCommand("k","Go one line up "+std::to_string(currentLinePos+1));
        } else {
            printCommand(std::to_string(number)+"k","Go "+std::to_string(number)+" line up "+std::to_string(currentLinePos+1));
        }

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size());
        break;
    case 'G' : // Goto Line found

        if (numberFound) {
            currentLinePos = number-1;

            printCommand(std::to_string(number)+"G","Goto line "+std::to_string(number));

            if (number==260) {

                if (flagNr<colors.size()-1)
                    edgeColor = colors[flagNr];
                else
                    edgeColor = "black";

                flagNr++;
            }



        } else {
            currentLinePos = lines.size()-1;
            printCommand("G","Goto end of file ");
        }

        currentCursorPos = 0;

        addDotLocation(currentLinePos, currentCursorPos);

        line = line.substr(1,line.size());
        break;
    case 'f' : // find command found

        c = line[1];
        ss << c;

        found = false;

        for (int i=currentCursorPos; i<lines[currentLinePos].size(); ++i) {
            // std::cout << "Looking at " << i << " " << lines[currentLinePos][i] << std::endl;
            if (lines[currentLinePos][i]==c) {
                // std::cout << "found at " << i;
                currentCursorPos = i;
                printCommand("f"+ss.str(),"find  "+ss.str()+" "+std::to_string(i+i));

                addDotLocation(currentLinePos, currentCursorPos);

                found = true;
                break;
            }
        }

        if (!found) {
            // std::cout << "Error, could not find character " << ss.str();
            line = "";
            return line;
        }




        line = line.substr(2,line.size());
        break;
    case 'F' : // reverse find command found


        c = line[1];
        ss << c;

        found = false;

        for (int i=currentCursorPos; i>=0; --i) {
            if (lines[currentLinePos][i]==c) {
                // std::cout << "found at " << i;
                currentCursorPos = i;

                printCommand("F"+ss.str(),"Reverse find  "+ss.str()+" Found @"+std::to_string(i+1));
                found = true;

                addDotLocation(currentLinePos, currentCursorPos);


                break;
            }
        }

        if (!found) {
            // std::cout << "Error, could not find character " << ss.str();
            line = "";
            return line;
        }

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
            // std::cout << "Setting register " << c << " to ";

            if (singleCharacter) {
                registerMap[regName] = lines[currentLinePos][currentCursorPos];

                line = line.substr(4,line.size()); // e.g "ayl

            } else

                if (multiCharacterWithCharacter) {
                std::string val;
                for (int i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                    if (lines[currentLinePos][i]==f) break;
                    val += lines[currentLinePos][i];
                }

                registerMap[regName] = val;

                line = line.substr(5,line.size()); // e.g "ayt.

            } else if (multiCharacterWithNr) {

                if (numberFound) {
                    std::string val = "";
                    for (int i=0; i<number; i++) {
                        val += lines[currentLinePos][currentCursorPos+i];
                    }

                    // std::cout << val << std::endl;
                    registerMap[regName] = val;

                }

                line = line.substr(4+lenNumberStr,line.size()); // "Gy3l


            }

            std::string debugStr;
            if (singleCharacter) debugStr="yl";
            if (multiCharacterWithCharacter) debugStr = "yt.";
            if (multiCharacterWithNr) debugStr="y"+std::to_string(number)+"l";

            std::string commandStr="\""+regName;

            if (singleCharacter) commandStr+="yl";
            if (multiCharacterWithCharacter) commandStr+="yt"+f;
            if (multiCharacterWithNr) commandStr+="y"+std::to_string(number)+"l";

            printCommand(commandStr,"Storing in register "+regName+" "+registerMap[regName]);

        } else {
            // Or an uppercase
            std::string registerCommand = regName;

           // if (regName=="B") {
           //     std::cout << std::endl;
           // }


            boost::algorithm::to_lower(regName);

            std::string val = "";



            if (singleCharacter) {
                registerMap[regName] = registerMap[regName] + lines[currentLinePos][currentCursorPos];
                line = line.substr(4,line.size()); // e.g "Ayl

                if (lines[currentLinePos][currentCursorPos]=='!') {
                    std::cout << "Ende erreicht"<< std::endl;
                    line = "";
                    return line;
                }


            } else
                if (multiCharacterWithCharacter) {
                    std::string val;
                    for (int i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                        if (lines[currentLinePos][i]==f) break;
                        val += lines[currentLinePos][i];
                    }

                    registerMap[regName] = registerMap[regName]+val;
                    line = line.substr(5,line.size()); // e.g "Ayt.



                }

                else if (multiCharacterWithNr) {


                    if (numberFound) {
                        val = "";
                        for (int i=0; i<number; i++) {
                            val += lines[currentLinePos][currentCursorPos+i];
                        }

                        registerMap[regName] += val;

                    }


                    line = line.substr(4+lenNumberStr,line.size()); // "Gy3l

                }


            std::string debugStr;
            if (singleCharacter) debugStr="yl";
            if (multiCharacterWithCharacter) debugStr = "yt.";
            if (multiCharacterWithNr) debugStr="y"+std::to_string(number)+"l";


            std::stringstream ssChr;
            ssChr << f;

            std::string commandStr="\""+registerCommand;

            if (singleCharacter) commandStr+="yl";
            if (multiCharacterWithCharacter) commandStr+="yt"+ssChr.str();
            if (multiCharacterWithNr) commandStr+="y"+std::to_string(number)+"l";

            if (regName=="d" && singleCharacter)
            {

                ccSS << lines[currentLinePos][currentCursorPos];
                // std::cout << ccSS.str()<< std::endl;

                currentCharacter = ccSS.str();

            }

            printCommand(commandStr,"Appending to register "+regName+" "+val+" Result is:"+registerMap[regName]);



            break;


        }
    }


        return line;


    }

    int main(int argc, char**argv) {

        initDotFile();

        std::ifstream is;
        // is.open("vim-5ca46d1e8afdc0b30b25fdf8f69f868b33a16241.txt.1", std::ios::binary);
        is.open("dummy2", std::ios::binary);


        std::vector<std::string> strs;



        std::string line;

        while(getline(is, line)) {
            strs.push_back(line);
        }

        is.close();

        //checkVimCodeValidities(strs);

        int lineNr = 1;


        // Execute last line..
        for (uint64_t i=0; i<4294967296-1;i++) {

            /*
            uint64_t divider = i;

            for (uint8_t idx = 0; idx<33; idx++) {
                uint32_t remainder = divider % alphabet.size();
                divider /= alphabet.size();
                flag[5+idx]= alphabet[remainder];
            }
            */

            registerMap.clear();
            ccSS.str("");

            for (std::size_t flagIdx=0; flagIdx<flag.size();flagIdx++){
                strs[3][flagIdx]=flag[flagIdx];
            }
            strs[3] = "34C3_";

            // std::cout << flag << std::endl;


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

            for (int idxStates = 0; idxStates<stateMachinePairs[flag].size(); idxStates++) {
                std::cout << "(" << std::get<0>(stateMachinePairs[flag][idxStates])
                          << " " << std::get<1>(stateMachinePairs[flag][idxStates]) << "["
                          << std::get<2>(stateMachinePairs[flag][idxStates])<< "]" << ") ";
            }
            //std::cout << std::endl;
            exit(1);
        }


    }
