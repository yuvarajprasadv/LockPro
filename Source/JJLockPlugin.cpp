//
//  JJLockPlugin.cpp
//  JJLock
//
//  Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 07/01/2021
//

#include "JJLockPlugin.h"
#include "AIMenuCommandNotifiers.h"
#include <QMacNativeWidget>
#include <QVBoxLayout>
#include "Tool.h"
#include "SDKErrors.h"
#include "AIPanel.h"
#include "JJLockPanel.hpp"
#include "AITypes.h"
#include "AppContext.hpp"
#include "SDKErrors.h"
#include "IText.h"
#include "JJLockUI.h"

#define debugMode 1

using namespace ATE;

JJLockPlugin *gPlugin = NULL;
JJLockPanel *objJJLockPanel;

bool openDocumentBool = false;
bool documentOpenedProcessCompleted = false;
bool documentIOpened = false;
bool boolGrouping = false;

Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
    return new JJLockPlugin(pluginRef);
}

void FixupReload(Plugin* plugin)
{
	JJLockPlugin::FixupVTable((JJLockPlugin*) plugin);
}

JJLockPlugin::JJLockPlugin(SPPluginRef pluginRef) :
    Plugin(pluginRef),
    fPanel(NULL),
    hDlg(NULL)
{
	strncpy(fPluginName, PLUGIN_NAME, kMaxStringLength);
    fAboutPluginMenu = NULL;
    fSubWindowMenu = NULL;
    fPanelFlyoutMenu = NULL;
}

JJLockPlugin::~JJLockPlugin()
{
}

/*
 */
ASErr JJLockPlugin::SetGlobal(Plugin *plugin)
{
    gPlugin = (JJLockPlugin *) plugin;
    return kNoErr;
}

ASErr JJLockPlugin::StartupPlugin( SPInterfaceMessage *message )
{
    ASErr error = kNoErr;
    error = Plugin::StartupPlugin(message);
    if (error) {
        return error;
    }
    
    if (kBetaVersion) {
        Tool tool;
        long int currentTimeStamp = tool.currentTimeStamp();
        if(currentTimeStamp > kTextExpireDate)
        {
            sAIUser->ErrorAlert(ai::UnicodeString(errUATExpired));
            return error;
        }
    }
    
    // Shanghai: 10.33.18.20, 10.33.18.21
    // Penang: 172.17.1.230, 172.17.1.248
    // Chennai: 10.52.2.81, 10.52.2.10
    // Shenzhen: 10.172.2.209, 10.172.2.210, 10.172.2.208
    // HongKong: 10.120.1.209, 10.120.1.208
    // Kala: 10.224.95.7, c3ptwip01p.amer.schawk.com
    // Antwerp: 192.168.91.25, 192.168.91.31
    // Manchester: 10.32.66.30, 10.32.66.23, 10.32.66.140
    // Sydney: dnyfile01.asia.schawk.com, sdnymt01.asia.schawk.com
    // NCL: 10.32.100.36, 10.32.104.71
    // Cincinati: 10.96.19.55, 10.96.19.50
    if(kAuthenticate)
    if ( !(system("ping -c 1 -t 1 10.33.18.20") == 0 || system("ping -c 1 -t 1 10.33.18.21") == 0 \
           || system("ping -c 1 -t 1 172.17.1.230") == 0 || system("ping -c 1 -t 1 172.17.1.248") == 0 \
           || system("ping -c 1 -t 1 10.52.2.81") == 0  || system("ping -c 1 -t 1 10.52.2.10") == 0 \
           || system("ping -c 1 -t 1 10.172.2.209") == 0 || system("ping -c 1 -t 1 10.172.2.210") == 0 || system("ping -c 1 -t 1 10.172.2.208") == 0 \
           || system("ping -c 1 -t 1 10.120.1.209") == 0 || system("ping -c 1 -t 1 10.120.1.208") == 0 \
           || system("ping -c 1 -t 1 10.224.95.7") == 0 || system("ping -c 1 -t 1 c3ptwip01p.amer.schawk.com") == 0 \
           || system("ping -c 1 -t 1 192.168.91.25") == 0 || system("ping -c 1 -t 1 192.168.91.31") == 0 \
           || system("ping -c 1 -t 1 10.32.66.30") == 0 || system("ping -c 1 -t 1 10.32.66.23") == 0 || system("ping -c 1 -t 1 10.32.66.140") == 0 \
           || system("ping -c 1 -t 1 dnyfile01.asia.schawk.com") == 0 || system("ping -c 1 -t 1 sdnymt01.asia.schawk.com") == 0 \
           || system("ping -c 1 -t 1 10.32.100.36") == 0 || system("ping -c 1 -t 1 10.32.104.71") == 0 \
           || system("ping -c 1 -t 1 10.96.19.55") == 0 || system("ping -c 1 -t 1 10.96.19.50") == 0 \
           ) ) {
        sAIUser->ErrorAlert(ai::UnicodeString(errAuthentication));
        return error;
    }
    
    
    //AIErr error = kNoErr;
    
    error = this->AddNotifiers(message);
    aisdk::check_ai_error(error);

    // Add About Plugins menu item for this plug-in.
    SDKAboutPluginsHelper aboutPluginsHelper;
    error = aboutPluginsHelper.AddAboutPluginsMenuItem(message, ABOUT_MENU_PLUGINS_GROUP_NAME, ai::UnicodeString(ABOUT_MENU_PLUGINS_GROUP_NAME_STRING), PLUGIN_NAME_STRING, &fAboutPluginMenu);
    if (error) {
        return error;
    }
    
    error = this->AddMenus(message);
    if (error) {
        return error;
    }
    error = this->CreatePanel();
    if (error) {
        return error;
    }
    error = AddWidgets();
    if (error) {
        return error;
    }
    
    return error;
}

ASErr JJLockPlugin::ShutdownPlugin( SPInterfaceMessage *message )
{
	ASErr error = kNoErr;
	error = Plugin::ShutdownPlugin(message);
	return error;
}

/*
 */
ASErr JJLockPlugin::GoMenuItem(AIMenuMessage *message)
{
    ASErr error = kNoErr;
    try {
        if (message->menuItem == this->fAboutPluginMenu) {
            SDKAboutPluginsHelper aboutPluginsHelper;
            aboutPluginsHelper.PopAboutBox(message, "About " PLUGIN_NAME, PLUGIN_ABOUT_MESSAGE);
        }
        else if (message->menuItem == this->fSubWindowMenu) {
            if (this->fPanel) {
                AIBoolean isShow = false;
                error = sAIPanel->IsShown(fPanel, isShow);
                
                if (isShow) {
                    error = sAIPanel->Show(fPanel, false);
                    // destructure the fPanel
                }
                else if(!isShow) {
                    error = sAIPanel->Show(fPanel, true);
                    // constructure the fPanel
                }
            }
        }
    } catch (ai::Error& ex) {
        error = ex;
    }
    return error;
}

/*
 */
ASErr JJLockPlugin::Message(char *caller, char *selector, void *message)
{
    ASErr result = kNoErr;
    try {
        result = Plugin::Message(caller, selector, message);
        if (result == kUnhandledMsgErr) {
            result = kNoErr;
        }
        else
            aisdk::check_ai_error(result);
    }
    catch (ai::Error& ex) {
        result = ex;
    }
    catch (...) {
        result = kCantHappenErr;
    }
    return result;
}

ASErr JJLockPlugin::AddNotifiers( SPInterfaceMessage *message )
{
    ASErr error = kNoErr;
    try {
   
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIApplicationShutdownNotifier, kAIApplicationShutdownNotifier, &fAppShutdownNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAllPluginStartedNotifier, kAIAllPluginStartedNotifier, &fAllPluginStartedNotifier);
        aisdk::check_ai_error(error);
    
         error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAICloseCommandPreNotifierStr,  kAICloseCommandPreNotifierStr, &fDocumentClosePreNotifier);
         aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAICloseCommandPostNotifierStr,  kAICloseCommandPostNotifierStr, &fDocumentClosePostNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIDocumentChangedNotifier, kAIDocumentChangedNotifier, &fDocumentChangedNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIDocumentOpenedNotifier, kAIDocumentOpenedNotifier, &fDocumentOpenedNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveCommandPreNotifierStr, kAISaveCommandPreNotifierStr, &fSaveCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveCommandPostNotifierStr, kAISaveCommandPostNotifierStr, &fSaveCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveCommandPreNotifierStr, kAISaveCommandPreNotifierStr, &fSaveCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveAsCommandPreNotifierStr, kAISaveAsCommandPreNotifierStr, &fSaveAsCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveACopyAsCommandPreNotifierStr, kAISaveACopyAsCommandPreNotifierStr, &fSaveACopyAsCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAdobeAISaveForWebCommandPreNotifierStr, kAIAdobeAISaveForWebCommandPreNotifierStr, &fAdobeAISaveForWebCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAdobeAISaveSelectedSlicesCommandPreNotifierStr, kAIAdobeAISaveSelectedSlicesCommandPreNotifierStr, &fAdobeAISaveSelectedSlicesCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveForOfficeCommandPreNotifierStr, kAISaveForOfficeCommandPreNotifierStr, &fSaveForOfficeCommandPreNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveCommandPostNotifierStr, kAISaveCommandPostNotifierStr, &fSaveCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveAsCommandPostNotifierStr, kAISaveAsCommandPostNotifierStr, &fSaveAsCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveACopyAsCommandPostNotifierStr, kAISaveACopyAsCommandPostNotifierStr, &fSaveACopyAsCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveAsTemplateCommandPostNotifierStr, kAISaveAsTemplateCommandPostNotifierStr, &fSaveAsTemplateCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAdobeAISaveForWebCommandPostNotifierStr, kAIAdobeAISaveForWebCommandPostNotifierStr, &fAdobeAISaveForWebCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAdobeAISaveSelectedSlicesCommandPostNotifierStr, kAIAdobeAISaveSelectedSlicesCommandPostNotifierStr, &fAdobeAISaveSelectedSlicesCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAISaveForOfficeCommandPostNotifierStr, kAISaveForOfficeCommandPostNotifierStr, &fSaveForOfficeCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIDocumentViewInvalidRectChangedNotifier,  kAIDocumentViewInvalidRectChangedNotifier, &fAIDocumentViewInvalidRectChangedNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIPasteCommandPostNotifierStr,  kAIPasteCommandPostNotifierStr,
                                         &fAIAfterPasteCommandPreNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIGroupCommandPreNotifierStr,  kAIGroupCommandPreNotifierStr,
                                         &fAIGroupCommandPreNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIGroupCommandPostNotifierStr,  kAIGroupCommandPostNotifierStr,
                                         &fAIGroupCommandPostNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIUngroupCommandPreNotifierStr,  kAIUngroupCommandPreNotifierStr,
                                         &fAIUngroupCommandPreNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIUngroupCommandPostNotifierStr,  kAIUngroupCommandPostNotifierStr,
                                         &fAIUngroupCommandPostNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAITextFontCommandPostNotifierStr,  kAITextFontCommandPostNotifierStr, &fAITextFontCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAILayerSetNotifier,  kAILayerSetNotifier, &fAILayerSetNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIAdobeLinkPaletteMenuItemCommandPostNotifierStr,  kAIAdobeLinkPaletteMenuItemCommandPostNotifierStr, &fAIAdobeLinkPaletteMenuItemCommandPostNotifierStr);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIArtSelectionChangedNotifier,  kAIArtSelectionChangedNotifier,
                                         &fAIArtSelectionChangedNotifier);
        aisdk::check_ai_error(error);
 
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAICurrentLayerNotifier,  kAICurrentLayerNotifier,
                                         &fAICurrentLayerNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAILayerDeletionNotifier,  kAILayerDeletionNotifier,
                                         &fAILayerDeletionNotifier);
        aisdk::check_ai_error(error);
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAILayerSetNotifier,  kAILayerSetNotifier,
                                         &fAILayerSetNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAILayerOptionsNotifier,  kAILayerOptionsNotifier,
                                         &fAILayerOptionsNotifier);
        aisdk::check_ai_error(error);
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAILayerArtStyleChangeNotifier,  kAILayerArtStyleChangeNotifier,
                                         &fAILayerArtStyleChangeNotifier);
        aisdk::check_ai_error(error);
        
        error = sAINotifier->AddNotifier( fPluginRef, PLUGIN_NAME " " kAIArtObjectsChangedNotifier,  kAIArtObjectsChangedNotifier,
                                         &fAIArtObjectsChangedNotifier);
        aisdk::check_ai_error(error);
        
        
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    return error;
}



/*
 */
