#pragma once
#include <vector>

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


std::string alphabet = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz0123456789\".@" };

