#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_


#include "MainAppWindow.h"
#include "juce/juce.h"
#include "timeline.h"
#include "localization.h"
#include "toolbox.h"
#include "ContainerBox.h"
#include "encodeVideo.h"
#include "events.h"
#include "taskTab.h"

class AskJumpDestanation;
class encodeVideo;

using namespace localization;

class MainAppWindow;
class MainComponent : public Component, public MenuBarModel, public ApplicationCommandTarget, public Timer, public ButtonListener, public DragAndDropContainer, public SliderListener, public ScrollBarListener, public DragAndDropTarget
{
public:
    int mouse_x;
    int mouse_y;




    AskJumpDestanation *ask_jump_target;

    void changeFileName(String new_filename);

    TooltipWindow tooltipWindow;

    DrawableButton* playButton;
    DrawableButton* pauseButton;
    DrawableButton* stopButton;
    DrawableButton* nextFrameButton;
    DrawableButton* prevFrameButton;
    DrawableButton* zoomInButton;
    DrawableButton* zoomOutButton;

    void initImageButton(String pic_name,DrawableButton*& button);

    void SetVisibleButtons(bool visible);


    enum CommandIDs
    {
        commandOpen                 = 0x2000,
        commandSave                 = 0x2001,
        commandJump                 = 0x2003,
        commandSaveFrame            = 0x2004,
        commandPlay                 = 0x2005,
        commandPause                = 0x2006,
        commandStop                 = 0x2007,
        commandNextFrame            = 0x2008,
        commandPrevFrame            = 0x2009,
        commandNext5Frame           = 0x200A,
        commandPrev5Frame           = 0x200B,
        commandNextSecond           = 0x200C,
        commandPrevSecond           = 0x200D,
        commandRemoveMovie          = 0x200E,
        commandSplit                = 0x200F,
        commandRemoveSpaces         = 0x2010,
        commandShowTasks            = 0x2011


    };

    MainAppWindow* mainWindow;

    Timeline *timeline;
    Timeline *timeline_original;
    void buttonClicked (Button* button) ;
    MainComponent (MainAppWindow* mainWindow_);

    ~MainComponent ();

    void resized ();

    void paint (Graphics& g);
    int GetArrowPosition(int arrow_position);
    int GetCurrentPosition();
    double GetSecond(int mouse);
    bool NeedDrawArrow();
    void timerCallback();
    void DrawSlider(Graphics& g);

    void DrawArrow(Graphics& g);

    void repaintSlider();

    void mouseMove (const MouseEvent& e);
    void mouseDown (const MouseEvent& e);
    const StringArray getMenuBarNames();
    const PopupMenu getMenuForIndex (int menuIndex, const String& menuName);

    void menuItemSelected (int menuItemID, int topLevelMenuIndex);

    bool perform (const InvocationInfo& info);
    ApplicationCommandTarget* getNextCommandTarget();

    void getAllCommands (Array <CommandID>& commands);
    bool isVideoReady ();
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);

    int64 miliseconds_start;
    bool video_playing;
    void StopVideo();
    void StartVideo();

    int GetMoviesBorder();

    ContainerBox * movies_list;

    void ResizeViewport();
    void AddMovieToList(Movie*movie);

    ScrollBar * timeline_scrollbar;

    double second_to_pixel;
    double timeline_position;
    Slider *scale_timeline;
    void sliderValueChanged(Slider* slider);
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved,double newRangeStart);
    bool isInterestedInDragSource (const String& sourceDescription,Component* sourceComponent);
    void itemDropped (const String& sourceDescription,Component* sourceComponent,int x, int y);
    void itemDragMove (const String& sourceDescription,Component* sourceComponent,int x, int y);

    bool shouldDrawDragImageWhenOver();
    int current_drag_x;
    int current_drag_y;


    double GetPositionSecond(int arrow_position);
    void mouseDrag (const MouseEvent& e);
    void mouseExit(const MouseEvent& e);

    void mouseMoveReaction();

    void cleanAfterDrag();

    int dragIntervalOffset;

    encodeVideo * encodeVideoWindow;

    EventList AfterChangePosition;
    void GotoSecondAndRead(double second);

    taskTab * tasks;

};

#endif//_MAINCOMPONENT_H_
