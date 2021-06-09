//
//  JJLock.hpp
//  J&J Lock
//
//  Created by Yuvaraj on 05/02/19.
//
//

#ifndef JJLock_hpp
#define JJLock_hpp

#include <stdio.h>
#include <qiterator.h>
#include <qset.h>
//#include <vector.h>
#include "JJLockPlugin.h"
#include "SDKErrors.h"

#include "AppContext.hpp"
#include "ArtSetHelper.h"
#include "JJLockPanel.hpp"
#include "IAIPaint.h"

#include "ATETypesDef.h"
#include "IText.h"
#include "ATETextSuitesExtern.h"

using namespace ATE;

extern JJLockPlugin* gPlugin;

class JJLock
{
public:
    
    bool IsDocumentOpened();
    AIArtHandle CreatePlacedArt(std::string iconFileName);
    AIArtHandle FindArtHandleUsingUUIDAndType(AIArtType artType, std::string key, ai::UnicodeString UUID);
    AIArtHandle GetLockIconHandle();
    AILayerHandle CreateLockParentLayer();
    
    void GetLockLayerHandle(AILayerHandle* lockLayer);
    void SetLockLayerEnable(bool boolLayer);
    void ClearBoundAndLockEntry(ai::UnicodeString lockBoundID, ai::UnicodeString lockIconID, bool isLockCountApplicable = false);
    bool ClearMissingArtLockAndBound(vector<ai::UnicodeString> sourceIDList, vector<ai::UnicodeString> checkIDListExists, ai::UnicodeString typeOfLock);
    void ClearLockEntry(AIArtHandle artHandle);
    void RecreateLockIconAndBound(AIArtHandle artHandle);
    void ApplyFillAndStrokeColors(AIArtHandle artBoundHandle);
    void GetCharacterColor(std::vector<AIFillStyle>* charFeatureColorArray, std::vector<int>* charColorFillBool, AIArtHandle textFrameArt);
    void GetCharacterStroke(std::vector<AIStrokeStyle>* charFeatureStrokeArray, std::vector<int>* charStrokeFillBool, AIArtHandle textFrameArt);
  //  void GetCharacterColor(std::vector<AIReal>* charFeatureColorArray, std::vector<int>* charColorFillBool, AIArtHandle textFrameArt);
    void CreateLockBound(AIArtHandle artHandle, AILayerHandle lockLayer, AIArtHandle placedArtIconHandle, std::string newPlacedFilePathString);
    void SetLayerTitleToHandle(AIArtHandle artHandle);
    void CreateOnlyIconAndLockBound(AIArtHandle artHandle, std::string newPlacedFilePathString);
    void TransformLockAndBound(AIArtHandle artHandle);
    void TransformMatrixForTextFrame(AIArtHandle textArtHandle, float scaleRatio, bool vDirection, bool hDirection);
    void TransformAdjustment(AIArtHandle transformHandle, AIRealRect parentBound, AIRealRect childBound, AIReal adjustmentOffset, AIReal pathStrokeWidth);
    void TransformLockArt(AIArtHandle parentHandle, AIArtHandle pathHandle, AIArtHandle iconHandle);
    void ReCreatePathHandle(AIArtHandle artHandle);
    void SetArrayEntryForFillColor(AIArtHandle artHandle);
    
    AIErr SetIntegerEntryToDocumentDict(std::string key, ai::int32 value);
    AIErr GetIntegerEntryToDocumentDict(std::string key, ai::int32* value);
    ASErr GetRealEntryFromDocumentDict(AIArtHandle artHandle, AIReal *realValue, std::string key);
    ASErr SetRealEntryToDocumentDict(AIArtHandle artHandle, AIReal realValue, std::string key);
    AIErr SetBooleanEntryToHandleDict(AIArtHandle artHandle, std::string key, AIBoolean value);
    AIErr GetUnicodeEntryFromDocumentDict(AIArtHandle artHandle, ai::UnicodeString *artContent, std::string key);
    AIErr SetUnicodeEntryToDocumentDict(AIArtHandle artHandle, ai::UnicodeString artContent, std::string key);
    AIErr DeleteDictionaryForAnEntry(AIArtHandle artHandle, std::string key);
    AIErr DeleteDictionaryUsingIterator(AIArtHandle artHandle);
    AIErr DeleteDictionaryUsingIteratorCheck(AIArtHandle artHandle, QStringList dictKeyList);
    
