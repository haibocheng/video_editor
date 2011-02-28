#include "taskTab.h"
#include "tasks.h"

void _RepaintTaskTab(void * object)
{
    taskTab * o = (taskTab *)object;
    if(!o->isVisible)
        o->add();

    o->table.updateContent();
}
void taskTab::resized()
{
    table.autoSizeAllColumns();
    DocumentWindow::resized();
}
taskTab::taskTab():DocumentWindow(LABEL_TASK_TAB,Colours::whitesmoke,DocumentWindow::allButtons)
{
    setTitleBarHeight (20);
    setResizable (true, false);
    setBounds(30,30,800,600);
    setResizeLimits(800,600,8000,6000);
    setContentComponent(&table);

    table.setModel (this);

    table.setColour (ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);


    table.getHeader().addColumn(LABEL_TASK_TAB_TYPE,1,70,70,70,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(String::empty,2,50,50,50,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(String::empty,3,50,50,50,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(LABEL_TASK_TAB_DESCRPTION,4,330,330,900,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable | TableHeaderComponent::resizable);
    table.getHeader().addColumn(LABEL_TASK_TAB_TIME_LEFT,5,100,100,100,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable );
    table.getHeader().addColumn(LABEL_TASK_TAB_PROGRESS,6,180,180,900,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable | TableHeaderComponent::resizable);
    table.setHeaderHeight(30);



    isVisible = false;

    AddEvent(OnChangeList,this,_RepaintTaskTab);

}
void taskTab::add()
{
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar || ComponentPeer::windowIsResizable);
    isVisible = true;
}

void taskTab::refresh()
{
    repaint();
}

void taskTab::remove()
{
    removeFromDesktop();
    isVisible = false;
}

taskTab::~taskTab()
{

}
int taskTab::getNumRows()
{
    return GetTaskLength();
}

void taskTab::paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
            g.fillAll (Colours::lightblue);
}

void taskTab::paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    task *t = FindTaskByNumber(rowNumber);
    if(columnId == 6)
    {
        g.drawText (t->status, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
    if(columnId == 4)
    {
        g.drawText (t->filename, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
    g.setColour (Colours::black.withAlpha (0.2f));
    g.fillRect (width - 1, 0, 1, height);
}

void taskTab::closeButtonPressed()
{
    remove();
}
