#include "Vimparser.h"


void VimParser::initDotFile() {
    std::ofstream os;
    os.open("dotFile.txt", std::ios::binary);
    os << "digraph graphname" << std::endl;
    os << "{" << std::endl;

    os.close();
}

VimParser::VimParser() {
    initDotFile();
}

void VimParser::resetFlagCounter() {
    flagCounter = 0;
    registerMap.clear();
    ccSS.str("");
    currentCursorPos = 0;
    currentLinePos =  0;
    parsingErrorOccured = false;

}

void VimParser::clearStates() {
    this->stateMachinePairs.clear();
}
void VimParser::setFlag(std::string flag) {
    this->flag = flag;
    lines[3] = flag;
}

void VimParser::setCursorAndLinePos(int cursorPos, int linePos) {
    this->currentCursorPos = cursorPos;
    this->currentLinePos = linePos;
}

void VimParser::setLines(std::vector<std::string> lines) {
    this->lines = lines;
}

void VimParser::addDotLocation(uint64_t row, uint64_t col) {
    // if (row<9 || row>258) return;
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



    std::stringstream issLabel;

    if (currentCharacter.size()>0) {
        issLabel << " [ label=\"";

        // Escape Quote
        if (currentCharacter[currentCharacter.size()-1]=='\"') {
            issLabel << "\\";
        }

        issLabel << currentCharacter[currentCharacter.size()-1] << "\" ];";
    }

    if (lastPositionString!="") {
        std::string currentPosString = iss.str();

        std::string stringToWrite =  lastPositionString + " -> " + currentPosString;

        if (writtenStrings.find(stringToWrite)==writtenStrings.end()) {
            // os << "edge [color=" << edgeColor << "]" << std::endl;
            os << stringToWrite << issLabel.str() << std::endl;
            // writtenStrings[stringToWrite]=true;
            // std::cout << flag << std::endl;
        }

        lastPositionString = currentPosString;
    } else {
        std::string currentPosString = iss.str();
        // writtenStrings[iss.str()]=true;
        os <<  currentPosString <<issLabel.str() << std::endl;
        lastPositionString = currentPosString;
    }


    os.close();

    if (row==107 && col==0) {
        std::cout << "Found destination with " << flag << std::endl;
        // exit(1);
    }

    /*
        if (row==3 && col==16) {
            exit(1);
        }*/
}

void VimParser::printCommand(std::string command, std::string comment) {
    /*
        std::cout << command;

        for (std::size_t i=0; i<20-command.length();i++) {
            std::cout << " ";
        }
        std::cout << comment << std::endl;
        */
}

std::string VimParser::parseGotoScreenColumn(std::string line, bool numberFound, uint64_t number) {
    if (numberFound) {
        line = line.substr(1,line.size());
        currentCursorPos = number-1;
        printCommand(std::to_string(number) +"|","Moving cursor to screen column "+std::to_string(number));
        addDotLocation(currentLinePos, currentCursorPos);
    }
    return line;
}

std::string VimParser::parseYankCharacter(std::string line) {
    std::stringstream ss;
    ss << lines[currentLinePos][currentCursorPos];

    line = line.substr(1,line.size());
    registerMap["\""] = ss.str();
    printCommand("y","Yank current character "+ss.str()+" to register \" ");
    return line;
}

std::string VimParser::parseYankWholeLine(std::string line) {
    line = line.substr(1,line.size());
    printCommand("Y","Y Yank whole line "+lines[currentLinePos]+" to register \" ");
    registerMap["\""] = lines[currentLinePos];

    return line;
}

std::string VimParser::parseExecuteRegister(std::string line) {
    std::stringstream ss;
    char c = line[1];
    ss << c;
    std::string regName=ss.str();

    line = line.substr(2,line.size()); // e.g @"
    // line = registerMap[regName]+ line;
    printCommand("@"+regName,"Executing code in register " + regName + " " + registerMap[regName]);

    std::string registerLine = registerMap[regName];
    while (registerLine.size()>0) {
        registerLine = parseNextCommand(registerLine);
    }

    return line;
}

std::string VimParser::parseGotoBeginOfLine(std::string line) {
    printCommand("^","Go to begining of line "+std::to_string(currentLinePos+1));
    currentCursorPos = 0;
    addDotLocation(currentLinePos, currentCursorPos);
    line = line.substr(1,line.size()); // e.g "Ayt.
    return line;
}

