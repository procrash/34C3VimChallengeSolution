#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

class VimParser
{

private:

    bool parsingErrorOccured{false};

    int currentCursorPos{0};
    int currentLinePos{0};
    std::vector<std::string> lines;

    // Optional...
    uint64_t locationsWritten = 0;
    std::map<std::string, bool> writtenStrings;

    std::string lastPositionString = "";

    std::map<std::string, std::string> registerMap;

    std::string currentCharacter = "";
    std::stringstream ccSS;
    std::string flag;

    void initDotFile();

    uint64_t flagNr = 0;

    std::vector<std::string> colors = {
        "red",
        "blue",
        "green",
        "orange",
        "pink"
    };

    int flagCounter = 0;

    std::string edgeColor = "black";

public:

    std::map<std::string, std::vector<std::tuple<uint64_t, uint64_t, std::string>>> stateMachinePairs;

    VimParser();

    void resetFlagCounter();

    void clearStates();

    void setFlag(std::string flag);

    void addDotLocation(uint64_t row, uint64_t col);

    void setCursorAndLinePos(int cursorPos, int linePos);

    void setLines(std::vector<std::string> lines);

    void printCommand(std::string command, std::string comment);


    bool tryReadNumber(std::string& line, uint64_t& number);

    std::string parseNextCommand(std::string line);

    std::string parseGotoScreenColumn(std::string line, bool numberFound, uint64_t number);
    std::string parseYankCharacter(std::string line);
    std::string parseYankWholeLine(std::string line);
    std::string parseExecuteRegister(std::string line);
    std::string parseGotoBeginOfLine(std::string line);
    std::string parseGotoEndOfLine(std::string line);

    std::string parseGotoCursorPositionZero(std::string line);
    std::string parseMoveCursorToRight(std::string line, bool numberFound, uint64_t number);
    std::string parseMoveCursorToLeft(std::string line, bool numberFound, uint64_t number);
    std::string parseMoveCursorDown(std::string line, bool numberFound, uint64_t number);
    std::string parseMoveCursorUp(std::string line, bool numberFound, uint64_t number);
    std::string parseGotoLine(std::string line, bool numberFound, uint64_t number);

    std::string parseFindForward(std::string line);
    std::string parseReverseFind(std::string line);
    std::string parseSetRegister(std::string line);
    std::string parseAppendRegister(std::string line);


};
