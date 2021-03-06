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

#ifndef __JUCE_THREADPOOL_JUCEHEADER__
#define __JUCE_THREADPOOL_JUCEHEADER__

#include "juce_Thread.h"
#include "juce_ScopedLock.h"
#include "../text/juce_StringArray.h"
#include "../containers/juce_Array.h"
#include "../containers/juce_OwnedArray.h"
class ThreadPool;
class ThreadPoolThread;


//==============================================================================
/**
    A task that is executed by a ThreadPool object.

    A ThreadPool keeps a list of ThreadPoolJob objects which are executed by
    its threads.

    The runJob() method needs to be implemented to do the task, and if the code that
    does the work takes a significant time to run, it must keep checking the shouldExit()
    method to see if something is trying to interrupt the job. If shouldExit() returns
    true, the runJob() method must return immediately.

    @see ThreadPool, Thread
*/
class JUCE_API  ThreadPoolJob
{
public:
    //==============================================================================
    /** Creates a thread pool job object.

        After creating your job, add it to a thread pool with ThreadPool::addJob().
    */
    explicit ThreadPoolJob (const String& name);

    /** Destructor. */
    virtual ~ThreadPoolJob();

    //==============================================================================
    /** Returns the name of this job.
        @see setJobName
    */
    const String getJobName() const;

    /** Changes the job's name.
        @see getJobName
    */
    void setJobName (const String& newName);

    //==============================================================================
    /** These are the values that can be returned by the runJob() method.
    */
    enum JobStatus
    {
        jobHasFinished = 0,     /**< indicates that the job has finished and can be
                                     removed from the pool. */

        jobHasFinishedAndShouldBeDeleted,  /**< indicates that the job has finished and that it
                                                should be automatically deleted by the pool. */

        jobNeedsRunningAgain    /**< indicates that the job would like to be called
                                     again when a thread is free. */
    };

    /** Peforms the actual work that this job needs to do.

        Your subclass must implement this method, in which is does its work.

        If the code in this method takes a significant time to run, it must repeatedly check
        the shouldExit() method to see if something is trying to interrupt the job.
        If shouldExit() ever returns true, the runJob() method must return immediately.

        If this method returns jobHasFinished, then the job will be removed from the pool
        immediately. If it returns jobNeedsRunningAgain, then the job will be left in the
        pool and will get a chance to run again as soon as a thread is free.

        @see shouldExit()
    */
    virtual JobStatus runJob() = 0;


    //==============================================================================
    /** Returns true if this job is currently running its runJob() method. */
    bool isRunning() const                  { return isActive; }

    /** Returns true if something is trying to interrupt this job and make it stop.

        Your runJob() method must call this whenever it gets a chance, and if it ever
        returns true, the runJob() method must return immediately.

        @see signalJobShouldExit()
    */
    bool shouldExit() const                 { return shouldStop; }

    /** Calling this will cause the shouldExit() method to return true, and the job
        should (if it's been implemented correctly) stop as soon as possible.

        @see shouldExit()
    */
    void signalJobShouldExit();

    //==============================================================================
private:
    friend class ThreadPool;
    friend class ThreadPoolThread;
    String jobName;
    ThreadPool* pool;
    bool shouldStop, isActive, shouldBeDeleted;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreadPoolJob);
};


//==============================================================================
/**
    A set of threads that will run a list of jobs.

    When a ThreadPoolJob object is added to the ThreadPool's list, its run() method
    will be called by the next pooled thread that becomes free.

    @see ThreadPoolJob, Thread
*/
class JUCE_API  ThreadPool
{
public:
    //==============================================================================
    /** Creates a thread pool.

        Once you've created a pool, you can give it some things to do with the addJob()
        method.

        @param numberOfThreads              the maximum number of actual threads to run.
        @param startThreadsOnlyWhenNeeded   if this is true, then no threads will be started
                                            until there are some jobs to run. If false, then
                                            all the threads will be fired-up immediately so that
                                            they're ready for action
        @param stopThreadsWhenNotUsedTimeoutMs  if this timeout is > 0, then if any threads have been
                                            inactive for this length of time, they will automatically
                                            be stopped until more jobs come along and they're needed
    */
    ThreadPool (int numberOfThreads,
                bool startThreadsOnlyWhenNeeded = true,
                int stopThreadsWhenNotUsedTimeoutMs = 5000);

    /** Destructor.

        This will attempt to remove all the jobs before deleting, but if you want to
        specify a timeout, you should call removeAllJobs() explicitly before deleting
        the pool.
    */
    ~ThreadPool();

