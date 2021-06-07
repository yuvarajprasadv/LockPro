//
//  TextHelper.cpp
//  Battu
//
//  Created by jae on 1/15/15.
//
//

#include <regex.h>

#include "IllustratorSDK.h"
#include "SDKErrors.h"
#include "ATETypesDef.h"

#include "IText.h"
#include "IThrowException.h"
#include "ATETextSuitesExtern.h"

#include "JJLockSuites.h"
#include "TextHelper.h"
#include "JJLockID.h"
#include "SelectionHelper.h"

#include "Tool.h"

#include "ArtSetHelper.h"
using namespace ATE;


// debug mode
#define debugMode 1

/*
 Get text from AI text frame
 */
ai::UnicodeString TextHelper::getText(AIArtHandle textFrameArt)
{
    AIErr result = kNoErr;
    ai::UnicodeString unicodeContent;
    TextRangeRef allTextRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &allTextRangeRef);
    ITextRange allTextRange(allTextRangeRef);
    //allTextRange.Remove();
    
    // Get contents from the text range
    ASInt32 rangeSize = allTextRange.GetSize();
    ai::AutoBuffer<ASUnicode> content(rangeSize);
    allTextRange.GetContents(content, rangeSize);
    if (content.IsValid()) {
        //ai::UnicodeString contentsAsUnicode(content, content.GetCount());
        unicodeContent = ai::UnicodeString(content, content.GetCount());
        //sAIUser->MessageAlert(unicodeContent);
    }
    return unicodeContent;
}

/*
 Get text within the text frame boundaries
 */
ai::UnicodeString TextHelper::getTextWithinTextFrame(AIArtHandle textFrameArt)
{
    
    AIErr result = kNoErr;
    ai::UnicodeString unicodeContent;
    
    TextFrameRef textFrameRef = NULL;
    result = sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
    aisdk::check_ai_error(result);
    ITextFrame textFrame(textFrameRef);
    ITextRange textRange = textFrame.GetTextRange(false);
    
    // Get contents from the text range
    ASInt32 rangeSize = textRange.GetSize();
    ai::AutoBuffer<ASUnicode> content(rangeSize);
    textRange.GetContents(content, rangeSize);
    if (content.IsValid()) {
        unicodeContent = ai::UnicodeString(content, content.GetCount());
    }
    //if (debugMode) { printf("getTextWithinTextFrame:: unicodeContent = \"%s\"\n", unicodeContent.as_UTF8().c_str()); }
    //}
    return unicodeContent;
}


/*
 Get number of linked text frames
 */
ai::int32 TextHelper::GetNumTextFramesFromLinkedTextFrameCount(AIArtHandle textFrameArt) {
    
    ai::int32 frameCount = 0;
    TextFrameRef textFrameRef = NULL;
    sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
    ITextFrame textFrame(textFrameRef);
    // Get story from linked text frame
    ITextRange textRange = textFrame.GetTextRange();
    IStory story = textRange.GetStory();
    ITextRange storyRange = story.GetTextRange();
    ITextFramesIterator framesIter = storyRange.GetTextFramesIterator();
    if (!framesIter.IsEmpty()) {
        while (framesIter.IsNotDone()) {
            frameCount++;
            framesIter.Next();
        } // while loop
    }
    else
    {
        aisdk::check_ai_error(kBadParameterErr);
    }
    
    return frameCount;
}



/*
 Get number of linked text frames
 */
