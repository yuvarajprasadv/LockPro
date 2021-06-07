
//
//  JJLockPanel.cpp
//  JJLock
//
//  Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 31/05/2021
//

#include "JJLockPanel.hpp"
#include "AppContext.hpp"
#include "SDKErrors.h"
#include "IText.h"
#include "AIDictionary.h"
#include "AIUID.h"
#include "IAIFilePath.hpp"
#include "AIATETextUtil.h"
#include "AIArtboard.h"
#include "AIUndo.h"
#include "IllustratorSDK.h"
#include <Quuid.h>
#include <QObject>
#include <QFileInfo>

using namespace ATE;

JJLock* jjLock;

JJLockPanel::JJLockPanel(QWidget *parent) : JJLockUI(parent)
{
    
}
JJLockPanel::~JJLockPanel()
{

}

QStringList keyStringListToClear = {"clock", "plock", "pclock", "layerName", "JJLockArtBoundBottom", "JJLockArtBoundLeft", "JJLockArtBoundRight", "JJLockArtBoundTop", "JJLockArtOwnBoundBottom", "JJLockArtOwnBoundLeft", "JJLockArtOwnBoundRight", "JJLockArtOwnBoundTop", "JJLockContent", "JJLockContent", "charFillStyleBool", "charFillStyle", "charStrokeStyleBool", "charStrokeStyle", "charFontStyleName", "charFontFamilyName", "charFontSize", "charHScaleSize", "ParaJustification", "pathSegments", "pathFillStyle", "pathStrokeStyle", "compPathFillStyle", "compPathStrokeStyle", "pathArea", "pathLength",  "textFrameFillStyleBool", "textFrameFillStyle", "textFrameStrokeStyleBool", "textFrameStrokeStyle"};

void JJLockPanel::ContentLockClicked()
{
    if(jjLock->IsDocumentOpened())
        LoggerAPI();
    ContentLockClicked(NULL);
}

void JJLockPanel::ContentLockClicked(AIArtHandle recreateArtHandle)
{
    AppContext appContext(gPlugin->GetPluginRef());
    Tool tool;
    if(jjLock->IsDocumentOpened())
    {
    ASErr result = kNoErr;
    AIArtHandle artHandle = NULL;
    short type;
    ASBoolean nameBool = true;
    AILayerHandle currentLayer;
    sAILayer->GetCurrentLayer(&currentLayer);

    if((jjLock->IsAnyArtSelected(recreateArtHandle) && recreateArtHandle == NULL) || recreateArtHandle != NULL)
    {
         AIArtSpec artSpec[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
        
        ArtSetHelper artSet(artSpec, 7);
        size_t count = 0;
        count = artSet.GetCount();
        if(count == 0 && recreateArtHandle != NULL)
        {
            count = 1;
        }
    
        for(int i = 0; i < count; i++)
        {
            if(recreateArtHandle == NULL)
                artHandle = artSet[i];
            else if(recreateArtHandle != NULL)
                artHandle = recreateArtHandle;
            sAIArt->GetArtType(artHandle, &type);
            std::vector<AIArtHandle> linkArtHanldes;

            if(jjLock->GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool"))
            {
                sAIUser->MessageAlert(ai::UnicodeString("Group art object group is locked. Please try after unlocking group to lock child art."));
                break;
            }
            
            if(artHandle != NULL &&  type == kTextFrameArt)
            {
                AIBool8 linkedText;
                result = sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
                ai::check_ai_error(result);
                if(linkedText)
                {
                    artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                    linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
                    
                }
                else
                    linkArtHanldes.push_back(artHandle);
                
                
                for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                {
                    std::vector<AIStrokeStyle> charFeatureStrokeArray;
                    std::vector<int> charStrokeStyleBool;
                    
                    std::vector<ai::UnicodeString> charfontStyleNameArray, charfontFamilyNameArray;
                    std::vector<double> charFontSizeArray;
                    artHandle = linkArtHanldes.at(i);

                    if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                    {
                         jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                    }
                    AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                    
                    ai::UnicodeString iconArtName;
                    if(artHandle != NULL)
                    {
                        sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                        if((type == kTextFrameArt) && iconArtName != ai::UnicodeString("LockIcon"))
                            jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("clock.pdf"));
                        
                        
                        ai::UnicodeString wholeTextFrameContent = jjLock->GetText(artHandle);
                        result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, wholeTextFrameContent, "JJLockContent");
                        aisdk::check_ai_error(result);
                        
                        jjLock->GetCharacterFontProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, artHandle);
                        jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
                        jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
                        
                        jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
                        jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
                        jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);
                        
          
                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                        AIReal top = artBounds.top;
                        AIReal left = artBounds.left;
                        AIReal right = artBounds.right;
                        AIReal bottom = artBounds.bottom;
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
                        aisdk::check_ai_error(result);
                        
                        result = jjLock->SetBooleanEntryToHandleDict(artHandle, "clock", true);
                        aisdk::check_ai_error(result);

                        CheckAndCreateParentIDForChild(artHandle);
                    }
                }
            }
            else if(artHandle != NULL &&  type == kPlacedArt)
            {
                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
                AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();

                ai::UnicodeString iconArtName;
                if(artHandle != NULL)
                {
                    sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                    if((type == kPlacedArt) && iconArtName != ai::UnicodeString("LockIcon"))
                        jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("clock.pdf"));

                    ai::UnicodeString placedFileContent;
                    placedFileContent = GetFileInfoContent(artHandle);
                   
                    result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, placedFileContent, "JJLockContent");
                    aisdk::check_ai_error(result);

                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);

                    result = jjLock->SetBooleanEntryToHandleDict(artHandle, "clock", true);
                    aisdk::check_ai_error(result);
                   CheckAndCreateParentIDForChild(artHandle);
                }
            }
            else if (type == kPathArt)
            {
                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
                
                AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                
                ai::int16 countSegment = 0;
                ai::UnicodeString iconArtName;
                if(artHandle != NULL)
                {
                    sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                    if(iconArtName != ai::UnicodeString("LockIcon"))
                        jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("clock.pdf"));
                    
                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                    result = jjLock->SetIntegerEntryToHandleDict(artHandle, "JJLockContent",  countSegment);
                    aisdk::check_ai_error(result);
                    
                    ai::int16 segNumber, countSegment = 0;
                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                    AIPathSegment pathSegments[countSegment];
                    sAIPath->GetPathSegments(artHandle, segNumber, countSegment, pathSegments);
                    
                    std::vector<AIRealPoint> realPointArray;
                    for(int i = 0; i < countSegment; i++)
                    {
                        realPointArray.push_back(pathSegments[i].p);
                    }
                    jjLock->SetArrayEntryForRealPoint(artHandle, "pathSegments", realPointArray);

                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    result = jjLock->SetBooleanEntryToHandleDict(artHandle, "clock", true);
                    aisdk::check_ai_error(result);
                    CheckAndCreateParentIDForChild(artHandle);
                }
            }
            else if(type == kCompoundPathArt)
            {
                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                 {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
                
                AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                
                ai::UnicodeString iconArtName;
                if(artHandle != NULL)
                {
                    sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                    if(iconArtName != ai::UnicodeString("LockIcon"))
                        jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("clock.pdf"));
                    
                    ParseCompoundPathForLock(artHandle, "clock");
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    
                    result = jjLock->SetBooleanEntryToHandleDict(artHandle, "clock", true);
                    aisdk::check_ai_error(result);
                    CheckAndCreateParentIDForChild(artHandle);
                }
            }
            else if (type == kGroupArt)
            {
                isNotChildLocked = true;
                ParseGroupChildForLockCheck(artHandle);
                if(isNotChildLocked)
                {
                    if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                    {
                        jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                    }
                    
                    AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                    
                    
                    ai::UnicodeString iconArtName;
                    if(artHandle != NULL)
                    {
                        sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                        if(( type == kGroupArt && (!jjLock->GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true)) && iconArtName != ai::UnicodeString("LockIcon"))
                            jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("clock.pdf"));
                        int totalChildCount = 0;
                 
                        ParseGroupChildForLock(artHandle, &totalChildCount, "clock");
                        qDebug()<< "totalChildCount C "<<totalChildCount;
                        result = jjLock->SetIntegerEntryToHandleDict(artHandle, "totalGroupChildCount",  totalChildCount);
                        aisdk::check_ai_error(result);
                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);

                        result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                        aisdk::check_ai_error(result);
                        
                        result = jjLock->SetBooleanEntryToHandleDict(artHandle, "clock", true);
                        aisdk::check_ai_error(result);
                        CheckAndCreateParentIDForChild(artHandle);
                    }
                }
            }
            else if(type == kPluginArt)
            {
                sAIUser->MessageAlert(ai::UnicodeString("User can do only position lock for Plugin art object."));
                break;
            }
            else if(type == kSymbolArt)
            {
                sAIUser->MessageAlert(ai::UnicodeString("User can do only position lock for Symbol art object."));
                break;
            }
            
        }
     }
    sAILayer->SetCurrentLayer(currentLayer);
  }
}
/*
 void JJLockPanel::ParseGroupChildForLock(AIArtHandle groupArtHandle, string lockType)
 {
 JJLock* jjLock;
 AIArtHandle lastChildHandle;
 short artType;
 sAIArt->GetArtType(groupArtHandle, &artType);
 if(artType == kGroupArt)
 sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
 else
 lastChildHandle = groupArtHandle;
 while(lastChildHandle != NULL)
 {
 if(lastChildHandle != NULL)
 {
 short childArtType;
 sAIArt->GetArtType(lastChildHandle, &childArtType);
 
 if(childArtType == kGroupArt)
 {
 if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
 {
 jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
 }
 
 jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
 jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
 ParseGroupChildForLock(lastChildHandle, lockType);
 }
 else if(childArtType == kCompoundPathArt)
 {
 if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
 {
 jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
 }
 
 jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
 jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
 if(lockType == "pclock")
 ParseCompoundPathForLock(lastChildHandle, "pclock");
 else if(lockType == "clock")
 ParseCompoundPathForLock(lastChildHandle, "clock");
 }
 else if(childArtType == kTextFrameArt)
 {
 ASErr result = kNoErr;
 
 
 AIArtHandle artHandle = lastChildHandle;
 std::vector<AIArtHandle> linkArtHanldes;
 AIBool8 linkedText;
 sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
 if(linkedText)
 {
 artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
 linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
 }
 else
 {
 if(linkArtHanldes.empty())
 linkArtHanldes.push_back(artHandle);
 }
 
 for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
 {
 if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
 {
 jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
 }
 jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
 jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
 
 std::vector<AIFillStyle> charFeatureColorArray;
 std::vector<int> charColorFillBool;
 
 std::vector<AIStrokeStyle> charFeatureStrokeArray;
 std::vector<int> charStrokeStyleBool;
 
 std::vector<ai::UnicodeString> charfontStyleNameArray, charfontFamilyNameArray;
 std::vector<double> charFontSizeArray, charHScaleSizeArray;
 artHandle = linkArtHanldes.at(i);
 
 //Content
 ai::UnicodeString wholeTextFrameContent = jjLock->GetText(artHandle);
 result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, wholeTextFrameContent, "JJLockContent");
 aisdk::check_ai_error(result);
 
 if(artHandle != NULL && lockType == "pclock")
 {
 jjLock->GetCharacterColor(&charFeatureColorArray, &charColorFillBool, artHandle);
 jjLock->SetArrayEntryForInteger(artHandle, "charFillStyleBool", charColorFillBool);
 jjLock->SetArrayEntryForFillStyle(artHandle, "charFillStyle", charFeatureColorArray);
 
 
 jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
 jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
 jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);
 
 jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, artHandle);
 
 jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
 jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
 
 jjLock->SetArrayEntryForReal(artHandle, "charFontSize", charFontSizeArray);
 jjLock->SetArrayEntryForReal(artHandle, "charHScaleSize", charHScaleSizeArray);
 
 ai::int32 getJustificationValue = 0;
 jjLock->GetParagraphJustification(artHandle, &getJustificationValue);
 result = jjLock->SetIntegerEntryToHandleDict(artHandle, "ParaJustification", getJustificationValue);
 aisdk::check_ai_error(result);
 
 //Position
 AIRealRect artBounds = {0,0,0,0};
 sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
 aisdk::check_ai_error(result);
 
 
 }
 else if(artHandle != NULL && lockType == "clock")
 {
 jjLock->GetCharacterFontProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, artHandle);
 jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
 jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
 
 jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
 jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
 jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);
 
 
 AIRealRect artBounds = {0,0,0,0};
 sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
 AIReal top = artBounds.top;
 AIReal left = artBounds.left;
 AIReal right = artBounds.right;
 AIReal bottom = artBounds.bottom;
 result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
 aisdk::check_ai_error(result);
 result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
 aisdk::check_ai_error(result);
 }
 }
 }
 else if(childArtType == kPathArt)
 {
 ASErr result = kNoErr;
 
 if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
 {
 jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
 }
 
 jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
 jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
 AIArtHandle nextChildArt = lastChildHandle;
 if(lockType == "pclock")
 {
 AIPathStyle pathStyle;
 std::vector<AIFillStyle> compPathFillArray;
 std::vector<AIStrokeStyle> compPathStrokeArray;
 
 result= sAIPathStyle->GetPathStyle(nextChildArt, &pathStyle);
 aisdk::check_ai_error(result);
 
 compPathFillArray.push_back(pathStyle.fill);
 compPathStrokeArray.push_back(pathStyle.stroke);
 jjLock->SetArrayEntryForFillStyle(nextChildArt, "pathFillStyle", compPathFillArray);
 jjLock->SetArrayEntryForStrokeStyle(nextChildArt, "pathStrokeStyle", compPathStrokeArray);
 }
 
 AIArtHandle pathArtHandle = nextChildArt;
 
 ai::int16 countSegment = 0;
 ai::UnicodeString iconArtName;
 if(pathArtHandle != NULL)
 {
 sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
 result = jjLock->SetIntegerEntryToHandleDict(pathArtHandle, "JJLockContent",  countSegment);
 aisdk::check_ai_error(result);
 
 AIReal pathArea, pathLength, pathFlatness;
 ai::int16 segNumber = 0, countSegment = 0;
 sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
 AIPathSegment pathSegments[countSegment];
 sAIPath->GetPathSegments(pathArtHandle, segNumber, countSegment, pathSegments);
 sAIPath->GetPathArea(pathArtHandle, &pathArea);
 sAIPath->GetPathLength(pathArtHandle, &pathLength, pathFlatness);
 
 std::vector<AIRealPoint> realPointArray;
 for(int i = 0; i < countSegment; i++)
 {
 realPointArray.push_back(pathSegments[i].p);
 }
 jjLock->SetArrayEntryForRealPoint(pathArtHandle, "pathSegments", realPointArray);
 jjLock->SetRealEntryToDocumentDict(pathArtHandle, pathArea, "pathArea");
 jjLock->SetRealEntryToDocumentDict(pathArtHandle, pathLength, "pathLength");
 }
 }
 sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
 }
 }
 }
 */
