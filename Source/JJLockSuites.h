//
//  JJLockSuites.h
//  JJLock
//
//  Created by Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 05/02/2019
//

#ifndef __JJLockSuites_H__
#define __JJLockSuites_H__

#include "IllustratorSDK.h"
#include "Suites.hpp"

// AI suite headers
#include "AIMenu.h"
#include "AIPanel.h"
#include "AIDocumentList.h"
#include "AIShapeConstruction.h"
#include "AITransformArt.h"
#include "AIATEPaint.h"
#include "AIAnnotator.h"
#include "AIAnnotatorDrawer.h"
#include "AIUID.h"
#include "AIUUID.h"

#include "AIBlock.h"
#include "AIPathInterpolate.h"
#include "AIPath.h"
#include "AIUITheme.h"
#include "AITransformArt.h"

#include "AIMenuCommandString.h"



// Suite externs
extern "C"  SPBlocksSuite               *sSPBlocks;

extern "C"  AIUnicodeStringSuite        *sAIUnicodeString;

extern "C"  AIMenuSuite                 *sAIMenu;

extern "C"  AIPanelSuite                *sAIPanel;

extern "C"  AIPanelFlyoutMenuSuite      *sAIPanelFlyoutMenu;

extern "C"  AIDocumentSuite             *sAIDocument;

extern "C"  AIDocumentListSuite         *sAIDocumentList;

extern "C"  AIDocumentViewSuite         *sAIDocumentView;

extern "C"  AIArtSuite                  *sAIArt;

extern "C"  AIArtSetSuite               *sAIArtSet;

extern "C"  AITextFrameSuite            *sAITextFrame;

extern "C"  AIShapeConstructionSuite    *sAIShapeConstruction;

extern "C"  AIRealMathSuite             *sAIRealMath;

extern "C"  AIPathSuite                 *sAIPath;

extern "C"  AIPathStyleSuite            *sAIPathStyle;

extern "C"  AIFontSuite                 *sAIFont;

extern "C"  AITransformArtSuite         *sAIAITransformArt;

extern "C"  AIATEPaintSuite             *sAIATEPaint;

extern	"C" AILayerSuite                *sAILayer;

extern "C" AIMatchingArtSuite           *sAIMatchingArt;

extern "C" AIDictionarySuite            *sAIDictionary;

extern	"C" AIAnnotatorSuite*			sAIAnnotator;

extern	"C"	AIAnnotatorDrawerSuite*		sAIAnnotatorDrawer;

extern	"C"	AITextFrameHitSuite			*sAITextFrameHit;

extern "C" AIUIDSuite                   *sAIUID;

extern "C" AIUIDUtilsSuite              *sAIUIDUtils;

extern "C" AIUndoSuite                  *sAIUndoSuite;

extern "C" AITransformArtSuite          *sAITransformArt;

extern "C" AIHardSoftSuite              *sAIHardSoft;

extern "C" AIPlacedSuite                *sAIPlaced;

extern "C" AIArraySuite                 *sAIArray;

extern "C" AIEntrySuite                 *sAIEntry;

extern "C" AISwatchListSuite            *sAISwatches;

extern "C" AISwatchGroupSuite           *sAISwatcheGroup;

extern "C" AICustomColorSuite           *sAICustomColor;

extern "C" AIDictionaryIteratorSuite    *sAIDictIterator;

extern "C" AIUIDUtilsSuite              *sAIUIDUtils;

extern "C" AIUIDREFSuite                *sAIUIDRef;

extern "C" AIUUIDSuite                  *sAIUUID;

extern "C" AIAppContextSuite            *sAIAppContext;

extern "C" AIUndoSuite                  *sAIUndo;

extern "C" AIRasterSuite                *sAIRaster;

extern "C" AIBlockSuite                 *sAIBlock;

extern "C" AIPathInterpolateSuite       *sAIPathInterpolate;



#endif
