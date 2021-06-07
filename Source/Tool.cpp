//
//  Tool.cpp
//  JJLock
//
//  Created by jae on 2/5/15.
//
//

#include <regex.h>
//#include <regex>

#include "Tool.h"

#define CARRIAGE_RETURN 0x0D
#define NEW_LINE 0x0A
#define END_OF_TEXT 0x03

#define KEY_UNKNOWN 0xFFFF
#define KEY_TAB 48
#define KEY_ENTER 36
#define MOD_CAPS 1024
#define MOD_CAPS_SHIFT 1536
#define MOD_SHIFT 512
#define MOD_NONE 0
#define CARRIAGE_RETURN 0x0D
#define END_OF_TEXT 0x03
#define HORIZONTAL_TAB 0X09
#define NEW_LINE 0x0A

#define UNDO_APP_PATH "open /Applications/undo.app"

ai::UnicodeString Tool::QStringToUnicodeString(const QString inputStr)
{
    std::string inputStdStr = inputStr.toStdString();
    AICharacterEncoding encoding = kAIUTF8CharacterEncoding;
    ai::UnicodeString outputStr = ai::UnicodeString(inputStdStr, encoding);
    return outputStr;
}

ai::UnicodeString Tool::QStringToUnicodeString_UTF16(const QString inputStr)
{
    std::string inputStdStr = inputStr.toStdString();
    AICharacterEncoding encoding = kAIUTF16CharacterEncoding;
    ai::UnicodeString outputStr = ai::UnicodeString(inputStdStr, encoding);
    return outputStr;
}

QString Tool::UnicodeStringToQString(const ai::UnicodeString inputStr)
{
    std::string inputStdStr = inputStr.as_UTF8();
    QString outputStr;
    outputStr = QString::fromStdString(inputStdStr);
    return outputStr;
}

std::string Tool::IntToString(int num)
{
    std::stringstream *strStream = new std::stringstream();
    *strStream << num;
    std::string str = strStream->str();
    delete strStream;
    return str;
}

int Tool::StringToInt(std::string str)
{
    int num = std::atoi(str.c_str());
    return num;
}

int Tool::SpecialCharacterToAscii(const char specialChar)
{
    int result = 0;
    if (!((specialChar >= 48 && specialChar <= 57) || (specialChar >= 65 && specialChar <= 90) || (specialChar >= 97 && specialChar <= 122))) {
        result = (int)specialChar;
    }
    return result;
}

std::string Tool::ReplaceSpecialCharacterWithAsciiInString(std::string str)
{
    const char *cstr = str.c_str();
    int i = 0;
    while (*cstr) {
        int ascii = 0;
        ascii = SpecialCharacterToAscii(*(cstr++));
        if (ascii) {
            std::string asciiString = IntToString(ascii);
            asciiString = "--" + asciiString + "--";
            unsigned int asciiStringLength = asciiString.length();
            str.replace(i, 1, asciiString);
            i += asciiStringLength;
        }
        else{
            i++;
        }
    }
    return str;
}

char Tool::AsciiToCharacter(int num)
{
    char *cstr = new char();
    *cstr = (char)num;
    return *cstr;
}

std::string Tool::ConvertBackAsciiToSpecialCharacterInString(std::string str)
{
    char pattern[100] = "--([0-9]{1,3})--";
    regex_t reg;
    regmatch_t pmatch[2];
    const size_t nmatch = 2;
    if (regcomp(&reg, pattern, REG_EXTENDED) == 0) {
        std::string strMatched = str;
        std::string *p;
        p = &strMatched;
        unsigned replacePos = 0;
        while (regexec(&reg, p->c_str(), nmatch, pmatch, 0) == 0) {
            unsigned pos = pmatch[0].rm_eo;
            if (replacePos == 0) {
                replacePos += pmatch[0].rm_so;
            }
            else{
                replacePos += 1 + pmatch[0].rm_so;
            }
            char numStr[20];
            if(substr(p->c_str(), pmatch[1].rm_so, pmatch[1].rm_eo, numStr)){
                std::string numString = (std::string) numStr;
                int num = StringToInt(numString);
                unsigned length = pmatch[0].rm_eo - pmatch[0].rm_so;
                char characters = AsciiToCharacter(num);
                str.replace(replacePos, length, &(characters));
                // Cut out the replaced part
                *p = p->substr(pos);
            }
        }
    }
    regfree(&reg);
    return str;
}

