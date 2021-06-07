//
//  JJLockPanel.hpp
//  JJLock
//
//  Created by Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 05/02/2019
//

#ifndef JJLockPanel_hpp
#define JJLockPanel_hpp
#include <stdio.h>
#include "JJLockSuites.h"
#include "JJLockPlugin.h"
#include "JJLockUI.h"
#include "JJLock.hpp"
#include "Tool.h"


extern JJLockPlugin* gPlugin;

class JJLockPanel : public JJLockUI
{
public:
    AIArtHandle tempArtHandle = NULL;
    AIPathStyle tempPathStyle;
    AIBoolean fillVisible = false;
    AIBoolean strokeVisible = false;
    
    bool isDocumentOpen();
    
    explicit JJLockPanel(QWidget *parent = 0);
    ~JJLockPanel();
    std::string GetTextureFilename(const char *Name);
    void ContentLockClicked();
    void ContentLockClicked(AIArtHandle artHandle);
    void PositionLockClicked();
    void PositionLockClicked(AIArtHandle artHandle);
    void ContentAndPositionLockClicked();
    void ContentAndPositionLockClicked(AIArtHandle artHandle);
    void ReleaseLockClicked();
    void ReleaseLockClicked(AIArtHandle artHandle);
    void LoggerAPI();
    ai::UnicodeString CreateUUID();
    string GetIconImagePath(string imageName);
    
    bool FindArtIsGroupItem(short type, AIArtHandle artHandle);
    void initializeProgressBar(QString title, int startValue, int maxValue);
    void deactivateProgressBarDialog();
    void setProgressBarValue(int value);
    
    void DeleteGroupArtPositionNoteDictionary(AIArtHandle artHandle);
    ai::UnicodeString GetFileInfoContent(AIArtHandle artHandle);
   // void ParseGroupArtObjectsForLocking(AIArtHandle artHandle, short type);
   // bool IsChildOfGroupIsLocked(AIArtHandle artHandle, short type);
    void MoveLockIcon();
  //  void ParseGroupArtObjects(AIArtHandle artHandle);
    void ParseCompoundPathForLock(AIArtHandle compoundPathArtHandle, string lockType);
    void ParseGroupChildForLock(AIArtHandle groupArtHandle, int* totalChildCount, string lockType);
    Boolean ParseGroupChildForLockCheck(AIArtHandle groupArtHandle);
    void ParseGroupChildForCount(AIArtHandle groupArtHandle, int* countChild);
    void CheckAndCreateParentIDForChild(AIArtHandle childHandle);
    void CheckAndReleaseParentIDForChild(AIArtHandle childHandle);
    
    
    void ReleaseLockClickedTest();
    
    
private:
    bool isNotChildLocked = true;
    
};


#endif