//ai::int32 TextHelper::GetNumTextFramesFromLinkedTextFrame(AIArtHandle textFrameArt)
ai::UnicodeString TextHelper::GetNumTextFramesFromLinkedTextFrame(AIArtHandle textFrameArt)
{
    
    ai::int32 frameCount = 0;
    TextHelper textEditor;
    Tool tool;
    ai::UnicodeString contentStory;
    
    ai::UnicodeString textContent;
    TextFrameRef textFrameRef = NULL;
    sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
    ITextFrame textFrame(textFrameRef);
    // Get story from linked text frame
    ITextRange textRange = textFrame.GetTextRange();
    //printf("\n%d\n",textRange.GetStart());
    //printf("\n%d\n",textRange.GetEnd());
    
    bool outOfRange = false;
    
    //printf("\n %d \n",RangeStart);
    
    //printf("\n %d \n",RangeEnd);
    
    
    /////////////////////////////////////////////////////////////////
    contentStory = textEditor.GetTextFromTextFrameArtAtRange(textFrameArt, textRange.GetStart(), textRange.GetEnd() , outOfRange);
    //sAIUser->MessageAlert(contentStory);
    /////////////////////////////////////////////////////////////////
    IStory story = textRange.GetStory();
    ITextRange storyRange = story.GetTextRange();
    ai::AutoBuffer<ASUnicode> contents(storyRange.GetSize());
    storyRange.GetContents(contents, storyRange.GetSize());
    storyRange.GetTextSelection();
    int startrange = storyRange.GetStart();
    int endrange = storyRange.GetEnd();
    
    //printf("\n%d\n",startrange);
    //printf("\n%d\n",endrange);
    
    TextRangesRef ateTextRef;
    sAITextFrame->GetATETextSelection(textFrameArt, &ateTextRef);
    sAIDocument->GetTextSelection(&ateTextRef);
    ITextRanges ranges(ateTextRef);
    ITextRange ateRangeStart = ranges.GetFirst();
    ITextRange ateRangeEnd = ranges.GetLast();
    
    ASInt32 RangeStart2 = ateRangeStart.GetStart();
    ASInt32 RangeEnd2 = ateRangeEnd.GetEnd();
    
    
    //printf("\n%d\n",RangeStart2);
    //printf("\n%d\n",RangeEnd2);
    
    outOfRange = true;
    ai::UnicodeString contentStory2 = textEditor.GetTextFromTextFrameArtAtRange(textFrameArt, RangeStart2, RangeEnd2 , outOfRange);
    //sAIUser->MessageAlert(contentStory2);
    
    
    
    //TextRangesRef rangesRef = NULL;
    //ASErr result = sAIDocument->GetTextSelection(&rangesRef);
    //aisdk::check_ai_error(result);
    //ITextRanges ranges(rangesRef);
    //int startrange1 = ranges.GetStart();
    //int endrange1 = storyRange.GetEnd();
    
    
    textContent = (ai::UnicodeString(contents, contents.GetCount()));
    
    ITextFramesIterator framesIter = storyRange.GetTextFramesIterator();
    if (!framesIter.IsEmpty()) {
        while (framesIter.IsNotDone()) {
            frameCount++;
            framesIter.Next();
        } // while loop
    }
    else
    {
        aisdk::check_ai_error(kBadParameterErr);
    }
    return textContent;
    
    //return frameCount;
}

/*
 Get text from the text frame art at the text range
 */
ai::UnicodeString TextHelper::GetTextFromTextFrameArtAtRange(AIArtHandle textFrameArt, int start, int end, bool &outOfRange)
{
    AIErr result = kNoErr;
    ai::UnicodeString content;
    outOfRange = false;
    try{
        ai::UnicodeString textContent;
        ai::int32 textFrameStart;
        ai::int32 textFrameEnd;
        if (!textFrameArt) {
            TextFrameRef textFrameRef = NULL;
            result = sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
            aisdk::check_ai_error(result);
            ITextFrame textFrame(textFrameRef);
            // Get story from selected text frame
            ITextRange textRange = textFrame.GetTextRange();
            IStory story = textRange.GetStory();
            ITextRange storyRange = story.GetTextRange();
            ai::AutoBuffer<ASUnicode> contents(storyRange.GetSize());
            storyRange.GetContents(contents, storyRange.GetSize());
            textContent = (ai::UnicodeString(contents, contents.GetCount()));
            
            // the last line break character should be excluded (not part of the added text)
            if (textContent.length() > 0) {
                if (textContent.at(textContent.length() - 1) == 13) {
                    textContent = textContent.erase(textContent.length() - 1, 1);
                }
            }
            
            textFrameStart = 0;
            textFrameEnd = textContent.length();
            
            if (textFrameStart > start) {
                outOfRange = true;
                return content;
            }
            if (textFrameEnd < end) {
                outOfRange = true;
                return content;
            }
            
            for (int i = start; i < end; i++) {
                content.append(ai::UnicodeString(1, textContent.at(i)));
            }
            if (debugMode) { printf("GetTextFromSelectedTextFrameArtAtRange:: start = %d, end = %d, Return content =\"%s\"\n", start, end, content.as_UTF8().c_str()); }
            
        } else {
            TextRangeRef textRangeRef;
            result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
            aisdk::check_ai_error(result);
            
            ITextRange textRange(textRangeRef);
            
            ai::int32 textFrameStart =  textRange.GetStart();
            ai::int32 textFrameEnd =  textRange.GetEnd();
            if (textFrameStart > start) {
                outOfRange = true;
                return content;
            }
            if (textFrameEnd < end) {
                outOfRange = true;
                return content;
            }
            
            textRange.SetRange(start, end);
            if (textRange.GetSize() > 0) {
                // Get contents from the text range
                ASInt32 rangeSize = textRange.GetSize();
                ai::AutoBuffer<ASUnicode> value(rangeSize);
                textRange.GetContents(value, rangeSize);
                if (value.IsValid()) {
                    content = ai::UnicodeString(value, value.GetCount());
                    //sAIUser->MessageAlert(ai::UnicodeString("contents of text range:\n").append(content));
                }
            }
        }
    }
    catch (ai::Error &ex) {
        result = ex;
    }
    catch (ATE::Exception &ex) {
        result = ex.error;
    }
    return content;
}
/*
 Get text from the text frame art at the text range
 */