ASErr JJLockPlugin::Notify( AINotifierMessage * message )
{
    AIErr result = kNoErr;
    
    const char *name;
    sAINotifier->GetNotifierName(message->notifier, &name);
    if (message->notifier == fAppShutdownNotifier) {
        if(fPanel){
            result = sAIPanel->Destroy(fPanel);
            fPanel = NULL;
        }
        if(fPanelFlyoutMenu){
            result = sAIPanelFlyoutMenu->Destroy(fPanelFlyoutMenu);
            fPanelFlyoutMenu = NULL;
        }
        if(qtApplication){
            qtApplication->exit();
        }
    }
    else if (message->notifier == fAllPluginStartedNotifier) {
        if(fPanel){
            AIBoolean isShow = false;
            result = sAIPanel->IsShown(fPanel, isShow);
            if (isShow) {
                result = sAIPanel->Show(fPanel, false);
                result = sAIPanel->Show(fPanel, true);
            }
        }
    }
    else if (message->notifier == fAIArtSelectionChangedNotifier)
    {
        JJLock jjLock;
        AIArtSpec artSpecs[7] = {{kTextFrameArt, 0, 0}, {kPlacedArt, 0, 0}, {kGroupArt, 0, 0}, {kPathArt, 0, kArtSelected}, {kPluginArt, 0, 0}, {kCompoundPathArt, 0, 0}, {kSymbolArt, 0, 0}};
        ArtSetHelper artSets(artSpecs, 7);
        size_t artCount= 0;
        artCount = artSets.GetCount();

        AIArtHandle selectedArtHandle;
        short artType;
        if(artCount !=0 )
        {
            for(int k=0; k<artCount; k++)
            {
                selectedArtHandle = artSets[k];
                sAIArt->GetArtType(selectedArtHandle, &artType);
                
                if(jjLock.GetBooleanEntryFromHandleDict(selectedArtHandle, "lock") && artType == kPlacedArt)
                {
                    objJJLockPanel->tempArtHandle = selectedArtHandle;
                }
                if(jjLock.GetUnicodeStringEntryFromHandleDict(selectedArtHandle, "ParentID") != ai::UnicodeString(""))
                {
                    AIArtHandle parentHandle;
                    sAIArt->GetArtParent(selectedArtHandle, &parentHandle);
                    if(parentHandle != NULL)
                    {
                        if(jjLock.GetUnicodeStringEntryFromHandleDict(selectedArtHandle, "ParentID") != jjLock.GetUnicodeStringEntryFromHandleDict(parentHandle, "GroupID"))
                        {
                         result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                            break;
                        }
                    }
                }
                
            }
        }
    }
    else if(message->notifier == fAIArtObjectsChangedNotifier)
    {
    //       qDebug() << "fAIArtObjectsChangedNotifier ";
    }else if(message->notifier == fAILayerArtStyleChangeNotifier)
    {
     //   qDebug() << "fAILayerArtStyleChangeNotifier ";
    }else if(message->notifier == fAILayerOptionsNotifier)
    {
     //   qDebug() << "fAILayerOptionsNotifier ";
    }else if(message->notifier == fAILayerSetNotifier)
    {
     //   qDebug() << "fAILayerSetNotifier ";
    }else if(message->notifier == fAILayerDeletionNotifier)
    {
     //   qDebug() << "fAILayerDeletionNotifier ";
    }else if(message->notifier == fAICurrentLayerNotifier)
    {
        JJLock jjLock;
        AILayerHandle layerHnd;
        ai::UnicodeString title;
        sAILayer->GetCurrentLayer(&layerHnd);
        sAILayer->GetLayerTitle(layerHnd, title);

        AIArtSpec specs[7] = {{kTextFrameArt, kArtSelected, kArtSelected}, {kPlacedArt, kArtSelected, kArtSelected},  {kGroupArt, kArtSelected, kArtSelected}, {kPathArt, kArtSelected, kArtSelected}, {kPluginArt, kArtSelected, kArtSelected}, {kCompoundPathArt, kArtSelected, kArtSelected}, {kSymbolArt, kArtSelected, kArtSelected}};
        ArtSetHelper artSet(specs, 7);
        AIArtHandle artHandle;
        size_t count = 0;
        count = artSet.GetCount();
        
        if(count > 0)
        {
            for(int i=0; i<count; i++)
            {
                artHandle = artSet[i];
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock"))
                {
                    
                    short shortType;
                    sAIArt->GetArtType(artHandle, &shortType);
                    //qDebug() << "Title " << title.as_UTF8().c_str() << "  " << shortType;
                    
                    if(title != jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "layerName") && title != ai::UnicodeString("Isolation Mode"))
                    {
                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                        break;
                    }
                }
            }
        }
    }
    else if(message->notifier == fAIAdobeLinkPaletteMenuItemCommandPostNotifierStr)
    {
    }
    else if(message->notifier == fAITextFontCommandPostNotifierStr)
    {
        //qDebug()<<"";
    }
    else if (message->notifier == fDocumentClosePreNotifier){
        
    }
    else if(message->notifier == fAIAfterPasteCommandPreNotifier)
    {
        JJLock jjLock;
        AIArtSpec specs[7] = {{kTextFrameArt, kArtSelected, kArtSelected}, {kPlacedArt, kArtSelected, kArtSelected}, {kGroupArt, kArtSelectedTopLevelGroups, kArtSelected}, {kPathArt, kArtSelected, kArtSelected}, {kPluginArt, kArtSelected, kArtSelected}, {kCompoundPathArt, kArtSelected, kArtSelected}, {kSymbolArt, kArtSelected, kArtSelected}};
        ArtSetHelper artSet(specs, 7);
        AIArtHandle artHandle;
        size_t count = 0;
        count = artSet.GetCount();
        
        if(count > 0)
        {
            for(int i=0; i<count; i++)
            {
                artHandle = artSet[i];
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") && (!jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock")))
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Copy and paste not allowed for content and full locked object."));
                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                    break;
                }
            }
        }
    }
    else if (message->notifier == fDocumentClosePostNotifier)
    {

    }
    
    else if (message->notifier == fDocumentChangedNotifier)
    {
        JJLock jjLock;
        if(jjLock.IsDocumentOpened())
        {
            sAIUndo->SetSilent(true);
            openDocumentBool = true;
            objJJLockPanel->SetEnableAllButtons(true);
        }
        else
        {
            openDocumentBool = false;
            objJJLockPanel->SetEnableAllButtons(true);
        }
        
    }
    else if (message->notifier == fDocumentOpenedNotifier)
    {
        JJLock jjLock;
        JJLockPanel jjLockPanel;
        AppContext appContext(gPlugin->GetPluginRef());

        vector<ai::UnicodeString> artBoundIDList, artIconIDList, lockBoundIDList, lockIconIDList;
        AIArtSpec artSpec[7] = {{kTextFrameArt, 0, 0}, {kPlacedArt, 0, 0},  {kGroupArt, 0, 0}, {kPathArt, 0, 0}, {kPluginArt, 0, 0}, {kCompoundPathArt, 0, 0}, {kSymbolArt, 0, 0}};
        ArtSetHelper artSet(artSpec, 7);
        size_t progressCount, count = 0;
        count = artSet.GetCount();
        progressCount = count;

        AIArtHandle artHandle;
        ai::int32 checkLockCount = 0;
        ai::int32 incCountConditionCheck = 0;
        jjLock.GetIntegerEntryToDocumentDict("lockCount", &checkLockCount);

        if(checkLockCount > 0)
            jjLock.CreateLockIconSwatch();
        artBoundIDList.clear();
        artIconIDList.clear();
        lockBoundIDList.clear();
        lockIconIDList.clear();
        
        short type;
        while(count > 0 && checkLockCount > 0)
        {
            count--;
            artHandle = artSet[count];
            sAIArt->GetArtType(artHandle, &type);
            if(sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") == true)
            {
                artBoundIDList.push_back(jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
                artIconIDList.push_back(jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
                incCountConditionCheck++;
                jjLock.RecreateLockIconAndBound(artHandle);

                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock"))
                {
                    jjLockPanel.ContentLockClicked(artHandle);
                }
                else if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock"))
                {
                    jjLockPanel.PositionLockClicked(artHandle);
                }
                else if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                {
                    jjLockPanel.ContentAndPositionLockClicked(artHandle);
                }
                
            }
            if(type == kPathArt && artHandle != NULL && sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "isBoundLock") == true)
            {
                lockBoundIDList.push_back(jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "lockBoundID"));
            }
            else if((type == kGroupArt || type == kPlacedArt) && artHandle != NULL && sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "isLockIcon") == true)
            {
                lockIconIDList.push_back(jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "lockIconID"));
            }
        }

        if(jjLock.ClearMissingArtLockAndBound(artBoundIDList, lockBoundIDList, ai::UnicodeString("boundLockType")))
        {
            sAIUser->MessageAlert( ai::UnicodeString("Some of the locked object(s) are missing or corrupted. \n Can't retain lock for those art object."));
        }
        jjLock.ClearMissingArtLockAndBound(artIconIDList, lockIconIDList, ai::UnicodeString("iconLockType"));
        
        if(checkLockCount != incCountConditionCheck)
        {
            sAIUser->MessageAlert( ai::UnicodeString("Some of the locked object(s) are missing. \n Can't retain those items."));
            jjLock.SetIntegerEntryToDocumentDict("lockCount", incCountConditionCheck);  
        }
        
        documentIOpened = true;
        documentOpenedProcessCompleted = true;
        
        jjLock.SetIntegerEntryToDocumentDict("lockAPI", 1);

    }
    else if (message->notifier == fAIGroupCommandPreNotifier)
    {
        AIArtSpec specs[7] = {{kTextFrameArt, kArtSelected, kArtSelected}, {kPlacedArt, kArtSelected, kArtSelected}, {kPathArt, kArtSelected, kArtSelected}, {kPluginArt, kArtSelected, kArtSelected}, {kGroupArt, kArtSelected, kArtSelected}, {kSymbolArt, kArtSelected, kArtSelected}, {kCompoundPathArt, kArtSelected, kArtSelected}};
        ArtSetHelper artSet(specs, 7);
        
        JJLock jjLock;
        AIArtHandle artHandle;
        size_t count = 0;
        short type;
        count = artSet.GetCount();
        if(count > 0)
        {
            for(int i=0; i<count; i++)
            {
                artHandle = artSet[i];
                sAIArt->GetArtType(artHandle, &type);
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Unlock locked items before grouping"));
                    boolGrouping = true;
                    break;
                }
            }
        }
        
    }
    else if(message->notifier == fAIGroupCommandPostNotifier)
    {
        if(boolGrouping)
        {
         result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
            boolGrouping = false;
        }
    }
    else if (message->notifier == fAIUngroupCommandPreNotifier)
    {
        AIArtSpec specs[1] = {{kGroupArt, kArtSelected, kArtSelected}};
        ArtSetHelper artSet(specs, 1);
        
        JJLock jjLock;
        short type;
        AIArtHandle artHandle;
        size_t count = 0;
        count = artSet.GetCount();
        if(count > 0)
        {
            for(int i=0; i<count; i++)
            {
                artHandle = artSet[i];
                sAIArt->GetArtType(artHandle, &type);
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                {
                    sAIUser->MessageAlert(ai::UnicodeString("Unlock locked items before doing un group"));
                    boolGrouping = true;
                    break;
                }
            }
        }
    }
    else if(message->notifier == fAIUngroupCommandPostNotifier)
    {
        if(boolGrouping)
        {
         result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
            boolGrouping = false;
        }
    }
    
    else if (message->notifier == fSaveCommandPreNotifierStr || message->notifier == fSaveAsCommandPreNotifierStr || message->notifier == fSaveACopyAsCommandPreNotifierStr || message->notifier == fSaveAsTemplateCommandPreNotifierStr || message->notifier == fAdobeAISaveForWebCommandPreNotifierStr || message->notifier == fAdobeAISaveSelectedSlicesCommandPreNotifierStr || message->notifier == fSaveForOfficeCommandPreNotifierStr){
        
    }
     else if (message->notifier == fSaveCommandPostNotifierStr || message->notifier == fSaveAsCommandPostNotifierStr || message->notifier == fSaveACopyAsCommandPostNotifierStr || message->notifier == fSaveAsTemplateCommandPostNotifierStr || message->notifier == fAdobeAISaveForWebCommandPostNotifierStr || message->notifier == fAdobeAISaveSelectedSlicesCommandPostNotifierStr || message->notifier == fSaveForOfficeCommandPostNotifierStr){
         
         
    }
    else if(message->notifier == fAIDocumentViewInvalidRectChangedNotifier){
        
        AppContext appContext(gPlugin->GetPluginRef());
        ASErr result = kNoErr;
        AIArtHandle artHandle;
        SelectionHelper selHelper;
        TextHelper textHelper;
        
        JJLock jjLock;
        short type;
        if(documentOpenedProcessCompleted)
        {
        if(jjLock.IsDocumentOpened())
          objJJLockPanel->SetEnableAllButtons(true);
        else
        {
            documentOpenedProcessCompleted = false;
            objJJLockPanel->SetEnableAllButtons(true);
        }
            
        LayerChangeCheck();
        AIArtSpec specs[7] = {{kTextFrameArt, kArtSelected, kArtSelected}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
//                    AIArtSpec specs[7] = {{kTextFrameArt, kArtTargeted, kArtTargeted}, {kPlacedArt, kArtTargeted, kArtTargeted}, {kGroupArt, kArtTargeted, kArtTargeted}, {kPathArt, kArtTargeted, kArtTargeted}, {kPluginArt, kArtTargeted, kArtTargeted}, {kSymbolArt, kArtTargeted, kArtTargeted}, {kCompoundPathArt, kArtTargeted, kArtTargeted}};
        ArtSetHelper artSetMain(specs, 7);
        size_t count = 0;
        count = artSetMain.GetCount();
        if(count > 0)
        {
            for(int i=0; i<count; i++)
            {
                artHandle = artSetMain[i];
                
                result = sAIArt->GetArtType(artHandle, &type);
                aisdk::check_ai_error(result);
                
                AIArtHandle topParentHandle = artHandle;
                if(topParentHandle != NULL)
                {
                    AIArtHandle findParentHandle;
                    ASBoolean isArtLayerGroup;
                    while(topParentHandle != NULL)
                    {
                        findParentHandle = topParentHandle;
                        sAIArt->GetArtParent(topParentHandle, &topParentHandle);
                        sAIArt->IsArtLayerGroup((topParentHandle), &isArtLayerGroup);
                        if(isArtLayerGroup)
                            break;
                    }
                    topParentHandle = findParentHandle;
                }

                if((jjLock.GetIntegerEntryFromHandleDict(topParentHandle, "ChildLockCount") > 0 && jjLock.GetUnicodeStringEntryFromHandleDict(topParentHandle, "GroupID") != ai::UnicodeString("")))
                {
                    ASErr result = kNoErr;
                    AIRealRect artBounds = {0,0,0,0};
                    AIRealRect rectDict = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(topParentHandle, NULL, kNoStrokeBounds, &artBounds);
                    result = jjLock.GetRealEntryFromDocumentDict(topParentHandle, &rectDict.top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(topParentHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(topParentHandle, &rectDict.left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(topParentHandle, &rectDict.right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                    
                    const AIRealRect constArtBounds = artBounds;
                    const AIRealRect constRectBounds = rectDict;
                    
                    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                    {
                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                        sAIUser->MessageAlert(ai::UnicodeString
                                              ("Child art object locked under this Group, not allowed to move(Child/Group) out of group boundary"));
//                        return;
                        continue;
                    }
                }
                
                
            if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool") || (jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") &&  type == kGroupArt))
            {
                ai::UnicodeString artLockType;
                result = jjLock.GetUnicodeEntryFromDocumentDict(artHandle, &artLockType, "groupChildLockType");
                
                
//                qDebug() << "groupChildLockType "<< artLockType.as_UTF8().c_str() ;
//                qDebug() << "Child locked " << jjLock.GetBooleanEntryFromHandleDict(artHandle, "groupChildLockBool");
//                qDebug()<< " clock " <<jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock");
//                qDebug()<< " plock " <<jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock");
//                qDebug()<< " pclock " <<jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock");
//                qDebug()<< " lock " <<jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock");
                

                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") && artLockType == ai::UnicodeString(""))
                {
                    AIRealRect groupArtBounds  = {0,0,0,0};
                    AIRealRect groupRectDict = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &groupArtBounds);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &groupRectDict.top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &groupRectDict.bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &groupRectDict.left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &groupRectDict.right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);
                   
                    if(!(groupRectDict.top == 0 && groupRectDict.bottom == 0 && groupRectDict.left == 0 && groupRectDict.right == 0) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                    {
                        const AIRealRect constArtBounds = groupArtBounds;
                        const AIRealRect constRectBounds = groupRectDict;
                      
                        if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                        {
                            try {
                             result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                break;
                                continue;
                            }
                            catch (ai::Error &ex){
                                result = ex;
                            }
                            catch (ATE::Exception &ex){
                                result = ex.error;
                            }
                        }

                        ParseGroupChildForLockConditionCheck(artHandle, "pclock");
                   //     qDebug() << " groupTotalChildCount "<<totalGroupChildCount;
                    
                    }
                    else if(IsDictAndCurrentOwnRectBoundSame(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock"))
                    {
                        if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") &&  type == kGroupArt)
                        {
                            int countGroupChild = 0;
                            int totalGroupChildCount = jjLock.GetIntegerEntryFromHandleDict(artHandle, "totalGroupChildCount");
                            objJJLockPanel->ParseGroupChildForCount(artHandle, &countGroupChild);
                            qDebug() << countGroupChild <<"  " << totalGroupChildCount;
                            if((countGroupChild != totalGroupChildCount) && totalGroupChildCount != 0)
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                return;
                                continue;
                            }
                            else
                            {
                                AILayerHandle currentLayer;
                                sAILayer->GetCurrentLayer(&currentLayer);
                                objJJLockPanel->ContentLockClicked(artHandle);
                                jjLock.ReCreatePathHandle(artHandle);
                                sAILayer->SetCurrentLayer(currentLayer);
//                                return;
                                continue;
                            }
                        }
                        else
                        {
                            ParseGroupChildForLockConditionCheck(artHandle, "clock");
                        }
                    }
                    else if(IsDictAndCurrentOwnRectWidhHeightBoundSame(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock"))
                    {
                         /* comment for requirement of resizing for content lock
//                        objJJLockPanel->ParseGroupChildForLock(artHandle, "clock");
//
//                        AIRealRect artBounds = {0,0,0,0};
//                        sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
//                        jjLock.ReCreatePathHandle(artHandle);
//
//                        result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
//                        aisdk::check_ai_error(result);
//                        result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
//                        aisdk::check_ai_error(result);
//                        result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
//                        aisdk::check_ai_error(result);
//                        result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                          )*/
                        
                        if(type == kGroupArt && jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                        {
                            int countGroupChild = 0;
                            int totalGroupChildCount = jjLock.GetIntegerEntryFromHandleDict(artHandle, "totalGroupChildCount");
                            objJJLockPanel->ParseGroupChildForCount(artHandle, &countGroupChild);
                            qDebug() << countGroupChild <<"  " << totalGroupChildCount;
                            if((countGroupChild != totalGroupChildCount) && totalGroupChildCount != 0)
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                return;
                                continue;
                            }
                            else
                            {
                                AILayerHandle currentLayer;
                                sAILayer->GetCurrentLayer(&currentLayer);
                                objJJLockPanel->ContentLockClicked(artHandle);
                                jjLock.ReCreatePathHandle(artHandle);
                                sAILayer->SetCurrentLayer(currentLayer);
//                                return;
                                continue;
                            }
                        }
                    }
                }
                else if (!jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock"))
                {
                    if (ParseGroupChildForLockConditionCheck(artHandle, artLockType.as_UTF8().c_str()))
                    {
//                        break;
                        continue;
                    }
                }
            }
            
            
            if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
            {
                
                result = sAIArt->GetArtType(artHandle, &type);
                aisdk::check_ai_error(result);
           
                std::vector<AIArtHandle> linkArtHanldes;
                if(artHandle != NULL &&  type == kTextFrameArt)
                {
                    AIBool8 linkedText;
                    sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
                    if(linkedText)
                    {
                        artHandle = jjLock.GetParentTextFrameFromLinkedTextFrame(artHandle);
                        linkArtHanldes = jjLock.GetTextFramesFromLinkedTextFrame(artHandle);
                    }
                    else
                    {
                        linkArtHanldes.clear();
                        linkArtHanldes.push_back(artHandle);
                    }
                    
                    for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                    {
                        std::vector<AIFillStyle> charFeatureColorArray;
                        std::vector<int> charColorFillBool;
                        std::vector<AIStrokeStyle> charFeatureStrokeArray;
                        std::vector<int> charStrokeVisibleBool;
                        
                        std::vector<AIFillStyle> textFrameFillColorArray;
                        std::vector<int> textFrameColorFillBool;
                        std::vector<AIStrokeStyle> textFrameStrokeColorArray;
                        std::vector<int> textFrameColorStrokeBool;
                        
                        AIArtHandle textFrameArtHandle = NULL;
                        textFrameArtHandle = linkArtHanldes.at(i);
                        if(!jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "plock"))
                        {
                        ai::UnicodeString wholeTextFrameContent = jjLock.GetText(textFrameArtHandle);
                        ai::UnicodeString dicContent;
                        result = jjLock.GetUnicodeEntryFromDocumentDict(textFrameArtHandle, &dicContent, "JJLockContent");
                        aisdk::check_ai_error(result);

                        if(dicContent.length() > 0 && type == kTextFrameArt && (jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "clock")) )
                        {
                            if(dicContent != wholeTextFrameContent)
                            {
                                try {
                                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                    if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock"))
                                    jjLock.SetArtHandleBoundInDict(textFrameArtHandle);
//                                    break;
                                    continue;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                    result = ex.error;
                                }
                            }

                            if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "clock") && type == kTextFrameArt)
                            {
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<ai::UnicodeString> charfontStyleName;
                                std::vector<ai::UnicodeString> charfontFamilyName;
                                std::vector<double> charFontSizeArray;
                                std::vector<int> charStrokeBool;
                                std::vector<AIStrokeStyle> charStrokeStyle;
                                
                                jjLock.GetCharacterFontProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, textFrameArtHandle);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                                
                                jjLock.GetCharacterStroke(&charFeatureStrokeArray, &charStrokeVisibleBool, textFrameArtHandle);
                                jjLock.GetArrayEntryForStrokeStyle(textFrameArtHandle, "charStrokeStyle", &charStrokeStyle);
                                jjLock.GetArrayEntryForInteger(textFrameArtHandle, "charStrokeStyleBool", &charStrokeBool);

                                for(int i = 0; i < charFeatureStrokeArray.size(); i++)
                                {
                                    if(( (charFeatureStrokeArray.at(i).width != charStrokeStyle.at(i).width) || (charStrokeVisibleBool.at(i) != charStrokeBool.at(i))) )
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind != charStrokeStyle.at(i).color.kind && (charStrokeVisibleBool.at(i) != charStrokeBool.at(i)))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kFourColor && charStrokeStyle.at(i).color.kind == kFourColor &&
                                             ((charFeatureStrokeArray.at(i).color.c.f.black != charStrokeStyle.at(i).color.c.f.black) ||
                                              (charFeatureStrokeArray.at(i).color.c.f.cyan != charStrokeStyle.at(i).color.c.f.cyan) ||
                                              (charFeatureStrokeArray.at(i).color.c.f.magenta != charStrokeStyle.at(i).color.c.f.magenta) ||
                                              (charFeatureStrokeArray.at(i).color.c.f.yellow != charStrokeStyle.at(i).color.c.f.yellow))
                                            )
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kGradient && charStrokeStyle.at(i).color.kind == kGradient &&
                                             (charFeatureStrokeArray.at(i).color.c.b.gradient != charStrokeStyle.at(i).color.c.b.gradient))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kGrayColor && charStrokeStyle.at(i).color.kind == kGrayColor &&
                                             (charFeatureStrokeArray.at(i).color.c.g.gray != charStrokeStyle.at(i).color.c.g.gray))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kCustomColor && charStrokeStyle.at(i).color.kind == kCustomColor &&
                                             (charFeatureStrokeArray.at(i).color.c.c.color != charStrokeStyle.at(i).color.c.c.color ||
                                              charFeatureStrokeArray.at(i).color.c.c.tint != charStrokeStyle.at(i).color.c.c.tint ))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kThreeColor && charStrokeStyle.at(i).color.kind == kThreeColor &&
                                             (charFeatureStrokeArray.at(i).color.c.rgb.blue != charStrokeStyle.at(i).color.c.rgb.blue ||
                                              charFeatureStrokeArray.at(i).color.c.rgb.green != charStrokeStyle.at(i).color.c.rgb.green ||
                                              charFeatureStrokeArray.at(i).color.c.rgb.red != charStrokeStyle.at(i).color.c.rgb.red ))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if(charFeatureStrokeArray.at(i).color.kind == kPattern && charStrokeStyle.at(i).color.kind == kPattern &&
                                             (charFeatureStrokeArray.at(i).color.c.p.pattern != charStrokeStyle.at(i).color.c.p.pattern))
                                    {
                                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                        break;
                                        continue;
                                    }
                                    else if((charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)))
                                    {
                                        jjLock.ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i));
                                    }
                                }
                            }
                            if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock") && type == kTextFrameArt)
                            {
                                std::vector<int> charFillBool;
                                std::vector<AIFillStyle> charFillStyle;
                                
                                std::vector<int> charStrokeBool;
                                std::vector<AIStrokeStyle> charStrokeStyle;
                                
                                std::vector<int> textFrameFillVisibleBool;
                                std::vector<AIFillStyle> textFrameObjectFillStyleArray;
                                
                                std::vector<int> textFrameStrokeVisibleBool;
                                std::vector<AIStrokeStyle> textFrameObjectStrokeStyleArray;
                                
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                                std::vector<double> charFontSize, charHScaleSize;
                                std::vector<ai::UnicodeString> charfontStyleName;
                                std::vector<ai::UnicodeString> charfontFamilyName;

                                jjLock.GetCharacterColor(&charFeatureColorArray, &charColorFillBool, textFrameArtHandle);
                                jjLock.GetArrayEntryForFillStyle(textFrameArtHandle, "charFillStyle", &charFillStyle);
                                jjLock.GetArrayEntryForInteger(textFrameArtHandle, "charFillStyleBool", &charFillBool);
                                
                                jjLock.GetCharacterStroke(&charFeatureStrokeArray, &charStrokeVisibleBool, textFrameArtHandle);
                                jjLock.GetArrayEntryForStrokeStyle(textFrameArtHandle, "charStrokeStyle", &charStrokeStyle);
                                jjLock.GetArrayEntryForInteger(textFrameArtHandle, "charStrokeStyleBool", &charStrokeBool);
                                
                                jjLock.GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                                
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charFontSize", &charFontSize);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", &charHScaleSize);
                                
                                
                                for(int i = 0; i < charFeatureColorArray.size(); i++)
                                {
                                    if(( (charFeatureStrokeArray.at(i).width != charStrokeStyle.at(i).width) || (charStrokeVisibleBool.at(i) != charStrokeBool.at(i))) )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if(((charFeatureColorArray.at(i).color.kind != charFillStyle.at(i).color.kind)  && (charColorFillBool.at(i) != charFillBool.at(i)) ) || ((charFeatureStrokeArray.at(i).color.kind != charStrokeStyle.at(i).color.kind) && (charStrokeVisibleBool.at(i) != charStrokeBool.at(i))) )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                        
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kFourColor && charFillStyle.at(i).color.kind == kFourColor &&
                                       ((charFeatureColorArray.at(i).color.c.f.black != charFillStyle.at(i).color.c.f.black) ||
                                       (charFeatureColorArray.at(i).color.c.f.cyan != charFillStyle.at(i).color.c.f.cyan) ||
                                       (charFeatureColorArray.at(i).color.c.f.magenta != charFillStyle.at(i).color.c.f.magenta) ||
                                       (charFeatureColorArray.at(i).color.c.f.yellow != charFillStyle.at(i).color.c.f.yellow)))
                                       ||
                                       (charFeatureStrokeArray.at(i).color.kind == kFourColor && charStrokeStyle.at(i).color.kind == kFourColor &&
                                        ((charFeatureStrokeArray.at(i).color.c.f.black != charStrokeStyle.at(i).color.c.f.black) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.cyan != charStrokeStyle.at(i).color.c.f.cyan) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.magenta != charStrokeStyle.at(i).color.c.f.magenta) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.yellow != charStrokeStyle.at(i).color.c.f.yellow)))
                                        )
                                        
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kGradient && charFillStyle.at(i).color.kind == kGradient &&
                                            (charFeatureColorArray.at(i).color.c.b.gradient != charFillStyle.at(i).color.c.b.gradient))
                                            ||
                                            (charFeatureStrokeArray.at(i).color.kind == kGradient && charStrokeStyle.at(i).color.kind == kGradient &&
                                             (charFeatureStrokeArray.at(i).color.c.b.gradient != charStrokeStyle.at(i).color.c.b.gradient))
                                            )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kGrayColor && charFillStyle.at(i).color.kind == kGrayColor &&
                                            (charFeatureColorArray.at(i).color.c.g.gray != charFillStyle.at(i).color.c.g.gray))
                                            ||
                                            (charFeatureStrokeArray.at(i).color.kind == kGrayColor && charStrokeStyle.at(i).color.kind == kGrayColor &&
                                            (charFeatureStrokeArray.at(i).color.c.g.gray != charStrokeStyle.at(i).color.c.g.gray))
                                            )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kCustomColor && charFillStyle.at(i).color.kind == kCustomColor &&
                                            (charFeatureColorArray.at(i).color.c.c.color != charFillStyle.at(i).color.c.c.color ||
                                             charFeatureColorArray.at(i).color.c.c.tint != charFillStyle.at(i).color.c.c.tint ))
                                            ||
                                            (charFeatureStrokeArray.at(i).color.kind == kCustomColor && charStrokeStyle.at(i).color.kind == kCustomColor &&
                                             (charFeatureStrokeArray.at(i).color.c.c.color != charStrokeStyle.at(i).color.c.c.color ||
                                              charFeatureStrokeArray.at(i).color.c.c.tint != charStrokeStyle.at(i).color.c.c.tint ))
                                            )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kThreeColor && charFillStyle.at(i).color.kind == kThreeColor &&
                                            (charFeatureColorArray.at(i).color.c.rgb.blue != charFillStyle.at(i).color.c.rgb.blue ||
                                             charFeatureColorArray.at(i).color.c.rgb.green != charFillStyle.at(i).color.c.rgb.green ||
                                             charFeatureColorArray.at(i).color.c.rgb.red != charFillStyle.at(i).color.c.rgb.red ))
                                            ||
                                            (charFeatureStrokeArray.at(i).color.kind == kThreeColor && charStrokeStyle.at(i).color.kind == kThreeColor &&
                                             (charFeatureStrokeArray.at(i).color.c.rgb.blue != charStrokeStyle.at(i).color.c.rgb.blue ||
                                              charFeatureStrokeArray.at(i).color.c.rgb.green != charStrokeStyle.at(i).color.c.rgb.green ||
                                              charFeatureStrokeArray.at(i).color.c.rgb.red != charStrokeStyle.at(i).color.c.rgb.red ))
                                            )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charFeatureColorArray.at(i).color.kind == kPattern && charFillStyle.at(i).color.kind == kPattern &&
                                            (charFeatureColorArray.at(i).color.c.p.pattern != charFillStyle.at(i).color.c.p.pattern))
                                            ||
                                        (charFeatureStrokeArray.at(i).color.kind == kPattern && charStrokeStyle.at(i).color.kind == kPattern &&
                                         (charFeatureStrokeArray.at(i).color.c.p.pattern != charStrokeStyle.at(i).color.c.p.pattern))
                                        )
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        break;
                                    }
                                    else if((charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)) ||
                                            (charFontSizeArray.at(i) != charFontSize.at(i)) )
                                    {
                                        jjLock.ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i), charFontSize.at(i), charHScaleSize.at(i));
                                    }
                                }
                            }
                         }
                        }
                        else  if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "plock") && type == kTextFrameArt )
                        {
                            AITextFrameType textFrameType;
                            sAITextFrame->GetType(textFrameArtHandle, &textFrameType);
                            
                            AIRealRect artBounds = {0,0,0,0};
                            sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                            
                            AIRealRect rectDict;
                            rectDict.top = 0;
                            rectDict.bottom = 0;
                            rectDict.left = 0;
                            rectDict.right = 0;
                            
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.top, "JJLockArtBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.left, "JJLockArtBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.right, "JJLockArtBoundRight");
                            aisdk::check_ai_error(result);
                            
                            const AIRealRect constArtBounds = artBounds;
                            const AIRealRect constRectBounds = rectDict;
                            
                            if(sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                            {
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                                
                                ai::UnicodeString dicContent;
                                ai::UnicodeString wholeTextFrameContent = jjLock.GetText(textFrameArtHandle);
                                result = jjLock.GetUnicodeEntryFromDocumentDict(textFrameArtHandle, &dicContent, "JJLockContent");
                                aisdk::check_ai_error(result);
                                if(dicContent != wholeTextFrameContent)
                                {
                                    result = jjLock.SetUnicodeEntryToDocumentDict(textFrameArtHandle, wholeTextFrameContent, "JJLockContent");
                                    aisdk::check_ai_error(result);

                                    jjLock.GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                                    jjLock.SetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", charfontStyleNameArray);
                                    jjLock.SetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", charfontFamilyNameArray);
                                    jjLock.SetArrayEntryForReal(textFrameArtHandle, "charFontSize", charFontSizeArray);
                                    jjLock.SetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", charHScaleSizeArray);
                                    
                                    ai::int32 getJustificationValue = 0;
                                    jjLock.GetParagraphJustification(textFrameArtHandle, &getJustificationValue);
                                    result = jjLock.SetIntegerEntryToHandleDict(textFrameArtHandle, "ParaJustification", getJustificationValue);
                                    aisdk::check_ai_error(result);
                                }
                            }
                            else if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds) && textFrameType == kPointTextType)
                            {
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                                std::vector<double> charFontSize, charHScaleSize;
                                std::vector<ai::UnicodeString> charfontStyleName;
                                std::vector<ai::UnicodeString> charfontFamilyName;
                                int arraySize = 0;
                                bool isFontChangeInt = false;
                                
                                jjLock.GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charFontSize", &charFontSize);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", &charHScaleSize);
                                
                                if (charfontStyleNameArray.size() >= charfontStyleName.size())
                                {
                                    arraySize = charfontStyleName.size();
                                }
                                else
                                {
                                    arraySize = charfontStyleNameArray.size();
                                }
                                for(int i = 0; i < arraySize; i++)
                                {
                                    if( (charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)) || (charFontSizeArray.at(i) != charFontSize.at(i)) || (charHScaleSizeArray.at(i) !=  charHScaleSize.at(i)) )
                                    {
                                        jjLock.ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i), charFontSize.at(i), charHScaleSize.at(i));
                                        isFontChangeInt = true;
                                    }
                                }
                                
                                if(isFontChangeInt == false)
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
//                                    break;
                                    continue;
                                }
                                
                            }
                            else if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds) && textFrameType != kPointTextType)
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                break;
                                continue;
                            }
                        }
                        
                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                        
                        AIRealRect rectDict;
                        rectDict.top = 0;
                        rectDict.bottom = 0;
                        rectDict.left = 0;
                        rectDict.right = 0;
                        
                        result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.top, "JJLockArtBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.left, "JJLockArtBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.right, "JJLockArtBoundRight");
                        aisdk::check_ai_error(result);
                        
                        if(type == kTextFrameArt  && !jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "clock"))
                        {
                            ai::int32 getParaJustify;
                            int justifyValue = jjLock.GetIntegerEntryFromHandleDict(textFrameArtHandle, "ParaJustification");
                            jjLock.GetParagraphJustification(textFrameArtHandle, &getParaJustify);
                            if(getParaJustify != justifyValue)
                            {
                                try
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
//                                    break;
                                    continue;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                    result = ex.error;
                                }
                            }
                        }
                        
                        if(!(rectDict.top == 0 && rectDict.bottom == 0 && rectDict.left == 0 && rectDict.right == 0))
                        {
                            AIRealRect constArtBounds = artBounds;
                            AIRealRect constRectBounds = rectDict;
                            AITextFrameType textFrameType;
                            sAITextFrame->GetType(textFrameArtHandle, &textFrameType);

                            if((jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "plock")) && type == kTextFrameArt)
                            {
                                std::vector<ai::UnicodeString> charfontStyleNameArray;
                                std::vector<ai::UnicodeString> charfontFamilyNameArray;
                                std::vector<double> charFontSizeArray, charHScaleSizeArray;
                                std::vector<double> charFontSize, charHScaleSize;
                                std::vector<ai::UnicodeString> charfontStyleName;
                                std::vector<ai::UnicodeString> charfontFamilyName;
                                int arraySize = 0;
                                bool isFontChanged = false;
                                
                                jjLock.GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                                jjLock.GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charFontSize", &charFontSize);
                                jjLock.GetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", &charHScaleSize);
                                
                                if (charfontStyleNameArray.size() >= charfontStyleName.size())
                                {
                                    arraySize = charfontStyleName.size();
                                }
                                else
                                {
                                    arraySize = charfontStyleNameArray.size();
                                }
                                 for(int i = 0; i < arraySize; i++)
                                 {
                                     if( (charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)) || (charFontSizeArray.at(i) != charFontSize.at(i)) || (charHScaleSizeArray.at(i) !=  charHScaleSize.at(i)) )
                                     {
                                         jjLock.ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i), charFontSize.at(i), charHScaleSize.at(i));
                                         isFontChanged = true;
                                     }
                                 }
                                AIRealRect artBounds = {0,0,0,0};
                                sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                                constArtBounds = artBounds;
                               
                                
                                if(isFontChanged == true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                                {
                                    AIRealRect artBounds = {0,0,0,0};
                                    sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                                    AIReal top = artBounds.top;
                                    AIReal left = artBounds.left;
                                    AIReal right = artBounds.right;
                                    AIReal bottom = artBounds.bottom;
                                    result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, top, "JJLockArtBoundTop");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, left, "JJLockArtBoundLeft");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, right, "JJLockArtBoundRight");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, bottom, "JJLockArtBoundBottom");
                                    aisdk::check_ai_error(result);
                                }

                                if(isFontChanged != true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                                {
                                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);

//                                    if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "plock"))
//                                        objJJLockPanel->PositionLockClicked(textFrameArtHandle);
//                                    else if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock"))
//                                        objJJLockPanel->ContentAndPositionLockClicked(textFrameArtHandle);
                                   // break;
                                    continue;
                                }
                                else if(isFontChanged == true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
//                                    if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "plock"))
//                                        objJJLockPanel->PositionLockClicked(textFrameArtHandle);
//                                    else if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "pclock"))
//                                        objJJLockPanel->ContentAndPositionLockClicked(textFrameArtHandle);
                                   // break;
                                    continue;
                                }
                            }
                            else if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                            {
                                try
                                {
                                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                                    break;
                                    continue;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                result = ex.error;
                                }
                            }
                        }
                        else if(jjLock.GetBooleanEntryFromHandleDict(textFrameArtHandle, "clock") && type == kTextFrameArt)
                        {
                            
                            AIRealRect ownRectDict;
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
                            aisdk::check_ai_error(result);
                            
                            const AIRealRect constArtBounds = artBounds;
                            const AIRealRect constRectBounds = ownRectDict;

                            if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                            {
                                jjLock.ReCreatePathHandle(textFrameArtHandle);
                                result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.top, "JJLockArtOwnBoundTop");
                                aisdk::check_ai_error(result);
                                result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                                aisdk::check_ai_error(result);
                                result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.right, "JJLockArtOwnBoundRight");
                                aisdk::check_ai_error(result);
                                result = jjLock.SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                            }
                        }
                    }
                    linkArtHanldes.clear();
                }
                else if(type == kPlacedArt || type == kPathArt || type == kGroupArt || type == kPluginArt || type == kCompoundPathArt || type == kSymbolArt)
                {
                    if((jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock")) && type == kPlacedArt)
                    {
                        ai::UnicodeString placedFileContent;
                        placedFileContent = objJJLockPanel->GetFileInfoContent(artHandle);
                        ai::UnicodeString dicContent;
                        result = jjLock.GetUnicodeEntryFromDocumentDict(artHandle, &dicContent, "JJLockContent");
                        aisdk::check_ai_error(result);
                        
                        if(dicContent.length() > 0)
                        {
                            if(dicContent != placedFileContent)
                            {
                                try {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
//                                    break;
                                    continue;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                    result = ex.error;
                                }
                            }
                        }
                    }
                    if((jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock")) && type == kPathArt)
                    {
                        ai::int16 pathSegmentCount;
                        sAIPath->GetPathSegmentCount(artHandle, &pathSegmentCount);
                        ai::int16 dicContent;
                        dicContent = jjLock.GetIntegerEntryFromHandleDict(artHandle, "JJLockContent");
                        aisdk::check_ai_error(result);
                        
                        if(dicContent != 0)
                        {
                            if(dicContent != pathSegmentCount)
                            {
                                try {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    //break;
                                    continue;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                    result = ex.error;
                                }
                            }
                       
                            bool boundWidthHeight = false;
                            {
                            AIReal boundWidth, boundHeight, currentWidth, currentHeight;
                            
                            
                            AIRealRect artBounds = {0,0,0,0};
                            sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                            
                            boundWidth = artBounds.right - artBounds.left;
                            boundHeight = artBounds.top - artBounds.bottom;
                            
                            AIRealRect ownRectDict;
                            result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
                            aisdk::check_ai_error(result);
                            
                            
                            currentWidth = ownRectDict.right - ownRectDict.left;
                            currentHeight = ownRectDict.top - ownRectDict.bottom;
                                ai::int16 pathSegSelected = 0, segSelectedCount = 0, selectedPoints = 0;
                                AIAnchorIdentifier pathAnchorIds;
                                AIBoolean fullyArtSelected = false;
                                sAIPath->GetPathSegmentSelected(artHandle, 0, &pathSegSelected);
                                sAIPath->GetPathAllSegmentsSelected(artHandle, &fullyArtSelected);
                              //  qDebug() << "SElectect " << pathSegSelected<< "  " << segSelectedCount << "  " << pathAnchorIds.segmentOffset <<" "<<pathAnchorIds.fractionalOffset<< " FS  " << fullyArtSelected;
                                
                                if( (((float)((int)(boundWidth * 100000))/100000) == ((float)((int)(currentWidth * 100000))/100000) && ((float)((int)(boundHeight * 100000))/100000) == ((float)((int)(currentHeight * 100000))/100000)) && (jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock")) && (fullyArtSelected == true) )
                                {
                                    boundWidthHeight = true;

                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                                    aisdk::check_ai_error(result);


                                    ai::int16 segNumber = 0, countSegment = 0;
                                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                                    AIPathSegment restorePathSegments[countSegment];
                                    sAIPath->GetPathSegments(artHandle, segNumber, countSegment, restorePathSegments);

                                    std::vector<AIRealPoint> realPointArray;
                                    for(int i = 0; i < countSegment; i++)
                                    {
                                        realPointArray.push_back(restorePathSegments[i].p);
                                    }
                                    jjLock.SetArrayEntryForRealPoint(artHandle, "pathSegments", realPointArray);
                                    jjLock.ReCreatePathHandle(artHandle);
                                    sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                                    //return;
                                    continue;
                                }
                                else if( (((float)((int)(boundWidth * 100000))/100000) != ((float)((int)(currentWidth * 100000))/100000) || ((float)((int)(boundHeight * 100000))/100000) != ((float)((int)(currentHeight * 100000))/100000)) && (jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock")) && (fullyArtSelected == true) )
                                {
                                    boundWidthHeight = true;
                                    
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                                    aisdk::check_ai_error(result);
                                    result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                                    aisdk::check_ai_error(result);
                                    
                                    ai::int16 segNumber=0, countSegment = 0;
                                    sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                                    AIPathSegment restorePathSegments[countSegment];
                                    sAIPath->GetPathSegments(artHandle, segNumber, countSegment, restorePathSegments);
                                    
                                    std::vector<AIRealPoint> realPointArray;
                                    for(int i = 0; i < countSegment; i++)
                                    {
                                        realPointArray.push_back(restorePathSegments[i].p);
                                    }
                                    jjLock.SetArrayEntryForRealPoint(artHandle, "pathSegments", realPointArray);
                                    jjLock.ReCreatePathHandle(artHandle);
                                    
                                    sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                                    return;
                                }
                                else
                                {
                                    boundWidthHeight = false;
                                }
                            }

                            bool isUndoCalled = false;
                            std::vector<AIRealPoint> realPointArray;
                            AIReal pathArea=0, pathLength=0, pathFlatness=0;
                            ai::int16 segNumber = 0, countSegment = 0;
                            sAIPath->GetPathSegmentCount(artHandle, &countSegment);
                            AIPathSegment pathSegments[countSegment];
                            sAIPath->GetPathSegments(artHandle, segNumber, countSegment, pathSegments);
                            sAIPath->GetPathArea(artHandle, &pathArea);
                            sAIPath->GetPathLength(artHandle, &pathLength, pathFlatness);
                            jjLock.GetArrayEntryForRealPoint(artHandle, "pathSegments", &realPointArray);
                            
                            if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                            for(int i = 0; i < pathSegmentCount; i++)
                            {
//                                qDebug() << pathSegments[i].p.h;
//                                qDebug() << realPointArray.at(i).h;
//
//                                qDebug() << " ";
//                                qDebug()<< pathSegments[i].p.v;
//                                qDebug() << realPointArray.at(i).v;
                                if(((realPointArray.at(i).h != pathSegments[i].p.h) || (realPointArray.at(i).v != pathSegments[i].p.v)) &&
                                    (!boundWidthHeight))
                                {
                                    try
                                    {
                                        result = sAIDocument->Undo();
                                        aisdk::check_ai_error(result);
                                        sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                                        isUndoCalled = true;
                                        break;
                                    }
                                    catch (ai::Error &ex){
                                        result = ex;
                                    }
                                    catch (ATE::Exception &ex){
                                        result = ex.error;
                                    }
                                }
                            }
                            if(isUndoCalled)
                            {
                              //  return;
                                
                                continue;
                            }
                        }
                        if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                        {
                            AIPathStyle pathStyle;
                            int i = 0;
                            std::vector<AIFillStyle> pathFillArray;
                            std::vector<AIStrokeStyle> pathStrokeArray;
                            
                            result= sAIPathStyle->GetPathStyle(artHandle, &pathStyle);
                            aisdk::check_ai_error(result);
                            
                            jjLock.GetArrayEntryForFillStyle(artHandle, "pathFillStyle", &pathFillArray);
                            jjLock.GetArrayEntryForStrokeStyle(artHandle, "pathStrokeStyle", &pathStrokeArray);
                            
                            
                            if((pathFillArray.at(i).color.kind != pathStyle.fill.color.kind) || (pathStrokeArray.at(i).color.kind != pathStyle.stroke.color.kind))
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                            //    return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kFourColor && pathStyle.fill.color.kind == kFourColor &&
                                     ((pathFillArray.at(i).color.c.f.black != pathStyle.fill.color.c.f.black) ||
                                      (pathFillArray.at(i).color.c.f.cyan != pathStyle.fill.color.c.f.cyan) ||
                                      (pathFillArray.at(i).color.c.f.magenta != pathStyle.fill.color.c.f.magenta) ||
                                      (pathFillArray.at(i).color.c.f.yellow != pathStyle.fill.color.c.f.yellow)))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kFourColor && pathStyle.stroke.color.kind == kFourColor &&
                                     ((pathStrokeArray.at(i).color.c.f.black != pathStyle.stroke.color.c.f.black) ||
                                      (pathStrokeArray.at(i).color.c.f.cyan != pathStyle.stroke.color.c.f.cyan) ||
                                      (pathStrokeArray.at(i).color.c.f.magenta != pathStyle.stroke.color.c.f.magenta) ||
                                      (pathStrokeArray.at(i).color.c.f.yellow != pathStyle.stroke.color.c.f.yellow)))
                                    )
                                
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                              //  return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kGradient && pathStyle.fill.color.kind == kGradient &&
                                     (pathFillArray.at(i).color.c.b.gradient != pathStyle.fill.color.c.b.gradient))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kGradient && pathStyle.stroke.color.kind == kGradient &&
                                     (pathStrokeArray.at(i).color.c.b.gradient != pathStyle.stroke.color.c.b.gradient))
                                    )
                            {
                                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                             //   return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kGrayColor && pathStyle.fill.color.kind == kGrayColor &&
                                     (pathFillArray.at(i).color.c.g.gray != pathStyle.fill.color.c.g.gray))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kGrayColor && pathStyle.stroke.color.kind == kGrayColor &&
                                     (pathStrokeArray.at(i).color.c.g.gray != pathStyle.stroke.color.c.g.gray))
                                    )
                            {
                             result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                             //   return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kCustomColor && pathStyle.fill.color.kind == kCustomColor &&
                                     (pathFillArray.at(i).color.c.c.color != pathStyle.fill.color.c.c.color ||
                                      pathFillArray.at(i).color.c.c.tint != pathStyle.fill.color.c.c.tint ))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kCustomColor && pathStyle.stroke.color.kind == kCustomColor &&
                                     (pathStrokeArray.at(i).color.c.c.color != pathStyle.stroke.color.c.c.color ||
                                      pathStrokeArray.at(i).color.c.c.tint != pathStyle.stroke.color.c.c.tint ))
                                    )
                            {
                             result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                             //   return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kThreeColor && pathStyle.fill.color.kind == kThreeColor &&
                                     (pathFillArray.at(i).color.c.rgb.blue != pathStyle.fill.color.c.rgb.blue ||
                                      pathFillArray.at(i).color.c.rgb.green != pathStyle.fill.color.c.rgb.green ||
                                      pathFillArray.at(i).color.c.rgb.red != pathStyle.fill.color.c.rgb.red ))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kThreeColor && pathStyle.stroke.color.kind == kThreeColor &&
                                     (pathStrokeArray.at(i).color.c.rgb.blue != pathStyle.stroke.color.c.rgb.blue ||
                                      pathStrokeArray.at(i).color.c.rgb.green != pathStyle.stroke.color.c.rgb.green ||
                                      pathStrokeArray.at(i).color.c.rgb.red != pathStyle.stroke.color.c.rgb.red ))
                                    )
                            {
                             result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                              //  return true;
                                continue;
                            }
                            else if((pathFillArray.at(i).color.kind == kPattern && pathStyle.fill.color.kind == kPattern &&
                                     (pathFillArray.at(i).color.c.p.pattern != pathStyle.fill.color.c.p.pattern))
                                    ||
                                    (pathStrokeArray.at(i).color.kind == kPattern && pathStyle.stroke.color.kind == kPattern &&
                                     (pathStrokeArray.at(i).color.c.p.pattern != pathStyle.stroke.color.c.p.pattern))
                                    )
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                sAIArt->SetArtUserAttr(artHandle, kArtSelected | kArtFullySelected, 0);
                           //     return true;
                                continue;
                            }
                            
                        }
                    }
                    if((jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock")) && type == kCompoundPathArt)
                    {
                        if(IsDictAndCurrentOwnRectBoundSame(artHandle))
                        {
                            if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock"))
                                ParseCompoundPathArtGroupCheck(artHandle, artHandle, "pclock");
                            else if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock"))
                                ParseCompoundPathArtGroupCheck(artHandle, artHandle, "clock");

                        }
                        else if(IsDictAndCurrentOwnRectWidhHeightBoundSame(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock"))
                        {
                            objJJLockPanel->ParseCompoundPathForLock(artHandle, "clock");
                            
                        }
                    }
                    
                    if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock") || (jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") && type != kGroupArt ))
                    {

                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                       
                        AIRealRect rectDict;
                        rectDict.top = 0;
                        rectDict.bottom = 0;
                        rectDict.left = 0;
                        rectDict.right = 0;
                       
                       
                        result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.top, "JJLockArtBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.left, "JJLockArtBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.right, "JJLockArtBoundRight");
                        aisdk::check_ai_error(result);
                       
                       
                        if(!(rectDict.top == 0 && rectDict.bottom == 0 && rectDict.left == 0 && rectDict.right == 0))
                        {
                            const AIRealRect constArtBounds = artBounds;
                            const AIRealRect constRectBounds = rectDict;
                            if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                            {
                                try {
                                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                    break;
                                }
                                catch (ai::Error &ex){
                                    result = ex;
                                }
                                catch (ATE::Exception &ex){
                                    result = ex.error;
                                }
                            }
                        }
                        else
                            jjLock.TransformLockAndBound(artHandle);
                    }

                    AIRealRect artBounds = {0,0,0,0};
                    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
                    
                    AIRealRect rectDict;
                    rectDict.top = 0;
                    rectDict.bottom = 0;
                    rectDict.left = 0;
                    rectDict.right = 0;
                    
                    
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.top, "JJLockArtBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.left, "JJLockArtBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &rectDict.right, "JJLockArtBoundRight");
                    aisdk::check_ai_error(result);

                    if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") && (type == kPlacedArt || type == kPathArt || type == kCompoundPathArt))
                    {
                        if(!IsDictAndCurrentOwnRectBoundSame(artHandle))
                        {
                            jjLock.ReCreatePathHandle(artHandle);
                            
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                            
                        }
                        else if(!IsDictAndCurrentOwnRectBoundLengthSame(artHandle) && (type == kPlacedArt))
                        {
                            jjLock.ReCreatePathHandle(artHandle);
                            
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.top, "JJLockArtOwnBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.right, "JJLockArtOwnBoundRight");
                            aisdk::check_ai_error(result);
                            result = jjLock.SetRealEntryToDocumentDict(artHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                        }
                    }
                }
              }
                
            }
            
            //// not to allow relink image - currently we don't have notifier and we are not allowing to delete the previous image ////
            if(type == kPlacedArt)
            {
                ai::int32 lockCount = 0;
                int lockHandleCount = 0;
                jjLock.GetIntegerEntryToDocumentDict("lockCount", &lockCount);
                
                AIArtSpec artSpec[7] = {{kTextFrameArt, 0, 0}, {kPlacedArt, 0, 0}, {kGroupArt, 0, 0}, {kPathArt, 0, 0}, {kPluginArt, 0, 0}, {kSymbolArt, 0, 0}, {kCompoundPathArt, 0, 0}};
                ArtSetHelper artSet(artSpec, 7);
                size_t count = 0;
                count = artSet.GetCount();
                short type;
                AIArtHandle artHandle;
                while(count > 0)
                {
                    count--;
                    artHandle = artSet[count];
                    result = sAIArt->GetArtType(artHandle, &type);
                    ai::check_ai_error(result);
                    
                    if(sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                        lockHandleCount++;
                }
                if(jjLock.GetBooleanEntryFromHandleDict(objJJLockPanel->tempArtHandle, "plock") && (lockHandleCount != lockCount))
                {
                    AIArtSpec artSpecs[1] = {{kPlacedArt, kArtSelected, kArtSelected}};
                    ArtSetHelper artSets(artSpecs, 1);
                    size_t artCount = 0;
                    artCount = artSets.GetCount();
                    AIArtHandle selectedArtHandle;
                    if(artCount == 1 )
                    {
                        
                        AIArtHandle IconHandle = jjLock.GetLockIconHandle();
                        AILayerHandle lockLayer =  jjLock.CreateLockParentLayer();
                        ASBoolean nameBool = true;
                        ai::UnicodeString iconArtName;
                        
                        selectedArtHandle = artSets[artCount-1];
                        
                        
                        AIRealRect artBounds = {0,0,0,0};
                        sAIArt->GetArtTransformBounds(selectedArtHandle, NULL, kNoStrokeBounds, &artBounds);
                        AIReal top = artBounds.top;
                        AIReal left = artBounds.left;
                        AIReal right = artBounds.right;
                        AIReal bottom = artBounds.bottom;
                        result = jjLock.SetRealEntryToDocumentDict(selectedArtHandle, top, "JJLockArtBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock.SetRealEntryToDocumentDict(selectedArtHandle, left, "JJLockArtBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock.SetRealEntryToDocumentDict(selectedArtHandle, right, "JJLockArtBoundRight");
                        aisdk::check_ai_error(result);
                        result = jjLock.SetRealEntryToDocumentDict(selectedArtHandle, bottom, "JJLockArtBoundBottom");
                        aisdk::check_ai_error(result);
                        
                        if(selectedArtHandle != NULL)
                        {
                            sAIArt->GetArtName(artHandle, iconArtName, &nameBool);
                            if(iconArtName != ai::UnicodeString("LockIcon"))
                                jjLock.CreateLockBound(selectedArtHandle, lockLayer, IconHandle, objJJLockPanel->GetIconImagePath("plock.pdf"));
                            
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "JJLockContent");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charFillStyleBool");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charFillStyle");
                            
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charStrokeStyleBool");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charStrokeStyle");
                            
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charFontFamilyName");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charFontStyleName");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charFontSize");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "charHScaleSize");
                            
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "pclock");
                            jjLock.DeleteDictionaryForAnEntry(selectedArtHandle, "clock");
                            sAIArt->SetArtBounds(selectedArtHandle);
                            result = jjLock.SetBooleanEntryToHandleDict(selectedArtHandle, "plock", true);
                            aisdk::check_ai_error(result);
                   
                            artHandle = selectedArtHandle;
                            
                            jjLock.ClearLockEntry(objJJLockPanel->tempArtHandle);
                            objJJLockPanel->tempArtHandle = NULL;
                        //    objJJLockPanel->tempArtHandle = artHandle;
                            lockHandleCount++;
                        }
                    }
                }
                else if((lockHandleCount != lockCount))
                {
                 result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                }
                
            }
            ////not to allow relink image - currently we don't have notifier and we are not allowing to delete the previous image ////

            // Release the link when text art is converted to outlined
            else if(type == kGroupArt || type == kTextFrameArt)
            {
                ai::int32 lockCount = 0;
                int lockHandleCount = 0;
                jjLock.GetIntegerEntryToDocumentDict("lockCount", &lockCount);
                
                AIArtSpec artSpec[7] = {{kTextFrameArt, 0, 0}, {kPlacedArt, 0, 0}, {kGroupArt, 0, 0}, {kPathArt, 0, 0}, {kPluginArt, 0, 0}, {kSymbolArt, 0, 0}, {kCompoundPathArt, 0, 0}};
                ArtSetHelper artSetGroup(artSpec, 7);
                size_t count = 0;
                count = artSetGroup.GetCount();
                short type;
                AIArtHandle artHandle;
                bool isBreakLoop = false;
                while(count > 0)
                {
                    count--;
                    artHandle = artSetGroup[count];
                    result = sAIArt->GetArtType(artHandle, &type);
                    ai::check_ai_error(result);
                    
                    if(sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle) && jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                        lockHandleCount++;
                    
                    if(type == kGroupArt && jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") && (jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") ))
                    {
                        int countGroupChild = 0;
                        int totalGroupChildCount = jjLock.GetIntegerEntryFromHandleDict(artHandle, "totalGroupChildCount");
                    //    int totalGroupChildCount = jjLock.GetIntegerEntryFromHandleDict(artHandle, "totalGroupChildCount");
                        objJJLockPanel->ParseGroupChildForCount(artHandle, &countGroupChild);
                        qDebug() << countGroupChild <<"  " << totalGroupChildCount;
                      //  if(totalGroupChildCount == 0 && )
                        if((countGroupChild != totalGroupChildCount) && totalGroupChildCount !=0)
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreakLoop = true;
                            break;
                        }
                        
                    }
                }
                
                if((lockHandleCount != lockCount) && isBreakLoop == false)
                {
                    result = sAIDocument->Undo();
                    aisdk::check_ai_error(result);
                    return;
                }
            }
            //- Release the link when text art is converted to outlined
        }
        else
        {
            ai::int32 lockCount = 0;
            int lockHandleCount = 0;
            jjLock.GetIntegerEntryToDocumentDict("lockCount", &lockCount);
            
            AIArtSpec artSpec[7] = {{kTextFrameArt, 0, 0}, {kPlacedArt, 0, 0},  {kGroupArt, 0, 0}, {kPathArt, 0, 0}, {kPluginArt, 0, 0}, {kCompoundPathArt, 0, 0}, {kSymbolArt, 0, 0}};
            ArtSetHelper artSetFinal(artSpec, 7);
            size_t count = 0;
            count = artSetFinal.GetCount();
            short artType;
            AIArtHandle artHandle;
            ai::UnicodeString title;
            bool isBreakLoop = false;
            while(count > 0)
            {
                count--;
                artHandle = artSetFinal[count];
                result = sAIArt->GetArtType(artHandle, &artType);
                ai::check_ai_error(result);

                if(sAIArt->HasDictionary(artHandle) && !sAIArt->IsDictionaryEmpty(artHandle))
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                {
                    lockHandleCount++;

                         if((jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock")))
                    {
                        AILayerHandle layerHandle;
                        sAIArt->GetLayerOfArt(artHandle, &layerHandle);
                        sAILayer->GetLayerTitle(layerHandle, title);
//                        if(title != jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "layerName") && title != ai::UnicodeString("Isolation Mode"))
//                        {
//                         result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
//                            lockHandleCount = 0;
//                            lockCount = 0;
//                            break;
//                        }
                        
                        
                        if(artType == kGroupArt && (jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock")) )
                        {
                            jjLock.FetchAllDictionaryUsingIterator(artHandle);
                            int countGroupChild = 0;
                            int totalGroupChildCount = jjLock.GetIntegerEntryFromHandleDict(artHandle, "totalGroupChildCount");
                            objJJLockPanel->ParseGroupChildForCount(artHandle, &countGroupChild);
                            qDebug() << countGroupChild <<"  " << totalGroupChildCount;
                            if((countGroupChild != totalGroupChildCount) && totalGroupChildCount != 0)
                            {
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                isBreakLoop = true;
                                break;
                            }
                            }
                        }
                        
                    }
                }
            if(lockHandleCount != lockCount && !isBreakLoop)
            {  
                result = sAIDocument->Undo();
                aisdk::check_ai_error(result);
                return;
            }

        }
            
        //YuvI
        AIArtSpec textArtSpec[1] = {{kTextFrameArt, 0, 0}};
        ArtSetHelper textArtSet(textArtSpec, 1);
        size_t textArtcount = 0;
        textArtcount = textArtSet.GetCount();
        short type;
        AIArtHandle artHandle;
        while(textArtcount > 0)
        {
            textArtcount--;
            artHandle = textArtSet[textArtcount];
            result = sAIArt->GetArtType(artHandle, &type);
            ai::check_ai_error(result);
        
        if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock") &&  jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") && type == kTextFrameArt)
        {
            std::vector<AIArtHandle> linkArtHanldes;
            
            if(artHandle != NULL)
            {
                AIBool8 linkedText;
                sAITextFrame->PartOfLinkedText(artHandle, &linkedText);
                if(linkedText)
                {
                    artHandle = jjLock.GetParentTextFrameFromLinkedTextFrame(artHandle);
                    linkArtHanldes = jjLock.GetTextFramesFromLinkedTextFrame(artHandle);
                }
                else
                {
                    linkArtHanldes.clear();
                    linkArtHanldes.push_back(artHandle);
                }
                
                for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
                {
                    std::vector<AIFillStyle> textFrameFillColorArray;
                    std::vector<int> textFrameColorFillBool;
                    std::vector<AIStrokeStyle> textFrameStrokeColorArray;
                    std::vector<int> textFrameColorStrokeBool;
                    
                    std::vector<int> textFrameFillVisibleBool;
                    std::vector<AIFillStyle> textFrameObjectFillStyleArray;
                    
                    std::vector<int> textFrameStrokeVisibleBool;
                    std::vector<AIStrokeStyle> textFrameObjectStrokeStyleArray;
                    
                    AIArtHandle textFrameArtHandle = NULL;
                    textFrameArtHandle = linkArtHanldes.at(i);
                    
                    if(sAIArt->ArtHasFill(textFrameArtHandle) || sAIArt->ArtHasStroke(textFrameArtHandle))
                    {
                        jjLock.GetTextFrameFillAndStrokeColour(&textFrameStrokeColorArray, &textFrameColorStrokeBool, &textFrameFillColorArray, &textFrameColorFillBool, textFrameArtHandle);
                        
                        jjLock.GetArrayEntryForInteger(textFrameArtHandle, "textFrameFillStyleBool", &textFrameFillVisibleBool);
                        jjLock.GetArrayEntryForFillStyle(textFrameArtHandle, "textFrameFillStyle", &textFrameObjectFillStyleArray);
                        jjLock.GetArrayEntryForInteger(textFrameArtHandle, "textFrameStrokeStyleBool", &textFrameStrokeVisibleBool);
                        jjLock.GetArrayEntryForStrokeStyle(textFrameArtHandle, "textFrameStrokeStyle", &textFrameObjectStrokeStyleArray);
                        
                        for(int i = 0; i < textFrameFillColorArray.size(); i++)
                        {
                            if(( (textFrameStrokeColorArray.at(i).width != textFrameObjectStrokeStyleArray.at(i).width) || (textFrameColorStrokeBool.at(i) != textFrameStrokeVisibleBool.at(i))) )
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind != textFrameObjectFillStyleArray.at(i).color.kind) || (textFrameStrokeColorArray.at(i).color.kind != textFrameObjectStrokeStyleArray.at(i).color.kind))
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kFourColor && textFrameObjectFillStyleArray.at(i).color.kind == kFourColor &&
                                     ((textFrameFillColorArray.at(i).color.c.f.black != textFrameObjectFillStyleArray.at(i).color.c.f.black) ||
                                      (textFrameFillColorArray.at(i).color.c.f.cyan != textFrameObjectFillStyleArray.at(i).color.c.f.cyan) ||
                                      (textFrameFillColorArray.at(i).color.c.f.magenta != textFrameObjectFillStyleArray.at(i).color.c.f.magenta) ||
                                      (textFrameFillColorArray.at(i).color.c.f.yellow != textFrameObjectFillStyleArray.at(i).color.c.f.yellow)))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kFourColor && textFrameObjectStrokeStyleArray.at(i).color.kind == kFourColor &&
                                     ((textFrameStrokeColorArray.at(i).color.c.f.black != textFrameObjectStrokeStyleArray.at(i).color.c.f.black) ||
                                      (textFrameStrokeColorArray.at(i).color.c.f.cyan != textFrameObjectStrokeStyleArray.at(i).color.c.f.cyan) ||
                                      (textFrameStrokeColorArray.at(i).color.c.f.magenta != textFrameObjectStrokeStyleArray.at(i).color.c.f.magenta) ||
                                      (textFrameStrokeColorArray.at(i).color.c.f.yellow != textFrameObjectStrokeStyleArray.at(i).color.c.f.yellow)))
                                    )
                                
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kGradient && textFrameObjectFillStyleArray.at(i).color.kind == kGradient &&
                                     (textFrameFillColorArray.at(i).color.c.b.gradient != textFrameObjectFillStyleArray.at(i).color.c.b.gradient))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kGradient && textFrameObjectStrokeStyleArray.at(i).color.kind == kGradient &&
                                     (textFrameStrokeColorArray.at(i).color.c.b.gradient != textFrameObjectStrokeStyleArray.at(i).color.c.b.gradient))
                                    )
                            {
                                 jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kGrayColor && textFrameObjectFillStyleArray.at(i).color.kind == kGrayColor &&
                                     (textFrameFillColorArray.at(i).color.c.g.gray != textFrameObjectFillStyleArray.at(i).color.c.g.gray))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kGrayColor && textFrameObjectStrokeStyleArray.at(i).color.kind == kGrayColor &&
                                     (textFrameStrokeColorArray.at(i).color.c.g.gray != textFrameObjectStrokeStyleArray.at(i).color.c.g.gray))
                                    )
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kCustomColor && textFrameObjectFillStyleArray.at(i).color.kind == kCustomColor &&
                                     (textFrameFillColorArray.at(i).color.c.c.color != textFrameObjectFillStyleArray.at(i).color.c.c.color ||
                                      textFrameFillColorArray.at(i).color.c.c.tint != textFrameObjectFillStyleArray.at(i).color.c.c.tint ))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kCustomColor && textFrameObjectStrokeStyleArray.at(i).color.kind == kCustomColor &&
                                     (textFrameStrokeColorArray.at(i).color.c.c.color != textFrameObjectStrokeStyleArray.at(i).color.c.c.color ||
                                      textFrameStrokeColorArray.at(i).color.c.c.tint != textFrameObjectStrokeStyleArray.at(i).color.c.c.tint ))
                                    )
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kThreeColor && textFrameObjectFillStyleArray.at(i).color.kind == kThreeColor &&
                                     (textFrameFillColorArray.at(i).color.c.rgb.blue != textFrameObjectFillStyleArray.at(i).color.c.rgb.blue ||
                                      textFrameFillColorArray.at(i).color.c.rgb.green != textFrameObjectFillStyleArray.at(i).color.c.rgb.green ||
                                      textFrameFillColorArray.at(i).color.c.rgb.red != textFrameObjectFillStyleArray.at(i).color.c.rgb.red ))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kThreeColor && textFrameObjectStrokeStyleArray.at(i).color.kind == kThreeColor &&
                                     (textFrameStrokeColorArray.at(i).color.c.rgb.blue != textFrameObjectStrokeStyleArray.at(i).color.c.rgb.blue ||
                                      textFrameStrokeColorArray.at(i).color.c.rgb.green != textFrameObjectStrokeStyleArray.at(i).color.c.rgb.green ||
                                      textFrameStrokeColorArray.at(i).color.c.rgb.red != textFrameObjectStrokeStyleArray.at(i).color.c.rgb.red ))
                                    )
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                            else if((textFrameFillColorArray.at(i).color.kind == kPattern && textFrameObjectFillStyleArray.at(i).color.kind == kPattern &&
                                     (textFrameFillColorArray.at(i).color.c.p.pattern != textFrameObjectFillStyleArray.at(i).color.c.p.pattern))
                                    ||
                                    (textFrameStrokeColorArray.at(i).color.kind == kPattern && textFrameObjectStrokeStyleArray.at(i).color.kind == kPattern &&
                                     (textFrameStrokeColorArray.at(i).color.c.p.pattern != textFrameObjectStrokeStyleArray.at(i).color.c.p.pattern))
                                    )
                            {
                                jjLock.ApplyFillColorForTextFrame(textFrameArtHandle, textFrameObjectFillStyleArray.at(i), textFrameObjectStrokeStyleArray.at(i), textFrameFillVisibleBool.at(i), textFrameStrokeVisibleBool.at(i));
                                break;
                            }
                        }
                    }
                    
                }
                linkArtHanldes.clear();
            }
        }
    }

    }
    
    }
}


