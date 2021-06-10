//
//  JJLock.cpp
//  J&J Lock
//
//  Created by Yuvaraj on 05/02/19.
//  Modified by Yuvaran on 31/05/20121
//

#include "JJLock.hpp"
#include "qlogging.h"

AILayerHandle LockLayer = NULL;
void JJLock::SetLockLayerEnable(bool boolLayer)
{
    sAILayer->SetLayerEditable(LockLayer, boolLayer);
}

void JJLock::GetLockLayerHandle(AILayerHandle* lockLayer)
{
    lockLayer = &LockLayer;
}

AIArtHandle JJLock::GetLockIconHandle()
{
    
    AIErr result = kNoErr;
    AIArtHandle artHandle;
    short type;
    ai::UnicodeString iconArtName;
    ASBoolean nameBool = true;
    AppContext appContext(gPlugin->GetPluginRef());
    
    AIArtSpec specs[1] = {kPlacedArt, 0, 0};
    ArtSetHelper artSet(specs, 1);
    size_t count = artSet.GetCount();
    for (size_t i = 0; i < count; i++)
    {
        artHandle = artSet[i];
        result = sAIArt->GetArtType(artHandle, &type);
        aisdk::check_ai_error(result);
        
        if(type == kPlacedArt && sAIArt->HasDictionary(artHandle) && !(sAIArt->IsDictionaryEmpty(artHandle)))
        {
            sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
            if(iconArtName == ai::UnicodeString("LockIcon"))
            {
                return artHandle;
            }
        }
    }
}

AILayerHandle JJLock::CreateLockParentLayer()
{

    AppContext appContext(gPlugin->GetPluginRef());
    AILayerHandle firstLayer;
    sAILayer->GetFirstLayer(&firstLayer);
    sAILayer->GetLayerByTitle(&LockLayer,  ai::UnicodeString("Locking Icons"));

    if(LockLayer==NULL)
    {
        sAILayer->InsertLayer(firstLayer,kPlaceBelowAll , &LockLayer);
        sAILayer->SetLayerTitle(LockLayer, ai::UnicodeString("Locking Icons"));
        sAILayer->SetLayerEditable(LockLayer, false);
    }
    return LockLayer;
}

void JJLock::CreateLockIconSwatch()
{
    AIErr result = kNoErr;
    AICustomColor color;
    AICustomColorHandle colorHandle;
    AISwatchRef newSwatch = NULL;
    AISwatchListRef swatchListRef;
    
    AIDocumentHandle documentHandle;
    sAIDocument->GetDocument(&documentHandle);
    
    sAISwatches->GetSwatchList(documentHandle, &swatchListRef);
    newSwatch = sAISwatches->GetSwatchByName(swatchListRef, ai::UnicodeString("Lock Icon"));
    
    if(newSwatch == NULL)
    {
        AIColor aiColor;
        newSwatch = sAISwatches->InsertNthSwatch(swatchListRef, 4);
        
        color.c.f.cyan    = 0;
        color.c.f.magenta = 1;
        color.c.f.yellow = 1;
        color.c.f.black = 0.10;
        color.flag = kCustomSpotColor;
        color.kind = kCustomFourColor;
        result = sAICustomColor->NewCustomColor(&color, ai::UnicodeString("Lock Icon"), &colorHandle);
        aiColor.kind = kCustomColor;
        aiColor.c.c.tint = 0.0f;
        aiColor.c.c.color = colorHandle;
        sAISwatches->SetAIColor(newSwatch, &aiColor);   
    }
}

void JJLock::CreateOnlyIconAndLockBound(AIArtHandle artHandle, std::string newPlacedFilePathString)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    
    AIArtHandle lockIconArtHandle;
    JJLockPanel jjLockPanel;
    JJLock jjlock;
    LockLayer =  jjlock.CreateLockParentLayer();
    CreateLockIconSwatch();
    if(artHandle != NULL && GetBooleanEntryFromHandleDict(artHandle, "lock") == true)
    {
        ai::UnicodeString lockBoundUUID, lockIconUUID;
        lockBoundUUID = jjLockPanel.CreateUUID();
        SetUnicodeStringEntryToHandleDict(artHandle, "lockBoundID", lockBoundUUID);
        lockIconUUID = jjLockPanel.CreateUUID();
        SetUnicodeStringEntryToHandleDict(artHandle, "lockIconID", lockIconUUID);
        AIBoolean layerVisbleBool;
        sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
        if(!layerVisbleBool)
            sAILayer->SetLayerVisible(LockLayer, true);
        sAILayer->SetLayerEditable(LockLayer, true);
        //set layer to lock to items under lock layer.
        sAILayer->SetCurrentLayer(LockLayer);
 
        AIArtHandle pathArtHandle;
        AIRealRect lockBound = {0, 0, 0, 0};
        AIRealRect pathArtBound = {0, 0, 0, 0};
        AIReal adjustmentOffset = 1;
        ai::int32 lockPosition = 0;
        result =  sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &lockBound);
        aisdk::check_ai_error(result);
        result = sAIShapeConstruction->NewRect(lockBound.top + adjustmentOffset, lockBound.left - adjustmentOffset, lockBound.bottom - adjustmentOffset, lockBound.right + adjustmentOffset, false, &pathArtHandle);
        aisdk::check_ai_error(result);
        SetBooleanEntryToHandleDict(pathArtHandle, "isBoundLock", true);
        SetUnicodeStringEntryToHandleDict(pathArtHandle, "lockBoundID", lockBoundUUID);
        
        aisdk::check_ai_error(result);
        ApplyFillAndStrokeColors(pathArtHandle);
        aisdk::check_ai_error(result);
        
        lockIconArtHandle = CreatePlacedArt(newPlacedFilePathString);
        
        AIRealMatrix matrix;
        AIRealMatrix resizeMatrix;
        
        sAIRealMath->AIRealMatrixSetScale(&resizeMatrix, 1.0f/02.0f, 1.0f/02.0f);
        result = sAITransformArt->TransformArt(lockIconArtHandle, &resizeMatrix, 1.0f, kTransformObjects | kTransformChildren);
        aisdk::check_ai_error(result);
        
        AIRealRect iconBound = {0, 0, 0, 0};
        result =  sAIArt->GetArtTransformBounds(lockIconArtHandle, NULL, kNoStrokeBounds, &iconBound);
        aisdk::check_ai_error(result);
        
        AIReal iconHeight = iconBound.top - iconBound.bottom;
        AIReal horizontalOffset = (lockBound.right - iconBound.left - adjustmentOffset);
        AIReal verticalOffset = (lockBound.top - iconBound.top) + iconHeight - adjustmentOffset;
        sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, verticalOffset);
        result = sAITransformArt->TransformArt(lockIconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
        aisdk::check_ai_error(result);
        
        result =  sAIArt->GetArtTransformBounds(pathArtHandle, NULL, kNoStrokeBounds, &pathArtBound);
        lockPosition = GetIntegerEntryFromHandleDict(artHandle, "lockPos");
        if(lockPosition > 1)
        {
            for(int j=1; j<lockPosition; j++)
            {
                horizontalOffset = (-((pathArtBound.right - pathArtBound.left)/(4)) - adjustmentOffset);
                sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, 0);
                result = sAITransformArt->TransformArt(lockIconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
                aisdk::check_ai_error(result);
            }
        }
        SetBooleanEntryToHandleDict(lockIconArtHandle, "isLockIcon", true);
        SetUnicodeStringEntryToHandleDict(lockIconArtHandle, "lockIconID", lockIconUUID);
        
        sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
        sAILayer->SetLayerEditable(LockLayer, false);
    }
}

void JJLock::SetLayerTitleToHandle(AIArtHandle artHandle)
{
    AppContext appContext(gPlugin->GetPluginRef());
    if(artHandle != NULL)
    {
        ai::UnicodeString layerName;
        AILayerHandle currentLayerHandle;
        sAIArt->GetLayerOfArt(artHandle, &currentLayerHandle);
        sAILayer->GetLayerTitle(currentLayerHandle, layerName);
        if(GetUnicodeStringEntryFromHandleDict(artHandle, "layerName") == ai::UnicodeString(""))
        {
            SetUnicodeStringEntryToHandleDict(artHandle, "layerName", layerName);
        }
    }
}