void JJLockPanel::ParseGroupChildForLock(AIArtHandle groupArtHandle, int *totalChildCount, string lockType)
{

    JJLock* jjLock;
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    if(artType == kGroupArt)
    {
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
        if(lastChildHandle != NULL)
        {
            *totalChildCount += 1;
            qDebug() << "INT " << totalChildCount << " " << *totalChildCount ;
        }
    }
    else
    {
        lastChildHandle = groupArtHandle;
        
    }
    while(lastChildHandle != NULL)
    {
        ai::UnicodeString gLockGID;
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        
        if(childArtType == kGroupArt)
        {
            if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
            {
                jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
            }
            
            
            
            
            AIArtHandle parentHandle;
            sAIArt->GetArtParent(lastChildHandle, &parentHandle);
            ASBoolean isLayerGroup;
            sAIArt->IsArtLayerGroup(parentHandle, &isLayerGroup);
            if(parentHandle!= NULL && !isLayerGroup)
            {
                gLockGID = JJLockPanel::CreateUUID();
                if(parentHandle)
                jjLock->SetUnicodeStringEntryToHandleDict(parentHandle, "GroupID", gLockGID);
                jjLock->SetUnicodeStringEntryToHandleDict(lastChildHandle, "ParentID", gLockGID);
            }
            
            jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
            jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
            ParseGroupChildForLock(lastChildHandle, totalChildCount, lockType);
        }
        else if(childArtType == kCompoundPathArt)
        {
            if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
            {
                jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
            }
            
            jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
            jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
            *totalChildCount += 1;
            
            if(lockType == "pclock")
                ParseCompoundPathForLock(lastChildHandle, "pclock");
            else if(lockType == "clock")
                ParseCompoundPathForLock(lastChildHandle, "clock");
        }
        else if(childArtType == kTextFrameArt)
        {
            ASErr result = kNoErr;
         

            AIArtHandle artHandle = lastChildHandle;
            std::vector<AIArtHandle> linkArtHanldes;
            AIBool8 linkedText;
            sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
            if(linkedText)
            {
                artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
            }
            else
            {
                if(linkArtHanldes.empty())
                    linkArtHanldes.push_back(artHandle);
            }
            
            for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
            {
                if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
                }
                jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
                *totalChildCount += 1;
                std::vector<AIFillStyle> charFeatureColorArray;
                std::vector<int> charColorFillBool;
                
                std::vector<AIStrokeStyle> charFeatureStrokeArray;
                std::vector<int> charStrokeStyleBool;
                
                std::vector<ai::UnicodeString> charfontStyleNameArray, charfontFamilyNameArray;
                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                artHandle = linkArtHanldes.at(i);
                
                //Content
                ai::UnicodeString wholeTextFrameContent = jjLock->GetText(artHandle);
                result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, wholeTextFrameContent, "JJLockContent");
                aisdk::check_ai_error(result);

                if(artHandle != NULL && lockType == "pclock")
                {
                    jjLock->GetCharacterColor(&charFeatureColorArray, &charColorFillBool, artHandle);
                    jjLock->SetArrayEntryForInteger(artHandle, "charFillStyleBool", charColorFillBool);
                    jjLock->SetArrayEntryForFillStyle(artHandle, "charFillStyle", charFeatureColorArray);
                    
                    
                    jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
                    jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
                    jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);
                    
                    jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, artHandle);
                    
                    jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
                    jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
                    
                    jjLock->SetArrayEntryForReal(artHandle, "charFontSize", charFontSizeArray);
                    jjLock->SetArrayEntryForReal(artHandle, "charHScaleSize", charHScaleSizeArray);
                    
                    ai::int32 getJustificationValue = 0;
                    jjLock->GetParagraphJustification(artHandle, &getJustificationValue);
                    result = jjLock->SetIntegerEntryToHandleDict(artHandle, "ParaJustification", getJustificationValue);
                    aisdk::check_ai_error(result);
                    
                    //Position
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);

                    
                }
                else if(artHandle != NULL && lockType == "clock")
                {
                    jjLock->GetCharacterFontProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, artHandle);
                    jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
                    jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
                    
                    jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
                    jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
                    jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);
                    
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);
                }
            }
        }
        else if(childArtType == kPathArt)
        {
            ASErr result = kNoErr;
            
            if(sAIArt->HasDictionary(lastChildHandle) && !(sAIArt->IsDictionaryEmpty(lastChildHandle)))
            {
                jjLock->DeleteDictionaryUsingIteratorCheck(lastChildHandle, keyStringListToClear);
            }
            
            jjLock->SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
            jjLock->SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString(lockType), "groupChildLockType");
            *totalChildCount += 1;
            AIArtHandle nextChildArt = lastChildHandle;
            if(lockType == "pclock")
            {
                AIPathStyle pathStyle;
                std::vector<AIFillStyle> compPathFillArray;
                std::vector<AIStrokeStyle> compPathStrokeArray;
                
                result= sAIPathStyle->GetPathStyle(nextChildArt, &pathStyle);
                aisdk::check_ai_error(result);
                
                compPathFillArray.push_back(pathStyle.fill);
                compPathStrokeArray.push_back(pathStyle.stroke);
                jjLock->SetArrayEntryForFillStyle(nextChildArt, "pathFillStyle", compPathFillArray);
                jjLock->SetArrayEntryForStrokeStyle(nextChildArt, "pathStrokeStyle", compPathStrokeArray);
            }
            
            AIArtHandle pathArtHandle = nextChildArt;
            
            ai::int16 countSegment = 0;
            ai::UnicodeString iconArtName;
            if(pathArtHandle != NULL)
            {
                sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
                result = jjLock->SetIntegerEntryToHandleDict(pathArtHandle, "JJLockContent",  countSegment);
                aisdk::check_ai_error(result);
                
                AIReal pathArea, pathLength, pathFlatness;
                ai::int16 segNumber = 0, countSegment = 0;
                sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
                AIPathSegment pathSegments[countSegment];
                sAIPath->GetPathSegments(pathArtHandle, segNumber, countSegment, pathSegments);
                sAIPath->GetPathArea(pathArtHandle, &pathArea);
                sAIPath->GetPathLength(pathArtHandle, &pathLength, pathFlatness);
                
                std::vector<AIRealPoint> realPointArray;
                for(int i = 0; i < countSegment; i++)
                {
                    realPointArray.push_back(pathSegments[i].p);
                }
                jjLock->SetArrayEntryForRealPoint(pathArtHandle, "pathSegments", realPointArray);
                jjLock->SetRealEntryToDocumentDict(pathArtHandle, pathArea, "pathArea");
                jjLock->SetRealEntryToDocumentDict(pathArtHandle, pathLength, "pathLength");
            }
        }
        sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
    }
}

