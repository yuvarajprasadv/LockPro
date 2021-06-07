//
//  Tool.h
//  JJLock
//
//  Created by jae on 2/5/15.
//
//

#ifndef __Tool_H__
#define __Tool_H__

#include <Cocoa/Cocoa.h>
#include <QTextCodec>


class Tool
{
public:
    Tool(){}
    virtual ~Tool(){}
    
    ai::UnicodeString QStringToUnicodeString(const QString inputStr);
    QString UnicodeStringToQString(const ai::UnicodeString inputStr);
    ai::UnicodeString QStringToUnicodeString_UTF16(const QString inputStr);
    
    std::string IntToString(int num);
    int StringToInt(std::string str);
    
    int SpecialCharacterToAscii(const char specialChar);
    std::string ReplaceSpecialCharacterWithAsciiInString(std::string str);
    
    char AsciiToCharacter(int num);
    std::string ConvertBackAsciiToSpecialCharacterInString(std::string str);
    
    int HindiToNumeric(ai::UnicodeString hindi);
    ai::UnicodeString NumericToHindi(int numeric);
    
    bool substr(const char* str, unsigned start, unsigned end, char* substr);
    void moveVector(vector< vector <int> > *elementLengthVector, size_t index, size_t pos);

    void swapVector(vector<int> *vect, size_t index1, size_t index2);
    void swapVector(vector< vector< int > > *vect, size_t index1, size_t index2);
    
    void sortIndexArray(int *p, int start, int dest);
    void sortIndexVector(vector< int > *vect, int start, int dest);
    int *generateIndexArray(vector<int> *vect);
    long int currentTimeStamp();
    
    /**
     * Current Greenwich Mean Time stirng
     * @Return std::string of current time
     */
    std::string currentGMTime();
    
    std::vector<ai::UnicodeString> SplitTextInToParagraph(ai::UnicodeString linkedElementValue);
    static bool isIgnoreKeyPress(CGKeyCode keycode, unsigned int modifier);
    static bool isSpecialKeyCode(CGKeyCode keycode, unsigned int modifier = 0, ai::UnicodeString::UTF32TextChar *ucSplCase = 0);
    static void GetIndexOfModifiedElement(ai::UnicodeString textFrameContent, ai::UnicodeString dictItemContent, int *indexOfModifiedElement);
    static int getCountOfSplCaseInTagInfo(ai::UnicodeString textFrameContent, ai::UnicodeString tagContent, int startIndex);
    /*
     * find the Regular Expressions in the string
     * @Param:
     *      chars: target string
     *      pattern: regular expresion pattern
     *      start: the start position of the pattern in string
     *      end: the end position of the pattern in string
     * Return:
     *      return true if found otherwise return false
     */
    bool regexFind(const char *chars, const char *pattern, long *start, long *end);
    
    /*
     * delete the characters in the range of the string
     * @Param:
     *      chars: tartget string
     *      start: the start of range
     *      end: the end of range
     */
    void deleteCharactersAtRange(char *chars, long start, long end);
    

    bool compareArabicText(ai::UnicodeString content1, ai::UnicodeString content2);
};

#endif /* defined(__JJLock__Tool__) */