void JJLock::CreateLockBound(AIArtHandle artHandle, AILayerHandle lockLayer, AIArtHandle placedArtIconHandle, std::string newPlacedFilePathString)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    
    AIArtHandle lockIconArtHandle;
    JJLockPanel jjLockPanel;
    CreateLockIconSwatch();
    if(artHandle != NULL && GetBooleanEntryFromHandleDict(artHandle, "lock") != true)
    {
        ai::UnicodeString lockBoundUUID, lockIconUUID;
        lockBoundUUID = jjLockPanel.CreateUUID();
        SetUnicodeStringEntryToHandleDict(artHandle, "lockBoundID", lockBoundUUID);
        lockIconUUID = jjLockPanel.CreateUUID();
        SetUnicodeStringEntryToHandleDict(artHandle, "lockIconID", lockIconUUID);
        AIBoolean layerVisbleBool;
        sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
        if(!layerVisbleBool)
            sAILayer->SetLayerVisible(LockLayer, true);
        sAILayer->SetLayerEditable(lockLayer, true);
        //set layer to lock to items under lock layer.
        sAILayer->SetCurrentLayer(lockLayer);
        
        AIArtHandle pathArtHandle;
        AIRealRect lockBound = {0, 0, 0, 0};
        AIReal adjustmentOffset = 1;
        result =  sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &lockBound);
        aisdk::check_ai_error(result);
        result = sAIShapeConstruction->NewRect(lockBound.top + adjustmentOffset, lockBound.left - adjustmentOffset, lockBound.bottom - adjustmentOffset, lockBound.right + adjustmentOffset, false, &pathArtHandle);
        aisdk::check_ai_error(result);
        SetBooleanEntryToHandleDict(pathArtHandle, "isBoundLock", true);
        SetUnicodeStringEntryToHandleDict(pathArtHandle, "lockBoundID", lockBoundUUID);
        
        aisdk::check_ai_error(result);
        ApplyFillAndStrokeColors(pathArtHandle);
        aisdk::check_ai_error(result);
        
        lockIconArtHandle = CreatePlacedArt(newPlacedFilePathString);
        
        AIRealMatrix matrix;
        AIRealMatrix resizeMatrix;
        
        sAIRealMath->AIRealMatrixSetScale(&resizeMatrix, 1.0f/02.0f, 1.0f/02.0f);
        result = sAITransformArt->TransformArt(lockIconArtHandle, &resizeMatrix, 1.0f, kTransformObjects | kTransformChildren);
        aisdk::check_ai_error(result);
        
        AIRealRect iconBound = {0, 0, 0, 0};
        result =  sAIArt->GetArtTransformBounds(lockIconArtHandle, NULL, kNoStrokeBounds, &iconBound);
        
        AIReal iconHeight = iconBound.top - iconBound.bottom;
        AIReal horizontalOffset = (lockBound.right - iconBound.left - adjustmentOffset);
        AIReal verticalOffset = (lockBound.top - iconBound.top) + iconHeight - adjustmentOffset;
        sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, verticalOffset);
        result = sAITransformArt->TransformArt(lockIconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
        aisdk::check_ai_error(result);
        
        SetBooleanEntryToHandleDict(lockIconArtHandle, "isLockIcon", true);
        SetUnicodeStringEntryToHandleDict(lockIconArtHandle, "lockIconID", lockIconUUID);
        
        AIBoolean lockBool = true;
        ai::int32 lockCount = 0;
        SetBooleanEntryToHandleDict(artHandle, "lock", lockBool);
        SetIntegerEntryToHandleDict(artHandle, "lockPos", 1);
        GetIntegerEntryToDocumentDict("lockCount", &lockCount);
        SetIntegerEntryToDocumentDict("lockCount", lockCount + 1);
        GetIntegerEntryToDocumentDict("lockCount", &lockCount);
        
        SetLayerTitleToHandle(artHandle);
        
        sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
        sAILayer->SetLayerEditable(lockLayer, false);
    }
    else if(GetBooleanEntryFromHandleDict(artHandle, "lock") == true)
    {
        
        AIBoolean layerVisbleBool;
        sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
        if(!layerVisbleBool)
            sAILayer->SetLayerVisible(LockLayer, true);
        sAILayer->SetLayerEditable(lockLayer, true);
        sAILayer->SetCurrentLayer(lockLayer);
        AIArtHandle iconHandle = NULL;
        AIArtHandle boundHandle = NULL;
        AIRealRect lockArtBound = {0, 0, 0, 0};
        AIRealRect pathArtBound = {0, 0, 0, 0};
        
        AIReal adjustmentOffset = 1;
        result =  sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &lockArtBound);
        
        iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                                   GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
        boundHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                   GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
        
        result =  sAIArt->GetArtTransformBounds(boundHandle, NULL, kNoStrokeBounds, &pathArtBound);
        
        if(iconHandle != NULL)
        {
            ai::UnicodeString oldIconUUID = GetUnicodeStringEntryFromHandleDict(iconHandle, "lockIconID");
            sAIArt->DisposeArt(iconHandle);
            lockIconArtHandle = CreatePlacedArt(newPlacedFilePathString);
            
            
            AIRealMatrix resizeMatrix;
            sAIRealMath->AIRealMatrixSetScale(&resizeMatrix, 1.0f/02.0f, 1.0f/02.0f);
            result = sAITransformArt->TransformArt(lockIconArtHandle, &resizeMatrix, 1.0f, kTransformObjects | kTransformChildren);
            aisdk::check_ai_error(result);
            
            AIRealMatrix matrix;
            AIRealRect iconBound = {0, 0, 0, 0};
            ai::int32 lockPosition = 0;
            result =  sAIArt->GetArtTransformBounds(lockIconArtHandle, NULL, kNoStrokeBounds, &iconBound);

            
            AIReal iconHeight = iconBound.top - iconBound.bottom;
            AIReal horizontalOffset = (lockArtBound.right - iconBound.left - adjustmentOffset);
            AIReal verticalOffset = (lockArtBound.top - iconBound.top) + iconHeight - adjustmentOffset;
            lockPosition = GetIntegerEntryFromHandleDict(artHandle, "lockPos");
           
            sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, verticalOffset);
            result = sAITransformArt->TransformArt(lockIconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
            aisdk::check_ai_error(result);
            
            if(lockPosition > 1)
            {
                for(int j=1; j<lockPosition; j++)
                {
                    horizontalOffset = (-((pathArtBound.right - pathArtBound.left)/(4)) - adjustmentOffset);
                    sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, 0);
                    result = sAITransformArt->TransformArt(lockIconArtHandle, &matrix, 0, kTransformObjects | kTransformChildren);
                    aisdk::check_ai_error(result);
                }
            }
            
            SetLayerTitleToHandle(artHandle);
            SetBooleanEntryToHandleDict(lockIconArtHandle, "isLockIcon", true);
            SetUnicodeStringEntryToHandleDict(lockIconArtHandle, "lockIconID", oldIconUUID);
        }
        sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
        sAILayer->SetLayerEditable(lockLayer, false);
    }
}

ASErr JJLock::CreatePlacedLinkFile(ai::FilePath& file, AIArtHandle &newIconHandle)
{
    ASErr result = kNoErr;
    try {
        ai::FilePath path = file;
        
        AIPlaceRequestData placeReqData;
        placeReqData.m_lPlaceMode = kVanillaPlace; // places the file and returns new art handle
        placeReqData.m_disableTemplate = true; // disable Template placement
        placeReqData.m_hNewArt = NULL; // new art handle
        placeReqData.m_hOldArt = NULL; // art to be replaced
        placeReqData.m_lParam = kPlacedArt ; // type of art to create
        placeReqData.m_pFilePath = NULL; // if NULL, shows File Place dialog

        if (path.Exists(false))
        {
            placeReqData.m_filemethod = 0; // place the file
            placeReqData.m_pFilePath = &path; // place the given file (prompt for file to be placed otherwise)
        }
  
        // Place the file.
        result = sAIPlaced->ExecPlaceRequest(placeReqData);
        aisdk::check_ai_error(result);
        
        // Verify that the handle of the placed art is returned.
        SDK_ASSERT(placeReqData.m_hNewArt);
        
        // Pass handle to placed art back to caller.
        newIconHandle = placeReqData.m_hNewArt;

       result = sAIPlaced->SetPlacedFileSpecification(newIconHandle, path);
       aisdk::check_ai_error(result);
        
        
        AIBoolean updated = false, force = true;
        result = sAIArt->UpdateArtworkLink(newIconHandle, force, &updated);
        aisdk::check_ai_error(result);
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    return result;
}

AIArtHandle JJLock::CreatePlacedArt(std::string iconFileName)
{
     AppContext appContext(gPlugin->GetPluginRef());
     JJLockPanel jjLockPanel;
     AIArtHandle newIconHandle = NULL;

    ai::FilePath newPlacedArtFilePath = ai::FilePath(ai::UnicodeString(iconFileName));
    CreatePlacedLinkFile(newPlacedArtFilePath, newIconHandle);
    return newIconHandle;
}

void JJLock::ApplyFillAndStrokeColors(AIArtHandle artBoundHandle)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    AIPathStyle pathArtStyle;
    result= sAIPathStyle->GetPathStyle(artBoundHandle, &pathArtStyle);
    aisdk::check_ai_error(result);

    AICustomColor customColor;
    AIColor aiColor;
    customColor.c.f.cyan    = 0;
    customColor.c.f.magenta = 1;
    customColor.c.f.yellow = 1;
    customColor.c.f.black = 0.10;
    customColor.flag = kCustomSpotColor;
    customColor.kind = kCustomFourColor;
    
    AICustomColorHandle customColorHandle;
    sAICustomColor->GetCustomColorByName(ai::UnicodeString("Lock Icon"), &customColorHandle);

    pathArtStyle.fillPaint = false;
    pathArtStyle.strokePaint = true;
    pathArtStyle.stroke.dash.length = 1;
    pathArtStyle.stroke.dash.offset = 1;
    pathArtStyle.stroke.dash.array[0] = 1;
    pathArtStyle.stroke.dash.array[1] = 1;
    pathArtStyle.stroke.dash.array[2] = 0.0;
    pathArtStyle.stroke.dash.array[3] = 0.0;
    pathArtStyle.stroke.dash.array[4] = 0.0;
    pathArtStyle.stroke.dash.array[5] = 0.0;
    pathArtStyle.stroke.color.Init();
    
    aiColor.kind = kCustomColor;
    aiColor.c.c.tint = 0.0f;
    aiColor.c.c.color = customColorHandle;
    pathArtStyle.stroke.color = aiColor;
    pathArtStyle.stroke.width = 0.2;
    result = sAIPathStyle->SetPathStyle(artBoundHandle, &pathArtStyle);
}