/*
void JJLockPanel::ParseGroupChildForCount(AIArtHandle groupArtHandle, int* countChild)
{
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    if(artType == kGroupArt)
    {
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
        if(lastChildHandle != NULL)
            *countChild += 1;
    }
    else
        lastChildHandle = groupArtHandle;
    
    while(lastChildHandle != NULL)
    {
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        
        if(childArtType == kTextFrameArt || childArtType == kGroupArt || childArtType == kPathArt || childArtType == kCompoundPathArt)
        {
            if(lastChildHandle != NULL)
            {
                short lastChildType;
                sAIArt->GetArtType(lastChildHandle, &lastChildType);
                if(lastChildType == kGroupArt)
                {
                    ParseGroupChildForCount(lastChildHandle, countChild);
                }
                AIArtHandle previousArtHandle;
                previousArtHandle = lastChildHandle;
                *countChild += 1;
                sAIArt->GetArtPriorSibling(previousArtHandle, &lastChildHandle);
            }
        }
    }
}
*/
void JJLockPanel::ParseGroupChildForCount(AIArtHandle groupArtHandle, int* countChild)
{
    
    JJLock* jjLock;
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    if(artType == kGroupArt)
    {
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
        if(lastChildHandle != NULL)
        {
            qDebug() << "INT " << *countChild ;
            *countChild += 1;
          
        }
    }
    else
    {
        lastChildHandle = groupArtHandle;
        
    }
    while(lastChildHandle != NULL)
    {
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        
        if(childArtType == kGroupArt)
        {
            ParseGroupChildForCount(lastChildHandle, countChild);
        }
        else if(childArtType == kCompoundPathArt)
        {
            *countChild += 1;
        }
        else if(childArtType == kTextFrameArt)
        {
            ASErr result = kNoErr;
            AIArtHandle artHandle = lastChildHandle;
            std::vector<AIArtHandle> linkArtHanldes;
            AIBool8 linkedText;
            sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
            if(linkedText)
            {
                artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
            }
            else
            {
                if(linkArtHanldes.empty())
                    linkArtHanldes.push_back(artHandle);
            }
            
            for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
            {
                *countChild += 1;
            }
        }
        else if(childArtType == kPathArt)
        {
            *countChild += 1;
        }
        sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
    }
}
/*
void JJLockPanel::ParseGroupSubGroupChildCount(AIArtHandle groupArtHandle, int* countChild)
{
    
    JJLock* jjLock;
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    int subChildCount = 0;
    if(artType == kGroupArt)
    {
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
        if(lastChildHandle != NULL)
        {
            qDebug() << "INT " << *countChild << " subCC "<< subChildCount;
            *countChild += 1;
            subChildCount += 1;
            
        }
    }
    else
    {
        lastChildHandle = groupArtHandle;
        
    }
    while(lastChildHandle != NULL)
    {
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        
        if(childArtType == kGroupArt)
        {
            ParseGroupChildForCount(lastChildHandle, countChild);
        }
        else if(childArtType == kCompoundPathArt)
        {
            *countChild += 1;
            subChildCount += 1;
        }
        else if(childArtType == kTextFrameArt)
        {
            ASErr result = kNoErr;
            AIArtHandle artHandle = lastChildHandle;
            std::vector<AIArtHandle> linkArtHanldes;
            AIBool8 linkedText;
            sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
            if(linkedText)
            {
                artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
            }
            else
            {
                if(linkArtHanldes.empty())
                    linkArtHanldes.push_back(artHandle);
            }
            
            for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
            {
                *countChild += 1;
                subChildCount += 1;
            }
        }
        else if(childArtType == kPathArt)
        {
            *countChild += 1;
            subChildCount += 1;
        }
        sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
    }
}
*/

Boolean JJLockPanel::ParseGroupChildForLockCheck(AIArtHandle groupArtHandle)
{
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    if(artType == kGroupArt)
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
    else
        lastChildHandle = groupArtHandle;

    while(lastChildHandle != NULL)
    {
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        if(childArtType == kTextFrameArt || childArtType == kGroupArt || childArtType == kPathArt || childArtType == kCompoundPathArt)
        {
            if(lastChildHandle != NULL)
            {
                short lastChildType;
                sAIArt->GetArtType(lastChildHandle, &lastChildType);
                if(lastChildType == kGroupArt)
                {
                   ParseGroupChildForLockCheck(lastChildHandle);
                }
                AIArtHandle previousArtHandle;
                previousArtHandle = lastChildHandle;
                if(jjLock->GetBooleanEntryFromHandleDict(lastChildHandle, "lock"))
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Under this group child object is locked, remove the lock \n and proceed to lock group"));
                    isNotChildLocked = false;
                    break;
                }
                sAIArt->GetArtPriorSibling(previousArtHandle, &lastChildHandle);
            }
        }
        else if((childArtType == kPlacedArt || childArtType == kPluginArt || kSymbolArt) & jjLock->GetBooleanEntryFromHandleDict(lastChildHandle, "lock"))
        {
            sAIUser->MessageAlert(ai::UnicodeString("Under this group child object is locked, remove the lock \n and proceed to lock group"));
            isNotChildLocked = false;
            break;
        }
        else
        {
            
            sAIUser->MessageAlert(ai::UnicodeString("Group art not allowed to lock content or full lock. Child art contains other than Group|Text|Path|Compound path art objects."));
            isNotChildLocked = false;
            break;
        }
    }
}