    AIErr SetUnicodeStringEntryToHandleDict(AIArtHandle artHandle, std::string key, ai::UnicodeString value);
    ASErr CreatePlacedLinkFile(ai::FilePath& file, AIArtHandle& art);
    ASErr SetArrayEntryForBoolean(AIArtHandle artHandle, std::string key, std::vector<bool> boolArray);
    AIErr GetArrayEntryForBoolean(AIArtHandle artHandle, std::string key, std::vector<bool>* boolArray);
    ASErr SetArrayEntryForInteger(AIArtHandle artHandle, std::string key, std::vector<int> intArray);
    AIErr GetArrayEntryForInteger(AIArtHandle artHandle, std::string key, std::vector<int>* intArray);
    ASErr SetArrayEntryForReal(AIArtHandle artHandle, std::string key, std::vector<AIReal> realArray);
    AIErr GetArrayEntryForReal(AIArtHandle artHandle, std::string key, std::vector<AIReal>* realArray);
    ASErr SetArrayEntryForStrokeStyle(AIArtHandle artHandle, std::string key, std::vector<AIStrokeStyle> fillStyleArray);
    ASErr GetArrayEntryForStrokeStyle(AIArtHandle artHandle, std::string key, std::vector<AIStrokeStyle>* fillStyleArray);
    ASErr SetArrayEntryForFillStyle(AIArtHandle artHandle, std::string key, std::vector<AIFillStyle> fillStyleArray);
    ASErr GetArrayEntryForFillStyle(AIArtHandle artHandle, std::string key, std::vector<AIFillStyle>* fillStyleArray);
    ASErr SetArrayEntryForRealPoint(AIArtHandle artHandle, std::string key, std::vector<AIRealPoint> realPointArray);
    ASErr GetArrayEntryForRealPoint(AIArtHandle artHandle, std::string key, std::vector<AIRealPoint>* realPointArray);
    AIErr SetIntegerEntryToHandleDict(AIArtHandle artHandle, std::string key, int value);
    AIErr GetArrayEntryForUnicodeString(AIArtHandle artHandle, std::string key, std::vector<ai::UnicodeString>* unicodeStringArray);
    AIErr SetArrayEntryForUnicodeString(AIArtHandle artHandle, std::string key, std::vector<ai::UnicodeString> unicodeStringArray);
    
    int GetIntegerEntryFromHandleDict(AIArtHandle artHandle, std::string key);
    ai::UnicodeString GetUnicodeStringEntryFromHandleDict(AIArtHandle artHandle, std::string key);
    ai::UnicodeString GetTextFromLinkedTextFrame(AIArtHandle artHandle);
    ai::UnicodeString GetText(AIArtHandle textFrameArt);
    AIBoolean GetBooleanEntryFromHandleDict(AIArtHandle artHandle, std::string key);
    AIRealRect SetArtHandleBoundInDict(AIArtHandle artHandle);
    ASErr GetSelectedTextFrame(AIArtHandle &frameArt);
    std::vector<AIArtHandle> GetTextFramesFromLinkedTextFrame(AIArtHandle textFrameArt);
    AIArtHandle GetParentTextFrameFromLinkedTextFrame(AIArtHandle textFrameArt);
    
    AIErr GetParagraphJustification(AIArtHandle textArtHandle, ai::int32* getJustifyValue);
    AIErr SetParagraphJustification(AIArtHandle textArtHandle, ATE::ParagraphJustification setValue);
    void GetCharacterFontProperty(std::vector<ai::UnicodeString>* charfontStyleNameArray, std::vector<ai::UnicodeString>* charfontFamilyNameArray, std::vector<double>* charFontSizeArray, AIArtHandle textFrameArt);
    void GetCharacterProperty(std::vector<ai::UnicodeString>* charfontStyleNameArray, std::vector<ai::UnicodeString>* charfontFamilyNameArray, std::vector<double>* charFontSizeArray, std::vector<double>* charHScaleSizeArray, AIArtHandle textFrameArt);
    void ReplaceFont(AIArtHandle artHandle, int i, ai::UnicodeString charfontFamilyName, ai::UnicodeString charfontStyleName);
 //   void ReplaceFont(AIArtHandle artHandle, int i, ai::UnicodeString charfontFamilyName, ai::UnicodeString charfontStyleName, AIReal charFontSize);
    void ReplaceFont(AIArtHandle artHandle, int i, ai::UnicodeString charfontFamilyName, ai::UnicodeString charfontStyleName, AIReal charFontSize, AIReal charHScaleSize = 0.0);
    bool IsAnyArtSelected(AIArtHandle recreateArtHandle);
    void CreateLockIconSwatch();
    
    bool IsKeyStringExists(std::string keyString);
    void FetchAllDictionaryUsingIterator(AIArtHandle artHandle);
    
    AIErr SetArrayEntryForCharFeature(AIArtHandle artHandle, std::string key, std::vector<ATE::ICharFeatures> unicodeStringArray);
    
    void ParseGroupChildForRemoveDictEntriesOrSetForPlock(AIArtHandle groupArtHandle, bool remove);
    void ClearDictEntries(AIArtHandle artHandle);
    void ParseGroupChildArtandClearLock(AIArtHandle groupArtHandle);
    
    void GetTextFrameFillAndStrokeColour(std::vector<AIStrokeStyle>* textFrameObjectStrokeStyle, std::vector<int>* textFrameStrokeVisibleBool, std::vector<AIFillStyle>* textFrameObjectFillStyle, std::vector<int>* textFrameFillVisibleBool, AIArtHandle textFrameArt);
    
    void ApplyFillColorForTextFrame(AIArtHandle textFrameArt, AIFillStyle textFrameFillStyle, AIStrokeStyle textFrameStrokeStyle, AIBoolean fillVisible, AIBoolean strokeVisible);
    bool DeSelectAll();

};

#endif /* JJLock_hpp */
