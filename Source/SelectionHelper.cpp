//========================================================================================
//  
//  $File: SelectionHelper.cpp $
//
//  Created by jae on 3/20/15
//
//
//========================================================================================

#include "IllustratorSDK.h"

// Project includes:
#include "SDKErrors.h"
#include "JJLockSuites.h"
#include "ArtSetHelper.h"
#include "DocumentHelper.h"
#include "SelectionHelper.h"

/*
*/
ASBoolean SelectionHelper::IsDocumentSelected(void)
{
	ASBoolean result = false;
	AIDocumentHandle document = NULL;
	DocumentHelper docHelper;
	ASErr status = docHelper.GetCurrentDocument(document);
	result = !status && document;
	return result;
}

/*
*/
ASBoolean SelectionHelper::IsArtSelected(AIArtType artType)
{
	ASBoolean result = false;
	if (this->IsDocumentSelected()) {
		ArtSetHelper selectedArt;
		selectedArt.FindSelectedArt();
		if (artType == kAnyArt) {
			result = selectedArt.GetCount() > 0;
		}
		else {
			selectedArt = selectedArt.Filter(artType);
			result = selectedArt.GetCount() > 0;
		}
	}
	return result;
}

/*
*/
ASBoolean SelectionHelper::IsTextRangeSelected(void)
{
	ASBoolean result = false;
	if (this->IsDocumentSelected()) {
		ATE::TextRangesRef rangesRef = NULL;
		AIErr error = sAIDocument->GetTextSelection(&rangesRef);
		aisdk::check_ai_error(error);

		ATE::ITextRanges ranges(rangesRef); 
		if (ranges.GetSize() > 0) {
			AIBoolean textFocus = false;
			error = sAIDocument->HasTextFocus(&textFocus);
			aisdk::check_ai_error(error);

			if (textFocus) {
				ATE::ITextRange textRange = ranges.Item(0);
				if (textRange.GetSize() > 0)
					result = true;
                textRange.~ITextRange();
			}
			else
				result = true;
		}
        ranges.~ITextRanges();
        rangesRef.ATE::~TextRangesRef();
	}
	return result;
}

/*
*/
ASBoolean SelectionHelper::CanSelectArt(AIArtHandle art)
{
	ASBoolean result = false;
	try {
		SDK_ASSERT(sAIArt);
		ai::int32 attr = 0;
		ASErr status = sAIArt->GetArtUserAttr(art, kArtLocked | kArtHidden, &attr);
		aisdk::check_ai_error(status);
		result = !(attr & kArtLocked) && !(attr & kArtHidden);
	}
	catch (ai::Error) {
	}
	return result;
}

/*
*/
ASErr SelectionHelper::SelectArt(AIArtHandle art)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(this->CanSelectArt(art));
		result = sAIArt->SetArtUserAttr(art, kArtSelected, kArtSelected);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr SelectionHelper::DeselectAll()
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIMatchingArt);
		result = sAIMatchingArt->DeselectAll();
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

ASErr SelectionHelper::GetSelectedTextFrame(AIArtHandle &frameArt)
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

ASErr SelectionHelper::GetSelectedArtSet(AIArtHandle **arts, ai::int32 &numMatched)
{
    ASErr result = kNoErr;
    //**arts = NULL;
    numMatched = 0;
    try {
        result = sAIMatchingArt->GetSelectedArt(&arts, &numMatched);
        aisdk::check_ai_error(result);
    } catch (ai::Error &ex) {
        result = ex;
    }
    return result;
}
// End SelectionHelper.cpp