void JJLock::GetCharacterStroke(std::vector<AIStrokeStyle>* charFeatureStrokeArray, std::vector<int>* charStrokeVisibleBool, AIArtHandle textFrameArt)
{
    AIErr result = kNoErr;
    IApplicationPaint paintRef;
    bool strokeFillColorBool  = true;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
    aisdk::check_ai_error(result);
    ITextRange textRange(textRangeRef);
    IStory story = textRange.GetStory();
    ICharFeatures charFeature;
    ICharInspector charInpector;
    ITextRange duplicateRange = textRange.Clone();
    ai::int16 colorModel;
    sAIDocument->GetDocumentColorModel(&colorModel);
    if (textRange.GetSize() > 0)
    {
        for (int i = textRange.GetStart(); i < textRange.GetEnd(); i++)
        {
            AIStrokeStyle storkeStyle;
            storkeStyle.Init();
            
            if(colorModel == 2)
            {
                storkeStyle.color.kind = kFourColor;
                storkeStyle.color.c.f.black = 1;
                storkeStyle.color.c.f.cyan = 0;
                storkeStyle.color.c.f.magenta = 0;
                storkeStyle.color.c.f.yellow = 0;
                storkeStyle.width = kAIRealZero;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    charInpector = duplicateRange.GetCharInspector();
                    IArrayReal relArray;
                    paintRef = charFeature.GetStrokeColor(&strokeFillColorBool);
                    
                    if(paintRef.GetRef() != NULL && strokeFillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &storkeStyle.color);
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    else
                    {
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    charStrokeVisibleBool->push_back(int(charInpector.GetStroke().GetLast())); // storing stroke visible for (0/1 issue)
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
            else if(colorModel == 1)
            {
                storkeStyle.color.kind = kThreeColor;
                storkeStyle.color.c.rgb.red = 1;
                storkeStyle.color.c.rgb.green = 1;
                storkeStyle.color.c.rgb.blue = 1;
                storkeStyle.width = kAIRealZero;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    charInpector = duplicateRange.GetCharInspector();
                    IArrayReal relArray;
                    paintRef = charFeature.GetStrokeColor(&strokeFillColorBool);

                    if(paintRef.GetRef() != NULL && strokeFillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &storkeStyle.color);
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    else
                    {
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    charStrokeVisibleBool->push_back(int(charInpector.GetStroke().GetLast())); // storing stroke visible for (0/1 issue)
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
            else if(colorModel == 0)
            {
                storkeStyle.color.kind = kGrayColor;
                storkeStyle.color.c.g.gray = 1;
                storkeStyle.width = kAIRealZero;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    charInpector = duplicateRange.GetCharInspector();
                    IArrayReal relArray;
                    paintRef = charFeature.GetStrokeColor(&strokeFillColorBool);
                    
                    if(paintRef.GetRef() != NULL && strokeFillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &storkeStyle.color);
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    else
                    {
                        storkeStyle.width = charInpector.GetLineWidth().GetLast();
                        charFeatureStrokeArray->push_back(storkeStyle);
                    }
                    charStrokeVisibleBool->push_back(int(charInpector.GetStroke().GetLast())); // storing stroke visible for (0/1 issue)
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
        }
    }
}

void JJLock::GetCharacterColor(std::vector<AIFillStyle>* charFeatureColorArray, std::vector<int>* charColorFillBool, AIArtHandle textFrameArt)
{
    AIErr result = kNoErr;
    IApplicationPaint paintRef;
    bool fillColorBool  = true;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
    aisdk::check_ai_error(result);
    ITextRange textRange(textRangeRef);
    IStory story = textRange.GetStory();
    ICharFeatures charFeature;
    ITextRange duplicateRange = textRange.Clone();

    ai::int16 colorModel;
    sAIDocument->GetDocumentColorModel(&colorModel);
    if (textRange.GetSize() > 0)
    {
        for (int i = textRange.GetStart(); i < textRange.GetEnd(); i++)
        {
            AIFillStyle fillStyle;
            fillStyle.Init();
            if(colorModel == 2)
            {
                fillStyle.color.kind = kFourColor;
                fillStyle.color.c.f.black = 1;
                fillStyle.color.c.f.cyan = 0;
                fillStyle.color.c.f.magenta = 0;
                fillStyle.color.c.f.yellow = 0;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    paintRef = charFeature.GetFillColor(&fillColorBool);
                    if(paintRef.GetRef() != NULL && fillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &fillStyle.color);
                        charFeatureColorArray->push_back(fillStyle);
                    }
                    else
                        charFeatureColorArray->push_back(fillStyle);
                    charColorFillBool->push_back(int(fillColorBool));
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
            else if(colorModel == 1)
            {
                fillStyle.color.kind = kThreeColor;
                fillStyle.color.c.rgb.red = 1;
                fillStyle.color.c.rgb.green = 1;
                fillStyle.color.c.rgb.blue = 1;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    paintRef = charFeature.GetFillColor(&fillColorBool);
                    if(paintRef.GetRef() != NULL && fillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &fillStyle.color);
                        charFeatureColorArray->push_back(fillStyle);
                    }
                    else
                        charFeatureColorArray->push_back(fillStyle);
                    charColorFillBool->push_back(int(fillColorBool));
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
            else if(colorModel == 0)
            {
                fillStyle.color.kind = kGrayColor;
                fillStyle.color.c.g.gray = 1;
                try
                {
                    duplicateRange.SetRange(i, i+1);
                    charFeature = duplicateRange.GetUniqueLocalCharFeatures();
                    paintRef = charFeature.GetFillColor(&fillColorBool);
                    if(paintRef.GetRef() != NULL && fillColorBool == true)
                    {
                        sAIATEPaint->GetAIColor(paintRef.GetRef(), &fillStyle.color);
                        charFeatureColorArray->push_back(fillStyle);
                    }
                    else
                        charFeatureColorArray->push_back(fillStyle);
                    charColorFillBool->push_back(int(fillColorBool));
                }
                catch(Exception ex)
                {
                    qDebug() << ex.what();
                }
            }
        }
    }
}


void JJLock::GetCharacterFontProperty(std::vector<ai::UnicodeString>* charfontStyleNameArray, std::vector<ai::UnicodeString>* charfontFamilyNameArray, std::vector<double>* charFontSizeArray, AIArtHandle textFrameArt)
{
    AIErr result = kNoErr;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
    aisdk::check_ai_error(result);
    ITextRange textRange(textRangeRef);
    IStory story = textRange.GetStory();
    ICharFeatures charFeature;
    ITextRange duplicateRange = textRange.Clone();
    bool fontSizeBool = true;
    if (textRange.GetSize() > 0)
    {
        for (int i = textRange.GetStart(); i < textRange.GetEnd(); i++)
        {
            ai::UnicodeString fontStyleName;
            ai::UnicodeString fontFamilyName;
            double charFontSize;
            
            bool isAssigned;
            try
            {
                duplicateRange.SetRange(i, i+1);
                charFeature = duplicateRange.GetUniqueCharFeatures();
                IFont font = charFeature.GetFont(&isAssigned);
                FontRef fontRef = font.GetRef();
                AIFontKey fontKey = NULL;
                sAIFont->FontKeyFromFont(fontRef, &fontKey);
                sAIFont->GetFontStyleNameUnicode(fontKey, fontStyleName, true);
                sAIFont->GetFontFamilyNameUnicode(fontKey, fontFamilyName, true);
                
                charFontSize = charFeature.GetFontSize(&fontSizeBool);
                charFontSizeArray->push_back(charFontSize);
                charfontStyleNameArray->push_back(fontStyleName);
                charfontFamilyNameArray->push_back(fontFamilyName);
            }
            catch(Exception ex)
            {
                qDebug() << ex.what();
            }
        }
    }
}

void JJLock::GetCharacterProperty(std::vector<ai::UnicodeString>* charfontStyleNameArray, std::vector<ai::UnicodeString>* charfontFamilyNameArray, std::vector<double>* charFontSizeArray, std::vector<double>* charHScaleSizeArray, AIArtHandle textFrameArt)
{
    
    AIErr result = kNoErr;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &textRangeRef);
    aisdk::check_ai_error(result);
    ITextRange textRange(textRangeRef);
    IStory story = textRange.GetStory();
    ICharFeatures charFeature;
    ITextRange duplicateRange = textRange.Clone();
    bool fontSizeBool, fontHorizBool = true;
    if (textRange.GetSize() > 0)
    {
        for (int i = textRange.GetStart(); i < textRange.GetEnd(); i++)
        {
            ai::UnicodeString fontStyleName;
            ai::UnicodeString fontFamilyName;
            double charFontSize, charHScaleSize;
            
            bool isAssigned;
            try
            {
                duplicateRange.SetRange(i, i+1);
                charFeature = duplicateRange.GetUniqueCharFeatures();
                IFont font = charFeature.GetFont(&isAssigned);
                FontRef fontRef = font.GetRef();
                AIFontKey fontKey = NULL;
                sAIFont->FontKeyFromFont(fontRef, &fontKey);
                sAIFont->GetFontStyleNameUnicode(fontKey, fontStyleName, true);
                sAIFont->GetFontFamilyNameUnicode(fontKey, fontFamilyName, true);
                
                charFontSize = charFeature.GetFontSize(&fontSizeBool);
                charHScaleSize = charFeature.GetHorizontalScale(&fontHorizBool);
                
                charFontSizeArray->push_back(charFontSize);
                charHScaleSizeArray->push_back(charHScaleSize);
                charfontStyleNameArray->push_back(fontStyleName);
                charfontFamilyNameArray->push_back(fontFamilyName);
            }
            catch(Exception ex)
            {
                qDebug() << ex.what();
            }
        }
    }
}

void JJLock::ReplaceFont(AIArtHandle artHandle, int i, ai::UnicodeString charfontFamilyName, ai::UnicodeString charfontStyleName)
{
    AIErr result = kNoErr;
    ai::int32 fontCount = 0;
    IFont fontUseTypeface;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(artHandle, &textRangeRef);
    ITextRange textRange(textRangeRef);
    ICharFeatures charFeature;
    ITextRange duplicateRange = textRange.Clone();
    
    if (duplicateRange.GetSize() > 0)
    {
        duplicateRange.SetRange(i, i+1);
        charFeature = duplicateRange.GetUniqueLocalCharFeatures();
    }
    result = sAIFont->CountFonts(&fontCount);
    aisdk::check_ai_error(result);

    for (int i = 0; i < fontCount; i++)
    {
        AIFontKey fontKey = NULL;
        ai::UnicodeString fontStyleName, fontFamilyName;
        result = sAIFont->IndexFontList(i, &fontKey);
        aisdk::check_ai_error(result);
        
        sAIFont->GetFontStyleNameUnicode(fontKey, fontStyleName, true);
        sAIFont->GetFontFamilyNameUnicode(fontKey, fontFamilyName, true);

        if (fontFamilyName == charfontFamilyName && fontStyleName == charfontStyleName)
        {
            FontRef fontRef;
            result = sAIFont->FontFromFontKey(fontKey, &fontRef);
            aisdk::check_ai_error(result);
            fontUseTypeface = IFont(fontRef);
            charFeature.SetFont(fontUseTypeface);
            duplicateRange.ReplaceOrAddLocalCharFeatures(charFeature);
            break;
        } 
    }
}

void JJLock::ReplaceFont(AIArtHandle artHandle, int j, ai::UnicodeString charfontFamilyName, ai::UnicodeString charfontStyleName, AIReal charFontSize, AIReal charHScaleSize)
{
    AIErr result = kNoErr;
    ai::int32 fontCount = 0;
    IFont fontUseTypeface;
    TextRangeRef textRangeRef;
    result = sAITextFrame->GetATETextRange(artHandle, &textRangeRef);
    ITextRange textRange(textRangeRef);
    ICharFeatures charFeature;
    ITextRange duplicateRange = textRange.Clone();
    
    if (duplicateRange.GetSize() > 0)
    {
        duplicateRange.SetRange(j, j+1);
        charFeature = duplicateRange.GetUniqueLocalCharFeatures();
    }
    result = sAIFont->CountFonts(&fontCount);
    aisdk::check_ai_error(result);
    
    for (int i = 0; i < fontCount; i++)
    {
        AIFontKey fontKey = NULL;
        ai::UnicodeString fontStyleName, fontFamilyName;
        result = sAIFont->IndexFontList(i, &fontKey);
        aisdk::check_ai_error(result);
        
        sAIFont->GetFontStyleNameUnicode(fontKey, fontStyleName, true);
        sAIFont->GetFontFamilyNameUnicode(fontKey, fontFamilyName, true);
        
        if (fontFamilyName == charfontFamilyName && fontStyleName == charfontStyleName)
        {
            FontRef fontRef;
            result = sAIFont->FontFromFontKey(fontKey, &fontRef);
            aisdk::check_ai_error(result);
            fontUseTypeface = IFont(fontRef);
            charFeature.SetFont(fontUseTypeface);
            charFeature.SetFontSize(charFontSize);
            if(charHScaleSize != 0.0)
                charFeature.SetHorizontalScale(charHScaleSize);
            duplicateRange.ReplaceOrAddLocalCharFeatures(charFeature);
            break;
        }
    }
}

void JJLock::TransformLockAndBound(AIArtHandle artHandle)
{
    LockLayer = CreateLockParentLayer();
    AIBoolean layerVisbleBool;
    sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
    if(!layerVisbleBool)
        sAILayer->SetLayerVisible(LockLayer, true);
    sAILayer->SetLayerEditable(LockLayer, true);
    
    AIArtHandle boundHandle = NULL;
    boundHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
    AIArtHandle iconHandle = NULL;
    iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                               GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
    TransformLockArt(artHandle, boundHandle, iconHandle);
    
    sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
    sAILayer->SetLayerEditable(LockLayer, false);
}

AIArtHandle JJLock::FindArtHandleUsingUUIDAndType(AIArtType artType, std::string key, ai::UnicodeString UUID)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    
    if(artType == kPathArt)
    {
        AIArtSpec specs[1] = {{kPathArt, 0, 0}};
        ArtSetHelper artSet(specs, 1);
        size_t count = artSet.GetCount();
        
        AIArtHandle artHandle = NULL;
        ai::UnicodeString ID;
        for (size_t i = 0; i < count; i++)
        {
            artHandle = artSet[i];
            aisdk::check_ai_error(result);
            if(artHandle != NULL && sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle))
            {
                if((GetUnicodeStringEntryFromHandleDict(artHandle, key) == UUID) &&  (GetBooleanEntryFromHandleDict(artHandle, "isBoundLock") == true))
                {
                    return artHandle;
                }
            }
        }
    }
    else if (artType == kGroupArt)
    {
        AIArtSpec specs[1] = {{kGroupArt, 0, 0}};
        ArtSetHelper artSet(specs, 1);
        size_t count = artSet.GetCount();
        
        AIArtHandle artHandle = NULL;
        ai::UnicodeString ID;
        for (size_t i = 0; i < count; i++)
        {
            artHandle = artSet[i];
            aisdk::check_ai_error(result);
            if(artHandle != NULL && sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle))
            {
                if((GetUnicodeStringEntryFromHandleDict(artHandle, key) == UUID) &&  (GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true))
                {
                    return artHandle;
                }
            }
        }
    }
    else if (artType == kPlacedArt)
    {
        AIArtSpec specs[1] = {{kPlacedArt, 0, 0}};
        ArtSetHelper artSet(specs, 1);
        size_t count = artSet.GetCount();
        
        AIArtHandle artHandle = NULL;
        ai::UnicodeString ID;
        for (size_t i = 0; i < count; i++)
        {
            artHandle = artSet[i];
            aisdk::check_ai_error(result);
            if(artHandle != NULL && sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle))
            {
                if((GetUnicodeStringEntryFromHandleDict(artHandle, key) == UUID) &&  (GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true))
                {
                    return artHandle;
                }
            }
        }
    }
    return NULL;
}