void JJLockPanel::PositionLockClicked()
{
    if(jjLock->IsDocumentOpened())
        LoggerAPI();
    PositionLockClicked(NULL);
}


void JJLockPanel::PositionLockClicked(AIArtHandle recreateArtHandle)
{
    AppContext appContext(gPlugin->GetPluginRef());
    if(jjLock->IsDocumentOpened())
    {
        ASErr result = kNoErr;
        AIArtHandle artHandle = NULL;
        short type;
        
        AILayerHandle currentLayer;
        sAILayer->GetCurrentLayer(&currentLayer);
        
        if((jjLock->IsAnyArtSelected(recreateArtHandle) && recreateArtHandle == NULL) || recreateArtHandle != NULL)
        {
            AIArtSpec specs[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
            ArtSetHelper artSet(specs, 7);
            size_t count = 0;
            count = artSet.GetCount();
            if(count == 0 && recreateArtHandle != NULL)
            {
                count = 1;
            }
        
            
            for(int i = 0; i < count; i++)
            {
                if(recreateArtHandle == NULL)
                    artHandle = artSet[i];
                else if(recreateArtHandle != NULL)
                    artHandle = recreateArtHandle;
                sAIArt->GetArtType(artHandle, &type);
                std::vector<AIArtHandle> linkArtHanldes;
  
                 if(artHandle != NULL &&  (type == kTextFrameArt  || type == kPlacedArt || type == kPluginArt || type == kSymbolArt || type == kCompoundPathArt || ( type == kGroupArt && (!jjLock->GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true)) || (type == kPathArt && (!jjLock->GetBooleanEntryFromHandleDict(artHandle, "isBoundLock") == true))))
                {

                    if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                    {
                        jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                    }

                    if(jjLock->GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool"))
                    {
                        sAIUser->MessageAlert(ai::UnicodeString("Group art object group is locked. Please try after unlocking group to lock child art."));
                        break;
                    }
                    if(type == kGroupArt)
                    {
                        isNotChildLocked = true;
                        ParseGroupChildForLockCheck(artHandle);
                        if(!isNotChildLocked)
                            break;
                        jjLock->ParseGroupChildForRemoveDictEntriesOrSetForPlock(artHandle, false);
                    }
                    
                    {

                        AIBool8 linkedText;
                        sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
                        if(linkedText)
                        {
                            artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                            linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
                            if(linkArtHanldes.size() > 1)
                                jjLock->SetIntegerEntryToHandleDict(artHandle, "size", linkArtHanldes.size());
                        }
                        else
                        {
                            if(linkArtHanldes.empty())
                                linkArtHanldes.push_back(artHandle);
                        }
                        
                        for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                        {
                            artHandle = linkArtHanldes.at(i);
                            
                            AIRealRect artBounds = {0,0,0,0};
                            sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                            AIReal top = artBounds.top;
                            AIReal left = artBounds.left;
                            AIReal right = artBounds.right;
                            AIReal bottom = artBounds.bottom;
                            result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                            aisdk::check_ai_error(result);
                            
                            
                            AIArtHandle IconHandle = jjLock->GetLockIconHandle();
                            AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                            sAIArt->GetArtType(artHandle, &type);
                            ASBoolean nameBool = true;
                            ai::UnicodeString iconArtName;
                            if(artHandle != NULL && type == kTextFrameArt)
                            {
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                                
                                ai::UnicodeString wholeTextFrameContent = jjLock->GetText(artHandle);
                                result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, wholeTextFrameContent, "JJLockContent");
                                aisdk::check_ai_error(result);

                                jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, artHandle);
                                
                                jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
                                jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
                                jjLock->SetArrayEntryForReal(artHandle, "charFontSize", charFontSizeArray);
                                jjLock->SetArrayEntryForReal(artHandle, "charHScaleSize", charHScaleSizeArray);
                                
                                
                                ai::int32 getJustificationValue = 0;
                                jjLock->GetParagraphJustification(artHandle, &getJustificationValue);
                                result = jjLock->SetIntegerEntryToHandleDict(artHandle, "ParaJustification", getJustificationValue);
                                aisdk::check_ai_error(result);

                            }
                            if(artHandle != NULL)
                            {
                                
                                jjLock->DeleteDictionaryForAnEntry(artHandle, "layerName");
                                sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                                if(iconArtName != ai::UnicodeString("LockIcon"))
                                    jjLock->CreateLockBound(artHandle, lockLayer, IconHandle, GetIconImagePath("plock.pdf"));

                                sAIArt->SetArtBounds(artHandle);
                                result = jjLock->SetBooleanEntryToHandleDict(artHandle, "plock", true);
                                aisdk::check_ai_error(result);
                                CheckAndCreateParentIDForChild(artHandle);

                            }
                        }
                 
                        if(type == kPlacedArt)
                        {
                            tempArtHandle = artHandle;
                        }
                    }
                }
            }
        }
        sAILayer->SetCurrentLayer(currentLayer);
    }
}
void JJLockPanel::ContentAndPositionLockClicked()
{
    if(jjLock->IsDocumentOpened())
        LoggerAPI();
    ContentAndPositionLockClicked(NULL);
}