bool Tool::substr(const char* str, unsigned start, unsigned end, char* substr)
{
    try {
        unsigned length = end - start;
        if (length <= 0) {
            return false;
        }
        strncpy(substr, str + start, length);
        substr[length] = 0;
        return true;
    } catch (exception &ex) {
        
    }
    return false;
}

void Tool::moveVector(vector< vector<int> > *elementLengthVector, size_t index, size_t pos)
{
    if (index < pos) {
        elementLengthVector->insert(elementLengthVector->begin() + pos + 1, elementLengthVector->at(index));
    }
    else{
        elementLengthVector->insert(elementLengthVector->begin() + pos, elementLengthVector->at(index));
    }
    if (index > pos) {
        elementLengthVector->erase(elementLengthVector->begin() + index + 1);
    }
    else{
        elementLengthVector->erase(elementLengthVector->begin() + index);
    }
}

void Tool::swapVector(vector<int> *vect, size_t index1, size_t index2)
{
    int temp = vect->at(index1);
    (*vect)[index1] = vect->at(index2);
    (*vect)[index2] = temp;
}

void Tool::swapVector(vector<vector<int> > *vect, size_t index1, size_t index2)
{
    std::vector<int> temp = vect->at(index1);
    (*vect)[index1] = vect->at(index2);
    (*vect)[index2] = temp;
}

void Tool::sortIndexArray(int *p, int start, int dest)
{
    *(p + start) = dest;
    if (start > dest) {
        for(int i = *(p + dest); i < start; i++){
            *(p + i) += 1;
        }
    }
    if(start < dest) {
        for (int i = *(p + start + 1); i <= dest; i++) {
            *(p + i) -= 1;
        }
    }
}

void Tool::sortIndexVector(vector< int > *vect, int start, int dest)
{
    if (start < dest) {
        vect->insert(vect->begin() + dest + 1, vect->at(start));
    }
    else{
        vect->insert(vect->begin() + dest, vect->at(start));
    }
    if (start > dest) {
        vect->erase(vect->begin() + start + 1);
    }
    else{
        vect->erase(vect->begin() + start);
    }
    
}

int * Tool::generateIndexArray(vector<int> *vect)
{
    int *indexArray = new int[vect->size()];
    for (int i = 0; i < vect->size(); i++) {
        *(indexArray + vect->at(i)) = i;
    }
    
    return indexArray;
}

long int Tool::currentTimeStamp()
{
    time_t timeStamp;
    
    time(&timeStamp);
    
    return timeStamp;
}

std::string Tool::currentGMTime()
{
    time_t currentTime;
    struct tm *gmTime;
    
    time( &currentTime );
    gmTime = gmtime( &currentTime );
    
    int Day     = gmTime->tm_mday;
    int Month   = gmTime->tm_mon + 1;
    int Year    = gmTime->tm_year + 1900;
    
    int Hour    = gmTime->tm_hour;
    int Min     = gmTime->tm_min;
    int Sec     = gmTime->tm_sec;
    
    std::string currentTimeString = IntToString(Month) + "/" + IntToString(Day) + "/" +  IntToString(Year) + " "
    + IntToString(Hour) + ":" + IntToString(Min) + ":" + IntToString(Sec) + " GMT";
    
    return currentTimeString;
    
}

