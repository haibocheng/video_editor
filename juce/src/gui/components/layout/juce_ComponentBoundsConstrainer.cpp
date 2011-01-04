/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_ComponentBoundsConstrainer.h"
#include "../juce_Desktop.h"
#include "../windows/juce_ComponentPeer.h"


//==============================================================================
ComponentBoundsConstrainer::ComponentBoundsConstrainer() throw()
    : minW (0),
      maxW (0x3fffffff),
      minH (0),
      maxH (0x3fffffff),
      minOffTop (0),
      minOffLeft (0),
      minOffBottom (0),
      minOffRight (0),
      aspectRatio (0.0)
{
}

ComponentBoundsConstrainer::~ComponentBoundsConstrainer()
{
}

//==============================================================================
void ComponentBoundsConstrainer::setMinimumWidth (const int minimumWidth) throw()
{
    minW = minimumWidth;
}

void ComponentBoundsConstrainer::setMaximumWidth (const int maximumWidth) throw()
{
    maxW = maximumWidth;
}

void ComponentBoundsConstrainer::setMinimumHeight (const int minimumHeight) throw()
{
    minH = minimumHeight;
}

void ComponentBoundsConstrainer::setMaximumHeight (const int maximumHeight) throw()
{
    maxH = maximumHeight;
}

void ComponentBoundsConstrainer::setMinimumSize (const int minimumWidth, const int minimumHeight) throw()
{
    jassert (maxW >= minimumWidth);
    jassert (maxH >= minimumHeight);
    jassert (minimumWidth > 0 && minimumHeight > 0);

    minW = minimumWidth;
    minH = minimumHeight;

    if (minW > maxW)
        maxW = minW;

    if (minH > maxH)
        maxH = minH;
}

void ComponentBoundsConstrainer::setMaximumSize (const int maximumWidth, const int maximumHeight) throw()
{
    jassert (maximumWidth >= minW);
    jassert (maximumHeight >= minH);
    jassert (maximumWidth > 0 && maximumHeight > 0);

    maxW = jmax (minW, maximumWidth);
    maxH = jmax (minH, maximumHeight);
}

void ComponentBoundsConstrainer::setSizeLimits (const int minimumWidth,
                                                const int minimumHeight,
                                                const int maximumWidth,
                                                const int maximumHeight) throw()
{
    jassert (maximumWidth >= minimumWidth);
    jassert (maximumHeight >= minimumHeight);
    jassert (maximumWidth > 0 && maximumHeight > 0);
    jassert (minimumWidth > 0 && minimumHeight > 0);

    minW = jmax (0, minimumWidth);
    minH = jmax (0, minimumHeight);
    maxW = jmax (minW, maximumWidth);
    maxH = jmax (minH, maximumHeight);
}

void ComponentBoundsConstrainer::setMinimumOnscreenAmounts (const int minimumWhenOffTheTop,
                                                            const int minimumWhenOffTheLeft,
                                                            const int minimumWhenOffTheBottom,
                                                            const int minimumWhenOffTheRight) throw()
{
    minOffTop = minimumWhenOffTheTop;
    minOffLeft = minimumWhenOffTheLeft;
    minOffBottom = minimumWhenOffTheBottom;
    minOffRight = minimumWhenOffTheRight;
}

void ComponentBoundsConstrainer::setFixedAspectRatio (const double widthOverHeight) throw()
{
    aspectRatio = jmax (0.0, widthOverHeight);
}

double ComponentBoundsConstrainer::getFixedAspectRatio() const throw()
{
    return aspectRatio;
}

void ComponentBoundsConstrainer::setBoundsForComponent (Component* const component,
                                                        const Rectangle<int>& targetBounds,
                                                        const bool isStretchingTop,
                                                        const bool isStretchingLeft,
                                                        const bool isStretchingBottom,
                                                        const bool isStretchingRight)
{
    jassert (component != 0);

    Rectangle<int> limits, bounds (targetBounds);
    BorderSize border;

    Component* const parent = component->getParentComponent();

    if (parent == 0)
    {
        ComponentPeer* peer = component->getPeer();
        if (peer != 0)
            border = peer->getFrameSize();

        limits = Desktop::getInstance().getMonitorAreaContaining (bounds.getCentre());
    }
    else
    {
        limits.setSize (parent->getWidth(), parent->getHeight());
    }

    border.addTo (bounds);

    checkBounds (bounds,
                 border.addedTo (component->getBounds()), limits,
                 isStretchingTop, isStretchingLeft,
                 isStretchingBottom, isStretchingRight);

    border.subtractFrom (bounds);

    applyBoundsToComponent (component, bounds);
}

void ComponentBoundsConstrainer::checkComponentBounds (Component* component)
{
    setBoundsForComponent (component, component->getBounds(),
                           false, false, false, false);
}