void JJLockPlugin::FindChildParentGroupLockHandle(AIArtHandle childHandle)
{
    JJLock jjLock;
    AIArtHandle parentArthandle;
    ASBoolean isArtLayerGroup;
    sAIArt->GetArtParent(childHandle, &parentArthandle);
    sAIArt->IsArtLayerGroup(parentArthandle, &isArtLayerGroup);
    while(parentArthandle != NULL && !isArtLayerGroup)
    {
        if(jjLock.GetBooleanEntryFromHandleDict(parentArthandle, "lock"))
        {
            childParentHandle = parentArthandle;
            break;
        }
        sAIArt->GetArtParent(parentArthandle, &parentArthandle);
        FindChildParentGroupLockHandle(parentArthandle);
    }
}

bool JJLockPlugin::ParseGroupChildForLockConditionCheck(AIArtHandle groupArtHandle, string lockType)
{
    JJLock* jjLock;
    AIErr result = kNoErr;
    AIArtHandle lastChildHandle = groupArtHandle;
    short artType;
    sAIArt->GetArtType(lastChildHandle, &artType);
    if(artType == kGroupArt)
    {
        sAIArt->GetArtLastChild(groupArtHandle, &lastChildHandle);
    }
    else if(lockType == "plock")
    {
        ASErr result = kNoErr;
        childParentHandle = NULL;
        FindChildParentGroupLockHandle(lastChildHandle);
        if(childParentHandle != NULL)
        {
            AIRealRect artBounds = {0,0,0,0};
            AIRealRect rectDict = {0,0,0,0};
            sAIArt->GetArtTransformBounds(childParentHandle, NULL, kNoStrokeBounds, &artBounds);
            result = jjLock->GetRealEntryFromDocumentDict(childParentHandle, &rectDict.top, "JJLockArtBoundTop");
            aisdk::check_ai_error(result);
            result = jjLock->GetRealEntryFromDocumentDict(childParentHandle, &rectDict.bottom, "JJLockArtBoundBottom");
            aisdk::check_ai_error(result);
            result = jjLock->GetRealEntryFromDocumentDict(childParentHandle, &rectDict.left, "JJLockArtBoundLeft");
            aisdk::check_ai_error(result);
            result = jjLock->GetRealEntryFromDocumentDict(childParentHandle, &rectDict.right, "JJLockArtBoundRight");
            aisdk::check_ai_error(result);
        
            const AIRealRect constArtBounds = artBounds;
            const AIRealRect constRectBounds = rectDict;
            
            if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
            {
                result = sAIDocument->Undo();
                aisdk::check_ai_error(result);
                return true;
            }
        }
    }
    bool isBreak = false;
    while(lastChildHandle != NULL)
    {
        short childArtType;
        sAIArt->GetArtType(lastChildHandle, &childArtType);
        if(childArtType == kGroupArt)
        {
            ParseGroupChildForLockConditionCheck(lastChildHandle, lockType);
        }
        else if(childArtType == kCompoundPathArt)
        {
            if(lockType == "pclock" && ParseCompoundPathArtGroupCheck(groupArtHandle, lastChildHandle, "pclock"))
            {
                isBreak = true;
                break;
            }
            else if(lockType == "clock" && ParseCompoundPathArtGroupCheck(groupArtHandle, lastChildHandle, "clock"))
            {
                isBreak = true;
                break;
            }
        }
        else if(lastChildHandle != NULL &&  childArtType == kTextFrameArt)
        {
            ASErr result = kNoErr;
            AIBool8 linkedText;
            std::vector<AIArtHandle> linkArtHanldes;
            sAITextFrame->PartOfLinkedText(lastChildHandle, &linkedText);
            if(linkedText)
            {
                lastChildHandle = jjLock->GetParentTextFrameFromLinkedTextFrame(lastChildHandle);
                linkArtHanldes = jjLock->GetTextFramesFromLinkedTextFrame(lastChildHandle);
            }
            else
            {
                linkArtHanldes.clear();
                linkArtHanldes.push_back(lastChildHandle);
            }
            
            for(int i=linkArtHanldes.size() - 1; i >= 0; i--)
            {
                std::vector<AIFillStyle> charFeatureColorArray;
                std::vector<int> charColorFillBool;
                std::vector<AIStrokeStyle> charFeatureStrokeArray;
                std::vector<int> charStrokeVisibleBool;
                
                AIArtHandle textFrameArtHandle = NULL;
                textFrameArtHandle = linkArtHanldes.at(i);
                
                if(lockType == "clock" || lockType == "pclock")
                {
                    ai::UnicodeString wholeTextFrameContent = jjLock->GetText(textFrameArtHandle);
                    ai::UnicodeString dicContent;
                    result = jjLock->GetUnicodeEntryFromDocumentDict(textFrameArtHandle, &dicContent, "JJLockContent");
                    aisdk::check_ai_error(result);
                    
                    if(dicContent.length() > 0 && childArtType == kTextFrameArt && (lockType == "pclock" || lockType == "clock"))
                    {
                        if(dicContent != wholeTextFrameContent)
                        {
                            try {
                             result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                isBreak = true;
                                break;
                            }
                            catch (ai::Error &ex){
                                result = ex;
                            }
                            catch (ATE::Exception &ex){
                                result = ex.error;
                            }
                        }
                        
                        if(!IsDictAndCurrentOwnRectBoundSame(textFrameArtHandle))
                        {
                         result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        
                        if(lockType == "clock" && childArtType == kTextFrameArt)
                        {
                            std::vector<ai::UnicodeString> charfontStyleNameArray;
                            std::vector<ai::UnicodeString> charfontFamilyNameArray;
                            std::vector<ai::UnicodeString> charfontStyleName;
                            std::vector<ai::UnicodeString> charfontFamilyName;
                            std::vector<double> charFontSizeArray;
                            std::vector<int> charStrokeBool;
                            std::vector<AIStrokeStyle> charStrokeStyle;
                            
                            jjLock->GetCharacterFontProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, textFrameArtHandle);
                            jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                            jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                            
                            jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeVisibleBool, textFrameArtHandle);
                            jjLock->GetArrayEntryForStrokeStyle(textFrameArtHandle, "charStrokeStyle", &charStrokeStyle);
                            jjLock->GetArrayEntryForInteger(textFrameArtHandle, "charStrokeStyleBool", &charStrokeBool);

                            for(int i = 0; i < charFeatureStrokeArray.size(); i++)
                            {
                                if(( (charFeatureStrokeArray.at(i).width != charStrokeStyle.at(i).width) || (charStrokeVisibleBool.at(i) != charStrokeBool.at(i))) )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind != charStrokeStyle.at(i).color.kind)
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kFourColor && charStrokeStyle.at(i).color.kind == kFourColor &&
                                        ((charFeatureStrokeArray.at(i).color.c.f.black != charStrokeStyle.at(i).color.c.f.black) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.cyan != charStrokeStyle.at(i).color.c.f.cyan) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.magenta != charStrokeStyle.at(i).color.c.f.magenta) ||
                                         (charFeatureStrokeArray.at(i).color.c.f.yellow != charStrokeStyle.at(i).color.c.f.yellow))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kGradient && charStrokeStyle.at(i).color.kind == kGradient &&
                                        (charFeatureStrokeArray.at(i).color.c.b.gradient != charStrokeStyle.at(i).color.c.b.gradient))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kGrayColor && charStrokeStyle.at(i).color.kind == kGrayColor &&
                                        (charFeatureStrokeArray.at(i).color.c.g.gray != charStrokeStyle.at(i).color.c.g.gray))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kCustomColor && charStrokeStyle.at(i).color.kind == kCustomColor &&
                                        (charFeatureStrokeArray.at(i).color.c.c.color != charStrokeStyle.at(i).color.c.c.color ||
                                         charFeatureStrokeArray.at(i).color.c.c.tint != charStrokeStyle.at(i).color.c.c.tint ))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kThreeColor && charStrokeStyle.at(i).color.kind == kThreeColor &&
                                        (charFeatureStrokeArray.at(i).color.c.rgb.blue != charStrokeStyle.at(i).color.c.rgb.blue ||
                                         charFeatureStrokeArray.at(i).color.c.rgb.green != charStrokeStyle.at(i).color.c.rgb.green ||
                                         charFeatureStrokeArray.at(i).color.c.rgb.red != charStrokeStyle.at(i).color.c.rgb.red ))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if(charFeatureStrokeArray.at(i).color.kind == kPattern && charStrokeStyle.at(i).color.kind == kPattern &&
                                        (charFeatureStrokeArray.at(i).color.c.p.pattern != charStrokeStyle.at(i).color.c.p.pattern))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)))
                                {
                                    jjLock->ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i));
                                }
                            }
                        }
                        if(lockType == "pclock" && childArtType == kTextFrameArt)
                        {
                            std::vector<int> charFillBool;
                            std::vector<AIFillStyle> charFillStyle;
                            
                            std::vector<int> charStrokeBool;
                            std::vector<AIStrokeStyle> charStrokeStyle;
                            
                            std::vector<ai::UnicodeString> charfontStyleNameArray;
                            std::vector<ai::UnicodeString> charfontFamilyNameArray;
                            std::vector<double> charFontSizeArray, charHScaleSizeArray;
                            std::vector<double> charFontSize, charHScaleSize;
                            std::vector<ai::UnicodeString> charfontStyleName;
                            std::vector<ai::UnicodeString> charfontFamilyName;
                            
                            jjLock->GetCharacterColor(&charFeatureColorArray, &charColorFillBool, textFrameArtHandle);
                            jjLock->GetArrayEntryForFillStyle(textFrameArtHandle, "charFillStyle", &charFillStyle);
                            jjLock->GetArrayEntryForInteger(textFrameArtHandle, "charFillStyleBool", &charFillBool);
                            
                            jjLock->GetCharacterStroke(&charFeatureStrokeArray, &charStrokeVisibleBool, textFrameArtHandle);
                            jjLock->GetArrayEntryForStrokeStyle(textFrameArtHandle, "charStrokeStyle", &charStrokeStyle);
                            jjLock->GetArrayEntryForInteger(textFrameArtHandle, "charStrokeStyleBool", &charStrokeBool);
                            
                            jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                            
                            jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                            jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                            jjLock->GetArrayEntryForReal(textFrameArtHandle, "charFontSize", &charFontSize);
                            jjLock->GetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", &charHScaleSize);
                            
                            for(int i = 0; i < charFeatureColorArray.size(); i++)
                            {
                                if(( (charFeatureStrokeArray.at(i).width != charStrokeStyle.at(i).width) || (charStrokeVisibleBool.at(i) != charStrokeBool.at(i))) )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind != charFillStyle.at(i).color.kind) || (charFeatureStrokeArray.at(i).color.kind != charStrokeStyle.at(i).color.kind))
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kFourColor && charFillStyle.at(i).color.kind == kFourColor &&
                                         ((charFeatureColorArray.at(i).color.c.f.black != charFillStyle.at(i).color.c.f.black) ||
                                          (charFeatureColorArray.at(i).color.c.f.cyan != charFillStyle.at(i).color.c.f.cyan) ||
                                          (charFeatureColorArray.at(i).color.c.f.magenta != charFillStyle.at(i).color.c.f.magenta) ||
                                          (charFeatureColorArray.at(i).color.c.f.yellow != charFillStyle.at(i).color.c.f.yellow)))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kFourColor && charStrokeStyle.at(i).color.kind == kFourColor &&
                                         ((charFeatureStrokeArray.at(i).color.c.f.black != charStrokeStyle.at(i).color.c.f.black) ||
                                          (charFeatureStrokeArray.at(i).color.c.f.cyan != charStrokeStyle.at(i).color.c.f.cyan) ||
                                          (charFeatureStrokeArray.at(i).color.c.f.magenta != charStrokeStyle.at(i).color.c.f.magenta) ||
                                          (charFeatureStrokeArray.at(i).color.c.f.yellow != charStrokeStyle.at(i).color.c.f.yellow)))
                                        )
                                    
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kGradient && charFillStyle.at(i).color.kind == kGradient &&
                                         (charFeatureColorArray.at(i).color.c.b.gradient != charFillStyle.at(i).color.c.b.gradient))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kGradient && charStrokeStyle.at(i).color.kind == kGradient &&
                                         (charFeatureStrokeArray.at(i).color.c.b.gradient != charStrokeStyle.at(i).color.c.b.gradient))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kGrayColor && charFillStyle.at(i).color.kind == kGrayColor &&
                                         (charFeatureColorArray.at(i).color.c.g.gray != charFillStyle.at(i).color.c.g.gray))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kGrayColor && charStrokeStyle.at(i).color.kind == kGrayColor &&
                                         (charFeatureStrokeArray.at(i).color.c.g.gray != charStrokeStyle.at(i).color.c.g.gray))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kCustomColor && charFillStyle.at(i).color.kind == kCustomColor &&
                                         (charFeatureColorArray.at(i).color.c.c.color != charFillStyle.at(i).color.c.c.color ||
                                          charFeatureColorArray.at(i).color.c.c.tint != charFillStyle.at(i).color.c.c.tint ))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kCustomColor && charStrokeStyle.at(i).color.kind == kCustomColor &&
                                         (charFeatureStrokeArray.at(i).color.c.c.color != charStrokeStyle.at(i).color.c.c.color ||
                                          charFeatureStrokeArray.at(i).color.c.c.tint != charStrokeStyle.at(i).color.c.c.tint ))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kThreeColor && charFillStyle.at(i).color.kind == kThreeColor &&
                                         (charFeatureColorArray.at(i).color.c.rgb.blue != charFillStyle.at(i).color.c.rgb.blue ||
                                          charFeatureColorArray.at(i).color.c.rgb.green != charFillStyle.at(i).color.c.rgb.green ||
                                          charFeatureColorArray.at(i).color.c.rgb.red != charFillStyle.at(i).color.c.rgb.red ))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kThreeColor && charStrokeStyle.at(i).color.kind == kThreeColor &&
                                         (charFeatureStrokeArray.at(i).color.c.rgb.blue != charStrokeStyle.at(i).color.c.rgb.blue ||
                                          charFeatureStrokeArray.at(i).color.c.rgb.green != charStrokeStyle.at(i).color.c.rgb.green ||
                                          charFeatureStrokeArray.at(i).color.c.rgb.red != charStrokeStyle.at(i).color.c.rgb.red ))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charFeatureColorArray.at(i).color.kind == kPattern && charFillStyle.at(i).color.kind == kPattern &&
                                         (charFeatureColorArray.at(i).color.c.p.pattern != charFillStyle.at(i).color.c.p.pattern))
                                        ||
                                        (charFeatureStrokeArray.at(i).color.kind == kPattern && charStrokeStyle.at(i).color.kind == kPattern &&
                                         (charFeatureStrokeArray.at(i).color.c.p.pattern != charStrokeStyle.at(i).color.c.p.pattern))
                                        )
                                {
                                    result = sAIDocument->Undo();
                                    aisdk::check_ai_error(result);
                                    isBreak = true;
                                    break;
                                }
                                else if((charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)) ||
                                        (charFontSizeArray.at(i) != charFontSize.at(i)) )
                                {
                                    jjLock->ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i), charFontSize.at(i), charHScaleSize.at(i));
                                }
                            }
                        }
                    }
                }

                AIRealRect artBounds = {0,0,0,0};
                sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                
                AIRealRect rectDict = {0,0,0,0};
                
                result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.top, "JJLockArtBoundTop");
                aisdk::check_ai_error(result);
                result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.bottom, "JJLockArtBoundBottom");
                aisdk::check_ai_error(result);
                result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.left, "JJLockArtBoundLeft");
                aisdk::check_ai_error(result);
                result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &rectDict.right, "JJLockArtBoundRight");
                aisdk::check_ai_error(result);
                
                if(childArtType == kTextFrameArt  && lockType != "clock")
                {
                    ai::int32 getParaJustify;
                    int justifyValue = jjLock->GetIntegerEntryFromHandleDict(textFrameArtHandle, "ParaJustification");
                    jjLock->GetParagraphJustification(textFrameArtHandle, &getParaJustify);
                    if(getParaJustify != justifyValue)
                    {
                        try
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        catch (ai::Error &ex){
                            result = ex;
                        }
                        catch (ATE::Exception &ex){
                            result = ex.error;
                        }
                    }
                }
                
                if(!(rectDict.top == 0 && rectDict.bottom == 0 && rectDict.left == 0 && rectDict.right == 0))
                {
                    const AIRealRect constArtBounds = artBounds;
                    const AIRealRect constRectBounds = rectDict;
                    AITextFrameType textFrameType;
                    sAITextFrame->GetType(textFrameArtHandle, &textFrameType);
                    
                    if((lockType == "pclock" || lockType == "plock") && childArtType == kTextFrameArt)
                    {
                        std::vector<ai::UnicodeString> charfontStyleNameArray;
                        std::vector<ai::UnicodeString> charfontFamilyNameArray;
                        std::vector<double> charFontSizeArray, charHScaleSizeArray;
                        std::vector<double> charFontSize, charHScaleSize;
                        std::vector<ai::UnicodeString> charfontStyleName;
                        std::vector<ai::UnicodeString> charfontFamilyName;
                        int arraySize = 0;
                        bool isFontChanged = false;
                        
                        jjLock->GetCharacterProperty(&charfontStyleNameArray, &charfontFamilyNameArray, &charFontSizeArray, &charHScaleSizeArray, textFrameArtHandle);
                        jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontStyleName", &charfontStyleName);
                        jjLock->GetArrayEntryForUnicodeString(textFrameArtHandle, "charFontFamilyName", &charfontFamilyName);
                        jjLock->GetArrayEntryForReal(textFrameArtHandle, "charFontSize", &charFontSize);
                        jjLock->GetArrayEntryForReal(textFrameArtHandle, "charHScaleSize", &charHScaleSize);
                        
                        if (charfontStyleNameArray.size() >= charfontStyleName.size())
                        {
                            arraySize = charfontStyleName.size();
                        }
                        else
                        {
                            arraySize = charfontStyleNameArray.size();
                        }
                        for(int i = 0; i < arraySize; i++)
                        {
                            if( (charfontStyleNameArray.at(i) != charfontStyleName.at(i)) || (charfontFamilyNameArray.at(i) != charfontFamilyName.at(i)) || (charFontSizeArray.at(i) != charFontSize.at(i)) || (charHScaleSizeArray.at(i) !=  charHScaleSize.at(i)) )
                            {
                                jjLock->ReplaceFont(textFrameArtHandle, i, charfontFamilyName.at(i), charfontStyleName.at(i), charFontSize.at(i), charHScaleSize.at(i));
                                isFontChanged = true;
                            }
                        }
                        if(isFontChanged == true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                        {
                            AIRealRect artBounds = {0,0,0,0};
                            sAIArt->GetArtTransformBounds(textFrameArtHandle, NULL, kNoStrokeBounds, &artBounds);
                            AIReal top = artBounds.top;
                            AIReal left = artBounds.left;
                            AIReal right = artBounds.right;
                            AIReal bottom = artBounds.bottom;
                            result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, top, "JJLockArtBoundTop");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, left, "JJLockArtBoundLeft");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, right, "JJLockArtBoundRight");
                            aisdk::check_ai_error(result);
                            result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, bottom, "JJLockArtBoundBottom");
                            aisdk::check_ai_error(result);
                        }
                        if(isFontChanged != true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            if(lockType == "pclock")
                            {
                                int totalChildCount = 0;
                                objJJLockPanel->ParseGroupChildForLock(textFrameArtHandle, &totalChildCount, "pclock");
                            }
                            isBreak = true;
                            break;
                        }
                        else if(isFontChanged == true && (!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds)))
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            if(lockType == "pclock")
                            {
                                int totalChildCount = 0;
                                objJJLockPanel->ParseGroupChildForLock(textFrameArtHandle, &totalChildCount, "pclock");
                            }
                            isBreak = true;
                            break;
                        }
                    }
                    else if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                    {
                        try
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        catch (ai::Error &ex){
                            result = ex;
                        }
                        catch (ATE::Exception &ex){
                            result = ex.error;
                        }
                    }
                }
                else if(lockType == "clock" && childArtType == kTextFrameArt)
                {
                    
                    AIRealRect ownRectDict;
                    result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
                    aisdk::check_ai_error(result);
                    result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
                    aisdk::check_ai_error(result);
                    result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
                    aisdk::check_ai_error(result);
                    result = jjLock->GetRealEntryFromDocumentDict(textFrameArtHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
                    aisdk::check_ai_error(result);
                    
                    const AIRealRect constArtBounds = artBounds;
                    const AIRealRect constRectBounds = ownRectDict;
                    
                    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
                    {
                   
                        result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.top, "JJLockArtOwnBoundTop");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.left, "JJLockArtOwnBoundLeft");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.right, "JJLockArtOwnBoundRight");
                        aisdk::check_ai_error(result);
                        result = jjLock->SetRealEntryToDocumentDict(textFrameArtHandle, artBounds.bottom, "JJLockArtOwnBoundBottom");
                        
                    }
                }
                
            }
            linkArtHanldes.clear();
        }
        else if(childArtType == kPathArt && lockType != "plock")
        {
            ASErr result = kNoErr;
            AIArtHandle pathParentHandle;
            AIArtHandle pathArtHandle = lastChildHandle;
            
            ai::int16 pathSegmentCount=0;
            sAIPath->GetPathSegmentCount(pathArtHandle, &pathSegmentCount);
            ai::int16 dicContent;
            dicContent = jjLock->GetIntegerEntryFromHandleDict(pathArtHandle, "JJLockContent");
            aisdk::check_ai_error(result);
            
            sAIArt->GetArtParent(lastChildHandle, &pathParentHandle);
            short parentType;
            sAIArt->GetArtType(pathParentHandle, &parentType);
         //   qDebug() <<  parentType << "  " << dicContent;
            if(dicContent != 0)
            {
                if(dicContent != pathSegmentCount)
                {
                    try {
                     result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    catch (ai::Error &ex){
                        result = ex;
                    }
                    catch (ATE::Exception &ex){
                        result = ex.error;
                    }
                }
                
                if(lockType == "pclock")
                {
                    AIPathStyle pathStyle;
                    int i = 0;
                    std::vector<AIFillStyle> compPathFillArray;
                    std::vector<AIStrokeStyle> compPathStrokeArray;
                    
                    result= sAIPathStyle->GetPathStyle(pathArtHandle, &pathStyle);
                    aisdk::check_ai_error(result);
                    
                    jjLock->GetArrayEntryForFillStyle(pathArtHandle, "pathFillStyle", &compPathFillArray);
                    jjLock->GetArrayEntryForStrokeStyle(pathArtHandle, "pathStrokeStyle", &compPathStrokeArray);
                    
                    
                    if((compPathFillArray.at(i).color.kind != pathStyle.fill.color.kind) || (compPathStrokeArray.at(i).color.kind != pathStyle.stroke.color.kind))
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kFourColor && pathStyle.fill.color.kind == kFourColor &&
                             ((compPathFillArray.at(i).color.c.f.black != pathStyle.fill.color.c.f.black) ||
                              (compPathFillArray.at(i).color.c.f.cyan != pathStyle.fill.color.c.f.cyan) ||
                              (compPathFillArray.at(i).color.c.f.magenta != pathStyle.fill.color.c.f.magenta) ||
                              (compPathFillArray.at(i).color.c.f.yellow != pathStyle.fill.color.c.f.yellow)))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kFourColor && pathStyle.stroke.color.kind == kFourColor &&
                             ((compPathStrokeArray.at(i).color.c.f.black != pathStyle.stroke.color.c.f.black) ||
                              (compPathStrokeArray.at(i).color.c.f.cyan != pathStyle.stroke.color.c.f.cyan) ||
                              (compPathStrokeArray.at(i).color.c.f.magenta != pathStyle.stroke.color.c.f.magenta) ||
                              (compPathStrokeArray.at(i).color.c.f.yellow != pathStyle.stroke.color.c.f.yellow)))
                            )
                        
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kGradient && pathStyle.fill.color.kind == kGradient &&
                             (compPathFillArray.at(i).color.c.b.gradient != pathStyle.fill.color.c.b.gradient))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kGradient && pathStyle.stroke.color.kind == kGradient &&
                             (compPathStrokeArray.at(i).color.c.b.gradient != pathStyle.stroke.color.c.b.gradient))
                            )
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kGrayColor && pathStyle.fill.color.kind == kGrayColor &&
                             (compPathFillArray.at(i).color.c.g.gray != pathStyle.fill.color.c.g.gray))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kGrayColor && pathStyle.stroke.color.kind == kGrayColor &&
                             (compPathStrokeArray.at(i).color.c.g.gray != pathStyle.stroke.color.c.g.gray))
                            )
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kCustomColor && pathStyle.fill.color.kind == kCustomColor &&
                             (compPathFillArray.at(i).color.c.c.color != pathStyle.fill.color.c.c.color ||
                              compPathFillArray.at(i).color.c.c.tint != pathStyle.fill.color.c.c.tint ))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kCustomColor && pathStyle.stroke.color.kind == kCustomColor &&
                             (compPathStrokeArray.at(i).color.c.c.color != pathStyle.stroke.color.c.c.color ||
                              compPathStrokeArray.at(i).color.c.c.tint != pathStyle.stroke.color.c.c.tint ))
                            )
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kThreeColor && pathStyle.fill.color.kind == kThreeColor &&
                             (compPathFillArray.at(i).color.c.rgb.blue != pathStyle.fill.color.c.rgb.blue ||
                              compPathFillArray.at(i).color.c.rgb.green != pathStyle.fill.color.c.rgb.green ||
                              compPathFillArray.at(i).color.c.rgb.red != pathStyle.fill.color.c.rgb.red ))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kThreeColor && pathStyle.stroke.color.kind == kThreeColor &&
                             (compPathStrokeArray.at(i).color.c.rgb.blue != pathStyle.stroke.color.c.rgb.blue ||
                              compPathStrokeArray.at(i).color.c.rgb.green != pathStyle.stroke.color.c.rgb.green ||
                              compPathStrokeArray.at(i).color.c.rgb.red != pathStyle.stroke.color.c.rgb.red ))
                            )
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    else if((compPathFillArray.at(i).color.kind == kPattern && pathStyle.fill.color.kind == kPattern &&
                             (compPathFillArray.at(i).color.c.p.pattern != pathStyle.fill.color.c.p.pattern))
                            ||
                            (compPathStrokeArray.at(i).color.kind == kPattern && pathStyle.stroke.color.kind == kPattern &&
                             (compPathStrokeArray.at(i).color.c.p.pattern != pathStyle.stroke.color.c.p.pattern))
                            )
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        isBreak = true;
                        break;
                    }
                    
                }
                
                
                AIReal pathArea=0, pathLength=0, pathFlatness=0;
                AIReal dictPathArea=0, dictPathLength=0;
                ai::int16 segNumber = 0;
                std::vector<AIRealPoint> realPointArray;
                AIPathSegment pathSegments[pathSegmentCount];
                
                sAIPath->GetPathSegments(pathArtHandle, segNumber, pathSegmentCount, pathSegments);
                sAIPath->GetPathArea(pathArtHandle, &pathArea);
                sAIPath->GetPathLength(pathArtHandle, &pathLength, pathFlatness);
                jjLock->GetRealEntryFromDocumentDict(pathArtHandle, &dictPathArea, "pathArea");
                jjLock->GetRealEntryFromDocumentDict(pathArtHandle, &dictPathLength, "pathLength");
                jjLock->GetArrayEntryForRealPoint(pathArtHandle, "pathSegments", &realPointArray);
                
                if(((float)((int)(dictPathArea * 10000))/10000) != ((float)((int)(pathArea * 10000))/10000)  || ((float)((int)(dictPathLength * 10000))/10000) != ((float)((int)(pathLength * 10000))/10000) )
                {
                    result = sAIDocument->Undo();
                    aisdk::check_ai_error(result);
                    isBreak = true;
                    break;
                }
                
                
                for(int i = 0; i < pathSegmentCount; i++)
                {
//                    qDebug() << realPointArray.at(i).h << "  " << pathSegments[i].p.h;
//                    qDebug() << realPointArray.at(i).v << "  " << pathSegments[i].p.v;
                    if((((float)((int)(realPointArray.at(i).h * 10000))/10000 != (float)((int)(pathSegments[i].p.h * 10000))/10000 ) || ((float)((int)(realPointArray.at(i).v * 10000))/10000 != (float)((int)(pathSegments[i].p.v * 10000))/10000 )))
                    {
                        try
                        {
//                            qDebug() << realPointArray.at(i).h << "  " << pathSegments[i].p.h;
//                            qDebug() << realPointArray.at(i).v << "  " << pathSegments[i].p.v;
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        catch (ai::Error &ex){
                            result = ex;
                        }
                        catch (ATE::Exception &ex){
                            result = ex.error;
                        }
                    }
                }
            }
        }
        sAIArt->GetArtPriorSibling(lastChildHandle, &lastChildHandle);
    }
    return isBreak;
}


