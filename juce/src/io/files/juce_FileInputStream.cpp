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

#include "../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_FileInputStream.h"


//==============================================================================
int64 juce_fileSetPosition (void* handle, int64 pos);


//==============================================================================
FileInputStream::FileInputStream (const File& f)
    : file (f),
      fileHandle (0),
      currentPosition (0),
      totalSize (0),
      needToSeek (true)
{
    openHandle();
}

FileInputStream::~FileInputStream()
{
    closeHandle();
}

//==============================================================================
int64 FileInputStream::getTotalLength()
{
    return totalSize;
}

int FileInputStream::read (void* buffer, int bytesToRead)
{
    if (needToSeek)
    {
        if (juce_fileSetPosition (fileHandle, currentPosition) < 0)
            return 0;

        needToSeek = false;
    }

    const size_t num = readInternal (buffer, bytesToRead);
    currentPosition += num;

    return (int) num;
}

bool FileInputStream::isExhausted()
{
    return currentPosition >= totalSize;
}

int64 FileInputStream::getPosition()
{
    return currentPosition;
}

bool FileInputStream::setPosition (int64 pos)
{
    pos = jlimit ((int64) 0, totalSize, pos);

    needToSeek |= (currentPosition != pos);
    currentPosition = pos;

    return true;
}

END_JUCE_NAMESPACE
