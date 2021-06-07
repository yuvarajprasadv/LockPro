//========================================================================================
//  
//  $File: DocumentHelper.cpp $
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
//#include "Chooser.h"
#include "DocumentHelper.h"
#include "SelectionHelper.h"

/*
*/
ASErr DocumentHelper::NewDocument(const ai::UnicodeString& preset, AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->New(const_cast<ai::UnicodeString&>(preset), NULL, kDialogNone, &document);
		aisdk::check_ai_error(result);
		SDK_ASSERT(document);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::NewCustomDocument(const ai::UnicodeString& preset, AIReal width, AIReal height, AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		AINewDocumentPreset settings;
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->GetPresetSettings(const_cast<ai::UnicodeString&>(preset), &settings);
		aisdk::check_ai_error(result);
		settings.docWidth = width;
		settings.docHeight = height;
		result = sAIDocumentList->New(const_cast<ai::UnicodeString&>(preset), &settings, kDialogNone, &document);
		aisdk::check_ai_error(result);
		SDK_ASSERT(document);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::NewCustomDocument(const ai::UnicodeString& preset, AIReal width, AIReal height, ASInt32 numArtboards, AIArtboardLayout artboardLayout, AIReal artboardSpacing, ASInt32 numRowsOrCols, AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		AINewDocumentPreset settings;
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->GetPresetSettings(const_cast<ai::UnicodeString&>(preset), &settings);
		aisdk::check_ai_error(result);
		settings.docWidth = width;
		settings.docHeight = height;
		settings.docNumArtboards = numArtboards;
		settings.docArtboardLayout = artboardLayout;
		settings.docArtboardSpacing = artboardSpacing;
		settings.docArtboardRowsOrCols = numRowsOrCols;
		result = sAIDocumentList->New(const_cast<ai::UnicodeString&>(preset), &settings, kDialogNone, &document);
		aisdk::check_ai_error(result);
		SDK_ASSERT(document);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
/*ASErr DocumentHelper::OpenDocument(const ai::UnicodeString& name, AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIDocumentList);
		AIColorModel colorModel = kAIUnknownColorModel;		
		ai::FilePath path = this->GetAssetPath(name);
		ActionDialogStatus dialogStatus = path.Exists(false) ? kDialogOff : kDialogOn;
		AIBoolean forceCopy = false;
		result = sAIDocumentList->Open(path, colorModel, dialogStatus, forceCopy, &document);
		aisdk::check_ai_error(result);
		SDK_ASSERT(document);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}*/
ASErr DocumentHelper::OpenDocument(ai::FilePath file, AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIDocumentList);
		AIColorModel colorModel = kAIUnknownColorModel;
		ai::FilePath path = file;
		ActionDialogStatus dialogStatus = path.Exists(false) ? kDialogOff : kDialogOn;
		AIBoolean forceCopy = false;
		result = sAIDocumentList->Open(path, colorModel, dialogStatus, forceCopy, &document);
		aisdk::check_ai_error(result);
		SDK_ASSERT(document);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::ActivateDocument(AIDocumentHandle document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(document);
		SDK_ASSERT(sAIDocumentList);
		ASBoolean bSetFocus = true;
		result = sAIDocumentList->Activate(document, bSetFocus);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}


/*
*/
ASErr DocumentHelper::PrintDocument(AIDocumentHandle document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(document);
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->Print(document, kDialogOn);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::SaveDocument(AIDocumentHandle document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(document);
		SDK_ASSERT(sAIDocumentList);
		ASErr status = sAIDocumentList->Save(document);
		aisdk::check_ai_error(status);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::CloseDocument(AIDocumentHandle document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(document);
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->Close(document);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::CloseAllDocuments()
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIDocumentList);
		result = sAIDocumentList->CloseAll();
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::GetCurrentDocument(AIDocumentHandle& document)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIDocument);
		result = sAIDocument->GetDocument(&document);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DocumentHelper::GetCurrentLayer(AILayerHandle& layer)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAILayer);
		result = sAILayer->GetCurrentLayer(&layer);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASBoolean DocumentHelper::CanPlaceFile(void)
{
	ASBoolean result = true;
	try {
		AILayerHandle layer = NULL;
		ASErr status = this->GetCurrentLayer(layer);
		aisdk::check_ai_error(status);
		AIBoolean editable = false;
		status = sAILayer->GetLayerEditable(layer, &editable);
		aisdk::check_ai_error(status);
		AIBoolean visible = false;
		status = sAILayer->GetLayerVisible(layer, &visible);
		aisdk::check_ai_error(status);
		result = editable && visible;
	}
	catch (ai::Error) {
	}
	return result;
}

/*
*/
/*ASErr DocumentHelper::PlaceFile(const ai::UnicodeString& name, AIArtHandle& art)
{
	ASErr result = kNoErr;

	try {
		ai::FilePath path = this->GetAssetPath(name);
		AIPlaceRequestData placeReqData;
		placeReqData.m_lPlaceMode = kVanillaPlace; // places the file and returns new art handle
		placeReqData.m_disableTemplate = true; // disable Template placement
		placeReqData.m_filemethod = 0; // place the file
		placeReqData.m_hNewArt = NULL; // new art handle
		placeReqData.m_hOldArt = NULL; // art to be replaced
		placeReqData.m_lParam = kPlacedArt; // type of art to create
		placeReqData.m_pFilePath = NULL; // if NULL, shows File Place dialog
		if (path.Exists(false) && name.size() != 0) {
			placeReqData.m_pFilePath = &path; // place the given file (prompt for file to be placed otherwise)
		}
		
		// Place the file.
		result = sAIPlaced->ExecPlaceRequest(placeReqData);
		aisdk::check_ai_error(result);

		// Verify that the handle of the placed art is returned.
		SDK_ASSERT(placeReqData.m_hNewArt);

		// Pass handle to placed art back to caller.
		art = placeReqData.m_hNewArt;
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}*/
ASErr DocumentHelper::PlaceFile(ai::FilePath& file, AIArtHandle& art)
{
	ASErr result = kNoErr;

	try {
		ai::FilePath path = file;
		AIPlaceRequestData placeReqData;
		placeReqData.m_lPlaceMode = kVanillaPlace; // places the file and returns new art handle
		placeReqData.m_disableTemplate = true; // disable Template placement
		placeReqData.m_filemethod = 0; // place the file
		placeReqData.m_hNewArt = NULL; // new art handle
		placeReqData.m_hOldArt = NULL; // art to be replaced
		placeReqData.m_lParam = kPlacedArt; // type of art to create
		placeReqData.m_pFilePath = NULL; // if NULL, shows File Place dialog
		if (path.Exists(false)) {
			placeReqData.m_pFilePath = &path; // place the given file (prompt for file to be placed otherwise)
		}
		
		// Place the file.
		result = sAIPlaced->ExecPlaceRequest(placeReqData);
		aisdk::check_ai_error(result);

		// Verify that the handle of the placed art is returned.
		SDK_ASSERT(placeReqData.m_hNewArt);

		// Pass handle to placed art back to caller.
		art = placeReqData.m_hNewArt;
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
/*ai::FilePath DocumentHelper::GetAssetPath(const ai::UnicodeString& name)
{
	ai::FilePath path = SnippetRunnerPreferences::Instance()->GetAssetsFolderPath();
	path.AddComponent(ai::FilePath(name));
	return path;
}*/


// End DocumentHelper.cpp

