//========================================================================================
//  
//  $File: ArtSetHelper.cpp $
//
//  Created by jae on 3/20/15
//
//  
//========================================================================================

#include "IllustratorSDK.h"

#include "SDKErrors.h"

// Framework includes:
#include "ArtSetHelper.h"
#include "JJLockSuites.h"
#include "SelectionHelper.h"

/* Creates empty art set.
*/
ArtSetHelper::ArtSetHelper() : fArtSet(NULL)
{
	ASErr status = sAIArtSet->NewArtSet(&this->fArtSet);
	SDK_ASSERT(!status);
}

/* Creates art set containing specified art.
*/
ArtSetHelper::ArtSetHelper(AIArtSpec* specs, short numSpecs) : fArtSet(NULL)
{
	ASErr status = kNoErr;
	try {
		status = sAIArtSet->NewArtSet(&this->fArtSet);
		aisdk::check_ai_error(status);
		
		status = sAIArtSet->MatchingArtSet(specs, numSpecs, this->fArtSet);
		aisdk::check_ai_error(status);
	}
	catch (ai::Error& ex) {
		status = ex;
		if (this->fArtSet) {
			sAIArtSet->DisposeArtSet(&this->fArtSet);
		}
	}
	SDK_ASSERT(!status);
}

/* Copy constructor
*/
ArtSetHelper::ArtSetHelper(const ArtSetHelper& rhs) : fArtSet(NULL)
{
	ASErr status = kNoErr;
	try {
		status = sAIArtSet->NewArtSet(&this->fArtSet);
		aisdk::check_ai_error(status);
		
		for (size_t i = 0; i < rhs.GetCount(); i++) {
			AIArtHandle art = rhs[i];
			status = sAIArtSet->AddArtToArtSet(this->fArtSet, art);
			aisdk::check_ai_error(status);
		}
	}
	catch (ai::Error& ex) {
		status = ex;
		if (this->fArtSet) {
			sAIArtSet->DisposeArtSet(&this->fArtSet);
		}
	}
	SDK_ASSERT(!status);
} // end copy constructor

/* Destructor.
*/
ArtSetHelper::~ArtSetHelper()
{
	if(this->fArtSet) {
		ASErr status = sAIArtSet->DisposeArtSet(&this->fArtSet);
		this->fArtSet = NULL;
		SDK_ASSERT(!status);
	}
}

/* = operator (assignment)
*/
ArtSetHelper& ArtSetHelper::operator=(const ArtSetHelper& rhs)
{
	ASErr status = kNoErr;
    if (this != &rhs) // make sure not same object
	{  
		try
		{
			status = sAIArtSet->DisposeArtSet(&this->fArtSet);
			aisdk::check_ai_error(status);

			this->fArtSet = NULL;

			status = sAIArtSet->NewArtSet(&this->fArtSet);
			aisdk::check_ai_error(status);
			
			for (size_t i = 0; i < rhs.GetCount(); i++)
			{
				AIArtHandle art = rhs[i];
				status = sAIArtSet->AddArtToArtSet(this->fArtSet, art);
				aisdk::check_ai_error(status);
			}
		}
		catch (ai::Error& ex)
		{
			status = ex;
			if (this->fArtSet != NULL)
				sAIArtSet->DisposeArtSet(&this->fArtSet);
		}
    }
	SDK_ASSERT(!status);
    return *this;    // Return ref for multiple assignment
}//end operator=

/* index operator
*/
AIArtHandle ArtSetHelper::operator [] (size_t index) const
{
	AIArtHandle result = NULL;
	if (index >= 0 && index < this->GetCount()) {
		ASErr status = sAIArtSet->IndexArtSet(this->fArtSet, index, &result);
		SDK_ASSERT(!status);
	}
	else {
		SDK_ASSERT_MSG(false, "Invalid index");
	}
	return result;
}

/*
*/
size_t ArtSetHelper::GetCount() const
{
	size_t result = 0;
	ASErr status = sAIArtSet->CountArtSet(this->fArtSet, &result);
	SDK_ASSERT(!status);
	return result;
}

/* Finds set of selected art.
*/
void ArtSetHelper::FindSelectedArt(void)
{
	ASErr status = sAIArtSet->SelectedArtSet(this->fArtSet);
	SDK_ASSERT(!status);
}


/* Finds art on given layer.
*/
void ArtSetHelper::FindLayerArt(const ai::UnicodeString& layerName)
{
	ASErr status = kNoErr;
	AILayerHandle layer = NULL;
	status = sAILayer->GetLayerByTitle(&layer, layerName);
	aisdk::check_ai_error(status);

	status = sAIArtSet->LayerArtSet(layer, this->fArtSet);
	aisdk::check_ai_error(status);
}

/*	Returns a new art set containing art in this set that 
	matches a given type.
*/
ArtSetHelper ArtSetHelper::Filter(AIArtType include)
{
	ArtSetHelper result;
	ASErr status = kNoErr;
	for (size_t i = 0; i < this->GetCount(); i++) {
		AIArtHandle art = this->operator [](i);
		short type = kUnknownArt;
		status = sAIArt->GetArtType(art, &type);
		aisdk::check_ai_error(status);
		if (type == include) {
			result.Add(art);
		}
	}
	SDK_ASSERT(!status);
	return result;
}


/*
*/
void ArtSetHelper::Add(AIArtHandle art)
{
	ASErr status = sAIArtSet->AddArtToArtSet(this->fArtSet,art);
	SDK_ASSERT(!status);
}

/*
*/
void ArtSetHelper::SelectArt(AIArtHandle art)
{
	SelectionHelper selectionHelper;
	if (art && selectionHelper.CanSelectArt(art)) {
		selectionHelper.SelectArt(art);
	}
}

/*
*/
void ArtSetHelper::SelectAll()
{
	SelectionHelper selectionHelper;
	selectionHelper.DeselectAll();
	for (size_t i = 0; i < this->GetCount(); i++) {
		AIArtHandle art = this->operator [](i);
		if (art && selectionHelper.CanSelectArt(art)) {
			selectionHelper.SelectArt(art);
		}
	}
}

void ArtSetHelper::ClearAll()
{
    ASErr status = sAIArtSet->ClearArtSet(this->fArtSet);
    SDK_ASSERT(!status);
}

// End ArtSetHelper.cpp