std::vector<ai::UnicodeString> Tool::SplitTextInToParagraph(ai::UnicodeString linkedElementValue){
    
    int selectedElementlength = linkedElementValue.length();
    int ucCharVlaue;
    std::vector<ai::UnicodeString> veclinkedElementValue;
    ai::UnicodeString singleLineLinkedElementValue;
    int startOffset = 0, length = 0;
    
    for (int index = 0; index < selectedElementlength; index ++) {
        
        if(selectedElementlength == (index + 1))
        {
            ucCharVlaue = NEW_LINE;//Assume if its end of char
            length = (index + 1) - startOffset;
        }
        else{
            ucCharVlaue = linkedElementValue.at(index);
            length = index - startOffset;
        }

        switch (ucCharVlaue) {
            case CARRIAGE_RETURN:
            case NEW_LINE:
            case END_OF_TEXT:
                
                singleLineLinkedElementValue = linkedElementValue.substr(startOffset, length/*length*/);
                veclinkedElementValue.push_back(singleLineLinkedElementValue);
                startOffset = index + 1;//Refer next line of first char
                break;
            default:
                break;
        }
    }
    return veclinkedElementValue;
}

bool Tool::isIgnoreKeyPress(CGKeyCode keycode, unsigned int modifier){
    
    switch((int) keycode){
        case  48: // return "<tab>";
            if(modifier == 0){//Need to chk caps lock
                return false;
            }
        //case  51: return "<delete>";
        case  53: // return "<escape>";
        case  55: // return "<command>";
        case  58: // return "<option>";
        case  59: // return "<control>";
        case  60: // return "<right-shift>";
        case  61: // return "<right-option>";
        case  62: // return "<right-control>";
        case  63: // return "<function>";
        case  64: // return "<f17>";
        case  72: // return "<volume-up>";
        case  73: // return "<volume-down>";
        case  74: // return "<mute>";
        case  79: // return "<f18>";
        case  80: // return "<f19>";
        case  90: // return "<f20>";
        case  96: // return "<f5>";
        case  97: // return "<f6>";
        case  98: // return "<f7>";
        case  99: // return "<f3>";
        case 100: // return "<f8>";
        case 101: // return "<f9>";
        case 103: // return "<f11>";
        case 105: // return "<f13>";
        case 106: // return "<f16>";
        case 107: // return "<f14>";
        case 109: // return "<f10>";
        case 111: // return "<f12>";
        case 113: // return "<f15>";
        case 114: // return "<help>";
        case 115: // return "<home>";
        case 116: // return "<pageup>";
        case 117: // return "<forward-delete>";
        case 118: // return "<f4>";
        case 119: // return "<end>";
        case 120: // return "<f2>";
        case 121: // return "<page-down>";
        case 122: // return "<f1>";
        case 123: // return "<left>";
        case 124: // return "<right>";
        case 125: // return "<down>";
        case 126: // return "<up>";
            return true;
    }
    
    return false;
}

bool Tool::isSpecialKeyCode(CGKeyCode keycode, unsigned int modifier, ai::UnicodeString::UTF32TextChar *ucSplCase){
    switch (keycode) {
        case KEY_TAB:
            if((modifier == MOD_NONE) || (modifier == MOD_CAPS)){
                *ucSplCase = HORIZONTAL_TAB;
                return true;
            }
            break;
        case KEY_ENTER:
            if((modifier == MOD_NONE) || (modifier == MOD_CAPS)){
                *ucSplCase = CARRIAGE_RETURN;
                return true;
            }
            
            if((modifier == MOD_SHIFT) || (modifier == MOD_CAPS_SHIFT)){
                *ucSplCase = END_OF_TEXT;
                return true;
            }
            
            break;
            
        default:
            break;
    }
    return false;
}

