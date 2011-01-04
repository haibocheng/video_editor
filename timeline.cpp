#include "timeline.h"
#include "movie.h"
#include "localization.h"
#include "toolbox.h"
using namespace localization;
Timeline::Timeline()
{
    loaded = false;
};


bool Timeline::Load(String &filename)
{
    Movie *movie = new Movie();
    movie->Load(filename);
    bool loaded_local = movie->loaded;
    if(loaded_local)
    {
        movies.push_front(movie);
        duration = movie->duration;
        current = movie->current;
    }
    else
    {
        delete movie;
    }

    loaded = loaded || loaded_local;
    return loaded_local;

}

Movie* Timeline::GetCurrentMovie()
{
    return movies.front();
}

Image* Timeline::GetImage()
{
    return GetCurrentMovie()->image;
}


void Timeline::Dispose()
{
    if(loaded)
    {
        for(list<Movie*>::iterator it = movies.begin(); it!=movies.end(); it++)
        {
            delete *it;
        }
    }
    loaded = false;
}

Timeline::~Timeline()
{
    Dispose();
}

bool Timeline::GotoRatioAndRead(double ratio,bool decode)
{
    return GotoSecondAndRead(ratio * duration,decode);
}


bool Timeline::GotoSecondAndRead(double dest,bool decode)
{
    bool res = GetCurrentMovie()->GotoSecondAndRead(dest,decode);
    current = GetCurrentMovie()->current;
    return res;
}

void Timeline::ReadAndDecodeFrame()
{
    GetCurrentMovie()->ReadAndDecodeFrame();
    current = GetCurrentMovie()->current;
}

void Timeline::SkipFrame()
{
    GetCurrentMovie()->SkipFrame();
    current = GetCurrentMovie()->current;
}

bool Timeline::GoBack(int frames)
{
    bool res = GetCurrentMovie()->GoBack(frames);
    current = GetCurrentMovie()->current;
    return res;
}

void Timeline::DecodeFrame()
{
    GetCurrentMovie()->DecodeFrame();
}

