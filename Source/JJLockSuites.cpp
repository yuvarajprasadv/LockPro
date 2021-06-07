//
//  JJLockSuites.cpp
//  JJLock
//
//  Created by Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 05/02/2019
//

#include "JJLockSuites.h"
#include "ATETextSuitesImportHelper.h"
#include "AIUID.h"


// Suite externs
extern "C"
{
	SPBlocksSuite               *sSPBlocks = NULL;
    
	AIUnicodeStringSuite        *sAIUnicodeString = NULL;
    
    AIMenuSuite                 *sAIMenu = NULL;
    
    AIPanelSuite                *sAIPanel = NULL;
    
    AIPanelFlyoutMenuSuite      *sAIPanelFlyoutMenu = NULL;

    AIDocumentSuite             *sAIDocument = NULL;
    
    AIDocumentListSuite         *sAIDocumentList = NULL;
    
    AIDocumentViewSuite         *sAIDocumentView = NULL;
    
    AIArtSuite                  *sAIArt = NULL;
    
    AIArtSetSuite               *sAIArtSet = NULL;
    
    AITextFrameSuite            *sAITextFrame = NULL;

    AIShapeConstructionSuite    *sAIShapeConstruction = NULL;

    AIRealMathSuite             *sAIRealMath = NULL;
    
    AIPathSuite                 *sAIPath = NULL;
    
    AIPathStyleSuite            *sAIPathStyle = NULL;

    AIFontSuite                 *sAIFont = NULL;

    AITransformArtSuite         *sAIAITransformArt = NULL;

    AIATEPaintSuite             *sAIATEPaint = NULL;
    
    AILayerSuite                *sAILayer = NULL;
    
    AIMatchingArtSuite          *sAIMatchingArt = NULL;
    
    AIDictionarySuite           *sAIDictionary = NULL;
    
    AIAnnotatorSuite*			sAIAnnotator = NULL;
    
    AIAnnotatorDrawerSuite*		sAIAnnotatorDrawer = NULL;
    
    AITextFrameHitSuite			*sAITextFrameHit;
    
    AIUIDSuite                  *sAIUID;

    AIUIDUtilsSuite             *sAIUIDUtils;
    
    AIUIDREFSuite               *sAIUIDRef = NULL;
    
    AIUndoSuite                 *sAIUndoSuite = NULL;
    
    AITransformArtSuite         *sAITransformArt = NULL;
  
    AIHardSoftSuite             *sAIHardSoft = NULL;
    
    AIPlacedSuite               *sAIPlaced;
    
    AIArraySuite                *sAIArray = NULL;
    
    AIEntrySuite                *sAIEntry = NULL;
    
    AISwatchListSuite           *sAISwatches = NULL;
    
    AISwatchGroupSuite          *sAISwatchGroup = NULL;
    
    AICustomColorSuite          *sAICustomColor = NULL;
    
    AIDictionaryIteratorSuite   *sAIDictIterator = NULL;
    
    AIUUIDSuite                 *sAIUUID = NULL;
    
    AIAppContextSuite           *sAIContextSuite = NULL;
    
    AIUndoSuite                 *sAIUndo=NULL;
    
    AIRasterSuite               *sAIRaster = NULL;
    
    AIBlockSuite                *sAIBlock = NULL;
    
    AIPathInterpolateSuite      *sAIPathInterpolate = NULL;


    // Add a line below for each additional suite your text editor needs.
    // NOTE: Also add a line to gPostStartupSuites and edit TextEditorSuites.h to declare your suite pointer.
    EXTERN_TEXT_SUITES
}

// Import suites
ImportSuite gImportSuites[] = 
{
	kSPBlocksSuite, kSPBlocksSuiteVersion, &sSPBlocks,
	kAIUnicodeStringSuite, kAIUnicodeStringVersion, &sAIUnicodeString,
    
    kAIMenuSuite, kAIMenuSuiteVersion, &sAIMenu,
    kAIPanelSuite, kAIPanelSuiteVersion, &sAIPanel,
    kAIPanelFlyoutMenuSuite, kAIPanelFlyoutMenuSuiteVersion, &sAIPanelFlyoutMenu,

    kAIDocumentSuite, kAIDocumentSuiteVersion, &sAIDocument,
    kAIDocumentListSuite, kAIDocumentListSuiteVersion, &sAIDocumentList,
    kAIArtSuite, kAIArtSuiteVersion, &sAIArt,
    kAIArtSetSuite, kAIArtSetSuiteVersion, &sAIArtSet,
    kAIDocumentViewSuite, kAIDocumentViewSuiteVersion, &sAIDocumentView,
    kAITextFrameSuite, kAITextFrameSuiteVersion, &sAITextFrame,

    kAIShapeConstructionSuite, kAIShapeConstructionSuiteVersion, &sAIShapeConstruction,

    kAIRealMathSuite, kAIRealMathVersion, &sAIRealMath,
    kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
    kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
    
    kAIUIDSuite, kAIUIDSuiteVersion, &sAIUID,
    
    kAIUIDUtilsSuite, kAIUIDUtilsSuiteVersion, &sAIUIDUtils,

    kAIFontSuite, kAIFontSuiteVersion, &sAIFont,
    
    kAILayerSuite,kAILayerSuiteVersion, &sAILayer,
    
    kAIMatchingArtSuite, kAIMatchingArtVersion, &sAIMatchingArt,

    kAITransformArtSuite, kAITransformArtSuiteVersion, &sAIAITransformArt,

    kAIATEPaintSuite, kAIATEPaintSuiteVersion, &sAIATEPaint,
    
    kAITextFrameHitSuite, kAITextFrameHitVersion, &sAITextFrameHit,
    
    kAIUndoSuite, kAIUndoSuiteVersion, &sAIUndoSuite,
    
    kAIDictionarySuite, kAIDictionarySuiteVersion, &sAIDictionary,
    
    kAITransformArtSuite, kAITransformArtSuiteVersion, &sAITransformArt,
    
    kAIHardSoftSuite, kAIHardSoftSuiteVersion, &sAIHardSoft,
    
    kAIPlacedSuite, kAIPlacedSuiteVersion, &sAIPlaced,
    
    kAIArraySuite, kAIArraySuiteVersion, &sAIArray,
    
    kAIEntrySuite, kAIEntrySuiteVersion, &sAIEntry,
    
    kAISwatchListSuite, kAISwatchListVersion, &sAISwatches,
    
    kAISwatchGroupSuite, kAISwatchGroupVersion, &sAISwatchGroup,
    
    kAICustomColorSuite, kAICustomColorVersion, &sAICustomColor,
    
    kAIDictionaryIteratorSuite, kAIDictionaryIteratorVersion, &sAIDictIterator,
    
    kAIUIDUtilsSuite, kAIUIDUtilsVersion, &sAIUIDUtils,
    
    kAIUIDREFSuite, kAIUIDREFSuiteVersion, &sAIUIDRef,
    
    kAIUUIDSuite, kAIUUIDSuiteVersion, &sAIUUID,
    
    kAIAppContextSuite, kAIAppContextSuiteVersion, &sAIAppContext,
    
    kAIUndoSuite, kAIUndoSuiteVersion, &sAIUndo,
    
    kAIRasterSuite, kAIRasterSuiteVersion, &sAIRaster,
    
    kAIBlockSuite, kAIBlockVersion, &sAIBlock,
    
    kAIPathInterpolateSuite, kAIPathInterpolateVersion, &sAIPathInterpolate,
    
    // Add a line below for each additional suite your text editor needs.
    IMPORT_TEXT_SUITES
	nil, 0, nil
};