void Tool::GetIndexOfModifiedElement(ai::UnicodeString textFrameContent, ai::UnicodeString dictItemContent, int *indexOfModifiedElement)
{
    int textFramelength = textFrameContent.length();
    int ucValueOfFrameChar, ucValueOfItemChar;
    int frameIndex, itemIndex;
    
    for (frameIndex = 0, itemIndex = 0; frameIndex < textFramelength; frameIndex++, itemIndex++){
        
        ucValueOfFrameChar = textFrameContent.at(frameIndex);
        ucValueOfItemChar = dictItemContent.at(itemIndex);
        
        if(ucValueOfFrameChar != ucValueOfItemChar){
            if(!(ucValueOfFrameChar == CARRIAGE_RETURN && ucValueOfItemChar == NEW_LINE)){
                printf("GetIndex:\n%s\n", dictItemContent.as_UTF8().c_str());
                *indexOfModifiedElement = frameIndex;
                return;
            }
        }//End if
    }//End for
    
    if(textFramelength < dictItemContent.length()){
        *indexOfModifiedElement = -2;
    }
    else{
        *indexOfModifiedElement = -1;
    }
    
    return;
}

int Tool::getCountOfSplCaseInTagInfo(ai::UnicodeString textFrameContent, ai::UnicodeString tagContent, int startIndex){
    
    int ucValueOfFrameChar, ucValueOfItemChar;
    int frameIndex, itemIndex;
    int splCaseCount = 0;
    
    for (frameIndex = startIndex, itemIndex = 0; itemIndex < tagContent.length(); frameIndex++){
        
        ucValueOfFrameChar = textFrameContent.at(frameIndex);
        ucValueOfItemChar = tagContent.at(itemIndex);
        
        if(ucValueOfFrameChar != ucValueOfItemChar){
            switch(ucValueOfFrameChar){
                case CARRIAGE_RETURN:
                case NEW_LINE:
                case HORIZONTAL_TAB:
                case END_OF_TEXT:
                    splCaseCount ++;
                    break;
                default:
                    itemIndex++;
            }
        }//End if
        else{
            itemIndex ++;
        }
    }//End for
    
    return splCaseCount;
}

bool Tool::regexFind(const char *chars, const char *pattern, long *start, long *end)
{
    *start = 0;
    *end = 0;
    bool flag = false;
    
    regex_t regx;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    if (regcomp(&regx, pattern, REG_EXTENDED) == 0) {
        if (regexec(&regx, chars, nmatch, pmatch, 0) == 0) {
            *start = pmatch[0].rm_so;
            *end = pmatch[0].rm_eo;
            flag = true;
        }
    }
    
    regfree(&regx);
    return flag;
}

void Tool::deleteCharactersAtRange(char *chars, long start, long end)
{
    long rangeSize = end - start;
    for (long i = start; i < strlen(chars) - rangeSize ; ++i) {
        chars[i] = chars[i + rangeSize];
    }
}

bool Tool::compareArabicText(ai::UnicodeString content1, ai::UnicodeString content2)
{
    bool result = true;
    
    std::string contentString1 = content1.as_UTF8();
    std::string contentString2 = content2.as_UTF8();
    int length;
    if (contentString1.length() >= contentString2.length()) {
        length = contentString2.length();
    } else {
        length = contentString1.length();
    }
    int m = 0;
    int n = 0;
    for (int i = 0; i < length; i++) {
        char char1 = contentString1.at(i + m);
        char char2 = contentString2.at(i + n);
        char convert1;
        char convert2;
        
        if (char1 != char2) {
            if ((char1 >= 48 && char1 <= 57) || (char2 >= 48 && char2 <= 57)) {
                if (char1 == -39) {
                    char nextChar1 = contentString1.at(i + m + 1);
                    if (nextChar1 >= -96 && nextChar1 <= -87) {
                        convert1 = nextChar1 + 144;
                        m++;
                    } else {
                        result = false;
                        break;
                    }
                } else {
                    convert2 = char1;
                }
                if (char2 == -39) {
                    char nextChar2 = contentString2.at(i + n + 1);
                    if (nextChar2 >= -96 && nextChar2 <= -87) {
                        convert1 = nextChar2 + 144;
                        n++;
                    } else {
                        result = false;
                        break;
                    }
                } else {
                    convert2 = char2;
                }
                if (convert1 != convert2) {
                    result = false;
                    break;
                }
            } else {
                result = false;
                break;
            }
        }
    }
    return result;
}