ai::UnicodeString TextHelper::GetTextFromThreadedTextFrameArtAtRange(AIArtHandle textFrameArt, int start, int end, bool &outOfRange)
{
    AIErr result = kNoErr;
    ai::UnicodeString content;
    outOfRange = false;
    try{
        ai::UnicodeString textContent;
        ai::int32 textFrameStart;
        ai::int32 textFrameEnd;
        if (!textFrameArt) {
            TextFrameRef textFrameRef = NULL;
            result = sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
            aisdk::check_ai_error(result);
            ITextFrame textFrame(textFrameRef);
            // Get story from selected text frame
            ITextRange textRange = textFrame.GetTextRange();
            IStory story = textRange.GetStory();
            ITextRange storyRange = story.GetTextRange();
            ai::AutoBuffer<ASUnicode> contents(storyRange.GetSize());
            storyRange.GetContents(contents, storyRange.GetSize());
            textContent = (ai::UnicodeString(contents, contents.GetCount()));
            
            // the last line break character should be excluded (not part of the added text)
            if (textContent.length() > 0) {
                if (textContent.at(textContent.length() - 1) == 13) {
                    textContent = textContent.erase(textContent.length() - 1, 1);
                }
            }
            
            textFrameStart = 0;
            textFrameEnd = textContent.length();
            
            if (textFrameStart > start) {
                outOfRange = true;
                return content;
            }
            if (textFrameEnd < end) {
                outOfRange = true;
                return content;
            }
            
            for (int i = start; i < end; i++) {
                content.append(ai::UnicodeString(1, textContent.at(i)));
            }
            if (debugMode) { printf("GetTextFromSelectedTextFrameArtAtRange:: start = %d, end = %d, Return content =\"%s\"\n", start, end, content.as_UTF8().c_str()); }
            
        } else {
            TextRangeRef textRangeRef;
            result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
            aisdk::check_ai_error(result);
            
            ITextRange textRange(textRangeRef);
            
            //ai::int32 textFrameStart =  textRange.GetStart();
            //ai::int32 textFrameEnd =  textRange.GetEnd();
            /* if (textFrameStart > start) {
             outOfRange = true;
             return content;
             }
             if (textFrameEnd < end) {
             outOfRange = true;
             return content;
             } */
            
            textRange.SetRange(start, end);
            if (textRange.GetSize() > 0) {
                // Get contents from the text range
                ASInt32 rangeSize = textRange.GetSize();
                ai::AutoBuffer<ASUnicode> value(rangeSize);
                textRange.GetContents(value, rangeSize);
                if (value.IsValid()) {
                    content = ai::UnicodeString(value, value.GetCount());
                    //sAIUser->MessageAlert(ai::UnicodeString("contents of text range:\n").append(content));
                }
            }
        }
    }
    catch (ai::Error &ex) {
        result = ex;
    }
    catch (ATE::Exception &ex) {
        result = ex.error;
    }
    return content;
}
/*
 Get text from linked text frame art
 */
