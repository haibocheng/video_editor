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

#include "../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_Timer.h"
#include "juce_MessageManager.h"
#include "juce_AsyncUpdater.h"
#include "../application/juce_Application.h"
#include "../utilities/juce_DeletedAtShutdown.h"
#include "../core/juce_Time.h"
#include "../threads/juce_Thread.h"
#include "../threads/juce_ScopedLock.h"


//==============================================================================
class InternalTimerThread  : private Thread,
                             private MessageListener,
                             private DeletedAtShutdown,
                             private AsyncUpdater
{
public:
    InternalTimerThread()
        : Thread ("Juce Timer"),
          firstTimer (0),
          callbackNeeded (0)
    {
        triggerAsyncUpdate();
    }

    ~InternalTimerThread() throw()
    {
        stopThread (4000);

        jassert (instance == this || instance == 0);
        if (instance == this)
            instance = 0;
    }

    void run()
    {
        uint32 lastTime = Time::getMillisecondCounter();
        Message::Ptr message (new Message());

        while (! threadShouldExit())
        {
            const uint32 now = Time::getMillisecondCounter();

            if (now <= lastTime)
            {
                wait (2);
                continue;
            }

            const int elapsed = now - lastTime;
            lastTime = now;

            const int timeUntilFirstTimer = getTimeUntilFirstTimer (elapsed);

            if (timeUntilFirstTimer <= 0)
            {
                /* If we managed to set the atomic boolean to true then send a message, this is needed
                   as a memory barrier so the message won't be sent before callbackNeeded is set to true,
                   but if it fails it means the message-thread changed the value from under us so at least
                   some processing is happenening and we can just loop around and try again
                */
                if (callbackNeeded.compareAndSetBool (1, 0))
                {
                    postMessage (message);

                    /* Sometimes our message can get discarded by the OS (e.g. when running as an RTAS
                       when the app has a modal loop), so this is how long to wait before assuming the
                       message has been lost and trying again.
                    */
                    const uint32 messageDeliveryTimeout = now + 2000;

                    while (callbackNeeded.get() != 0)
                    {
                        wait (4);

                        if (threadShouldExit())
                            return;

                        if (Time::getMillisecondCounter() > messageDeliveryTimeout)
                            break;
                    }
                }
            }
            else
            {
                // don't wait for too long because running this loop also helps keep the
                // Time::getApproximateMillisecondTimer value stay up-to-date
                wait (jlimit (1, 50, timeUntilFirstTimer));
            }
        }
    }

    void callTimers()
    {
        const ScopedLock sl (lock);

        while (firstTimer != 0 && firstTimer->countdownMs <= 0)
        {
            Timer* const t = firstTimer;
            t->countdownMs = t->periodMs;

            removeTimer (t);
            addTimer (t);

            const ScopedUnlock ul (lock);

            JUCE_TRY
            {
                t->timerCallback();
            }
            JUCE_CATCH_EXCEPTION
        }

        /* This is needed as a memory barrier to make sure all processing of current timers is done
           before the boolean is set. This set should never fail since if it was false in the first place,
           we wouldn't get a message (so it can't be changed from false to true from under us), and if we
           get a message then the value is true and the other thread can only  set  it to true again and
           we will get another callback to set it to false.
        */
        callbackNeeded.set (0);
    }

    void handleMessage (const Message&)
    {
        callTimers();
    }

    void callTimersSynchronously()
    {
        if (! isThreadRunning())
        {
            // (This is relied on by some plugins in cases where the MM has
            // had to restart and the async callback never started)
            cancelPendingUpdate();
            triggerAsyncUpdate();
        }

        callTimers();
    }

    static void callAnyTimersSynchronously()
    {
        if (InternalTimerThread::instance != 0)
            InternalTimerThread::instance->callTimersSynchronously();
    }

    static inline void add (Timer* const tim) throw()
    {
        if (instance == 0)
            instance = new InternalTimerThread();

        const ScopedLock sl (instance->lock);
        instance->addTimer (tim);
    }

    static inline void remove (Timer* const tim) throw()
    {
        if (instance != 0)
        {
            const ScopedLock sl (instance->lock);
            instance->removeTimer (tim);
        }
    }

    static inline void resetCounter (Timer* const tim,
                                     const int newCounter) throw()
    {
        if (instance != 0)
        {
            tim->countdownMs = newCounter;
            tim->periodMs = newCounter;

            if ((tim->next != 0 && tim->next->countdownMs < tim->countdownMs)
                 || (tim->previous != 0 && tim->previous->countdownMs > tim->countdownMs))
            {
                const ScopedLock sl (instance->lock);
                instance->removeTimer (tim);
                instance->addTimer (tim);
            }
        }
    }

private:
    friend class Timer;
    static InternalTimerThread* instance;
    static CriticalSection lock;
    Timer* volatile firstTimer;
    Atomic <int> callbackNeeded;

    //==============================================================================
    void addTimer (Timer* const t) throw()
    {
#if JUCE_DEBUG
        Timer* tt = firstTimer;

        while (tt != 0)
        {
            // trying to add a timer that's already here - shouldn't get to this point,
            // so if you get this assertion, let me know!
            jassert (tt != t);

            tt = tt->next;
        }

        jassert (t->previous == 0 && t->next == 0);
#endif

        Timer* i = firstTimer;

        if (i == 0 || i->countdownMs > t->countdownMs)
        {
            t->next = firstTimer;
            firstTimer = t;
        }
        else
        {
            while (i->next != 0 && i->next->countdownMs <= t->countdownMs)
                i = i->next;

            jassert (i != 0);

            t->next = i->next;
            t->previous = i;
            i->next = t;
        }

        if (t->next != 0)
            t->next->previous = t;

        jassert ((t->next == 0 || t->next->countdownMs >= t->countdownMs)
                  && (t->previous == 0 || t->previous->countdownMs <= t->countdownMs));

        notify();
    }

    void removeTimer (Timer* const t) throw()
    {
#if JUCE_DEBUG
        Timer* tt = firstTimer;
        bool found = false;

        while (tt != 0)
        {
            if (tt == t)
            {
                found = true;
                break;
            }

            tt = tt->next;
        }

        // trying to remove a timer that's not here - shouldn't get to this point,
        // so if you get this assertion, let me know!
        jassert (found);
#endif

        if (t->previous != 0)
        {
            jassert (firstTimer != t);
            t->previous->next = t->next;
        }
        else
        {
            jassert (firstTimer == t);
            firstTimer = t->next;
        }

        if (t->next != 0)
            t->next->previous = t->previous;

        t->next = 0;
        t->previous = 0;
    }

    int getTimeUntilFirstTimer (const int numMillisecsElapsed) const
    {
        const ScopedLock sl (lock);

        for (Timer* t = firstTimer; t != 0; t = t->next)
            t->countdownMs -= numMillisecsElapsed;

        return firstTimer != 0 ? firstTimer->countdownMs : 1000;
    }

    void handleAsyncUpdate()
    {
        startThread (7);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalTimerThread);
};