    //==============================================================================
    /** A callback class used when you need to select which ThreadPoolJob objects are suitable
        for some kind of operation.
        @see ThreadPool::removeAllJobs
    */
    class JUCE_API  JobSelector
    {
    public:
        virtual ~JobSelector() {}

        /** Should return true if the specified thread matches your criteria for whatever
            operation that this object is being used for.

            Any implementation of this method must be extremely fast and thread-safe!
        */
        virtual bool isJobSuitable (ThreadPoolJob* job) = 0;
    };

    //==============================================================================
    /** Adds a job to the queue.

        Once a job has been added, then the next time a thread is free, it will run
        the job's ThreadPoolJob::runJob() method. Depending on the return value of the
        runJob() method, the pool will either remove the job from the pool or add it to
        the back of the queue to be run again.
    */
    void addJob (ThreadPoolJob* job);

    /** Tries to remove a job from the pool.

        If the job isn't yet running, this will simply remove it. If it is running, it
        will wait for it to finish.

        If the timeout period expires before the job finishes running, then the job will be
        left in the pool and this will return false. It returns true if the job is sucessfully
        stopped and removed.

        @param job                  the job to remove
        @param interruptIfRunning   if true, then if the job is currently busy, its
                                    ThreadPoolJob::signalJobShouldExit() method will be called to try
                                    to interrupt it. If false, then if the job will be allowed to run
                                    until it stops normally (or the timeout expires)
        @param timeOutMilliseconds  the length of time this method should wait for the job to finish
                                    before giving up and returning false
    */
    bool removeJob (ThreadPoolJob* job,
                    bool interruptIfRunning,
                    int timeOutMilliseconds);

    /** Tries to remove all jobs from the pool.

        @param interruptRunningJobs if true, then all running jobs will have their ThreadPoolJob::signalJobShouldExit()
                                    methods called to try to interrupt them
        @param timeOutMilliseconds  the length of time this method should wait for all the jobs to finish
                                    before giving up and returning false
        @param deleteInactiveJobs   if true, any jobs that aren't currently running will be deleted. If false,
                                    they will simply be removed from the pool. Jobs that are already running when
                                    this method is called can choose whether they should be deleted by
                                    returning jobHasFinishedAndShouldBeDeleted from their runJob() method.
        @param selectedJobsToRemove if this is non-zero, the JobSelector object is asked to decide which
                                    jobs should be removed. If it is zero, all jobs are removed
        @returns    true if all jobs are successfully stopped and removed; false if the timeout period
                    expires while waiting for one or more jobs to stop
    */
    bool removeAllJobs (bool interruptRunningJobs,
                        int timeOutMilliseconds,
                        bool deleteInactiveJobs = false,
                        JobSelector* selectedJobsToRemove = 0);

    /** Returns the number of jobs currently running or queued.
    */
    int getNumJobs() const;

    /** Returns one of the jobs in the queue.

        Note that this can be a very volatile list as jobs might be continuously getting shifted
        around in the list, and this method may return 0 if the index is currently out-of-range.
    */
    ThreadPoolJob* getJob (int index) const;

    /** Returns true if the given job is currently queued or running.

        @see isJobRunning()
    */
    bool contains (const ThreadPoolJob* job) const;

    /** Returns true if the given job is currently being run by a thread.
    */
    bool isJobRunning (const ThreadPoolJob* job) const;

    /** Waits until a job has finished running and has been removed from the pool.

        This will wait until the job is no longer in the pool - i.e. until its
        runJob() method returns ThreadPoolJob::jobHasFinished.

        If the timeout period expires before the job finishes, this will return false;
        it returns true if the job has finished successfully.
    */
    bool waitForJobToFinish (const ThreadPoolJob* job,
                             int timeOutMilliseconds) const;

    /** Returns a list of the names of all the jobs currently running or queued.

        If onlyReturnActiveJobs is true, only the ones currently running are returned.
    */
    const StringArray getNamesOfAllJobs (bool onlyReturnActiveJobs) const;

    /** Changes the priority of all the threads.

        This will call Thread::setPriority() for each thread in the pool.
        May return false if for some reason the priority can't be changed.
    */
    bool setThreadPriorities (int newPriority);


private:
    //==============================================================================
    const int threadStopTimeout;
    int priority;
    class ThreadPoolThread;
    friend class OwnedArray <ThreadPoolThread>;
    OwnedArray <ThreadPoolThread> threads;
    Array <ThreadPoolJob*> jobs;

    CriticalSection lock;
    uint32 lastJobEndTime;
    WaitableEvent jobFinishedSignal;

    friend class ThreadPoolThread;
    bool runNextJob();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreadPool);
};


#endif   // __JUCE_THREADPOOL_JUCEHEADER__