bool JJLockPlugin::ParseCompoundPathArtGroupCheck(AIArtHandle topParentArtHandle, AIArtHandle compoundPathArtHandle, std::string lockType)
{
    
    AppContext appContext(gPlugin->GetPluginRef());
    
    AIErr result = kNoErr;
    JJLock jjLock;
    short type, childArtType;
    ASBoolean childBool = true;
    ai::UnicodeString childArtName;
    
    
    AIArtHandle pathArtHandle, groupArtHandle, nextChildArt;
    sAIArt->GetArtType(compoundPathArtHandle, &type);
    sAIArt->GetArtFirstChild(compoundPathArtHandle, &nextChildArt);
    groupArtHandle = compoundPathArtHandle;
    bool isBreak = 0;
    while(groupArtHandle != NULL && (type == kGroupArt || type == kCompoundPathArt))
    {
        sAIArt->GetArtName(groupArtHandle, childArtName, &childBool);
        while(nextChildArt != NULL)
        {
            sAIArt->GetArtType(nextChildArt, &childArtType);
            if(childArtType == kGroupArt)
            {
                ParseCompoundPathArtGroupCheck(topParentArtHandle, nextChildArt, lockType);
            }
            else if(childArtType == kPathArt)
            {
                
                pathArtHandle = nextChildArt;
                ai::int16 pathSegmentCount = 0;
                sAIPath->GetPathSegmentCount(pathArtHandle, &pathSegmentCount);
                ai::int16 dicContent;
                dicContent = jjLock.GetIntegerEntryFromHandleDict(pathArtHandle, "JJLockContent");
                aisdk::check_ai_error(result);
                
                if(dicContent != 0)
                {
                    if(dicContent != pathSegmentCount)
                    {
                        try {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        catch (ai::Error &ex){
                            result = ex;
                        }
                        catch (ATE::Exception &ex){
                            result = ex.error;
                        }
                    }
                    
                    if(lockType == "pclock")
                    {
                        AIPathStyle pathStyle;
                        int i = 0;
                        std::vector<AIFillStyle> compPathFillArray;
                        std::vector<AIStrokeStyle> compPathStrokeArray;
                        
                        result= sAIPathStyle->GetPathStyle(nextChildArt, &pathStyle);
                        aisdk::check_ai_error(result);
                        
                        jjLock.GetArrayEntryForFillStyle(nextChildArt, "compPathFillStyle", &compPathFillArray);
                        jjLock.GetArrayEntryForStrokeStyle(nextChildArt, "compPathStrokeStyle", &compPathStrokeArray);
                        
                        
                        if((compPathFillArray.at(i).color.kind != pathStyle.fill.color.kind) || (compPathStrokeArray.at(i).color.kind != pathStyle.stroke.color.kind))
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kFourColor && pathStyle.fill.color.kind == kFourColor &&
                                 ((compPathFillArray.at(i).color.c.f.black != pathStyle.fill.color.c.f.black) ||
                                  (compPathFillArray.at(i).color.c.f.cyan != pathStyle.fill.color.c.f.cyan) ||
                                  (compPathFillArray.at(i).color.c.f.magenta != pathStyle.fill.color.c.f.magenta) ||
                                  (compPathFillArray.at(i).color.c.f.yellow != pathStyle.fill.color.c.f.yellow)))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kFourColor && pathStyle.stroke.color.kind == kFourColor &&
                                 ((compPathStrokeArray.at(i).color.c.f.black != pathStyle.stroke.color.c.f.black) ||
                                  (compPathStrokeArray.at(i).color.c.f.cyan != pathStyle.stroke.color.c.f.cyan) ||
                                  (compPathStrokeArray.at(i).color.c.f.magenta != pathStyle.stroke.color.c.f.magenta) ||
                                  (compPathStrokeArray.at(i).color.c.f.yellow != pathStyle.stroke.color.c.f.yellow)))
                                )
                            
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kGradient && pathStyle.fill.color.kind == kGradient &&
                                 (compPathFillArray.at(i).color.c.b.gradient != pathStyle.fill.color.c.b.gradient))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kGradient && pathStyle.stroke.color.kind == kGradient &&
                                 (compPathStrokeArray.at(i).color.c.b.gradient != pathStyle.stroke.color.c.b.gradient))
                                )
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kGrayColor && pathStyle.fill.color.kind == kGrayColor &&
                                 (compPathFillArray.at(i).color.c.g.gray != pathStyle.fill.color.c.g.gray))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kGrayColor && pathStyle.stroke.color.kind == kGrayColor &&
                                 (compPathStrokeArray.at(i).color.c.g.gray != pathStyle.stroke.color.c.g.gray))
                                )
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kCustomColor && pathStyle.fill.color.kind == kCustomColor &&
                                 (compPathFillArray.at(i).color.c.c.color != pathStyle.fill.color.c.c.color ||
                                  compPathFillArray.at(i).color.c.c.tint != pathStyle.fill.color.c.c.tint ))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kCustomColor && pathStyle.stroke.color.kind == kCustomColor &&
                                 (compPathStrokeArray.at(i).color.c.c.color != pathStyle.stroke.color.c.c.color ||
                                  compPathStrokeArray.at(i).color.c.c.tint != pathStyle.stroke.color.c.c.tint ))
                                )
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kThreeColor && pathStyle.fill.color.kind == kThreeColor &&
                                 (compPathFillArray.at(i).color.c.rgb.blue != pathStyle.fill.color.c.rgb.blue ||
                                  compPathFillArray.at(i).color.c.rgb.green != pathStyle.fill.color.c.rgb.green ||
                                  compPathFillArray.at(i).color.c.rgb.red != pathStyle.fill.color.c.rgb.red ))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kThreeColor && pathStyle.stroke.color.kind == kThreeColor &&
                                 (compPathStrokeArray.at(i).color.c.rgb.blue != pathStyle.stroke.color.c.rgb.blue ||
                                  compPathStrokeArray.at(i).color.c.rgb.green != pathStyle.stroke.color.c.rgb.green ||
                                  compPathStrokeArray.at(i).color.c.rgb.red != pathStyle.stroke.color.c.rgb.red ))
                                )
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        else if((compPathFillArray.at(i).color.kind == kPattern && pathStyle.fill.color.kind == kPattern &&
                                 (compPathFillArray.at(i).color.c.p.pattern != pathStyle.fill.color.c.p.pattern))
                                ||
                                (compPathStrokeArray.at(i).color.kind == kPattern && pathStyle.stroke.color.kind == kPattern &&
                                 (compPathStrokeArray.at(i).color.c.p.pattern != pathStyle.stroke.color.c.p.pattern))
                                )
                        {
                            result = sAIDocument->Undo();
                            aisdk::check_ai_error(result);
                            isBreak = true;
                            break;
                        }
                        
                    }
                    
                    
                    AIReal pathArea=0, pathLength=0, pathFlatness=0;
                    AIReal dictPathArea=0, dictPathLength=0;
                    ai::int16 segNumber = 0, countSegment = 0;
                    sAIPath->GetPathSegmentCount(pathArtHandle, &countSegment);
                    std::vector<AIRealPoint> realPointArray;
                    AIPathSegment pathSegments[countSegment];
                    
                    
                    sAIPath->GetPathArea(pathArtHandle, &pathArea);
                    result = sAIPath->GetPathSegments(pathArtHandle, segNumber, countSegment, pathSegments);
                    
                    sAIPath->GetPathLength(pathArtHandle, &pathLength, pathFlatness);
                    jjLock.GetRealEntryFromDocumentDict(pathArtHandle, &dictPathArea, "pathArea");
                    jjLock.GetRealEntryFromDocumentDict(pathArtHandle, &dictPathLength, "pathLength");
                    jjLock.GetArrayEntryForRealPoint(pathArtHandle, "pathSegments", &realPointArray);
                    
                    
                    ai::int16 pathSegSelected=0, segSelectedCount=0, selectedPoints=0;
                    AIAnchorIdentifier pathAnchorIds;
                    AIBoolean pathFullySelected = false;
                    sAIPath->GetPathSegmentSelected(pathArtHandle, 0, &pathSegSelected);
                    sAIPath->GetPathAllSegmentsSelected(pathArtHandle, &pathFullySelected);
                    qDebug() << "SElectect " << pathSegSelected<< "  " << segSelectedCount << "  " << pathAnchorIds.segmentOffset <<" "<<pathAnchorIds.fractionalOffset<< " FS "<<pathFullySelected;
                    
                 //   jjLock.FetchAllDictionaryUsingIterator(pathArtHandle);

               //     if((((float)((int)(dictPathArea * 10000))/10000) != ((float)((int)(pathArea * 10000))/10000)  || ((float)((int)(dictPathLength * 10000))/10000) != ((float)((int)(pathLength * 10000))/10000) ) && (pathSegSelected == 2 && segSelectedCount == 1 && pathAnchorIds.fractionalOffset == 0) && (jjLock.GetBooleanEntryFromHandleDict(topParentArtHandle, "clock")) )
                    if((((float)((int)(dictPathArea * 10000))/10000) != ((float)((int)(pathArea * 10000))/10000)  || ((float)((int)(dictPathLength * 10000))/10000) != ((float)((int)(pathLength * 10000))/10000) ) && (pathFullySelected) && (jjLock.GetBooleanEntryFromHandleDict(topParentArtHandle, "clock")) )
                    {
                        AILayerHandle currentLayer;
                        sAILayer->GetCurrentLayer(&currentLayer);
                        objJJLockPanel->ContentLockClicked(topParentArtHandle);
                        jjLock.ReCreatePathHandle(topParentArtHandle);
                        sAILayer->SetCurrentLayer(currentLayer);
                        isBreak = true;
                        sAIArt->SetArtUserAttr(pathArtHandle, kArtSelected | kArtFullySelected, 0);
                        break;
                    }
                    else if( (((float)((int)(dictPathArea * 10000))/10000) != ((float)((int)(pathArea * 10000))/10000)  || ((float)((int)(dictPathLength * 10000))/10000) != ((float)((int)(pathLength * 10000))/10000)) )
                    {
                        result = sAIDocument->Undo();
                        ai::check_ai_error(result);
                        isBreak = true;
                        sAIArt->SetArtUserAttr(pathArtHandle, kArtSelected | kArtFullySelected, 0);
                        break;
                    }
                    else if(countSegment != 0 && realPointArray.size() != 0)
                    {
//                        qDebug() << realPointArray.size() << " c " << countSegment;
                        for(int i = 0; i < countSegment; i++)
                        {
//                            qDebug()  << "  " << pathSegments[i].p.h;
//                            qDebug() << realPointArray.at(i).h;
//                            qDebug() << realPointArray.at(i).v << "  " << pathSegments[i].p.v;

                            if((((float)((int)(realPointArray.at(i).h * 10000))/10000 != (float)((int)(pathSegments[i].p.h * 10000))/10000 ) || ((float)((int)(realPointArray.at(i).v * 10000))/10000 != (float)((int)(pathSegments[i].p.v * 10000))/10000 )))
                            {
//                                qDebug() << realPointArray.at(i).h << "  " << pathSegments[i].p.h;
//                                qDebug() << realPointArray.at(i).v << "  " << pathSegments[i].p.v;
                                result = sAIDocument->Undo();
                                aisdk::check_ai_error(result);
                                isBreak = true;
                                sAIArt->SetArtUserAttr(pathArtHandle, kArtSelected | kArtFullySelected, 0);
                                break;
                            }
                        }
                    }
                    
                }
            }

            AIArtHandle subHandle;
            subHandle = nextChildArt;
            sAIArt->GetArtSibling(subHandle, &nextChildArt);
        }
        if(nextChildArt != NULL && isBreak == false)
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
    return isBreak;
}