ai::UnicodeString TextHelper::GetTextFromLinkedTextFrame(AIArtHandle textFrameArt)
{
    ASErr result = kNoErr;
    ai::UnicodeString textContent;
    try {
        TextFrameRef textFrameRef = NULL;
        result = sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
        aisdk::check_ai_error(result);
        ITextFrame textFrame(textFrameRef);
        // Get story from linked text frame
        ITextRange textRange = textFrame.GetTextRange();
        IStory story = textRange.GetStory();
        ITextRange storyRange = story.GetTextRange();
        ai::AutoBuffer<ASUnicode> contents(storyRange.GetSize());
        storyRange.GetContents(contents, storyRange.GetSize());
        textContent = (ai::UnicodeString(contents, contents.GetCount()));
        
        // the last line break character should be excluded (not part of the added text)
        if (textContent.at(textContent.length() - 1) == 13) {
            textContent = textContent.erase(textContent.length() - 1, 1);
        }
        
        //if (debugMode) { printf("GetTextFromLinkedTextFrame:: textContent = \"%s\"\n", textContent.as_UTF8().c_str()); }
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    catch (ATE::Exception& ex) {
        result = ex.error;
    }
    return textContent;
}
ai::UnicodeString TextHelper::ConvertToEscapeCharacter(ai::UnicodeString str, int *count)
{
    TextHelper textEditor;
    *count = 0;
    ai::UnicodeString result = str;
    bool flag = true;
    while (flag) {
        result = textEditor.FindAndReplace(result, ai::UnicodeString("^L"), ai::UnicodeString(1, characterCodeEndOfText), flag);
        if (flag) {
            (*count)++;
        }
    }
    flag = true;
    while (flag) {
        result = textEditor.FindAndReplace(result, ai::UnicodeString("^P"), ai::UnicodeString("\r"), flag);
        if (flag) {
            (*count)++;
        }
    }
    flag = true;
    while (flag) {
        result = textEditor.FindAndReplace(result, ai::UnicodeString("^T"), ai::UnicodeString("\t"), flag);
        if (flag) {
            (*count)++;
        }
    }
    return result;
}
ai::UnicodeString TextHelper::FindAndReplace(ai::UnicodeString str, ai::UnicodeString findStr, ai::UnicodeString replaceStr, bool &flag)
{
    ASErr result = kNoErr;
    ai::UnicodeString resultStr = str;
    flag = false;
    try {
        /*std::string contentsStr = contents.as_UTF8();
         char pattern[20] = "\\\r";
         regex_t reg;
         regmatch_t pmatch[2];
         const size_t nmatch = 2;
         if (regcomp(&reg, pattern, REG_EXTENDED) == 0) {
         std::string strMatched = *str;
         std::string *p;
         p = &strMatched;
         unsigned replacePos = 0;
         while (regexec(&reg, p->c_str(), nmatch, pmatch, 0) == 0) {
         }
         }*/
        ai::UnicodeString::size_type findStrLength = findStr.length();
        //ai::UnicodeString::size_type replaceStrLength = replaceStr.length();
        ai::UnicodeString::size_type pos = str.find(findStr);
        if (pos != ai::UnicodeString::npos ) {
            resultStr = str.replace(pos, findStrLength, replaceStr);
            flag = true;
        }
        
    } catch (ai::Error &ex) {
        result = ex;
    }
    catch (ATE::Exception &ex) {
        result = ex.error;
    }
    return resultStr;
}
ai::UnicodeString TextHelper::removeDuplicateSpaces(ai::UnicodeString string, bool *flag)
{
    Tool tool;
    
    *flag = false;
    bool isMatched = false;
    const char *pattern = "( ){2,}";
    
    std::string str = string.as_UTF8();
    const char *chars = str.c_str();
    long contentSpaceStart = 0, contentSpaceEnd = 0;
    
    isMatched = tool.regexFind(chars, pattern, &contentSpaceStart, &contentSpaceEnd);
    if (isMatched) {
        *flag = true;
    }
    
    while (isMatched) {
        isMatched = false;
        string.erase(contentSpaceStart, contentSpaceEnd - contentSpaceStart - 1);
        str = string.as_UTF8();
        const char *chars = str.c_str();
        
        isMatched = tool.regexFind(chars, pattern, &contentSpaceStart, &contentSpaceEnd);
    }
    
    return string;
}

ai::UnicodeString TextHelper::removeParagraphs(ai::UnicodeString string, bool *flag)
{
    ai::UnicodeString findStr = ai::UnicodeString("\r");
    *flag = false;
    bool isMatched = false;
    
    string =  FindAndDelete(string, findStr, isMatched);
    if (isMatched) {
        *flag = true;
    }
    while (isMatched) {
        string = FindAndDelete(string, findStr, isMatched);
    }
    
    findStr = ai::UnicodeString("\n");
    string = FindAndDelete(string, findStr, isMatched);
    if (isMatched) {
        *flag = true;
    }
    while (isMatched) {
        string = FindAndDelete(string, findStr, isMatched);
    }
    
    return string;
}

ai::UnicodeString TextHelper::FindAndDelete(ai::UnicodeString str, ai::UnicodeString findStr, bool &flag)
{
    ASErr result = kNoError;
    ai::UnicodeString resultStr = str;
    flag = false;
    
    try {
        ai::UnicodeString::size_type findStrLength = findStr.length();
        ai::UnicodeString::size_type pos = str.find(findStr);
        if (pos != ai::UnicodeString::npos) {
            resultStr = str.erase(pos, findStrLength);
            flag = true;
        }
    } catch (ai::Error &ex) {
        result = ex;
    }
    catch (ATE::Exception &ex) {
        result = ex.error;
    }
    return resultStr;
}

ai::UnicodeString TextHelper::dos2unix(ai::UnicodeString string)
{
    ai::UnicodeString cr = ai::UnicodeString(1, characterCodeCR);
    ai::UnicodeString lf = ai::UnicodeString(1, characterCodeLF);
    ai::UnicodeString windowsNewline = cr;
    windowsNewline += lf;
    bool foundFlag = true;
    
    while (foundFlag) {
        string = FindAndReplace(string, windowsNewline, cr, foundFlag);
    }
    
    do {
        string = FindAndReplace(string, lf, cr, foundFlag);
    } while (foundFlag);
    
    return string;
}
ai::int32 TextHelper::getSizeOfTextFrameArt(AIArtHandle artHandle)
{
    ai::int32 size = 0;
    ASErr result = kNoErr;
    try {
        TextRangeRef allTextRangeRef;
        result = sAITextFrame->GetATETextRange(artHandle, &allTextRangeRef);
        ITextRange allTextRange(allTextRangeRef);
        
        size = allTextRange.GetSize();
    }
    catch (ai::Error &ex) {
        result = ex;
    }
    catch (ATE::Exception &ex) {
        result = ex.error;
    }
    return size;
}

AIArtHandle TextHelper::getParentTextFrameFromLinkedTextFrame(AIArtHandle textFrameArt) {
    AIErr result = kNoErr;
    AIBool8 linkedText;
    sAITextFrame->PartOfLinkedText(textFrameArt, &linkedText);
    AIArtHandle artHandle = NULL;
    if (linkedText) {
        TextFrameRef textFrameRef = NULL;
        result = sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
        aisdk::check_ai_error(result);
        
        ITextFrame textFrame(textFrameRef);
        ITextRange textRange = textFrame.GetTextRange();
        IStory story = textRange.GetStory();
        ITextRange storyRange = story.GetTextRange();
        ITextFramesIterator framesIter = storyRange.GetTextFramesIterator();
        if (!framesIter.IsEmpty()) {
            ITextFrame textFrame = framesIter.Item();
            TextFrameRef textFrameIterRef = textFrame.GetRef();
            result = sAITextFrame->GetAITextFrame(textFrameIterRef, &artHandle);
            aisdk::check_ai_error(result);
        } // !framesIter.IsEmpty()
    } // linkedText
    return artHandle;
} // TextHelper::getParentTextFrameFromLinkedTextFrame

