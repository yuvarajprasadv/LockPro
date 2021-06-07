//
//  JJLockPlugin.h
//  JJLock
//
//  Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 05/02/2019
//

#ifndef __JJLockPlugin_h__
#define __JJLockPlugin_h__

#include <qapplication.h>
#include <Cocoa/Cocoa.h>
#include <qdebug.h>
#include "Plugin.hpp"
#include "SDKAboutPluginsHelper.h"

#include "JJLockID.h"
#include "JJLockSuites.h"
#include "SelectionHelper.h"
#include "TextHelper.h"


/**	Creates a new JJLockPlugin.
 @param pluginRef IN unique reference to this plugin.
 @return pointer to new JJLockPlugin.
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef);

/**	Reloads the JJLockPlugin class state when the plugin is
 reloaded by the application.
 @param plugin IN pointer to plugin being reloaded.
*/
void FixupReload(Plugin* plugin);

/**	Hooks JJLock up as an Illustrator plug-in.
 @ingroup JJLock
*/
class JJLockPlugin : public Plugin
{
public:
    /** Constructor.
     @param pluginRef IN reference to this plugin.
	*/
	JJLockPlugin(SPPluginRef pluginRef);

	/** Destructor.
	*/
	virtual ~JJLockPlugin();
    
    /**	Restores state of JJLockPlugin during reload.
     */
    FIXUP_VTABLE_EX(JJLockPlugin, Plugin);

protected:
    /**	Creates a global instance of the plugin.
     @param plugin IN plugin to create.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr SetGlobal(Plugin * plugin);

    /**	Handler called when a menu item is selected in the UI.
     @param message IN details of menu item.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr GoMenuItem( AIMenuMessage * message );
    
    /**	Initializes the plugin.
     @param message IN message sent by the plugin manager.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr StartupPlugin( SPInterfaceMessage * message );
    
    /**	Removes the plugin.
     @param message IN message sent by the plugin manager.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr ShutdownPlugin( SPInterfaceMessage * message );
    
    /** Notify's the panel that the log has changed.
     @param caller IN sender of the message.
     @param selector IN action to take.
     @param message IN pointer to data required for action.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr Message(char *caller, char *selector, void *message);

    /**	Handles notifiers this plugin is registered for when they are received.
     @param message IN contains details of notifier.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr Notify( AINotifierMessage * message );
    
    ASErr AddNotifiers( SPInterfaceMessage *message );


private:
    
    AIArtHandle childParentHandle = NULL;
    /** Handle for app shutting down notifier.
     */
    AINotifierHandle fAppShutdownNotifier;
    
    /** Handle for all plugin started notifier.
     */
    AINotifierHandle fAllPluginStartedNotifier;
    
    /** Handle for before doucument closed notifier.
     */
    AINotifierHandle fDocumentClosePreNotifier;
    
    /** Handle for before doucument closed notifier.
     */
    AINotifierHandle fDocumentClosePostNotifier;
    
    /** Handle for document changed notifier.
     */
    AINotifierHandle fDocumentChangedNotifier;
    
    /** Handle for document opened notifier.
     */
    AINotifierHandle fDocumentOpenedNotifier;
    
    /** Handle for pre save document notifier.
     */
    AINotifierHandle fSaveCommandPreNotifierStr;
    AINotifierHandle fSaveAsCommandPreNotifierStr;
    AINotifierHandle fSaveACopyAsCommandPreNotifierStr;
    AINotifierHandle fSaveAsTemplateCommandPreNotifierStr;
    AINotifierHandle fAdobeAISaveForWebCommandPreNotifierStr;
    AINotifierHandle fAdobeAISaveSelectedSlicesCommandPreNotifierStr;
    AINotifierHandle fSaveForOfficeCommandPreNotifierStr;
    
    AINotifierHandle fAfterAltOptionDragCopyCommandPostNotifierStr;
    
    /** Handle for post save document notifier.
     */
    AINotifierHandle fSaveCommandPostNotifierStr;
    AINotifierHandle fSaveAsCommandPostNotifierStr;
    AINotifierHandle fSaveACopyAsCommandPostNotifierStr;
    AINotifierHandle fSaveAsTemplateCommandPostNotifierStr;
    AINotifierHandle fAdobeAISaveForWebCommandPostNotifierStr;
    AINotifierHandle fAdobeAISaveSelectedSlicesCommandPostNotifierStr;
    AINotifierHandle fSaveForOfficeCommandPostNotifierStr;
    
    AINotifierHandle fAIDocumentViewInvalidRectChangedNotifier;
    AINotifierHandle fAIAfterPasteCommandPreNotifier;
    AINotifierHandle fAIGroupCommandPreNotifier;
    AINotifierHandle fAIGroupCommandPostNotifier;
    AINotifierHandle fAIUngroupCommandPreNotifier;
    AINotifierHandle fAIUngroupCommandPostNotifier;
    AINotifierHandle fAITextFontCommandPostNotifierStr;
    AINotifierHandle fAILayerSetNotifier;
    AINotifierHandle fAIAdobeLinkPaletteMenuItemCommandPostNotifierStr;
    AINotifierHandle fAINotifier;
    AINotifierHandle fAIArtSelectionChangedNotifier;
    
    AINotifierHandle fAILayerArtStyleChangeNotifier;
    AINotifierHandle fAILayerOptionsNotifier;
    AINotifierHandle fAILayerDeletionNotifier;
    AINotifierHandle fAICurrentLayerNotifier;
    
    AINotifierHandle fAIArtObjectsChangedNotifier;
    

    /* Handle for the About Plug-ins menu item. */
    AIMenuItemHandle fAboutPluginMenu;
    
    /* Handle for the sub menu items */
    AIMenuItemHandle fSubWindowMenu;
    
    /* Pointer to instance of Panel. */
    AIPanelRef fPanel;
    
    /* Pointer to instance of Panel flyout menu */
    AIPanelFlyoutMenuRef fPanelFlyoutMenu;
    
    AIPanelPlatformWindow hDlg;
    
    QApplication *qtApplication;
    
    /** Handle for the annotator added by this plug-in.
     */
    AIAnnotatorHandle	fAnnotatorHandle;
    
    /** Pointer to Annotator object.
     */
    //Annotator* fAnnotator;


    /** Add menu item associated with this plugin.
     @param message IN contains plugin information.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr AddMenus(SPInterfaceMessage* message);
    
    /** Check if a menu group has already been created.
     @param targetGroupName IN name of the menu group to look for.
     @param groupAlreadyMade OUT true if group already exists, false if does not exist.
     @return kNoErr on success, other ASErr otherwise.
     */
    ASErr MenuGroupExists(const char* targetGroupName, bool& groupExists);
    
    /** Create Panel for the Plug-in
     */
    ASErr CreatePanel();

    /** Add Widget to the Plug-in
     */
    ASErr AddWidgets();
    
   // void ParseGroupArtObjects(AIArtHandle artHandle, short type);
    void LayerChangeCheck();
    bool IsDictAndCurrentOwnRectBoundSame(AIArtHandle artHandle);
    bool IsDictAndCurrentOwnRectBoundLengthSame(AIArtHandle artHandle);
    bool IsDictAndCurrentOwnRectWidhHeightBoundSame(AIArtHandle artHandle);
    bool ParseCompoundPathArtGroupCheck(AIArtHandle topParentArtHandle, AIArtHandle compoundPathArtHandle, std::string lockType);
    bool ParseGroupChildForLockConditionCheck(AIArtHandle groupArtHandle, string lockType);
    void FindChildParentGroupLockHandle(AIArtHandle parentHandle);

};

#endif
