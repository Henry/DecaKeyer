/// Copyright 2012 Henry G. Weller
// -----------------------------------------------------------------------------
//  This file is part of
/// ---     DecaKeyer: Hand-held Chorded Keyboard
// -----------------------------------------------------------------------------
//
//  DecaKeyer is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  DecaKeyer is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with DecaKeyer.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
/// Title: Convert keymap text files into struct definitions
// -----------------------------------------------------------------------------

// Reading a text file
#define CONVERT_H
#include "deca_keyer.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef struct
{
    uint8_t len;
    mkuc codes[3];
} chordCodes;

int chordToId(const int key)
{
    if (key == 0)
    {
        return 1;
    }
    else
    {
        return (1<<key);
    }
}

mkuc charToKey(const char c)
{
    mkuc k = {0, 0};

    if ('a' <= c && c <= 'z')
    {
        k.code = c - ('a' - KEY_A);
    }
    else
    {
        switch (c)
        {
            case '0':
                k.code = KEY_0;
                break;
            case '1':
                k.code = KEY_1;
                break;
            case '2':
                k.code = KEY_2;
                break;
            case '3':
                k.code = KEY_3;
                break;
            case '4':
                k.code = KEY_4;
                break;
            case '5':
                k.code = KEY_5;
                break;
            case '6':
                k.code = KEY_6;
                break;
            case '7':
                k.code = KEY_7;
                break;
            case '8':
                k.code = KEY_8;
                break;
            case '9':
                k.code = KEY_9;
                break;
            case ' ':
                k.code = KEY_SPACE;
                break;
            case '-':
                k.code = KEY_MINUS;
                break;
            case '=':
                k.code = KEY_EQUAL;
                break;
            case '\\':
                k.code = KEY_BACKSLASH;
                break;
            case ';':
                k.code = KEY_SEMICOLON;
                break;
            case '\'':
                k.code = KEY_QUOTE;
                break;
            case '\n':
                k.code = KEY_ENTER;
                break;
            case ',':
                k.code = KEY_COMMA;
                break;
            case '.':
                k.code = KEY_PERIOD;
                break;
            case '/':
                k.code = KEY_SLASH;
                break;
            case '[':
                k.code = KEY_LEFT_BRACE;
                break;
            case ']':
                k.code = KEY_RIGHT_BRACE;
                break;
            case '?':
                k.code = KEY_SLASH;
                k.mod = KEY_SHIFT;
                break;
            case '"':
                k.code = KEY_QUOTE;
                k.mod = KEY_SHIFT;
                break;
            case '`':
                k.code = KEY_TILDE;
                break;
            case '~':
                k.code = KEY_TILDE;
                k.mod = KEY_SHIFT;
                break;
            case '!':
                k.code = KEY_1;
                k.mod = KEY_SHIFT;
                break;
            case '@':
                k.code = KEY_2;
                k.mod = KEY_SHIFT;
                break;
            case '#':
                k.code = KEY_3;
                k.mod = KEY_SHIFT;
                break;
            case '$':
                k.code = KEY_4;
                k.mod = KEY_SHIFT;
                break;
            case '%':
                k.code = KEY_5;
                k.mod = KEY_SHIFT;
                break;
            case '^':
                k.code = KEY_6;
                k.mod = KEY_SHIFT;
                break;
            case '&':
                k.code = KEY_7;
                k.mod = KEY_SHIFT;
                break;
            case '*':
                k.code = KEY_8;
                k.mod = KEY_SHIFT;
                break;
            case '(':
                k.code = KEY_9;
                k.mod = KEY_SHIFT;
                break;
            case ')':
                k.code = KEY_0;
                k.mod = KEY_SHIFT;
                break;
            case '_':
                k.code = KEY_MINUS;
                k.mod = KEY_SHIFT;
                break;
            case '+':
                k.code = KEY_EQUAL;
                k.mod = KEY_SHIFT;
                break;
            case ':':
                k.code = KEY_SEMICOLON;
                k.mod = KEY_SHIFT;
                break;
            case '<':
                k.code = KEY_COMMA;
                k.mod = KEY_SHIFT;
                break;
            case '>':
                k.code = KEY_PERIOD;
                k.mod = KEY_SHIFT;
                break;
            case '{':
                k.code = KEY_LEFT_BRACE;
                k.mod = KEY_SHIFT;
                break;
            case '|':
                k.code = KEY_BACKSLASH;
                k.mod = KEY_SHIFT;
                break;
            case '}':
                k.code = KEY_RIGHT_BRACE;
                k.mod = KEY_SHIFT;
                break;
            default:
                k.code = c;
        }
    }

    return k;
}