void JJLockPanel::ContentAndPositionLockClicked(AIArtHandle recreateArtHandle)
{
    AppContext appContext(gPlugin->GetPluginRef());
    if(jjLock->IsDocumentOpened())
    {
    ASErr result = kNoErr;
    short type;
    AIArtHandle artHandle = NULL;
    
    ASBoolean nameBool = true;
    AILayerHandle currentLayer;
    sAILayer->GetCurrentLayer(&currentLayer);

    if((jjLock->IsAnyArtSelected(recreateArtHandle) && recreateArtHandle == NULL) || recreateArtHandle != NULL)
    {
        AIArtSpec specs[7] = {{kTextFrameArt, kArtSelected, kArtSelected}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
        ArtSetHelper artSet(specs, 7);
        size_t count = 0;
        count = artSet.GetCount();
        if(count == 0 && recreateArtHandle != NULL)
        {
            count = 1;
        }
        for(int i = 0; i < count; i++)
        {
            if(recreateArtHandle == NULL)
            {
                artHandle = artSet[i];
                if(jjLock->GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool"))
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Group art object group is locked. Please try after unlocking group to lock child art."));
                    break;
                }
            }
            else if(recreateArtHandle != NULL)
                artHandle = recreateArtHandle;
            sAIArt->GetArtType(artHandle, &type);
            std::vector<AIArtHandle> linkArtHanldes;
      //      qDebug() << "TYPE " << type << "count " <<count;
      //      qDebug() << jjLock->GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool");
          /*  if(jjLock->GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool"))
            {
                sAIUser->MessageAlert(ai::UnicodeString("Group art object group is locked. Please try after unlocking group to lock child art."));
                break;
            }*/
            
            if(artHandle != NULL &&  type == kTextFrameArt )
            {
                AIBool8 linkedText;
                sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
                if(linkedText)
                {
                    artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                    linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
                }
                else
                {
                    if(linkArtHanldes.empty())
                        linkArtHanldes.push_back(artHandle);
                }
                
                for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                {
                    if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                    {
                        jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                    }
                    
                    std::vector<AIFillStyle> charFeatureColorArray;
                    std::vector<int> charColorFillBool;
                    
                    std::vector<AIStrokeStyle> charFeatureStrokeArray;
                    std::vector<int> charStrokeStyleBool;
                    
                    std::vector<AIStrokeStyle> textFrameObjectStrokeStyleArray;
                    std::vector<int> textFrameStrokeVisibleBool;
                    std::vector<AIFillStyle> textFrameObjectFillStyleArray;
                    std::vector<int> textFrameFillVisibleBool;
                    
                    
                    std::vector<ai::UnicodeString> charfontStyleNameArray, charfontFamilyNameArray;
                    std::vector<double> charFontSizeArray, charHScaleSizeArray;
                    artHandle = linkArtHanldes.at(i);
                    
                    //Content
                    ai::UnicodeString wholeTextFrameContent = jjLock->GetText(artHandle);
                    result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, wholeTextFrameContent, "JJLockContent");
                    aisdk::check_ai_error(result);
                    
                    //Position
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);

                    AIArtHandle IconHandle = jjLock->GetLockIconHandle();
                    AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                    
                    ASBoolean nameBool = true;
                    ai::UnicodeString iconArtName;
                    
                    if(artHandle != NULL)
                    {
                        sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                        if((type == kTextFrameArt ) && iconArtName != ai::UnicodeString("LockIcon"))
                            jjLock->CreateLockBound(artHandle, lockLayer, IconHandle,  GetIconImagePath("pclock.pdf"));
                        
                        jjLock->GetTextFrameFillAndStrokeColour(&textFrameObjectStrokeStyleArray, &textFrameStrokeVisibleBool, &textFrameObjectFillStyleArray,&textFrameFillVisibleBool, artHandle);
                        jjLock->SetArrayEntryForInteger(artHandle, "textFrameFillStyleBool", textFrameFillVisibleBool);
                        jjLock->SetArrayEntryForFillStyle(artHandle, "textFrameFillStyle", textFrameObjectFillStyleArray);
                        jjLock->SetArrayEntryForInteger(artHandle, "textFrameStrokeStyleBool", textFrameStrokeVisibleBool);
                        jjLock->SetArrayEntryForStrokeStyle(artHandle, "textFrameStrokeStyle", textFrameObjectStrokeStyleArray);
                        
                        
                        jjLock->GetCharacterColor(&charFeatureColorArray, &charColorFillBool, artHandle);
                        jjLock->SetArrayEntryForInteger(artHandle, "charFillStyleBool", charColorFillBool);
                        jjLock->SetArrayEntryForFillStyle(artHandle, "charFillStyle", charFeatureColorArray);
                        
       
                        jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeStyleBool, artHandle);
                        jjLock->SetArrayEntryForInteger(artHandle, "charStrokeStyleBool", charStrokeStyleBool);
                        jjLock->SetArrayEntryForStrokeStyle(artHandle, "charStrokeStyle", charFeatureStrokeArray);

                        jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, artHandle);
                        
                        jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontStyleName", charfontStyleNameArray);
                        jjLock->SetArrayEntryForUnicodeString(artHandle, "charFontFamilyName", charfontFamilyNameArray);
                        jjLock->SetArrayEntryForReal(artHandle, "charFontSize", charFontSizeArray);
                        jjLock->SetArrayEntryForReal(artHandle, "charHScaleSize", charHScaleSizeArray);
                        
                        ai::int32 getJustificationValue = 0;
                        jjLock->GetParagraphJustification(artHandle, &getJustificationValue);
                        result = jjLock->SetIntegerEntryToHandleDict(artHandle, "ParaJustification", getJustificationValue);
                        aisdk::check_ai_error(result);
                     
                        result = jjLock->SetBooleanEntryToHandleDict(artHandle, "pclock", true);
                        aisdk::check_ai_error(result);
                        CheckAndCreateParentIDForChild(artHandle);
                    }
                }
            }
            else if(artHandle != NULL &&  type == kPlacedArt)
            {
                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
             
                 AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
             
                ASBoolean nameBool = true;
                 ai::UnicodeString iconArtName;
                 if(artHandle != NULL)
                 {
                 sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                 if((type == kPlacedArt) && iconArtName != ai::UnicodeString("LockIcon"))
                 jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("pclock.pdf"));
             
             
                 ai::UnicodeString placedFileContent;
                 placedFileContent = GetFileInfoContent(artHandle);
             
                 result = jjLock->SetUnicodeEntryToDocumentDict(artHandle, placedFileContent, "JJLockContent");
                 aisdk::check_ai_error(result);
             
                 //Position
                 AIRealRect artBounds = {0,0,0,0};
                 sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                 AIReal top = artBounds.top;
                 AIReal left = artBounds.left;
                 AIReal right = artBounds.right;
                 AIReal bottom = artBounds.bottom;
                 result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                 aisdk::check_ai_error(result);
                 result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                 aisdk::check_ai_error(result);
                 result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                 aisdk::check_ai_error(result);
                 result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                 aisdk::check_ai_error(result);
             
                 result = jjLock->SetBooleanEntryToHandleDict(artHandle, "pclock", true);
                 aisdk::check_ai_error(result);
                 CheckAndCreateParentIDForChild(artHandle);
                 }
             }
            else if (type == kPathArt)
            {
                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
                
                AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                
                ai::int16 countSegment = 0;
                ai::UnicodeString iconArtName;
                if(artHandle != NULL)
                {
                    sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                    if(iconArtName != ai::UnicodeString("LockIcon"))
                        jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("pclock.pdf"));
                    
                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                    result = jjLock->SetIntegerEntryToHandleDict(artHandle, "JJLockContent",  countSegment);
                    aisdk::check_ai_error(result);
                    
                    ai::int16 segNumber, countSegment = 0;
                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                    AIPathSegment pathSegments[countSegment];
                    sAIPath->GetPathSegments(artHandle, segNumber, countSegment, pathSegments);
                    
                    std::vector<AIRealPoint> realPointArray;
                    for(int i = 0; i < countSegment; i++)
                    {
                        realPointArray.push_back(pathSegments[i].p);
                    }
                    jjLock->SetArrayEntryForRealPoint(artHandle, "pathSegments", realPointArray);

                    AIPathStyle pathStyle;
                    std::vector<AIFillStyle> pathFillArray;
                    std::vector<AIStrokeStyle> pathStrokeArray;
                    
                    result= sAIPathStyle->GetPathStyle(artHandle, &pathStyle);
                    aisdk::check_ai_error(result);
                    
                    pathFillArray.push_back(pathStyle.fill);
                    pathStrokeArray.push_back(pathStyle.stroke);
                    jjLock->SetArrayEntryForFillStyle(artHandle, "pathFillStyle", pathFillArray);
                    jjLock->SetArrayEntryForStrokeStyle(artHandle, "pathStrokeStyle", pathStrokeArray);

                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    result = jjLock->SetBooleanEntryToHandleDict(artHandle, "pclock", true);
                    aisdk::check_ai_error(result);
                    CheckAndCreateParentIDForChild(artHandle);
                    
                }
            }
            else if(type == kCompoundPathArt)
            {
                JJLock *jjLock;

                if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                {
                    jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                }
                AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                ai::UnicodeString iconArtName;
                if(artHandle != NULL)
                {
                    sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                    if(iconArtName != ai::UnicodeString("LockIcon"))
                        jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("pclock.pdf"));

                    ParseCompoundPathForLock(artHandle, "pclock");
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);

                    result = jjLock->SetBooleanEntryToHandleDict(artHandle, "pclock", true);
                    aisdk::check_ai_error(result);
                    CheckAndCreateParentIDForChild(artHandle);
                }
            }
            else if (type == kGroupArt)
            {
                isNotChildLocked = true;
                ParseGroupChildForLockCheck(artHandle);
                if(isNotChildLocked)
                {
                    if(sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
                    {
                        jjLock->DeleteDictionaryUsingIteratorCheck(artHandle, keyStringListToClear);
                    }
                    AILayerHandle lockLayer =  jjLock->CreateLockParentLayer();
                    
                    ai::UnicodeString iconArtName;
                    if(artHandle != NULL)
                    {
                        sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                        if(( type == kGroupArt && (!jjLock->GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true)) && iconArtName != ai::UnicodeString("LockIcon"))
                            jjLock->CreateLockBound(artHandle, lockLayer, NULL,  GetIconImagePath("pclock.pdf"));
                        int totalChildCount = 0;
                        ParseGroupChildForLock(artHandle, &totalChildCount, "pclock");
                        qDebug()<< "totalChildCount C "<<totalChildCount;
                        result = jjLock->SetIntegerEntryToHandleDict(artHandle, "totalGroupChildCount",  totalChildCount);
                        aisdk::check_ai_error(result);
                        
                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                        AIReal top = artBounds.top;
                        AIReal left = artBounds.left;
                        AIReal right = artBounds.right;
                        AIReal bottom = artBounds.bottom;
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
                        aisdk::check_ai_error(result);

                        result = jjLock->SetBooleanEntryToHandleDict(artHandle, "pclock", true);
                        aisdk::check_ai_error(result);
                        CheckAndCreateParentIDForChild(artHandle);
                        sAIArt->GetArtUserAttr(artHandle, kArtFullySelected, 0);
                    }
                }
                
            }
            else if(type == kPluginArt)
            {
                sAIUser->MessageAlert(ai::UnicodeString("User can do only position lock for Plugin art object."));
                break;
            }
            else if(type == kSymbolArt)
            {
                sAIUser->MessageAlert(ai::UnicodeString("User can do only position lock for Symbol art object."));
                break;
            }
            
        }
    }
    sAILayer->SetCurrentLayer(currentLayer);
    }
}


bool JJLockPanel::FindArtIsGroupItem(short type, AIArtHandle artHandle)
{
    short groupType;
    AIArtHandle parentGroupArtHandle;
    AIArtHandle firstChild;
    sAIArt->GetArtParent(artHandle, &parentGroupArtHandle);
    sAIArt->GetArtSibling(parentGroupArtHandle, &firstChild);
    
    sAIArt->GetArtType(parentGroupArtHandle, &groupType);
    if(parentGroupArtHandle != NULL && groupType == kGroupArt && type != kGroupArt && firstChild != NULL)
    {
        sAIUser->MessageAlert(ai::UnicodeString("Select the group and do position lock"));
        return true;
    }
    return false;
}

ai::UnicodeString JJLockPanel::CreateUUID()
{
    Tool tool;
    QUuid qid = QUuid::createUuid();
    return tool.QStringToUnicodeString(qid.toString());
}

string JJLockPanel::GetIconImagePath(string imageName)
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    QString imagePath = appDirPath + QObject::tr("/../../..") + QObject::tr("/Plug-ins.localized/Sgk/Images/") + imageName.c_str();
    return imagePath.toStdString();
}

