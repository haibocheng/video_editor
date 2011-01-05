#include "DrawableButtonAndDrag.h"

DrawableButtonAndDrag::DrawableButtonAndDrag(const String& buttonName,ButtonStyle buttonStyle,Timeline* timeline,ContainerBox * box,MainComponent * main):DrawableButton(buttonName,buttonStyle)
{
    this->timeline = timeline;
    this->box = box;
    this->main = main;
}


void DrawableButtonAndDrag::mouseDrag (const MouseEvent& e)
{
    int index = box->getIndexOfChildComponent(this->getParentComponent());
    Movie*movie = timeline->movies[index];
    main->startDragging(String("movie_to_timeline"),this,*movie->image_preview);
}