void JJLock::TransformAdjustment(AIArtHandle transformHandle, AIRealRect parentBound, AIRealRect childBound, AIReal adjustmentOffset, AIReal pathStrokeWidth)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    AIRealMatrix matrixIcon;
    if(transformHandle !=NULL)
    {
        AIReal horizontalOffsetIcon = (parentBound.left - childBound.left) - (adjustmentOffset + ((pathStrokeWidth/2) + ((pathStrokeWidth/2)/2)));
        AIReal verticalOffsetIcon = (parentBound.top - childBound.top) + (adjustmentOffset + (pathStrokeWidth/2));
        sAIRealMath->AIRealMatrixSetTranslate(&matrixIcon, horizontalOffsetIcon, verticalOffsetIcon);
        result = sAITransformArt->TransformArt(transformHandle, &matrixIcon, 0, kTransformObjects);
        aisdk::check_ai_error(result);
    }
}

void JJLock::TransformMatrixForTextFrame(AIArtHandle artHandle, float scaleRatio, bool vDirection, bool hDirection)
{
    AIErr result = kNoErr;
    AIRealMatrix matrix;
    AIBoolean layerVisbleBool;
    sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
    if(!layerVisbleBool)
        sAILayer->SetLayerVisible(LockLayer, true);
    sAILayer->SetLayerEditable(LockLayer, true);
    
    AIArtHandle pathHandle = NULL;
    pathHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
    
    if(pathHandle != NULL)
    {
        if(hDirection)
        {
            sAIRealMath->AIRealMatrixSetScale(&matrix, 1, scaleRatio);
            result = sAITransformArt->TransformArt(pathHandle, &matrix, 1.0f, kTransformObjects);
            aisdk::check_ai_error(result);
            
        }
        if(vDirection)
        {
            sAIRealMath->AIRealMatrixSetScale(&matrix, scaleRatio, 1);
            result = sAITransformArt->TransformArt(pathHandle, &matrix, 1.0f, kTransformObjects);
            aisdk::check_ai_error(result);
        }
    }
    sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
    sAILayer->SetLayerEditable(LockLayer, false);
}

void JJLock::ReCreatePathHandle(AIArtHandle artHandle)
{
     AIErr result = kNoErr;
     JJLockPanel jjLockPanel;
     AIBoolean layerVisbleBool;
     LockLayer = CreateLockParentLayer();
     sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
     if(!layerVisbleBool)
         sAILayer->SetLayerVisible(LockLayer, true);
     sAILayer->SetLayerEditable(LockLayer, true);
     sAILayer->SetCurrentLayer(LockLayer);
     AIArtHandle pathHandle = NULL;
     pathHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
 
     AIArtHandle iconHandle = NULL;
     iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                                GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
 
     DeleteDictionaryForAnEntry(pathHandle, "isBoundLock");
     DeleteDictionaryForAnEntry(pathHandle, "lockBoundID");
     sAIArt->DisposeArt(pathHandle);
 
     ai::UnicodeString lockBoundUUID, lockIconUUID;
     lockBoundUUID = GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID");
     lockIconUUID = GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID");
     AIArtHandle pathArtHandle;
     AIRealRect lockBound = {0, 0, 0, 0};
     AIReal adjustmentOffset = 1;
     result =  sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &lockBound);
     aisdk::check_ai_error(result);
     result = sAIShapeConstruction->NewRect(lockBound.top + adjustmentOffset, lockBound.left - adjustmentOffset, lockBound.bottom - adjustmentOffset, lockBound.right + adjustmentOffset, false, &pathArtHandle);
     aisdk::check_ai_error(result);
     SetBooleanEntryToHandleDict(pathArtHandle, "isBoundLock", true);
     SetUnicodeStringEntryToHandleDict(pathArtHandle, "lockBoundID", lockBoundUUID);
 
     aisdk::check_ai_error(result);
     ApplyFillAndStrokeColors(pathArtHandle);
     aisdk::check_ai_error(result);

    if(iconHandle == NULL)
    {
        if(GetBooleanEntryFromHandleDict(artHandle, "clock"))
            iconHandle = CreatePlacedArt(jjLockPanel.GetIconImagePath("clock.pdf"));
        else if(GetBooleanEntryFromHandleDict(artHandle, "plock"))
            iconHandle = CreatePlacedArt(jjLockPanel.GetIconImagePath("plock.pdf"));
        else if(GetBooleanEntryFromHandleDict(artHandle, "pclock"))
            iconHandle = CreatePlacedArt(jjLockPanel.GetIconImagePath("pclock.pdf"));
        
        AIRealMatrix resizeMatrix;
        sAIRealMath->AIRealMatrixSetScale(&resizeMatrix, 1.0f/02.0f, 1.0f/02.0f);
        result = sAITransformArt->TransformArt(iconHandle, &resizeMatrix, 1.0f, kTransformObjects | kTransformChildren);
        aisdk::check_ai_error(result);
  
        SetBooleanEntryToHandleDict(iconHandle, "isLockIcon", true);
        SetUnicodeStringEntryToHandleDict(iconHandle, "lockIconID", lockIconUUID);
    }

     AIRealMatrix matrix;
     AIRealRect iconBound = {0, 0, 0, 0};
     result =  sAIArt->GetArtTransformBounds(iconHandle, NULL, kNoStrokeBounds, &iconBound);

     AIReal iconHeight = iconBound.top - iconBound.bottom;
     AIReal horizontalOffset = (lockBound.right - iconBound.left - adjustmentOffset);
     AIReal verticalOffset = (lockBound.top - iconBound.top) + iconHeight - adjustmentOffset;

     sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, verticalOffset);
     result = sAITransformArt->TransformArt(iconHandle, &matrix, 0, kTransformObjects | kTransformChildren);
    
    ai::int32 lockPosition = 0;
    AIRealRect pathArtBound = {0, 0, 0, 0};
    result =  sAIArt->GetArtTransformBounds(pathArtHandle, NULL, kNoStrokeBounds, &pathArtBound);
    lockPosition = GetIntegerEntryFromHandleDict(artHandle, "lockPos");
    if(lockPosition > 1)
    {
        for(int j=1; j<lockPosition; j++)
        {
            horizontalOffset = (-((pathArtBound.right - pathArtBound.left)/(4)) - adjustmentOffset);
            sAIRealMath->AIRealMatrixSetTranslate(&matrix, horizontalOffset, 0);
            result = sAITransformArt->TransformArt(iconHandle, &matrix, 0, kTransformObjects | kTransformChildren);
            aisdk::check_ai_error(result);
        }
    }
     sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
     sAILayer->SetLayerEditable(LockLayer, false);
}

void JJLock::TransformLockArt(AIArtHandle parentHandle, AIArtHandle pathHandle, AIArtHandle iconHandle)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    AIReal adjustmentOffset = 1;
    AIReal pathStrokeWidth = 1;
    AIRealRect parentBound = {0, 0, 0, 0};
    result = sAIArt->GetArtTransformBounds(parentHandle, NULL, kNoStrokeBounds, &parentBound);

    AIRealRect pathBound = {0, 0, 0, 0};
    result = sAIArt->GetArtTransformBounds(pathHandle, NULL, kNoStrokeBounds, &pathBound);
    
    TransformAdjustment(pathHandle, parentBound, pathBound, adjustmentOffset, pathStrokeWidth);
    TransformAdjustment(iconHandle, parentBound, pathBound, adjustmentOffset, pathStrokeWidth);
}