ai::UnicodeString JJLockPanel::GetFileInfoContent(AIArtHandle artHandle)
{
    Tool tool;
    ai::UnicodeString placedArtFilePath, placedFileContent;
    SPPlatformFileInfo placedFileInfo;
    sAIPlaced->GetPlacedFilePathFromArt(artHandle, placedArtFilePath);
    sAIPlaced->GetPlacedFileInfoFromFile(artHandle, &placedFileInfo);
    QString qFileInfo = QString::number(placedFileInfo.creationDate);
    placedFileContent = tool.QStringToUnicodeString(qFileInfo);
    placedFileContent = ai::UnicodeString("-") + placedArtFilePath;
    return placedFileContent;
}

bool JJLockPanel::isDocumentOpen()
{
    AIErr result = kNoErr;
    bool openFlag = false;
    try {
        AppContext appContext(gPlugin->GetPluginRef());
        AIDocumentHandle document = NULL;
        result = sAIDocument->GetDocument(&document);
        //aisdk::check_ai_error(result);
        if (document) {
            openFlag = true;
        }
    } catch (ai::Error &ex) {
        result = ex;
    }
    return openFlag;
}


void JJLockPanel::MoveLockIcon()
{
    AppContext appContext(gPlugin->GetPluginRef());
    ASErr result = kNoErr;
    AIArtHandle artHandle = NULL;
    short shortType;
    
    if(jjLock->IsDocumentOpened())
    {
        AIArtSpec artSpec[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
    
        ArtSetHelper artSet(artSpec, 7);
        size_t count = 0;
        count = artSet.GetCount();
        ai::uuid auuid;
        AIRealMatrix matrix;
        AIReal adjustmentOffset = 1;
    
        AIRealRect boundPathArtBounds = {0,0,0,0};
        AIRealRect iconBound = {0,0,0,0};
        AIArtHandle boundPathArtHandle;
        AIArtHandle iconArtHandle;
        AILayerHandle LockLayer = NULL;
        ai::int32 lockPosition = 0;
        for(int i = 0; i < count; i++)
        {
            artHandle = artSet[i];
            sAIArt->GetArtType(artHandle, &shortType);
            if((jjLock->GetBooleanEntryFromHandleDict(artHandle, "lock") == true))
            {
                
                boundPathArtHandle = jjLock->FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                                           jjLock->GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
                iconArtHandle = jjLock->FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                                           jjLock->GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
                
                
                LockLayer = jjLock->CreateLockParentLayer();
                AIBoolean layerVisbleBool;
                sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
                if(!layerVisbleBool)
                    sAILayer->SetLayerVisible(LockLayer, true);
                sAILayer->SetLayerEditable(LockLayer, true);
                

                
                sAIArt->GetArtTransformBounds(boundPathArtHandle, NULL, kNoStrokeBounds, &boundPathArtBounds);
                sAIArt->GetArtTransformBounds(iconArtHandle, NULL, kNoStrokeBounds, &iconBound);
                
                AIReal iconHeight = iconBound.top - iconBound.bottom;
                AIReal iconWidth = iconBound.right - iconBound.left;
                AIReal horizontalOffset = 0;
                AIReal verticalOffset = 0;
                 lockPosition = jjLock->GetIntegerEntryFromHandleDict(artHandle, "lockPos");
                if (lockPosition <= 4)
                {
                    horizontalOffset = (-((boundPathArtBounds.right - boundPathArtBounds.left)/(4)) - adjustmentOffset);
                }
                else
                {
                    horizontalOffset = ((boundPathArtBounds.right - boundPathArtBounds.left) + 4);
                }
                sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, verticalOffset);
                result = sAITransformArt->TransformArt(iconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
                aisdk::check_ai_error(result);
                
                if(lockPosition <= 4 && lockPosition !=0)
                    jjLock->SetIntegerEntryToHandleDict(artHandle, "lockPos", lockPosition + 1);
                else if(lockPosition == 0)
                    jjLock->SetIntegerEntryToHandleDict(artHandle, "lockPos", 2);
                else if(lockPosition)
                    jjLock->SetIntegerEntryToHandleDict(artHandle, "lockPos", 1);
                
                if(!layerVisbleBool)
                    sAILayer->SetLayerVisible(LockLayer, false);
                sAILayer->SetLayerEditable(LockLayer, false);
            }
        }
    
    }
}

void JJLockPanel::LoggerAPI()
{
    AppContext appContext(gPlugin->GetPluginRef());
    JJLock jjLock;
    int toggleLockAPI;
    jjLock.GetIntegerEntryToDocumentDict("lockAPI", &toggleLockAPI);
    if(toggleLockAPI==1 && jjLock.IsDocumentOpened() && !kBetaVersion)
    {
        toggleLockAPI = 0;
        ai::FilePath file;
        sAIDocument->GetDocumentFileSpecification(file);
        CallLogger(QString::fromStdString(file.GetFileNameNoExt().as_UTF8().c_str()));
        sleep(1);
        jjLock.SetIntegerEntryToDocumentDict("lockAPI", toggleLockAPI);
    }
}
/*
// ORGINAL
void JJLockPanel::ReleaseLockClicked()
{
    ReleaseLockClicked(NULL);
}

 // ORGINAL
void JJLockPanel::ReleaseLockClicked(AIArtHandle releaseHandle)
{
    
    AppContext appContext(gPlugin->GetPluginRef());
    if(jjLock->IsDocumentOpened())
    {
        LoggerAPI();
        if(jjLock->IsAnyArtSelected(NULL) && releaseHandle == NULL)
        {
            AIArtHandle artHandle = NULL;
            AIArtSpec artSpec[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}};
            
            ArtSetHelper artSet(artSpec, 7);
            size_t count = 0;
            short artType;
            count = artSet.GetCount();
            
            for(int i = 0; i < count; i++)
            {
                artHandle = artSet[i];
                sAIArt->GetArtType(artHandle, &artType);

                if(artHandle != NULL && jjLock->GetBooleanEntryFromHandleDict(artHandle, "lock") != true && count == 1 && artType != kGroupArt)
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Selected art is not locked object, please try with locked art object to unlock"));
                    break;
                }
                else if(artHandle != NULL && artType == kGroupArt)
                {
                    if(count == 1)
                    {
                        if(sAIUser->YesNoAlert(ai::UnicodeString("Do you want to remove all locked child art from Group (Yes/No)")))
                        {
                            jjLock->ParseGroupChildArtandClearLock(artHandle);
                        }
                    }
                    else if(count > 1)
                         jjLock->ParseGroupChildArtandClearLock(artHandle);
                }
                
                if(artHandle != NULL && jjLock->GetBooleanEntryFromHandleDict(artHandle, "lock") == true)
                {   AIBool8 ThreadedText;
                    sAITextFrame->PartOfLinkedText(artHandle, &ThreadedText);
                    if(!ThreadedText)
                        jjLock->ClearLockEntry(artHandle);
                    else
                    {
                        std::vector<AIArtHandle> linkArtHanldes;
                        artHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(artHandle);
                        linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(artHandle);
                        
                        for(int j = linkArtHanldes.size()-1; j>=0 ; j--)
                            jjLock->ClearLockEntry(linkArtHanldes.at(j));
                    }
                }
            }
        }
        else if(releaseHandle != NULL)
        {
            short releaseArtType;
            sAIArt->GetArtType(releaseHandle, &releaseArtType);
            
            if(releaseHandle != NULL && releaseArtType == kGroupArt)
            {
                jjLock->ParseGroupChildArtandClearLock(releaseHandle);
            }
            
            if(releaseHandle != NULL && jjLock->GetBooleanEntryFromHandleDict(releaseHandle, "lock") == true)
            {   AIBool8 ThreadedText;
                sAITextFrame->PartOfLinkedText(releaseHandle, &ThreadedText);
                if(!ThreadedText)
                    jjLock->ClearLockEntry(releaseHandle);
                else
                {
                    std::vector<AIArtHandle> linkArtHanldes;
                    releaseHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(releaseHandle);
                    linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(releaseHandle);
                    
                    for(int j = linkArtHanldes.size()-1; j>=0 ; j--)
                        jjLock->ClearLockEntry(linkArtHanldes.at(j));
                }
            }
        }
    }
}
*/


void JJLockPanel::ParseCompoundPathForLock(AIArtHandle compoundPathArtHandle, string lockType)
{
    AIErr result = kNoErr;
    JJLock jjLock;
    short type;
    ASBoolean childBool = true;
    ai::UnicodeString childArtName;

    AIArtHandle pathArtHandle, groupArtHandle, nextChildArt;
    sAIArt->GetArtType(compoundPathArtHandle, &type);
    sAIArt->GetArtFirstChild(compoundPathArtHandle, &nextChildArt);
    groupArtHandle = compoundPathArtHandle;
    ASBoolean nameBool = true;
    
    while(groupArtHandle != NULL && (type == kGroupArt || type == kCompoundPathArt))
    {
        sAIArt->GetArtName(groupArtHandle, childArtName, &childBool);
    
        while(nextChildArt != NULL)
        {
            short childArtType;
            sAIArt->GetArtName(nextChildArt, childArtName, &childBool);

            sAIArt->GetArtType(nextChildArt, &childArtType);
            if(childArtType == kGroupArt)
            {
                ParseCompoundPathForLock(nextChildArt, lockType);
            }
            else if(childArtType == kPathArt)
            {
                if(sAIArt->HasDictionary(nextChildArt) && !(sAIArt->IsDictionaryEmpty(nextChildArt)))
                {
                    jjLock.DeleteDictionaryUsingIteratorCheck(nextChildArt, keyStringListToClear);
                }

                if(lockType == "pclock")
                {
                    AIPathStyle pathStyle;
                    std::vector<AIFillStyle> compPathFillArray;
                    std::vector<AIStrokeStyle> compPathStrokeArray;
                    
                    result= sAIPathStyle->GetPathStyle(nextChildArt, &pathStyle);
                    aisdk::check_ai_error(result);
                    
                    compPathFillArray.push_back(pathStyle.fill);
                    compPathStrokeArray.push_back(pathStyle.stroke);
                    jjLock.SetArrayEntryForFillStyle(nextChildArt, "compPathFillStyle", compPathFillArray);
                    jjLock.SetArrayEntryForStrokeStyle(nextChildArt, "compPathStrokeStyle", compPathStrokeArray);
                }

                pathArtHandle = nextChildArt;

                ai::int16 countSegment = 0;
                ai::UnicodeString iconArtName;
                if(pathArtHandle != NULL)
                {
                    sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
                    result = jjLock.SetIntegerEntryToHandleDict(pathArtHandle, "JJLockContent",  countSegment);
                    aisdk::check_ai_error(result);
                    
                    AIReal pathArea=0, pathLength=0, pathFlatness=0;
                    ai::int16 segNumber = 0, countSegment = 0;
                    sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
                    AIPathSegment pathSegments[countSegment];
                    sAIPath->GetPathSegments(pathArtHandle, segNumber, countSegment, pathSegments);
                    sAIPath->GetPathArea(pathArtHandle, &pathArea);
                    sAIPath->GetPathLength(pathArtHandle, &pathLength, pathFlatness);
                    
                    std::vector<AIRealPoint> realPointArray;
                    for(int i = 0; i < countSegment; i++)
                    {
                        realPointArray.push_back(pathSegments[i].p);
//                        qDebug() << realPointArray.at(i).h;
//                        qDebug() << realPointArray.at(i).v;
                    }
                    jjLock.SetArrayEntryForRealPoint(pathArtHandle, "pathSegments", realPointArray);
                    jjLock.SetRealEntryToDocumentDict(pathArtHandle, pathArea, "pathArea");
                    jjLock.SetRealEntryToDocumentDict(pathArtHandle, pathLength, "pathLength");
                  //  jjLock.FetchAllDictionaryUsingIterator(pathArtHandle);
                }
 
            }
            AIArtHandle subHandle;
            subHandle = nextChildArt;
            sAIArt->GetArtSibling(subHandle, &nextChildArt);
        }
        if(nextChildArt != NULL)
        {
            ASBoolean isArtLayerGroup;
            sAIArt->GetArtParent(nextChildArt, &groupArtHandle);
            sAIArt->IsArtLayerGroup(groupArtHandle, &isArtLayerGroup);
            if(isArtLayerGroup)
                break;
            sAIArt->GetArtSibling(groupArtHandle, &nextChildArt);
        }
        else
        {
            break;
        }
    }
}


void JJLockPanel::CheckAndCreateParentIDForChild(AIArtHandle childHandle)
{
    if(jjLock->GetBooleanEntryFromHandleDict(childHandle, "LockedInGroup"))
       return;
    AIArtHandle parentHandle;
    sAIArt->GetArtParent(childHandle, &parentHandle);
    ASBoolean isLayerGroup;
    sAIArt->IsArtLayerGroup(parentHandle, &isLayerGroup);
    if(parentHandle != NULL && !isLayerGroup)
    {
        ai::UnicodeString GID = jjLock->GetUnicodeStringEntryFromHandleDict(parentHandle, "GroupID");
        if(GID != ai::UnicodeString(""))
        {
            if(jjLock->GetUnicodeStringEntryFromHandleDict(childHandle, "GroupID") == ai::UnicodeString(""))
                jjLock->SetBooleanEntryToHandleDict(childHandle, "LockedInGroup", true);
            
            jjLock->SetUnicodeStringEntryToHandleDict(childHandle, "ParentID", GID);
            int childLockCount = jjLock->GetIntegerEntryFromHandleDict(parentHandle, "ChildLockCount");
            if(childLockCount > 0)
            {
                childLockCount = childLockCount + 1;
                jjLock->SetIntegerEntryToHandleDict(parentHandle, "ChildLockCount", childLockCount);
            }
            else
            {
                jjLock->SetIntegerEntryToHandleDict(parentHandle, "ChildLockCount", 1);
            }
        }
        else
        {
            GID = JJLockPanel::CreateUUID();
            jjLock->SetUnicodeStringEntryToHandleDict(parentHandle, "GroupID", GID);
            jjLock->SetUnicodeStringEntryToHandleDict(childHandle, "ParentID", GID);
            if(jjLock->GetUnicodeStringEntryFromHandleDict(childHandle, "GroupID") == ai::UnicodeString(""))
                jjLock->SetBooleanEntryToHandleDict(childHandle, "LockedInGroup", true);
            jjLock->SetIntegerEntryToHandleDict(parentHandle, "ChildLockCount", 1);
            
            ASErr result = kNoErr;
            AIRealRect artBounds = {0,0,0,0};
            sAIArt->GetArtTransformBounds(parentHandle, NULL, kNoStrokeBounds, &artBounds);
            result = jjLock->SetRealEntryToDocumentDict(parentHandle, artBounds.top, "JJLockArtBoundTop");
            aisdk::check_ai_error(result);
            result = jjLock->SetRealEntryToDocumentDict(parentHandle, artBounds.left, "JJLockArtBoundLeft");
            aisdk::check_ai_error(result);
            result = jjLock->SetRealEntryToDocumentDict(parentHandle, artBounds.right, "JJLockArtBoundRight");
            aisdk::check_ai_error(result);
            result = jjLock->SetRealEntryToDocumentDict(parentHandle, artBounds.bottom, "JJLockArtBoundBottom");
            aisdk::check_ai_error(result);
        }
        CheckAndCreateParentIDForChild(parentHandle);
    }
}

void JJLockPanel::CheckAndReleaseParentIDForChild(AIArtHandle childHandle)
{
    AIArtHandle parentHandle;
    sAIArt->GetArtParent(childHandle, &parentHandle);
    ASBoolean isLayerGroup;
    sAIArt->IsArtLayerGroup(parentHandle, &isLayerGroup);
    short artType;
    sAIArt->GetArtType(childHandle, &artType);
    //qDebug() << isLayerGroup << artType;
    if(parentHandle != NULL && !isLayerGroup)
    {
        ai::UnicodeString PID = jjLock->GetUnicodeStringEntryFromHandleDict(childHandle, "ParentID");
        ai::UnicodeString GID = jjLock->GetUnicodeStringEntryFromHandleDict(parentHandle, "GroupID");
        if(PID != ai::UnicodeString("") || (PID == ai::UnicodeString("") && GID != ai::UnicodeString("")) )
        {
            int childLockCount = jjLock->GetIntegerEntryFromHandleDict(parentHandle, "ChildLockCount");
            if(childLockCount > 0)
            {
                childLockCount = childLockCount - 1;
                jjLock->SetIntegerEntryToHandleDict(parentHandle, "ChildLockCount", childLockCount);
                if(childLockCount == 0)
                {
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "ChildLockCount");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "ParentID");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "GroupID");
                    jjLock->DeleteDictionaryForAnEntry(childHandle, "ParentID");
                    if(jjLock->GetBooleanEntryFromHandleDict(childHandle, "LockedInGroup") == true )
                    {
                        jjLock->DeleteDictionaryForAnEntry(childHandle, "LockedInGroup");
                        jjLock->DeleteDictionaryForAnEntry(childHandle, "GroupID");
                    }

                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundRight");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundLeft");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundBottom");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundTop");
                    
                }
                else if(jjLock->GetBooleanEntryFromHandleDict(childHandle, "lock") == true && jjLock->GetBooleanEntryFromHandleDict(childHandle, "LockedInGroup") == true )
                {
                    jjLock->DeleteDictionaryForAnEntry(childHandle, "ParentID");
                    jjLock->DeleteDictionaryForAnEntry(childHandle, "LockedInGroup");
                }
                else if(PID == ai::UnicodeString("") && GID != ai::UnicodeString("") && childLockCount > 0)
                {
                    if(childLockCount == 0)
                    {
                    jjLock->SetIntegerEntryToHandleDict(parentHandle, "ChildLockCount", 0);
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "ChildLockCount");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "ParentID");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "GroupID");
                    jjLock->DeleteDictionaryForAnEntry(childHandle, "ParentID");
                    if(jjLock->GetBooleanEntryFromHandleDict(childHandle, "LockedInGroup") == true )
                    {
                        jjLock->DeleteDictionaryForAnEntry(childHandle, "LockedInGroup");
                        jjLock->DeleteDictionaryForAnEntry(childHandle, "GroupID");
                    }
                    
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundRight");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundLeft");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundBottom");
                    jjLock->DeleteDictionaryForAnEntry(parentHandle, "JJLockArtBoundTop");
                    }
                }
            }
            else
            {
                jjLock->DeleteDictionaryForAnEntry(parentHandle, "ChildLockCount");
                jjLock->DeleteDictionaryForAnEntry(parentHandle, "GroupID");
            }
            CheckAndReleaseParentIDForChild(parentHandle);
        }
    }
}