InternalTimerThread* InternalTimerThread::instance = 0;
CriticalSection InternalTimerThread::lock;

void juce_callAnyTimersSynchronously()
{
    InternalTimerThread::callAnyTimersSynchronously();
}

//==============================================================================
#if JUCE_DEBUG
static SortedSet <Timer*> activeTimers;
#endif

Timer::Timer() throw()
   : countdownMs (0),
     periodMs (0),
     previous (0),
     next (0)
{
#if JUCE_DEBUG
    activeTimers.add (this);
#endif
}

Timer::Timer (const Timer&) throw()
   : countdownMs (0),
     periodMs (0),
     previous (0),
     next (0)
{
#if JUCE_DEBUG
    activeTimers.add (this);
#endif
}

Timer::~Timer()
{
    stopTimer();

#if JUCE_DEBUG
    activeTimers.removeValue (this);
#endif
}

void Timer::startTimer (const int interval) throw()
{
    const ScopedLock sl (InternalTimerThread::lock);

#if JUCE_DEBUG
    // this isn't a valid object! Your timer might be a dangling pointer or something..
    jassert (activeTimers.contains (this));
#endif

    if (periodMs == 0)
    {
        countdownMs = interval;
        periodMs = jmax (1, interval);
        InternalTimerThread::add (this);
    }
    else
    {
        InternalTimerThread::resetCounter (this, interval);
    }
}

void Timer::stopTimer() throw()
{
    const ScopedLock sl (InternalTimerThread::lock);

#if JUCE_DEBUG
    // this isn't a valid object! Your timer might be a dangling pointer or something..
    jassert (activeTimers.contains (this));
#endif

    if (periodMs > 0)
    {
        InternalTimerThread::remove (this);
        periodMs = 0;
    }
}

END_JUCE_NAMESPACE