void JJLock::ClearBoundAndLockEntry(ai::UnicodeString lockBoundID, ai::UnicodeString lockIconID, bool isLockCountApplicable)
{
    AIBoolean layerVisbleBool;
    sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
    if(!layerVisbleBool)
        sAILayer->SetLayerVisible(LockLayer, true);
    sAILayer->SetLayerEditable(LockLayer, true);
    AIArtHandle boundHandle = NULL;
    if(lockBoundID != ai::UnicodeString(""))
        boundHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID", lockBoundID);
    AIArtHandle iconHandle = NULL;
    if(lockIconID != ai::UnicodeString(""))
        iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID", lockIconID);
    
    if(iconHandle != NULL)
    {
        DeleteDictionaryForAnEntry(iconHandle, "isLockIcon");
        DeleteDictionaryForAnEntry(iconHandle, "lockIconID");
        sAIArt->DisposeArt(iconHandle);
    }
    if(boundHandle != NULL)
    {
        DeleteDictionaryForAnEntry(boundHandle, "isBoundLock");
        DeleteDictionaryForAnEntry(boundHandle, "lockBoundID");
        sAIArt->DisposeArt(boundHandle);
    }
    if(isLockCountApplicable)
    {
        ai::int32 lockCount = 0;
        GetIntegerEntryToDocumentDict("lockCount", &lockCount);
        if(lockCount!=0)
            SetIntegerEntryToDocumentDict("lockCount", lockCount-1);
        else
            SetIntegerEntryToDocumentDict("lockCount", 0);
    }
    sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
    sAILayer->SetLayerEditable(LockLayer, false);
}

bool JJLock::ClearMissingArtLockAndBound(vector<ai::UnicodeString> sourceIDList, vector<ai::UnicodeString> checkIDListExists, ai::UnicodeString typeOfLock)
{
    vector<ai::UnicodeString> missingList;
    int listCount = sourceIDList.size()-1;
    int checkCount = checkIDListExists.size();
    int incrementCount = 0;
    bool corruptedArtWork = false;
    while(listCount >= 0 && incrementCount < checkCount && (sourceIDList.size() != checkIDListExists.size()))
    {
        if(checkIDListExists.at(incrementCount) == sourceIDList.at(listCount))
        {
            listCount = sourceIDList.size()-1;
            incrementCount++;
        }
        listCount--;
        if(listCount == -1)
        {
             listCount = sourceIDList.size()-1;
            if(typeOfLock == ai::UnicodeString("boundLockType"))
                ClearBoundAndLockEntry(checkIDListExists.at(incrementCount), ai::UnicodeString(""), false); //lock bound type
            else if(typeOfLock == ai::UnicodeString("iconLockType"))
                ClearBoundAndLockEntry(ai::UnicodeString(""), checkIDListExists.at(incrementCount), false); //lock icon type
            incrementCount++;
            corruptedArtWork = true;
        }
            
    }
    return corruptedArtWork;
}

QStringList dictKeyStringList = {"charStrokeStyle", "charStrokeStyleBool","charFillStyle", "charFillStyleBool", "charFontFamilyName", "charFontStyleName", "JJLockArtBoundBottom", "JJLockArtBoundLeft", "JJLockArtBoundRight", "JJLockArtBoundTop", "JJLockArtOwnBoundBottom", "JJLockArtOwnBoundLeft", "JJLockArtOwnBoundRight", "JJLockArtOwnBoundTop", "JJLockContent", "lock", "lockBoundID", "lockIconID", "ParaJustification", "charHScaleSize", "clock", "pclock", "plock", "layerName", "compPathFillStyle", "compPathStrokeStyle", "textFrameFillStyleBool", "textFrameFillStyle", "textFrameStrokeStyleBool", "textFrameStrokeStyle", "charFontSize"};

void JJLock::ClearDictEntries(AIArtHandle artHandle)
{
    LockLayer = CreateLockParentLayer();
    AIBoolean layerVisbleBool;
    sAILayer->GetLayerVisible(LockLayer, &layerVisbleBool);
    if(!layerVisbleBool)
        sAILayer->SetLayerVisible(LockLayer, true);
    sAILayer->SetLayerEditable(LockLayer, true);
    AIArtHandle boundHandle = NULL;
    boundHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
    AIArtHandle iconHandle = NULL;
    iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                               GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
    
    if(iconHandle != NULL)
    {
        DeleteDictionaryForAnEntry(iconHandle, "isLockIcon");
        DeleteDictionaryForAnEntry(iconHandle, "lockIconID");
        sAIArt->DisposeArt(iconHandle);
    }
    
    if(boundHandle != NULL)
    {
        DeleteDictionaryForAnEntry(boundHandle, "isBoundLock");
        DeleteDictionaryForAnEntry(boundHandle, "lockBoundID");
        sAIArt->DisposeArt(boundHandle);
    }
    DeleteDictionaryUsingIteratorCheck(artHandle, dictKeyStringList);
    ai::int32 lockCount = 0;
    GetIntegerEntryToDocumentDict("lockCount", &lockCount);
    if(lockCount!=0)
        SetIntegerEntryToDocumentDict("lockCount", lockCount-1);
    else
        SetIntegerEntryToDocumentDict("lockCount", 0);
    sAILayer->SetLayerVisible(LockLayer, layerVisbleBool);
    sAILayer->SetLayerEditable(LockLayer, false);
}