//Dummy for testing

 void JJLockPanel::ReleaseLockClicked()
 {
 AppContext appContext(gPlugin->GetPluginRef());
 ASErr result = kNoErr;
 AIArtHandle artHandle = NULL;
 short shortType;
 
 if(jjLock->IsDocumentOpened())
 {
     ai::int16 colorModel;
     AIColorProfile rgbP, cmykP, grayP;
     sAIDocument->GetDocumentColorModel(&colorModel);
     sAIDocument->GetDocumentProfiles(&rgbP, &cmykP, &grayP);
     //qDebug() << "\n\n Color model "<<colorModel << " " <<rgbP <<" "<<cmykP << " "<<grayP;
     
     
//     AILayerHandle layerHandle;
//     sAILayer->GetLayerByTitle(&layerHandle,  ai::UnicodeString("Layer 1"));
//
//     sAILayer->DeselectArtOnLayer(layerHandle);
//     sAIDocument->RedrawDocument();
//
//

     if(jjLock->IsAnyArtSelected(NULL))
     {
     //   AIArtSpec artSpec[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}};
     
         AIArtSpec artSpec[1] = {{kAnyArt, kArtTargeted, kArtTargeted}};
         
         ArtSetHelper artSet(artSpec, 1);
         size_t count = 0;
         count = artSet.GetCount();
         AIArtHandle firstChild, editArtHandle;
         ai::uuid auuid;
         AIRealRect artBound = {0, 0, 0, 0};
         AIRealRect artRotatedBound = {0, 0, 0, 0};
          ai::int16 segNumber, countSegment = 0;
         ai::UnicodeString artTitle;
         int countGroupChild = 0;
        
             for(int i = 0; i < count; i++)
             {
                 artHandle = artSet[i];
                 
                //sAIPluginGroup->GetPluginArtEditArt(artHandle, &editArtHandle);
                  sAIArt->GetArtType(artHandle, &shortType);
                 jjLock->FetchAllDictionaryUsingIterator(artHandle);
                 ParseGroupChildForCount(artHandle, &countGroupChild);
                 qDebug() << " ccountGroupChild " << countGroupChild;
//                  if(shortType == kGroupArt)
//                  {
//                      ParseGroupChildForLock(artHandle, "");
//                  }
              
//

//
//
//
//                 sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBound);
//                 sAIArt->GetArtRotatedBounds(artHandle, 30, kNoStrokeBounds, &artRotatedBound);
//
//                 qDebug() << " artRotatedBound L " << artRotatedBound.left;
//                 qDebug() << " artRotatedBound R " << artRotatedBound.right;
//                 qDebug() << " artRotatedBound T " << artRotatedBound.top;
//                 qDebug() << " artRotatedBound B " << artRotatedBound.bottom;
//
//                 qDebug() << " artRotatedBound L " << artBound.left;
//                 qDebug() << " artRotatedBound R " << artBound.right;
//                 qDebug() << " artRotatedBound T " << artBound.top;
//                 qDebug() << " artRotatedBound B " << artBound.bottom;
//
//                 // ( AIArtHandle art, AIReal angle, ai::int32 flags, AIRealRect *bounds );
//
//             sAIPath->GetPathSegmentCount(artHandle, &countSegment);
//           //  result = jjLock->SetIntegerEntryToHandleDict(artHandle, "JJLockContent",  countSegment);
//           //  aisdk::check_ai_error(result);
//                 AIArtHandle newArtHandle;
//                 AIArtHandle childArt, firstChild;
//                 ai::int32 pathPolarity;
//            qDebug() << sAIArt->IsArtClipping(artHandle);
//                 qDebug() << sAIArt->ArtHasFill(artHandle);
//                 qDebug() << sAIArt->ArtHasStroke(artHandle);
//         //        qDebug() << sAIArt->ConvertPointTypeToAreaType(artHandle, &newArtHandle);
//
//
//
//                 qDebug() << sAIArt->GetArtFirstChild(artHandle, &firstChild);
//
//                 AIColor aiStrokeColor;
//                 AIColor aiFillColor;
//                 AIColor aiFillNoColor;
//                 AIColor aiStrokeNoColor;
//                 AIArtHandle gropARTHandle;
//                 gropARTHandle =  firstChild;
//
//                 sAIArt->GetArtType(firstChild, &shortType);
//                 if(shortType == kGroupArt){
//                     if (result == kNoErr) {
//
//                         result = sAIArt->GetArtFirstChild(firstChild, &firstChild);
//
//                     }
//                     while ((result == kNoErr) && (firstChild != NULL)) {
//                         if (result == kNoErr) {
//                             if (result == kNoErr) {
//                                 result = sAIArt->GetArtType(firstChild, &shortType);
//                                 qDebug() << "c Art type " << shortType;
//                                 sAIArt->DisposeArt(firstChild);
//                                 sAIDocument->RedrawDocument();
//
//                                 firstChild = gropARTHandle;
//                             }
//                             if(shortType == kPathArt){
//                                 ai::int32 isLock;
//                                 result = sAIArt->GetArtUserAttr(firstChild, kArtLocked, &isLock);
//
//                                 result = sAIArt->GetArtName(firstChild, artTitle, NULL);
//                                 aisdk::check_ai_error(result);
//                            //     if(!isLock && artTitle == ai::UnicodeString("FillStrokeObject"))
//                                 qDebug() <<"artTitle " << artTitle.as_UTF8().c_str();
//                                if(!isLock)
//                                 {
//                                     AIPathStyle pathStyle;
//                                     result= sAIPathStyle->GetPathStyle(firstChild, &pathStyle);
//                                     aisdk::check_ai_error(result);
//                                     aiStrokeColor = pathStyle.stroke.color;
//                                     aiFillColor = pathStyle.fill.color;
//                                     aiFillNoColor.kind = kNoneColor;
//                                     aiStrokeNoColor.kind = kNoneColor;
//                                 }
//                             }
//                         }
//                             sAIArt->GetArtSibling(firstChild, &firstChild);
//                     }
//                 }
//
////                 sAIArt->DisposeArt(firstChild);
////                 sAIArt->DisposeArt(childArt);
////                 return;
//                 qDebug() << sAIArt->GetPathPolarity(artHandle, &pathPolarity);
//
//                 sAIArt->GetArtType(firstChild, &shortType);
//                 qDebug() << "c Art type " << shortType;
//                 ai::int16 pathSegmentCount;
//
//                 sAIPath->GetPathSegmentCount(firstChild, &pathSegmentCount);
//
//
//             sAIPath->GetPathSegmentCount(childArt, &countSegment);
//             AIPathSegment pathSegments[countSegment];
//                  qDebug() << "countSegment " << pathSegmentCount << "-----"<< countSegment;
//
//                 sAIPath->GetPathSegments(artHandle, segNumber, pathSegmentCount, pathSegments);
//             sAIPath->GetPathSegments(childArt, segNumber, countSegment, pathSegments);
//
//             std::vector<AIRealPoint> realPointArray;
//             for(int i = 0; i < countSegment; i++)
//             {
//                 realPointArray.push_back(pathSegments[i].p);
//             }
//
//
//
//
////             sAIUUID->GetArtUUID(artHandle, auuid);
////             qDebug()<< "uuid " << auuid.mData[0];
////             qDebug()<< "uuid " << auuid.mData[1];
////             qDebug()<< "uuid " << auuid.mData[2];
////             qDebug()<< "uuid " << auuid.mData[3];
////             qDebug()<< "uuid " << auuid.mData[4];
////             qDebug()<< "uuid " << auuid.mData[5];
////             qDebug()<< "uuid " << auuid.mData[6];
////
////             qDebug()<< "uuid " << auuid.mData[15];
////
////             AIArtHandle diffHand;
////
////             sAIUUID->GetArtHandle(auuid, diffHand);
////             jjLock->FetchAllDictionaryUsingIterator(diffHand);
////
////
////             sAIArt->GetArtFirstChild(diffHand, &firstChild);
////             qDebug() << "Type : " << shortType << "  " << firstChild;
////             //   qDebug() << "  : " << jjLock->GetText(diffHand).as_UTF8().c_str();
            }
        } 
    }
}