bool JJLockPlugin::IsDictAndCurrentOwnRectWidhHeightBoundSame(AIArtHandle artHandle)
{
    AIErr result = kNoErr;
    JJLock jjLock;
    sAIDocument->RedrawDocument();
    AIRealRect artBounds, ownRectDict;
    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
    
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
    aisdk::check_ai_error(result);
  
    const AIRealRect constArtBounds = artBounds;
    const AIRealRect constRectBounds = ownRectDict;
    

    
    //qDebug() << (float)((int)((artBounds.right - artBounds.left)* 100))/100;
    //qDebug() << (float)((int)((artBounds.top - artBounds.bottom)* 100))/100;
    
    //qDebug() << (float)((int)((ownRectDict.right - ownRectDict.left)* 100))/100;
    //qDebug() << (float)((int)((ownRectDict.top - ownRectDict.bottom)* 100))/100;
    
    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds) && (((float)((int)((artBounds.right - artBounds.left)* 100))/100) != ((float)((int)((ownRectDict.right - ownRectDict.left)* 100))/100)) && (((float)((int)((artBounds.top - artBounds.bottom)* 100))/100) != ((float)((int)((ownRectDict.top - ownRectDict.bottom)* 100))/100)))
        return false;
    else
        return true;
    
}

bool JJLockPlugin::IsDictAndCurrentOwnRectBoundSame(AIArtHandle artHandle)
{
    AIErr result = kNoErr;
    JJLock jjLock;
    AIRealRect artBounds, ownRectDict;
    sAIDocument->RedrawDocument();
    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);

    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
    aisdk::check_ai_error(result);
    
    const AIRealRect constArtBounds = artBounds;
    const AIRealRect constRectBounds = ownRectDict;
    
    