void convertFile
(
    const string fileNameBase,
    const string prefix,
    const int n_chord_key_pairs,
    const int n_chords
)
{
    string fileName(fileNameBase + ".txt");

    ifstream chordsFile(fileName.c_str());

    if (!chordsFile.is_open())
    {
        cout<< "Unable to open file" << fileName << endl;
        return;
    }

    static const int maxChars = 3;
    chordCodes chordMap[n_chords];
    string chordMapStrings[n_chords];

    for (int chordID=0; chordID<n_chords; chordID++)
    {
        for (int j=0; j<maxChars; j++)
        {
            chordMap[chordID].len = 0;
            chordMap[chordID].codes[j].mod = 0;
            chordMap[chordID].codes[j].code = 0;
        }
    }

    string line;

    while (chordsFile.good())
    {
        getline(chordsFile, line);

        // Ignore comment lines (blank or starting with #)
        if (!line.size() || line[0] == '#')
        {
            continue;
        }

        int chordShift = 0;

        int i = 0;

        // Check if this chord includes the thumb chord key
        if (line[i++] == '_')
        {
            chordShift = 1<<2*n_chord_key_pairs;
        }

        // Ignore the single space
        i++;

        int chordID = chordShift - 1;

        for (int j=0; j<n_chord_key_pairs; j++)
        {
            switch (line[i + j])
            {
                case '>':
                    chordID += chordToId(2*j);
                    break;
                case '<':
                    chordID += chordToId(2*j + 1);
                    break;
                case '_':
                    chordID += chordToId(2*j) + chordToId(2*j + 1);
                    break;
            }
        }

        i += n_chord_key_pairs;

        while (line[i++] == ' ');

        if (line[i-1] == '"')
        {
            chordMap[chordID].len = line.size() - 1 - i;
            for (int j=i; j<(line.size() - 1); j++)
            {
                chordMap[chordID].codes[j-i] = charToKey(line[j]);
            }
        }
        else
        {
            chordMapStrings[chordID] = line.substr(i-1);
        }
    }

    chordsFile.close();


    string outputFileName(fileNameBase + ".h");

    ofstream outputFile(outputFileName.c_str());

    if (!outputFile.is_open())
    {
        cout<< "Unable to open file" << outputFileName << endl;
        return;
    }

    for (int chordID=0; chordID<n_chords; chordID++)
    {
        if (chordMapStrings[chordID].size())
        {
            if (chordMapStrings[chordID][0] == '{')
            {
                outputFile
                   << "const mkuc " << prefix << "chord_" << chordID
                   << "[] PROGMEM = "
                   <<  chordMapStrings[chordID] << ";" << endl;
            }
            else
            {
                outputFile
                    << "const mkuc " << prefix << "chord_" << chordID
                    << "[] PROGMEM = "
                    << "{{0, " << chordMapStrings[chordID] << "}};" << endl;
            }
        }
        else if (chordMap[chordID].len)
        {
            outputFile
                << "const mkuc " << prefix << "chord_" << chordID
                << "[] PROGMEM = {";

            for (int j=0; j<chordMap[chordID].len; j++)
            {
                outputFile
                    << "{" << int(chordMap[chordID].codes[j].mod)  << ", "
                    << int(chordMap[chordID].codes[j].code) << "},";
            }

            outputFile
                << "};" << endl;
        }
    }

    outputFile
        << "const chord_codes "
        << prefix << "chord_map[" << n_chords << "] PROGMEM =" << endl
        << "{" << endl;

    for (int chordID=0; chordID<n_chords; chordID++)
    {
        if (chordMap[chordID].len)
        {
            outputFile
                << "    {" << int(chordMap[chordID].len) << ", "
                << prefix << "chord_" << chordID << "}," << endl;
        }
        else if (chordMapStrings[chordID].size())
        {
            outputFile
                << "    {1, " << prefix << "chord_" << chordID << "}," << endl;
        }
        else
        {
            outputFile
                << "    {0, 0}," << endl;
        }
    }

    outputFile<< "};" << endl;
}


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout<< "Wrong number of arguments" << endl;
        return 1;
    }

    convertFile
    (
        argv[1],
        "",
        N_CHORD_KEY_PAIRS,
        N_CHORDS
    );

    convertFile
    (
        argv[2],
        "mouse_",
        N_MOUSE_CHORD_KEY_PAIRS,
        N_MOUSE_CHORDS
    );

    return 0;
}