void ComponentBoundsConstrainer::applyBoundsToComponent (Component* component,
                                                         const Rectangle<int>& bounds)
{
    component->setBounds (bounds);
}

//==============================================================================
void ComponentBoundsConstrainer::resizeStart()
{
}

void ComponentBoundsConstrainer::resizeEnd()
{
}

//==============================================================================
void ComponentBoundsConstrainer::checkBounds (Rectangle<int>& bounds,
                                              const Rectangle<int>& old,
                                              const Rectangle<int>& limits,
                                              const bool isStretchingTop,
                                              const bool isStretchingLeft,
                                              const bool isStretchingBottom,
                                              const bool isStretchingRight)
{
    // constrain the size if it's being stretched..
    if (isStretchingLeft)
        bounds.setLeft (jlimit (old.getRight() - maxW, old.getRight() - minW, bounds.getX()));

    if (isStretchingRight)
        bounds.setWidth (jlimit (minW, maxW, bounds.getWidth()));

    if (isStretchingTop)
        bounds.setTop (jlimit (old.getBottom() - maxH, old.getBottom() - minH, bounds.getY()));

    if (isStretchingBottom)
        bounds.setHeight (jlimit (minH, maxH, bounds.getHeight()));

    if (bounds.isEmpty())
        return;

    if (minOffTop > 0)
    {
        const int limit = limits.getY() + jmin (minOffTop - bounds.getHeight(), 0);

        if (bounds.getY() < limit)
        {
            if (isStretchingTop)
                bounds.setTop (limits.getY());
            else
                bounds.setY (limit);
        }
    }

    if (minOffLeft > 0)
    {
        const int limit = limits.getX() + jmin (minOffLeft - bounds.getWidth(), 0);

        if (bounds.getX() < limit)
        {
            if (isStretchingLeft)
                bounds.setLeft (limits.getX());
            else
                bounds.setX (limit);
        }
    }

    if (minOffBottom > 0)
    {
        const int limit = limits.getBottom() - jmin (minOffBottom, bounds.getHeight());

        if (bounds.getY() > limit)
        {
            if (isStretchingBottom)
                bounds.setBottom (limits.getBottom());
            else
                bounds.setY (limit);
        }
    }

    if (minOffRight > 0)
    {
        const int limit = limits.getRight() - jmin (minOffRight, bounds.getWidth());

        if (bounds.getX() > limit)
        {
            if (isStretchingRight)
                bounds.setRight (limits.getRight());
            else
                bounds.setX (limit);
        }
    }

    // constrain the aspect ratio if one has been specified..
    if (aspectRatio > 0.0)
    {
        bool adjustWidth;

        if ((isStretchingTop || isStretchingBottom) && ! (isStretchingLeft || isStretchingRight))
        {
            adjustWidth = true;
        }
        else if ((isStretchingLeft || isStretchingRight) && ! (isStretchingTop || isStretchingBottom))
        {
            adjustWidth = false;
        }
        else
        {
            const double oldRatio = (old.getHeight() > 0) ? std::abs (old.getWidth() / (double) old.getHeight()) : 0.0;
            const double newRatio = std::abs (bounds.getWidth() / (double) bounds.getHeight());

            adjustWidth = (oldRatio > newRatio);
        }

        if (adjustWidth)
        {
            bounds.setWidth (roundToInt (bounds.getHeight() * aspectRatio));

            if (bounds.getWidth() > maxW || bounds.getWidth() < minW)
            {
                bounds.setWidth (jlimit (minW, maxW, bounds.getWidth()));
                bounds.setHeight (roundToInt (bounds.getWidth() / aspectRatio));
            }
        }
        else
        {
            bounds.setHeight (roundToInt (bounds.getWidth() / aspectRatio));

            if (bounds.getHeight() > maxH || bounds.getHeight() < minH)
            {
                bounds.setHeight (jlimit (minH, maxH, bounds.getHeight()));
                bounds.setWidth (roundToInt (bounds.getHeight() * aspectRatio));
            }
        }

        if ((isStretchingTop || isStretchingBottom) && ! (isStretchingLeft || isStretchingRight))
        {
            bounds.setX (old.getX() + (old.getWidth() - bounds.getWidth()) / 2);
        }
        else if ((isStretchingLeft || isStretchingRight) && ! (isStretchingTop || isStretchingBottom))
        {
            bounds.setY (old.getY() + (old.getHeight() - bounds.getHeight()) / 2);
        }
        else
        {
            if (isStretchingLeft)
                bounds.setX (old.getRight() - bounds.getWidth());

            if (isStretchingTop)
                bounds.setY (old.getBottom() - bounds.getHeight());
        }
    }

    jassert (! bounds.isEmpty());
}


END_JUCE_NAMESPACE