//    qDebug() << (float)((int)((artBounds.right - artBounds.left)* 100))/100;
//    qDebug() << (float)((int)((artBounds.top - artBounds.bottom)* 100))/100;
//
//    qDebug() << (float)((int)((ownRectDict.right - ownRectDict.left)* 100))/100;
//    qDebug() << (float)((int)((ownRectDict.top - ownRectDict.bottom)* 100))/100;

    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds) && (((float)((int)((artBounds.right - artBounds.left)* 100))/100) == ((float)((int)((ownRectDict.right - ownRectDict.left)* 100))/100)) && (((float)((int)((artBounds.top - artBounds.bottom)* 100))/100) == ((float)((int)((ownRectDict.top - ownRectDict.bottom)* 100))/100)))
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool JJLockPlugin::IsDictAndCurrentOwnRectBoundLengthSame(AIArtHandle artHandle)
{
    AIErr result = kNoErr;
    JJLock jjLock;
    AIRealRect artBounds, ownRectDict;
    sAIDocument->RedrawDocument();
    sAIArt->GetArtTransformBounds(artHandle, NULL, kNoStrokeBounds, &artBounds);
    
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.top, "JJLockArtOwnBoundTop");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.bottom, "JJLockArtOwnBoundBottom");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.left, "JJLockArtOwnBoundLeft");
    aisdk::check_ai_error(result);
    result = jjLock.GetRealEntryFromDocumentDict(artHandle, &ownRectDict.right, "JJLockArtOwnBoundRight");
    aisdk::check_ai_error(result);
    
    const AIRealRect constArtBounds = artBounds;
    const AIRealRect constRectBounds = ownRectDict;

    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
        return false;
    else
        return true;
}