std::string VimParser::parseGotoEndOfLine(std::string line) {
    printCommand("$","Go to end of line "+std::to_string(currentLinePos+1));

    currentCursorPos = lines[currentLinePos].size()-1;

    addDotLocation(currentLinePos, currentCursorPos);

    line = line.substr(1,line.size()); // e.g "Ayt.
    return line;

}

std::string VimParser::parseMoveCursorToRight(std::string line, bool numberFound, uint64_t number) {
    if (numberFound) {
        currentCursorPos+=number;
    } else {
        currentCursorPos++;
    }

    if (static_cast<std::size_t>(currentCursorPos)>lines[currentLinePos].size()-1) {
        currentCursorPos = lines[currentLinePos].size()-1;

        if (!numberFound) {
            printCommand("l","Go one to the right "+std::to_string(currentCursorPos+1));
            parsingErrorOccured = true;
            line = "";
            return line;
        } else {
            printCommand(std::to_string(number)+"l","Go "+std::to_string(number)+" to the right "+std::to_string(currentCursorPos+1));
        }
    }

    if (!numberFound) {
        printCommand("l","Go one to the right "+std::to_string(currentCursorPos+1));
    } else {
        printCommand(std::to_string(number)+"l","Go "+std::to_string(number)+" to the right "+std::to_string(currentCursorPos+1));
    }

    addDotLocation(currentLinePos, currentCursorPos);

    line = line.substr(1,line.size()); // e.g "Ayt.
    return line;
}

