//
// Created by athena on 14/1/20.
//

#ifndef TEMPPROJ_JOBSCHEDULER_H
#define TEMPPROJ_JOBSCHEDULER_H
#include <queue>
#include <pthread.h>
#include <cstdlib>


#include <queue>
#include <cstdlib>
#include <iostream>
class Job {
public:
    virtual ~Job() = default;
    virtual bool run() = 0;
};
typedef struct thread_Info {
    std::queue <Job *> *jobQueue;
    pthread_mutex_t *queueLock;
    pthread_cond_t *queueCond;
    pthread_cond_t *jobsHaveFinished;
    volatile uint8_t *jobsRunning;
    volatile bool *threadExit;
}ThreadInfo;

void *thread_start(void *args);

class JobScheduler {
    uint8_t threadNum;
    std::queue<Job *> jobQueue;
    pthread_t *threads;
    pthread_mutex_t queueMutex;
    pthread_cond_t isNotEmpty;
    pthread_cond_t jobsHaveFinished;
    volatile uint8_t currJobsRunning;
    volatile bool done;
    thread_Info *threadInfo;
public:
    explicit JobScheduler(uint8_t threadNum = 1);
    ~JobScheduler();
    void schedule(Job *job);
    bool allJobsHaveFinished();
    void waitUntilAllJobsHaveFinished();
    uint8_t getThreadNum() { return threadNum; }

    static void
    initTI(thread_Info *pInfo, std::queue<Job *> *pQueue, pthread_mutex_t *queueMutex, pthread_cond_t *emptyCheckMutex,
           volatile uint8_t *currJobNum, volatile bool *doneCheck, pthread_cond_t *finishedCheck);
};

#endif //TEMPPROJ_JOBSCHEDULER_H