void JJLockPlugin::LayerChangeCheck()
{
    ASErr result = kNoErr;
    JJLock jjLock;
    AILayerHandle layerHnd;
    ai::UnicodeString title;
    sAILayer->GetCurrentLayer(&layerHnd);
    sAILayer->GetLayerTitle(layerHnd, title);
    
    AIArtSpec hiddenArtSpec[7] = {{kTextFrameArt, kArtHidden, kArtHidden}, {kPlacedArt,  kArtHidden, kArtHidden},  {kGroupArt,  kArtHidden, kArtHidden}, {kPathArt,  kArtHidden, kArtHidden}, {kPluginArt,  kArtHidden, kArtHidden}, {kCompoundPathArt,  kArtHidden, kArtHidden}, {kSymbolArt,  kArtHidden, kArtHidden}};
    ArtSetHelper hiddenArtSet(hiddenArtSpec, 7);
    size_t count = hiddenArtSet.GetCount();
    AIArtHandle artHandle;
 
    if(count > 0)
    {
        for(int i=0; i<count; i++)
        {
            artHandle = hiddenArtSet[i];
            if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "lock"))
                if(jjLock.GetBooleanEntryFromHandleDict(artHandle, "clock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "pclock") || jjLock.GetBooleanEntryFromHandleDict(artHandle, "plock"))
                {
                    AILayerHandle layerHandle;
                 //   //qDebug() << "Title "<< title.as_UTF8().c_str() << "       " << jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "layerName").as_UTF8().c_str();
                    sAIArt->GetLayerOfArt(artHandle, &layerHandle);
                    sAILayer->GetLayerTitle(layerHandle, title);
                //    qDebug() << "Title2 "<< title.as_UTF8().c_str() << "       " << jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "layerName").as_UTF8().c_str();
                    
                    if(title != jjLock.GetUnicodeStringEntryFromHandleDict(artHandle, "layerName"))
                    {
                        result = sAIDocument->Undo();
                        aisdk::check_ai_error(result);
                        break;
                    }
                }
        }
    }
}