std::string VimParser::parseMoveCursorToLeft(std::string line, bool numberFound, uint64_t number) {
    if (numberFound) currentCursorPos-=number; else currentCursorPos--;

    if (currentCursorPos<0)  {
        currentCursorPos=0;

        if (!numberFound) {
            printCommand("h","Go one to the left "+std::to_string(currentCursorPos+1));
            parsingErrorOccured = true;
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
    return line;
}

std::string VimParser::parseMoveCursorDown(std::string line, bool numberFound, uint64_t number) {
    // Not implemented yet, if one goes up again, go to the oldposition...
    if (numberFound) {
        currentLinePos += number;
    } else {
        currentLinePos++;
    }

    if (static_cast<std::size_t>(currentLinePos)>lines.size()-1) {
        currentLinePos = lines.size()-1;

        if (!numberFound) {
            printCommand("j","Go one line down "+std::to_string(currentLinePos+1));
            parsingErrorOccured = true;
            line = "";
            return line;
        }
    }

    if (static_cast<std::size_t>(currentCursorPos)>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

    if (!numberFound) {
        printCommand("j","Go one line down "+std::to_string(currentLinePos+1));
    } else {
        printCommand(std::to_string(number)+"j","Go "+std::to_string(number)+" line down "+std::to_string(currentLinePos+1));
    }

    addDotLocation(currentLinePos, currentCursorPos);

    line = line.substr(1,line.size());
    return line;
}

std::string VimParser::parseMoveCursorUp(std::string line, bool numberFound, uint64_t number) {
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
            parsingErrorOccured = true;
            line = "";
            return line;
        }
    }

    if (static_cast<std::size_t>(currentCursorPos)>lines[currentLinePos].size()-1) currentCursorPos = lines[currentLinePos].size()-1;

    if (!numberFound) {
        printCommand("k","Go one line up "+std::to_string(currentLinePos+1));
    } else {
        printCommand(std::to_string(number)+"k","Go "+std::to_string(number)+" line up "+std::to_string(currentLinePos+1));
    }

    addDotLocation(currentLinePos, currentCursorPos);

    line = line.substr(1,line.size());
    return line;
}

std::string VimParser::parseGotoLine(std::string line, bool numberFound, uint64_t number) {
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
    return line;
}

std::string VimParser::parseFindForward(std::string line) {
    std::stringstream ss;

    char c = line[1];
    ss << c;

    bool found = false;

    for (std::size_t i=currentCursorPos; i<lines[currentLinePos].size(); ++i) {
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
    return line;
}

std::string VimParser::parseReverseFind(std::string line) {
    std::stringstream ss;

    char c = line[1];
    ss << c;

    bool found = false;

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
    return line;
}

std::string VimParser::parseSetRegister(std::string line) {
    char c = line[1];
    std::stringstream ss;

    ss << c;

    std::string regName=ss.str();

    char d = line[2];
    char e = line[3];

    char f = line[4];


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

    // std::cout << "Setting register " << c << " to ";

    if (singleCharacter) {
        std::stringstream ss;
        ss << lines[currentLinePos][currentCursorPos];

        registerMap[regName] = ss.str();

        line = line.substr(4,line.size()); // e.g "ayl

    } else

        if (multiCharacterWithCharacter) {
            std::string val;
            for (std::size_t i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                if (lines[currentLinePos][i]==f) break;
                val += lines[currentLinePos][i];
            }

            registerMap[regName] = val;

            line = line.substr(5,line.size()); // e.g "ayt.

        } else if (multiCharacterWithNr) {

            if (numberFound) {
                std::string val = "";
                for (uint64_t i=0; i<number; i++) {
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
    if (multiCharacterWithCharacter) {
        std::stringstream ssMultiStr;
        ssMultiStr << f;

        commandStr+="yt"+ssMultiStr.str();
    }

    if (multiCharacterWithNr) commandStr+="y"+std::to_string(number)+"l";

    printCommand(commandStr,"Storing in register "+regName+" "+registerMap[regName]);
    return line;

}


std::string VimParser::parseAppendRegister(std::string line) {
    // Or an uppercase

    char c = line[1];
    std::stringstream ss;

    ss << c;

    std::string regName=ss.str();

    char d = line[2];
    char e = line[3];

    char f = line[4];


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


    std::string registerCommand = regName;



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
            for (std::size_t i=currentCursorPos; i<lines[currentLinePos].size();i++) {
                if (lines[currentLinePos][i]==f) break;
                val += lines[currentLinePos][i];
            }

            registerMap[regName] = registerMap[regName]+val;
            line = line.substr(5,line.size()); // e.g "Ayt.



        }

        else if (multiCharacterWithNr) {


            if (numberFound) {
                val = "";
                for (uint64_t i=0; i<number; i++) {
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
        flagCounter++;
        if (flagCounter>0)
            ccSS << lines[currentLinePos][currentCursorPos];
        // std::cout << ccSS.str()<< std::endl;

        currentCharacter = ccSS.str();

    }

    printCommand(commandStr,"Appending to register "+regName+" "+val+" Result is:"+registerMap[regName]);
    return line;
}

std::string VimParser::parseGotoCursorPositionZero(std::string line) {
    // line = line.substr(1,line.size());
    currentCursorPos = 0;
    printCommand("0","Go to Cursor Position 0 ");
    return line;
}

std::string VimParser::parseNextCommand(std::string line) {

    // static int gotoCounter =0;

    if (parsingErrorOccured) return "";

    std::string debugBefore = line;

    // Test if line starts with number...
    uint64_t number = -1;
    bool numberFound = tryReadNumber(line, number);

    char c; // ,d,e,f;
    // std::stringstream ss;
    // std::string regName;
    // bool found = false;

    // Parse first charcter of command
    // std::cout << line << std::endl;
    if (numberFound && number==0) {
        line = parseGotoCursorPositionZero(line);
    } else
        switch (line[0]) {

        case '|' : // to screen column "
            line = parseGotoScreenColumn(line, numberFound, number);
            break;

        case 'y' :
            line =parseYankCharacter(line);
            break;

        case 'Y' :

            line = parseYankWholeLine(line);
            break;
        case '@' : // Execute
            line = parseExecuteRegister(line);
            break;

        case '^' : // Go to beginning of line

            line = parseGotoBeginOfLine(line);
            break;
        case '$' : // Go to the end of line

            line = parseGotoEndOfLine(line);
            break;
        case 'l' : // Go one to the right

            line = parseMoveCursorToRight(line, numberFound, number);
            break;

        case 'h' : // Go one to the left
            line = parseMoveCursorToLeft(line, numberFound, number);

            break;
        case 'j' : // Go one down, if down line is less strlen then go to the end,
            line = parseMoveCursorDown(line, numberFound, number);
            break;

        case 'k' : // Go one up, if up line is less strlen then go to the end,
            line = parseMoveCursorUp(line, numberFound, number);
            break;
        case 'G' : // Goto Line found
            line = parseGotoLine(line, numberFound, number);
            break;
        case 'f' : // find command found

            line = parseFindForward(line);
            break;
        case 'F' : // reverse find command found
            line = parseReverseFind(line);
            break;
        case '\"' : // Register command found

            c = line[1];

            if (isdigit(c) || islower(c)) {
                line = parseSetRegister(line);
            } else {
                line = parseAppendRegister(line);
            }

            break;
        }


    return line;
}

bool VimParser::tryReadNumber(std::string &line, uint64_t &number) {
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