void JJLock::ParseGroupChildArtandClearLock(AIArtHandle groupArtHandle)
{
    JJLockPanel jjLockPanel;
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
                if(GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
                {
                    int totalChildCount = 0;
                    ParseGroupChildForRemoveDictEntriesOrSetForPlock(lastChildHandle, &totalChildCount, true);
                    if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                    {
                         jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                    }
                    ClearDictEntries(lastChildHandle);
                }
                ParseGroupChildArtandClearLock(lastChildHandle);
            }
            else if(childArtType == kCompoundPathArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            else if(childArtType == kTextFrameArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            else if(childArtType == kPathArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            else if(childArtType == kPluginArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            else if(childArtType == kPlacedArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            else if(childArtType == kSymbolArt && GetBooleanEntryFromHandleDict(lastChildHandle, "lock") == true)
            {
                if(GetUnicodeStringEntryFromHandleDict(lastChildHandle, "ParentID") != ai::UnicodeString(""))
                {
                    jjLockPanel.CheckAndReleaseParentIDForChild(lastChildHandle);
                }
                ClearDictEntries(lastChildHandle);
            }
            sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
        }
    }
}

void JJLock::ParseGroupChildForRemoveDictEntriesOrSetForPlock(AIArtHandle groupArtHandle, int* totalChildCount, bool remove)
{
    
    ASErr result = kNoErr;
    JJLockPanel objJJLockPanel;
    AIArtHandle lastChildHandle;
    short artType;
    sAIArt->GetArtType(groupArtHandle, &artType);
    if(artType == kGroupArt)
    {
        if(remove)
        {
            DeleteDictionaryForAnEntry(groupArtHandle, "totalGroupChildCount");
            DeleteDictionaryForAnEntry(groupArtHandle, "GGroupID");
            DeleteDictionaryForAnEntry(groupArtHandle, "GParentID");
        }
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
        
        if(groupArtHandle != NULL && !remove)
        {
            *totalChildCount += 1;
            AIArtHandle parentHandle;
            ai::UnicodeString gLockGID;
            sAIArt->GetArtParent(groupArtHandle, &parentHandle);
            ASBoolean isLayerGroup;
            sAIArt->IsArtLayerGroup(parentHandle, &isLayerGroup);
            if(GetUnicodeStringEntryFromHandleDict(parentHandle, "GGroupID") != ai::UnicodeString(""))
            {
                SetUnicodeStringEntryToHandleDict(groupArtHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(parentHandle, "GGroupID"));
                
                gLockGID = objJJLockPanel.CreateUUID();
                SetUnicodeStringEntryToHandleDict(groupArtHandle, "GGroupID", gLockGID);
            }
            else
            {
                gLockGID = objJJLockPanel.CreateUUID();
                SetUnicodeStringEntryToHandleDict(groupArtHandle, "GGroupID", gLockGID);
            }
        }
    }
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
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GGroupID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
                ParseGroupChildForRemoveDictEntriesOrSetForPlock(lastChildHandle, totalChildCount, remove);
            }
            else if(childArtType == kCompoundPathArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
            }
            else if(childArtType == kTextFrameArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                    
                    std::vector<AIArtHandle> linkArtHanldes;
                    AIBool8 linkedText;
                    sAITextFrame->PartOfLinkedText(lastChildHandle, &linkedText);
                    if(linkedText)
                    {
                        lastChildHandle = GetParentTextFrameFromLinkedTextFrame(lastChildHandle);
                        linkArtHanldes = GetTextFramesFromLinkedTextFrame(lastChildHandle);
                        if(linkArtHanldes.size() > 1)
                            SetIntegerEntryToHandleDict(lastChildHandle, "size", linkArtHanldes.size());
                    }
                    else
                    {
                        if(linkArtHanldes.empty())
                            linkArtHanldes.push_back(lastChildHandle);
                    }
                
                    for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                    {
                        lastChildHandle = linkArtHanldes.at(i);

                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                        AIReal top = artBounds.top;
                        AIReal left = artBounds.left;
                        AIReal right = artBounds.right;
                        AIReal bottom = artBounds.bottom;
                        result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                        aisdk::check_ai_error(result);
                        result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                        aisdk::check_ai_error(result);
                        result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                        aisdk::check_ai_error(result);
                        result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                        aisdk::check_ai_error(result);

                        if(lastChildHandle != NULL )
                        {
                            std::vector<ai::UnicodeString> charfontStyleNameArray;
                            std::vector<ai::UnicodeString> charfontFamilyNameArray;
                            std::vector<double> charFontSizeArray, charHScaleSizeArray;
                            
                            ai::UnicodeString wholeTextFrameContent = GetText(lastChildHandle);
                            result = SetUnicodeEntryToDocumentDict(lastChildHandle, wholeTextFrameContent, "JJLockContent");
                            aisdk::check_ai_error(result);
                            
                            GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, lastChildHandle);
                            
                            SetArrayEntryForUnicodeString(lastChildHandle, "charFontStyleName", charfontStyleNameArray);
                            SetArrayEntryForUnicodeString(lastChildHandle, "charFontFamilyName", charfontFamilyNameArray);
                            SetArrayEntryForReal(lastChildHandle, "charFontSize", charFontSizeArray);
                            SetArrayEntryForReal(lastChildHandle, "charHScaleSize", charHScaleSizeArray);
                            
                            
                            ai::int32 getJustificationValue = 0;
                            GetParagraphJustification(lastChildHandle, &getJustificationValue);
                            result = SetIntegerEntryToHandleDict(lastChildHandle, "ParaJustification", getJustificationValue);
                            aisdk::check_ai_error(result);
                            
                        }
                    }
                }
            }
            else if(childArtType == kPathArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
            }
            else if(childArtType == kPluginArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
            }
            else if(childArtType == kPlacedArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
            }
            else if(childArtType == kSymbolArt)
            {
                if(remove)
                {
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockBool");
                    DeleteDictionaryForAnEntry(lastChildHandle, "groupChildLockType");
                    DeleteDictionaryForAnEntry(lastChildHandle, "GParentID");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLocked");
                    DeleteDictionaryForAnEntry(lastChildHandle, "childLockCount");
                    DeleteDictionaryUsingIteratorCheck(lastChildHandle, dictKeyStringList);
                }
                else
                {
                    
                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(lastChildHandle, NULL, kNoStrokeBounds, &artBounds);
                    AIReal top = artBounds.top;
                    AIReal left = artBounds.left;
                    AIReal right = artBounds.right;
                    AIReal bottom = artBounds.bottom;
                    result = SetRealEntryToDocumentDict(lastChildHandle, top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    result = SetRealEntryToDocumentDict(lastChildHandle, bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    
                    *totalChildCount += 1;
                    if(GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID") != ai::UnicodeString(""))
                    {
                        SetUnicodeStringEntryToHandleDict(lastChildHandle, "GParentID", GetUnicodeStringEntryFromHandleDict(groupArtHandle, "GGroupID"));
                    }
                    SetBooleanEntryToHandleDict(lastChildHandle, "groupChildLockBool", true);
                    SetUnicodeEntryToDocumentDict(lastChildHandle, ai::UnicodeString("plock"), "groupChildLockType");
                }
            }
            sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
        }
    }
}

bool JJLock::IsKeyStringExists(std::string keyString)
{
    if(dictKeyStringList.contains(QString::fromStdString(keyString)))
        return true;
    return false;
}


void JJLock::ClearLockEntry(AIArtHandle artHandle)
{
    short groupType;
    JJLockPanel jjLockPanel;
    sAIArt->GetArtType(artHandle, &groupType);

    if(groupType == kGroupArt && GetBooleanEntryFromHandleDict(artHandle, "lock"))
    {
        int totalChildCount = 0;
        ParseGroupChildForRemoveDictEntriesOrSetForPlock(artHandle, &totalChildCount, true);
    }
    if(GetUnicodeStringEntryFromHandleDict(artHandle, "ParentID") != ai::UnicodeString(""))
    {
        jjLockPanel.CheckAndReleaseParentIDForChild(artHandle);
    }
    ClearDictEntries(artHandle);
}

void JJLock::RecreateLockIconAndBound(AIArtHandle artHandle)
{
    JJLock jjlock;
    JJLockPanel jjLockPanel;

    if(artHandle != NULL)
    {
        AIArtHandle boundHandle = NULL;
        boundHandle = FindArtHandleUsingUUIDAndType(kPathArt, "lockBoundID",
                                                    GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
        AIArtHandle iconHandle = NULL;
        iconHandle = FindArtHandleUsingUUIDAndType(kGroupArt, "lockIconID",
                                                   GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));

        if(GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID") != ai::UnicodeString("") ||
           GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID") != ai::UnicodeString(""))
            if(boundHandle == NULL || iconHandle == NULL)
            {
                if(iconHandle != NULL)
                {
                    DeleteDictionaryForAnEntry(iconHandle, "isLockIcon");
                    DeleteDictionaryForAnEntry(iconHandle, "lockIconID");
                    sAIArt->DisposeArt(iconHandle);
                }
                
                if(boundHandle != NULL)
                {
                    DeleteDictionaryForAnEntry(boundHandle, "isBoundLock");
                    DeleteDictionaryForAnEntry(boundHandle, "lockBoundID");
                    sAIArt->DisposeArt(boundHandle);
                }
                
                if(GetBooleanEntryFromHandleDict(artHandle, "clock"))
                {
                    jjlock.CreateOnlyIconAndLockBound(artHandle, jjLockPanel.GetIconImagePath("clock.pdf"));
                }
                else if(GetBooleanEntryFromHandleDict(artHandle, "plock"))
                {
                     jjlock.CreateOnlyIconAndLockBound(artHandle, jjLockPanel.GetIconImagePath("plock.pdf"));
                }
                else if(GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                {
                     jjlock.CreateOnlyIconAndLockBound(artHandle, jjLockPanel.GetIconImagePath("pclock.pdf"));
                }
            }
    }
}

bool JJLock::IsDocumentOpened()
{
    AIErr result = kNoErr;
    bool docOpenFlag = false;
    try
    {
        AppContext appContext(gPlugin->GetPluginRef());
        AIDocumentHandle document = NULL;
        result = sAIDocument->GetDocument(&document);
        aisdk::check_ai_error(result);
        if (document)
            docOpenFlag = true;
    }
    catch (ai::Error &ErrMsg)
    {
        result = ErrMsg;
    }
    return docOpenFlag;
}

ASErr JJLock::GetSelectedTextFrame(AIArtHandle &frameArt)
{
    ASErr result = kNoErr;
    try {
        
        ATE::TextRangesRef rangesRef = NULL;
        result = sAIDocument->GetTextSelection(&rangesRef);
        aisdk::check_ai_error(result);
        
        ATE::ITextRanges ranges(rangesRef);
        if (ranges.GetSize() > 0) {
            ATE::ITextRange range = ranges.Item(0);
            
            // Get the art handle for the text frame containing the selected text
            ATE::ITextFrame frame = range.GetStory().GetFrame(0);
            ATE::TextFrameRef frameRef = frame.GetRef();
            // Get the art handle for the text frame
            result = sAITextFrame->GetAITextFrame(frameRef, &frameArt);
        }
        else {
            frameArt = NULL;
        }
    } catch (ai::Error &ex) {
        result = ex;
    }
    return result;
}

ai::UnicodeString JJLock::GetText(AIArtHandle textFrameArt)
{
    AIErr result = kNoErr;
    ai::UnicodeString unicodeContent;
    TextRangeRef allTextRangeRef;
    result = sAITextFrame->GetATETextRange(textFrameArt, &allTextRangeRef);
    ITextRange allTextRange(allTextRangeRef);
    ASInt32 rangeSize = allTextRange.GetSize();
    
    ai::AutoBuffer<ASUnicode> content(rangeSize);
    allTextRange.GetContents(content, rangeSize);
    if (content.IsValid()) {
        unicodeContent = ai::UnicodeString(content, content.GetCount());
    }
    return unicodeContent;
}

ai::UnicodeString JJLock::GetTextFromLinkedTextFrame(AIArtHandle artHandle)
{
    ASErr result = kNoErr;
    std::vector<ITextFrame> txtFrames;
    ai::UnicodeString textContent;
    try {
        TextFrameRef textFrameRef = NULL;
        result = sAITextFrame->GetATETextFrame(artHandle, &textFrameRef);
        aisdk::check_ai_error(result);
        ITextFrame textFrame(textFrameRef);
        // Get story from linked text frame
        ITextRange textRange = textFrame.GetTextRange();
        IStory story = textRange.GetStory();
        ITextRange storyRange = story.GetTextRange();
        ai::AutoBuffer<ASUnicode> contents(storyRange.GetSize());
        storyRange.GetContents(contents, storyRange.GetSize());
        textContent = (ai::UnicodeString(contents, contents.GetCount()));
        
        if (textContent.at(textContent.length() - 1) == 13) {
            textContent = textContent.erase(textContent.length() - 1, 1);
        }
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    catch (ATE::Exception& ex) {
        result = ex.error;
    }
    return textContent;
}

std::vector<AIArtHandle> JJLock::GetTextFramesFromLinkedTextFrame(AIArtHandle textFrameArt)
{
    std::vector<AIArtHandle> textFrameArtSetArray;
    std::vector<ITextFrame> textFrameArtSet;
    TextFrameRef textFrameRef = NULL;
    sAITextFrame->GetATETextFrame(textFrameArt, &textFrameRef);
    ITextFrame textFrame(textFrameRef);
    // Get all the stories in the current document.
    ITextRange textRange = textFrame.GetTextRange();
    IStory story = textRange.GetStory();
    ITextRange storyRange = story.GetTextRange();
    ITextFramesIterator framesIter = storyRange.GetTextFramesIterator();
    if (!framesIter.IsEmpty())
    {
        while (framesIter.IsNotDone())
        {
            textFrameArtSet.push_back(framesIter.Item());
            framesIter.Next();
        }
    }
    else
    {
        aisdk::check_ai_error(kBadParameterErr);
    }
    for (int i = 0; i < textFrameArtSet.size(); i++) {
        TextFrameRef textFrameRef = textFrameArtSet[i].GetRef();
        AIArtHandle textFrameArt = NULL;
        sAITextFrame->GetAITextFrame(textFrameRef, &textFrameArt);
        textFrameArtSetArray.push_back(textFrameArt);
    }
    return textFrameArtSetArray;
}


AIArtHandle JJLock::GetParentTextFrameFromLinkedTextFrame(AIArtHandle textFrameArt)
{
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
        }
    }
    return artHandle;
}


AIRealRect JJLock::SetArtHandleBoundInDict(AIArtHandle artHandle){
    AIErr result;
    AIRealRect artBounds = {0,0,0,0};
    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
    AIReal top = artBounds.top;
    AIReal left = artBounds.left;
    AIReal right = artBounds.right;
    AIReal bottom = artBounds.bottom;
    result = SetRealEntryToDocumentDict(artHandle, top, "JJLockArtBoundTop");
    aisdk::check_ai_error(result);
    result = SetRealEntryToDocumentDict(artHandle, left, "JJLockArtBoundLeft");
    aisdk::check_ai_error(result);
    result = SetRealEntryToDocumentDict(artHandle, right, "JJLockArtBoundRight");
    aisdk::check_ai_error(result);
    result = SetRealEntryToDocumentDict(artHandle, bottom, "JJLockArtBoundBottom");
    aisdk::check_ai_error(result);
    
}

ai::UnicodeString JJLock::GetUnicodeStringEntryFromHandleDict(AIArtHandle artHandle, std::string key)
{
    AIErr result = kNoErr;
    ai::UnicodeString value;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetUnicodeStringEntry(artDict, dictKey, value);
            aisdk::check_ai_error(result);
            return value;
        }
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return ai::UnicodeString("");
}

AIErr JJLock::SetUnicodeStringEntryToHandleDict(AIArtHandle artHandle, std::string key, ai::UnicodeString value)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetUnicodeStringEntry(artDict, dictKey, value);
        aisdk::check_ai_error(result);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

int JJLock::GetIntegerEntryFromHandleDict(AIArtHandle artHandle, std::string key)
{
    AIErr result = kNoErr;
    int value = 0;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetIntegerEntry(artDict, dictKey, &value);
            aisdk::check_ai_error(result);
            return value;
        }
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return NULL;
}