//void JJLockPlugin::ParseGroupArtObjects(AIArtHandle artHandle, short type)
//{
//    AIErr result = kNoErr;
//    JJLock jjLock;
//    bool breakInnerLoop = false;
//    AIArtHandle groupArtHandle, nextChildArt;
//    sAIArt->GetArtFirstChild(artHandle, &nextChildArt);
//    groupArtHandle = artHandle;
//    
//    while(jjLock.GetBooleanEntryFromHandleDict(groupArtHandle, "childLocked") && groupArtHandle != NULL && type == kGroupArt && !breakInnerLoop)
//    {
//        ai::UnicodeString groupArtName;
//        ASBoolean groupBool = true;
//        sAIArt->GetArtName(groupArtHandle, groupArtName, &groupBool);
//
//        while(nextChildArt != NULL)
//        {
//            ai::UnicodeString childArtName;
//            ASBoolean childBool = true;
//            short childArtType;
//            sAIArt->GetArtName(nextChildArt, childArtName, &childBool);
//            sAIArt->GetArtType(nextChildArt, &childArtType);
//            if(childArtType == kGroupArt)
//            {
//                
//                if((jjLock.GetBooleanEntryFromHandleDict(nextChildArt, "lock") == true) )
//                {
//                    AIRealRect artBounds = {0,0,0,0};
//                    sAIArt->GetArtTransformBounds(nextChildArt, NULL, kNoStrokeBounds, &artBounds);
//                    
//                    AIRealRect rectDict;
//                    rectDict.top = 0;
//                    rectDict.bottom = 0;
//                    rectDict.left = 0;
//                    rectDict.right = 0;
//                    
//                    
//                    result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.top, "JJLockArtBoundTop");
//                    aisdk::check_ai_error(result);
//                    result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.bottom, "JJLockArtBoundBottom");
//                    aisdk::check_ai_error(result);
//                    result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.left, "JJLockArtBoundLeft");
//                    aisdk::check_ai_error(result);
//                    result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.right, "JJLockArtBoundRight");
//                    aisdk::check_ai_error(result);
//                    
//                    
//                    if(!(rectDict.top == 0 && rectDict.bottom == 0 && rectDict.left == 0 && rectDict.right == 0))
//                    {
//                        const AIRealRect constArtBounds = artBounds;
//                        const AIRealRect constRectBounds = rectDict;
//                        if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
//                        {
//                            try {
//                             result = sAIDocument->Undo();
//                        aisdk::check_ai_error(result);
//                                breakInnerLoop = true;
//                                break;
//                            }
//                            catch (ai::Error &ex){
//                                result = ex;
//                            }
//                            catch (ATE::Exception &ex){
//                                result = ex.error;
//                            }
//                        }
//                    }
//                }
//                else
//                {
//                    sAIArt->GetArtFirstChild(nextChildArt, &nextChildArt);
//                    sAIArt->GetArtType(nextChildArt, &childArtType);
//                    ParseGroupArtObjects(nextChildArt, childArtType);
//                }
//            }
//            
//            if((jjLock.GetBooleanEntryFromHandleDict(nextChildArt, "lock") == true) )
//            {
//                AIRealRect artBounds = {0,0,0,0};
//                sAIArt->GetArtTransformBounds(nextChildArt, NULL, kNoStrokeBounds, &artBounds);
//                
//                AIRealRect rectDict;
//                rectDict.top = 0;
//                rectDict.bottom = 0;
//                rectDict.left = 0;
//                rectDict.right = 0;
//                
//                
//                result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.top, "JJLockArtBoundTop");
//                aisdk::check_ai_error(result);
//                result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.bottom, "JJLockArtBoundBottom");
//                aisdk::check_ai_error(result);
//                result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.left, "JJLockArtBoundLeft");
//                aisdk::check_ai_error(result);
//                result = jjLock.GetRealEntryFromDocumentDict(nextChildArt, &rectDict.right, "JJLockArtBoundRight");
//                aisdk::check_ai_error(result);
//                
//                
//                if(!(rectDict.top == 0 && rectDict.bottom == 0 && rectDict.left == 0 && rectDict.right == 0))
//                {
//                    const AIRealRect constArtBounds = artBounds;
//                    const AIRealRect constRectBounds = rectDict;
//                    if(!sAIRealMath->AIRealRectEqual(&constRectBounds, &constArtBounds))
//                    {
//                        try {
//                         result = sAIDocument->Undo();
//                        aisdk::check_ai_error(result);
//                            breakInnerLoop = true;
//                            break;
//                        }
//                        catch (ai::Error &ex){
//                            result = ex;
//                        }
//                        catch (ATE::Exception &ex){
//                            result = ex.error;
//                        }
//                    }
//                }
//            }
//            sAIArt->GetArtSibling(nextChildArt, &nextChildArt);
//        }
//        if(breakInnerLoop)
//            break;
//        sAIArt->GetArtParent(groupArtHandle, &groupArtHandle);
//        sAIArt->GetArtFirstChild(groupArtHandle, &nextChildArt);
//        sAIArt->GetArtSibling(nextChildArt, &nextChildArt);
//    }
//}

/*
 */
ASErr JJLockPlugin::AddMenus( SPInterfaceMessage *message )
{
    ASErr error = kNoErr;
    
    // Add an SGK menu group to the Windows menu.
    const char *windowMenuGroupName = COMPANY_NAME;
    bool exists = false;
    error = this->MenuGroupExists(windowMenuGroupName, exists);
    aisdk::check_ai_error(error);
    if (!exists) {
        AIPlatformAddMenuItemDataUS menuItemData;
        menuItemData.groupName = kOtherPalettesMenuGroup;
        menuItemData.itemText = ai::UnicodeString(windowMenuGroupName);
        AIMenuItemHandle menuItemHandle = nil;
        error = sAIMenu->AddMenuItem(message->d.self, NULL, &menuItemData, kMenuItemNoOptions, &menuItemHandle);
        aisdk::check_ai_error(error);
        AIMenuGroup menuGroup = nil;
        error = sAIMenu->AddMenuGroupAsSubMenu(windowMenuGroupName, kMenuGroupSortedAlphabeticallyOption, menuItemHandle, &menuGroup);
        aisdk::check_ai_error(error);
    }
    
    // Add menu item for this plug-in under the company's about plug-ins menu group.
    AIPlatformAddMenuItemDataUS showHidePanelMenuData;
    showHidePanelMenuData.groupName = windowMenuGroupName;
    showHidePanelMenuData.itemText = ai::UnicodeString(PLUGIN_NAME_STRING);
    AIMenuItemHandle showHidePanelMenuItemHandle = nil;
    error = sAIMenu->AddMenuItem(message->d.self, NULL, &showHidePanelMenuData, kMenuItemWantsUpdateOption, &showHidePanelMenuItemHandle);
    aisdk::check_ai_error(error);
    fSubWindowMenu = showHidePanelMenuItemHandle;
    
    return error;
}

/*
 */
ASErr JJLockPlugin::MenuGroupExists(const char* targetGroupName, bool& groupExists)
{
    ASErr error = kNoErr;
    groupExists = false;
    ai::int32 count = 0;
    AIMenuGroup dummyGroup = nil;
    error = sAIMenu->CountMenuGroups( &count );
    if ( error ) return error;
    for (ai::int32 i = 0; i < count; i++) {
        error = sAIMenu->GetNthMenuGroup( i, &dummyGroup );
        aisdk::check_ai_error(error);
        const char* name;
        error = sAIMenu->GetMenuGroupName( dummyGroup, &name );
        aisdk::check_ai_error(error);
        if ( std::strcmp(name, targetGroupName ) == 0 ) {
            groupExists = true;
            break;
        }
    }
    return error;
}

void PanelFlyoutMenuProc(AIPanelRef inPanel, ai::uint32 itemID)
{
    AIErr error = kNoErr;
    AIPanelFlyoutMenuRef flyoutMenu = NULL;
    error = sAIPanel->GetFlyoutMenu(inPanel, flyoutMenu);
    AIPanelFlyoutMenuItemMark currentItemMark;
    JJLockPanel jjLockPanel;
    switch (itemID) {
        case 1:
            error = sAIPanelFlyoutMenu->SetItemMark(flyoutMenu, 1, kAIPanelFlyoutMenuItemMark_NONE);
            jjLockPanel.MoveLockIcon();
            break;
//        case 2:
//            error = sAIPanelFlyoutMenu->SetItemMark(flyoutMenu, 2, kAIPanelFlyoutMenuItemMark_NONE);
//            break;
        default:
            break;
    }
}

/** Prototype of a function that is called after the visibility of a panel is changed.
 */
void PanelVisibilityChangedNotifyProc(AIPanelRef inPanel, AIBoolean isVisible)
{
    //sAIUser->MessageAlert(ai::UnicodeString("Panel visiblility changed!"));
}

/** Prototype of a function that is called after the size of a panel is changed.
 */
void PanelSizeChangedNotifyProc(AIPanelRef inPanel)
{
    //sAIUser->MessageAlert(ai::UnicodeString("Panel size changed!"));
}

/** Prototype of a function that is called after the state of a panel is changed.
 */
void PanelStateChangedNotifyProc(AIPanelRef inPanel, ai::int16 newState)
{
    //sAIUser->MessageAlert(ai::UnicodeString("Panel state changed!"));
    
    ASErr error = kNoErr;
    AISize prefSize;
    switch(newState)
    {
        case 1:
            prefSize.width = 310;
            prefSize.height = 280;
            break;
        case 2:
            prefSize.width = 500;
            prefSize.height = 450;
            break;
            
        default:
            break;
    }
    
    error = sAIPanel->SetPreferredSizes(inPanel, prefSize);
}

/** Prototype of a function that is called after a panel is closed by clicking on the close button.
 */
void PanelClosedNotifyProc(AIPanelRef inPanel)
{
    //sAIUser->MessageAlert(ai::UnicodeString("Panel closed!"));
}

/*
 */


ASErr JJLockPlugin::CreatePanel()
{
    ASErr error = kNoErr;
    AISize minSize = {350, 375};
    AISize maxSize = {350, 375};
    AISize prefConstSize = {350, 375};
    AISize prefUnconstSize = {350, 375};
    AIBoolean isResizeable = false;
    int id = 1;
    AIPanelUserData inUserData = &id;
    
    fPanelFlyoutMenu = NULL;
    error = sAIPanelFlyoutMenu->Create(fPanelFlyoutMenu);
    if (error) {
        return error;
    }
    
    error = sAIPanelFlyoutMenu->AppendItem(fPanelFlyoutMenu, 1, ai::UnicodeString("Move Lock Icon"));
  //  error = sAIPanelFlyoutMenu->AppendItem(fPanelFlyoutMenu, 2, ai::UnicodeString("NULL"));
    
    error = sAIPanelFlyoutMenu->SetItemMark(fPanelFlyoutMenu, 1 , kAIPanelFlyoutMenuItemMark_NONE);
   // error = sAIPanelFlyoutMenu->SetItemMark(fPanelFlyoutMenu, 2 , kAIPanelFlyoutMenuItemMark_NONE);
    
    
    error = sAIPanel->Create(fPluginRef, ai::UnicodeString("Locking"), ai::UnicodeString("Locking"), 2, minSize, isResizeable, fPanelFlyoutMenu, inUserData, fPanel);
    aisdk::check_ai_error(error);
    
    error = sAIPanel->SetSizes(fPanel, minSize, prefUnconstSize, prefConstSize, maxSize);
    aisdk::check_ai_error(error);
    
    error = sAIPanel->SetVisibilityChangedNotifyProc(fPanel, PanelVisibilityChangedNotifyProc);
    error = sAIPanel->SetFlyoutMenuProc(fPanel, PanelFlyoutMenuProc);
    error = sAIPanel->SetSizeChangedNotifyProc(fPanel, PanelSizeChangedNotifyProc);
    error = sAIPanel->SetStateChangedNotifyProc(fPanel, PanelStateChangedNotifyProc);
    error = sAIPanel->SetClosedNotifyProc(fPanel, PanelClosedNotifyProc);
                            
    aisdk::check_ai_error(error);
    
    return error;
}

ASErr JJLockPlugin::AddWidgets()
{
    ASErr error = kNoErr;
    
    int arg = 0;
    char **argv = NULL;
    QApplication::setAttribute(Qt::AA_MacPluginApplication);
    QApplication::setAttribute(Qt::AA_ImmediateWidgetCreation);
    
    gPlugin->qtApplication = new QApplication(arg, argv);
    error = sAIPanel->GetPlatformWindow(fPanel, hDlg);
    aisdk::check_ai_error(error);
    
    
    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->move(0, 0);
    /*QPalette p = nativeWidget->palette();
     QColor color(211, 211, 211);
     p.setColor(nativeWidget->backgroundRole(), color);
     nativeWidget->setPalette(p);*/
    
    nativeWidget->setAutoFillBackground(true);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    
    //olive->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    objJJLockPanel = new JJLockPanel(nativeWidget);
    layout->addWidget(objJJLockPanel);
    nativeWidget->setLayout(layout);
    nativeWidget->adjustSize();
    
    // Adjust Cocoa layouts
    NSView* nativeWidgetView = reinterpret_cast<NSView* >(nativeWidget->winId());
    [hDlg setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [hDlg setAutoresizesSubviews:YES];
    
    [nativeWidgetView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [nativeWidgetView setAutoresizesSubviews:YES];
    
    // Add the nativeWidget to the window.
    nativeWidget->nativeView();
    [hDlg setFrame:[nativeWidgetView frame]];
    [hDlg addSubview:nativeWidgetView];
    nativeWidget->show();
    
    return error;
}



