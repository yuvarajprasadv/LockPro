//
//  TextHelper.h
//  Battu
//
//  Created by jae on 1/15/15.
//
//

#ifndef __Battu__TextHelper__
#define __Battu__TextHelper__

class TextHelper
{
public:
    TextHelper(){}
    virtual ~TextHelper(){}
    
    ai::UnicodeString getText(AIArtHandle textFrameArt);
    ai::UnicodeString GetTextFromLinkedTextFrame(AIArtHandle textFrameArt);
    // //ai::UnicodeString GetTextFromSelectedTextFrame(AIArtHandle textFrameArt);
    ai::UnicodeString getTextWithinTextFrame(AIArtHandle textFrameArt);
    ai::int32 GetNumTextFramesFromLinkedTextFrameCount(AIArtHandle textFrameArt);
    ai::UnicodeString GetNumTextFramesFromLinkedTextFrame(AIArtHandle textFrameArt);
    ai::UnicodeString getTextWithinSelectedTextFrame(AIArtHandle textFrameArt);
    // //ai::UnicodeString::UTF32TextChar GetFirstCharFromTextFrame(AIArtHandle textFrameArt);
    // //ai::UnicodeString::UTF32TextChar GetLastCharFromTextFrame(AIArtHandle textFrameArt);
    // //ai::int32 GetNumTextFramesFromSelectedTextFrame(AIArtHandle textFrameArt);
    // //AIArtHandle * GetTextFramesFromSelectedTextFrame(AIArtHandle textFrameArt, ai::int32 * frameCounter);
    /* get the text content from text frame art at the Range(start, end)
     textFrameArt: text frame art
     start: start of range
     end: end of range
     outOfRange: whether the ranage(start, end) is beyond of the all range of the text frame art. true means out of range, flase means ok
     */
    ai::UnicodeString GetTextFromTextFrameArtAtRange(AIArtHandle textFrameArt, int start, int end, bool &outOfRange);
    ai::UnicodeString GetTextFromThreadedTextFrameArtAtRange(AIArtHandle textFrameArt, int start, int end, bool &outOfRange);
    // //ASErr InsertText(ai::UnicodeString content);
    // //ASErr ReplaceText(ai::UnicodeString content);
    // //ASErr FillText(ai::UnicodeString content);
    // //ASErr AppendText(ai::UnicodeString content);
    // //ai::UnicodeString FindAndReplaceForSplCase(ai::UnicodeString str, int *count);
    
    /** Find the findStr in str and replace it with replaceStr
     @Param
     str: target string
     findStr: which string to find
     repalceStr: which string to replace with
     flag: is the findStr found in str
     @Return
     result string of replacing the findStr with replaceStr in str
     */
    ai::UnicodeString FindAndReplace(ai::UnicodeString str, ai::UnicodeString findStr, ai::UnicodeString replaceStr, bool &flag);
    
    /* Find the findStr in str and delete it
     @Param str: target string
     @Param findStr: which string to find
     @Param flag: whether the findStr found in the str
     @Return: result string of deleting the findStr in str
     */
    ai::UnicodeString FindAndDelete(ai::UnicodeString str, ai::UnicodeString findStr, bool &flag);
    
    // //ai::int32 GetEndPointOfTextFrame(AIArtHandle textFrameArt);
    
    // //ASErr FillTextToTextFrameArt(AIArtHandle textFrameArt, ai::UnicodeString content);
    // //AIErr AppendTextToTextFrameArt(AIArtHandle textFrameArt, ai::UnicodeString content);
    
    /* Insert the contents into the textFrameArt at the the pos
     @Param textFrameArt: Art Handle of the textframe
     @Param content: the text content you want to insert
     @Param pos: the postion to insert the content
     */
    // //AIErr InsertTextOnTextFrameArtAtPos(AIArtHandle textFrameArt, ai::UnicodeString content, int pos);
    
    /*
     * Insert the character at the cursor
     * */
    // //bool InsertTextAtCursor(ai::UnicodeString content);
    
    /** Replace the contents of text range(start end) at textFrameArt
     @Param textFrameArt: Art Handle of the textframe
     @Param content: the text content you want to replace with
     @Param start: the start of text range
     @Param end: the end of text range
     @Param outOfRange: whether the text range is out of the range of the textframe
     */
    // //AIErr ReplaceTextOnTextFrameArtAtRange(AIArtHandle textFrameArt, ai::UnicodeString content, int start, int end, bool &outOfRange);
    
    /*
     * delete the character before the cursor
     * */
    // //bool DeleteCharacterAtCursor();
    
    // //AIErr DeleteTextOnTextFrameArtAtRange(AIArtHandle textFrameArt, int start, int end);
    
    // Move the text of the text range(start, end) to the pos on the given text frame art
    // // AIErr MoveTextOfRangeOnTextFrameArtToPos(AIArtHandle textFrameArt, ai::int32 start, ai::int32 end, ai::int32 pos, ai::UnicodeString separator = ai::UnicodeString(""));
    
    // Swap text range 1(start1, end1) with text range2(start2, end2)
    // //AIErr SwapTextRanges(AIArtHandle textFrameArt, ai::int32 start1, ai::int32 end1, ai::int32 start2, ai::int32 end2);
    
    ai::UnicodeString ConvertToEscapeCharacter(ai::UnicodeString str, int *count);
    
    AIErr SetFontOnTextFrameAtRange(AIArtHandle textFrameArt, ATE::IFont font, ai::int32 start, ai::int32 end);
    //std::vector<std::string> &split(const std::string &s, std::string delim, std::vector<std::string> &elems);
    std::vector<string> split(const std::string &s, const std::string &delim, const bool keep_empty);
    std::string join(std::vector<string> strList, const std::string delim);
    
    /* remove duplicate spaces(more than one continuous spaces) in the string
     @Param string: target string
     @Param flag: whether has duplicate spaces;
     @Return string that all duplicated spaces has been removed
     */
    ai::UnicodeString removeDuplicateSpaces(ai::UnicodeString string, bool *flag);
    
    /* remove paragraphs in the string
     @Param string: target string
     @Param flag: whether has paragraphs;
     @Return string that all paragraphs has been removed
     */
    ai::UnicodeString removeParagraphs(ai::UnicodeString string, bool *flag);
    
    /**
     * replace windows newline "\r\n" and "\n" with unix newline "\r"
     * @Param string: target string
     * @Return result string
     */
    ai::UnicodeString dos2unix(ai::UnicodeString string);
    
    /**
     * get size of text frame
     * @Param artHandle: art handle of the text frame
     * @Return int32 that the size of the text frame
     */
    ai::int32 getSizeOfTextFrameArt(AIArtHandle artHandle);
    
    AIArtHandle getParentTextFrameFromLinkedTextFrame(AIArtHandle textFrameArt);
    
};
#endif /* defined(__Battu__TextHelper__) */