AIErr JJLock::SetIntegerEntryToHandleDict(AIArtHandle artHandle, std::string key, int value)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetIntegerEntry(artDict, dictKey, value);
        aisdk::check_ai_error(result);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIBoolean JJLock::GetBooleanEntryFromHandleDict(AIArtHandle artHandle, std::string key)
{
    AIErr result = kNoErr;
    AIBoolean value = false;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    
    try
    {
        if(sAIArt->HasDictionary(artHandle))
        {
            result = sAIArt->GetDictionary(artHandle, &artDict);
            aisdk::check_ai_error(result);
            AIDictKey dictKey = sAIDictionary->Key(key.c_str());
            if (sAIDictionary->IsKnown(artDict, dictKey))
            {
                result = sAIDictionary->GetBooleanEntry(artDict, dictKey, &value);
                aisdk::check_ai_error(result);
               
            }
            sAIDictionary->Release(artDict);
            return value;
        }
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    return NULL;
}

AIErr JJLock::SetBooleanEntryToHandleDict(AIArtHandle artHandle, std::string key, AIBoolean value)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetBooleanEntry(artDict, dictKey, value);
        aisdk::check_ai_error(result);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetIntegerEntryToDocumentDict(std::string key, ai::int32 intValue)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        result = sAIDocument->GetDictionary(&artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetIntegerEntry(artDict, dictKey, intValue);
        aisdk::check_ai_error(result);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetIntegerEntryToDocumentDict(std::string key, ai::int32* intValue)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        result = sAIDocument->GetDictionary(&artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetIntegerEntry(artDict, dictKey, intValue);
            aisdk::check_ai_error(result);
        }
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}



AIErr JJLock::SetArrayEntryForBoolean(AIArtHandle artHandle, std::string key, std::vector<bool> boolArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef booleanArray;
        result = sAIArray->CreateArray(&booleanArray);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < boolArray.size(); i++)
        {
            result = sAIArray->AppendEntry(booleanArray, sAIEntry->FromBoolean(boolArray.at(i)));
            aisdk::check_ai_error(result);
        }
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, booleanArray);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(booleanArray);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForBoolean(AIArtHandle artHandle, std::string key, std::vector<bool>* boolArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef booleanArray;
        result = sAIArray->CreateArray(&booleanArray);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &booleanArray);
            aisdk::check_ai_error(result);
        }
        for (int i = 0; i < sAIArray->Size(booleanArray) ; i++)
        {
            ASBoolean boolValue;
            result = sAIArray->GetBooleanEntry(booleanArray, i, &boolValue);
            aisdk::check_ai_error(result);
            boolArray->push_back(boolValue);
        }
        sAIArray->Release(booleanArray);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetArrayEntryForReal(AIArtHandle artHandle, std::string key, std::vector<AIReal> realArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef RealArray;
        result = sAIArray->CreateArray(&RealArray);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < realArray.size(); i++)
        {
            result = sAIArray->AppendEntry(RealArray, sAIEntry->FromReal(realArray.at(i)));
            aisdk::check_ai_error(result);
        }
        
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, RealArray);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(RealArray);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForReal(AIArtHandle artHandle, std::string key, std::vector<AIReal>* realArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef realArrayRef;
        result = sAIArray->CreateArray(&realArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &realArrayRef);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(realArrayRef) ; i++)
        {
            AIReal realValue;
            result = sAIEntry->ToReal(sAIArray->Get(realArrayRef, i), &realValue);
            aisdk::check_ai_error(result);
            realArray->push_back(realValue);

        }
        sAIArray->Release(realArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetArrayEntryForUnicodeString(AIArtHandle artHandle, std::string key, std::vector<ai::UnicodeString> unicodeStringArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef unicodeStringArrayRef;
        result = sAIArray->CreateArray(&unicodeStringArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < unicodeStringArray.size(); i++)
        {
            result = sAIArray->AppendEntry(unicodeStringArrayRef, sAIEntry->FromUnicodeString(unicodeStringArray.at(i)));
            aisdk::check_ai_error(result);
        }
        
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, unicodeStringArrayRef);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(unicodeStringArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForUnicodeString(AIArtHandle artHandle, std::string key, std::vector<ai::UnicodeString>* unicodeStringArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef unicodeStringArrayRef;
        result = sAIArray->CreateArray(&unicodeStringArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &unicodeStringArrayRef);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(unicodeStringArrayRef) ; i++)
        {
            ai::UnicodeString unicodeStringValue;
            result = sAIArray->GetUnicodeStringEntry(unicodeStringArrayRef, i, unicodeStringValue);
            aisdk::check_ai_error(result);
            unicodeStringArray->push_back(unicodeStringValue);
        }
        
        sAIArray->Release(unicodeStringArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetArrayEntryForInteger(AIArtHandle artHandle, std::string key, std::vector<int> intArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef IntegerArray;
        result = sAIArray->CreateArray(&IntegerArray);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < intArray.size(); i++)
        {
            result = sAIArray->AppendEntry(IntegerArray, sAIEntry->FromInteger(intArray.at(i)));
            aisdk::check_ai_error(result);
            
        }
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, IntegerArray);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(IntegerArray);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForInteger(AIArtHandle artHandle, std::string key, std::vector<int>* intArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef integerArray;
        result = sAIArray->CreateArray(&integerArray);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &integerArray);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(integerArray) ; i++)
        {
            int intValue;
            result = sAIArray->GetIntegerEntry(integerArray, i, &intValue);
            aisdk::check_ai_error(result);
            intArray->push_back(intValue);
        }
        sAIArray->Release(integerArray);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetArrayEntryForStrokeStyle(AIArtHandle artHandle, std::string key, std::vector<AIStrokeStyle> strokeStyleArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef strokeFillStyleArrayRef;
        result = sAIArray->CreateArray(&strokeFillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < strokeStyleArray.size(); i++)
        {
            result = sAIArray->AppendEntry(strokeFillStyleArrayRef, sAIEntry->FromStrokeStyle(&strokeStyleArray.at(i)));
            aisdk::check_ai_error(result);
        }
        
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, strokeFillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(strokeFillStyleArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}



AIErr JJLock::GetArrayEntryForStrokeStyle(AIArtHandle artHandle, std::string key, std::vector<AIStrokeStyle>* fillStyleArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef strokeFillStyleArrayRef;
        result = sAIArray->CreateArray(&strokeFillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &strokeFillStyleArrayRef);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(strokeFillStyleArrayRef) ; i++)
        {
            AIStrokeStyle aiStrokeStyle;
            sAIArray->Get(strokeFillStyleArrayRef, i);
            sAIEntry->ToStrokeStyle(sAIArray->Get(strokeFillStyleArrayRef, i), &aiStrokeStyle);
            aisdk::check_ai_error(result);
            fillStyleArray->push_back(aiStrokeStyle);
        }
        
        sAIArray->Release(strokeFillStyleArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}


AIErr JJLock::SetArrayEntryForFillStyle(AIArtHandle artHandle, std::string key, std::vector<AIFillStyle> fillStyleArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef fillStyleArrayRef;
        result = sAIArray->CreateArray(&fillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < fillStyleArray.size(); i++)
        {
            result = sAIArray->AppendEntry(fillStyleArrayRef, sAIEntry->FromFillStyle(&fillStyleArray.at(i)));
            aisdk::check_ai_error(result);
        }
        
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, fillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(fillStyleArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForFillStyle(AIArtHandle artHandle, std::string key, std::vector<AIFillStyle>* fillStyleArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef fillStyleArrayRef;
        result = sAIArray->CreateArray(&fillStyleArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &fillStyleArrayRef);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(fillStyleArrayRef) ; i++)
        {
            AIFillStyle aiFillStyle;
            sAIArray->Get(fillStyleArrayRef, i);
            sAIEntry->ToFillStyle(sAIArray->Get(fillStyleArrayRef, i), &aiFillStyle);
            aisdk::check_ai_error(result);
            fillStyleArray->push_back(aiFillStyle);
        }
        
        sAIArray->Release(fillStyleArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetArrayEntryForRealPoint(AIArtHandle artHandle, std::string key, std::vector<AIRealPoint> realPointArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef realPointArrayRef;
        result = sAIArray->CreateArray(&realPointArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        for (int i = 0; i < realPointArray.size(); i++)
        {
            result = sAIArray->AppendEntry(realPointArrayRef, sAIEntry->FromRealPoint(&realPointArray.at(i)));
            aisdk::check_ai_error(result);
        }
        result = sAIDictionary->SetArrayEntry(artDict, dictKey, realPointArrayRef);
        aisdk::check_ai_error(result);
        
        sAIArray->Release(realPointArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::GetArrayEntryForRealPoint(AIArtHandle artHandle, std::string key, std::vector<AIRealPoint>* realPointArray)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    
    AppContext appContext(gPlugin->GetPluginRef());
    try
    {
        AIArrayRef realPointArrayRef;
        result = sAIArray->CreateArray(&realPointArrayRef);
        aisdk::check_ai_error(result);
        
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        
        if (sAIDictionary->IsKnown(artDict, dictKey))
        {
            result = sAIDictionary->GetArrayEntry(artDict, dictKey, &realPointArrayRef);
            aisdk::check_ai_error(result);
        }
        
        for (int i = 0; i < sAIArray->Size(realPointArrayRef) ; i++)
        {
            AIRealPoint aiRealPoint;
            aiRealPoint.h = 0;
            aiRealPoint.v = 0;
            sAIArray->Get(realPointArrayRef, i);
            sAIEntry->ToRealPoint(sAIArray->Get(realPointArrayRef, i), &aiRealPoint);
            aisdk::check_ai_error(result);
            realPointArray->push_back(aiRealPoint);
        }
        sAIArray->Release(realPointArrayRef);
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    sAIDictionary->Release(artDict);
    return result;
}

AIErr JJLock::SetRealEntryToDocumentDict(AIArtHandle artHandle, AIReal realValue, std::string key)
{
    AIErr result = kNoErr;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetRealEntry(artDict, dictKey, realValue);
        aisdk::check_ai_error(result);
        
        sAIDictionary->Release(artDict);
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

AIErr JJLock::GetRealEntryFromDocumentDict(AIArtHandle artHandle, AIReal *realValue, std::string key)
{
    AIErr result = kNoErr;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey)){
            result = sAIDictionary->GetRealEntry(artDict, dictKey, realValue);
            aisdk::check_ai_error(result);
        }
        
        sAIDictionary->Release(artDict);
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

AIErr JJLock::SetUnicodeEntryToDocumentDict(AIArtHandle artHandle, ai::UnicodeString artContent, std::string key)
{
    AIErr result = kNoErr;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        result = sAIDictionary->SetUnicodeStringEntry(artDict, dictKey, artContent);
        aisdk::check_ai_error(result);
        
        sAIDictionary->Release(artDict);
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

AIErr JJLock::GetUnicodeEntryFromDocumentDict(AIArtHandle artHandle, ai::UnicodeString *artContent, std::string key)
{
    AIErr result = kNoErr;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey)) {
            result = sAIDictionary->GetUnicodeStringEntry(artDict, dictKey, *artContent);
            aisdk::check_ai_error(result);
        }
        
        sAIDictionary->Release(artDict);
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

AIErr JJLock::DeleteDictionaryForAnEntry(AIArtHandle artHandle, std::string key)
{
    AIErr result = kNoErr;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        
        AIDictKey dictKey = sAIDictionary->Key(key.c_str());
        if (sAIDictionary->IsKnown(artDict, dictKey)){
            result = sAIDictionary->DeleteEntry(artDict, dictKey);
            aisdk::check_ai_error(result);
        }

        sAIDictionary->Release(artDict);
        
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

AIErr JJLock::DeleteDictionaryUsingIterator(AIArtHandle artHandle)
{
    
    AIErr result = kNoErr;
    AIDictionaryIterator dictIterator;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);

        sAIDictionary->Begin(artDict, &dictIterator);
        
        while(!sAIDictIterator->AtEnd(dictIterator))
        {
           sAIArt->GetDictionary(artHandle, &artDict);
           AIDictKey dictKey =  sAIDictIterator->GetKey(dictIterator);
           DeleteDictionaryForAnEntry(artHandle, sAIDictionary->GetKeyString(dictKey));
           sAIDictionary->Begin(artDict, &dictIterator);
        }
        sAIDictionary->Release(artDict);
        sAIDictIterator->Release(dictIterator);
        
       
        
    } catch ( ai::Error &ex) {
        result = ex;
    }
    return result;
}

void JJLock::FetchAllDictionaryUsingIterator(AIArtHandle artHandle)
{
    AIErr result = kNoErr;
    AIDictionaryIterator dictIterator;
    try {
        AIDictionaryRef artDict;
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        
        sAIDictionary->Begin(artDict, &dictIterator);
        
        while(!sAIDictIterator->AtEnd(dictIterator) )
        {
            sAIArt->GetDictionary(artHandle, &artDict);
            AIDictKey dictKey =  sAIDictIterator->GetKey(dictIterator);
            
            AIEntryType entityType;
            sAIDictionary->GetEntryType(artDict, dictKey, &entityType);
            qDebug() << " dictkey " << sAIDictionary->GetKeyString(dictKey) <<   " " << entityType;
            sAIDictIterator->Next(dictIterator);
        }
        sAIDictionary->Release(artDict);
        sAIDictIterator->Release(dictIterator);
    }
    catch ( ai::Error &ex)
    {
        result = ex;
    }
    return;
}


AIErr JJLock::DeleteDictionaryUsingIteratorCheck(AIArtHandle artHandle, QStringList dictKeyList)
{
    AIErr result = kNoErr;
    AIDictionaryRef artDict;
    try
    {
        result = sAIArt->GetDictionary(artHandle, &artDict);
        aisdk::check_ai_error(result);
        for(int i=0; i < dictKeyList.length(); i++ )
        {
            AIDictKey dictKey = sAIDictionary->Key(dictKeyList.at(i).toStdString().c_str());
            if (sAIDictionary->IsKnown(artDict, dictKey))
            {
                DeleteDictionaryForAnEntry(artHandle, sAIDictionary->GetKeyString(dictKey));
            }
        }
        sAIDictionary->Release(artDict);
    }
    catch (ai::Error &ex)
    {
        result = ex;
    }
    return true;
}

AIErr JJLock::GetParagraphJustification(AIArtHandle textArtHandle, ai::int32* getJustifyValue)
{
    AIErr result = kNoErr;
    try {
        IParaFeatures paraFeatures;
        TextRangeRef textRangeRef;
        bool boolJustification = true;
        result = sAITextFrame->GetATETextRange(textArtHandle, &textRangeRef);
        ITextRange textRange(textRangeRef);
        paraFeatures = textRange.GetUniqueParaFeatures();
        ATE::ParagraphJustification getParaJustification;
        getParaJustification = paraFeatures.GetJustification(&boolJustification);
        int enumJustifyValue = 0;
        switch(getParaJustification)
        {
            case kLeftJustify:
            {
                enumJustifyValue = 0;
                break;
            }
            case kRightJustify:
            {
                enumJustifyValue = 1;
                break;
            }
            case kCenterJustify:
            {
                enumJustifyValue = 2;
                break;
            }
            case kFullJustifyLastLineLeft:
            {
                enumJustifyValue = 3;
                break;
            }
            case kFullJustifyLastLineRight:
            {
                enumJustifyValue = 4;
                break;
            }
            case kFullJustifyLastLineCenter:
            {
                enumJustifyValue = 5;
                break;
            }
            case kFullJustifyLastLineFull:
            {
                enumJustifyValue = 6;
                break;
            }
        }
        *getJustifyValue = enumJustifyValue;
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    catch (ATE::Exception& ex) {
        result = ex.error;
    }
    return result;
}

AIErr JJLock::SetParagraphJustification(AIArtHandle textArtHandle, ATE::ParagraphJustification setValue)
{
    AIErr result = kNoErr;
    try {
        IParaFeatures paraFeatures;
        TextRangeRef textRangeRef;
        result = sAITextFrame->GetATETextRange(textArtHandle, &textRangeRef);
        ITextRange textRange(textRangeRef);
        paraFeatures = textRange.GetUniqueParaFeatures();
        paraFeatures.SetJustification(ParagraphJustification(setValue));
        textRange.ReplaceOrAddLocalParaFeatures(paraFeatures);
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    catch (ATE::Exception& ex) {
        result = ex.error;
    }
    return result;
}

bool JJLock::IsAnyArtSelected(AIArtHandle recreateArtHandle)
{
    if(recreateArtHandle == NULL)
    {
        AIArtHandle** selectedArtHandle = NULL;
        ai::int32 numMatches;
        sAIMatchingArt->GetSelectedArt(&selectedArtHandle, &numMatches);
        if(numMatches > 0)
            return true;
        else if(numMatches == 0)
        {
            sAIUser->MessageAlert(ai::UnicodeString("Art object not selected\nPlease select art object to set lock."));
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool JJLock::DeSelectAll()
{
   AppContext appContext(gPlugin->GetPluginRef());
    AIArtHandle** selectedArtHandle = NULL;
    ai::int32 numMatches;
    sAIMatchingArt->GetSelectedArt(&selectedArtHandle, &numMatches);
    for (int i = 0; i < numMatches; ++i )
    {
        sAIArt->SetArtUserAttr((*selectedArtHandle)[i], kArtSelected, 0);
        sAIArt->SetArtUserAttr((*selectedArtHandle)[i], kArtFullySelected, 0);
        sAIArt->SetArtUserAttr((*selectedArtHandle)[i], kArtFullySelected, 0);
    }

}



void JJLock::GetTextFrameFillAndStrokeColour(std::vector<AIStrokeStyle>* textFrameObjectStrokeStyle, std::vector<int>* textFrameStrokeVisibleBool, std::vector<AIFillStyle>* textFrameObjectFillStyle, std::vector<int>* textFrameFillVisibleBool, AIArtHandle textFrameArt)
{
    try
    {
        AIArtHandle textFramePathHandle;
        AIPathStyle textFramePathStyle;
        sAITextFrame->GetPathObject(textFrameArt, &textFramePathHandle);

        AIBoolean fillVisible, strokeVisible;
        sAIPathStyle->GetPathStyleEx(textFramePathHandle, &textFramePathStyle, &fillVisible, &strokeVisible);
        
        AIStrokeStyle storkeStyle;
        storkeStyle.Init();
        storkeStyle.color.kind = kFourColor;
        storkeStyle.color.c.f.black = 1;
        storkeStyle.color.c.f.cyan = 0;
        storkeStyle.color.c.f.magenta = 0;
        storkeStyle.color.c.f.yellow = 0;
        storkeStyle.width = kAIRealZero;
        
        storkeStyle = textFramePathStyle.stroke;
        
        AIFillStyle fillStyle;
        fillStyle.Init();
        fillStyle.color.kind = kFourColor;
        fillStyle.color.c.f.black = 0.0;
        fillStyle.color.c.f.cyan = 0.0;
        fillStyle.color.c.f.magenta = 0.0;
        fillStyle.color.c.f.yellow = 0.0;

        fillStyle = textFramePathStyle.fill;

        textFrameObjectStrokeStyle->push_back(storkeStyle);
        textFrameStrokeVisibleBool->push_back(textFramePathStyle.strokePaint);
        
        textFrameObjectFillStyle->push_back(fillStyle);
        textFrameFillVisibleBool->push_back(textFramePathStyle.fillPaint);
        
    }
    catch (Exception ex)
    {
        qDebug() << ex.what();
    }
}


void JJLock::ApplyFillColorForTextFrame(AIArtHandle textFrameArt, AIFillStyle textFrameFillStyle, AIStrokeStyle textFrameStrokeStyle, AIBoolean fillPaint, AIBoolean strokePaint)
{
    AIErr result = kNoErr;
    AppContext appContext(gPlugin->GetPluginRef());
    
    try
    {
        AIArtHandle textFramePathHandle;
        AIPathStyle textFramePathStyle;
        sAITextFrame->GetPathObject(textFrameArt, &textFramePathHandle);
        
        AIBoolean fillVisible, strokeVisible;
        
        sAIPathStyle->GetPathStyleEx(textFramePathHandle, &textFramePathStyle, &fillVisible, &strokeVisible);
        textFramePathStyle.fillPaint = fillPaint;
        textFramePathStyle.strokePaint = strokePaint;
        textFramePathStyle.fill = textFrameFillStyle;
        textFramePathStyle.stroke = textFrameStrokeStyle;
        result = sAIPathStyle->SetPathStyleEx(textFramePathHandle, &textFramePathStyle, fillVisible, strokeVisible);
    }
    catch (Exception ex)
    {
        qDebug() << ex.what();
    }
}




