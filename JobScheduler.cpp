#include <iostream>
#include "JobScheduler.h"
using namespace std;
void JobScheduler::initTI(thread_Info *pInfo, queue<Job *> *pQueue, pthread_mutex_t *queueMutex,
                          pthread_cond_t *emptyCheckMutex, volatile uint8_t *currJobNum, volatile bool *doneCheck,
                          pthread_cond_t *finishedCheck) {
    pInfo->jobQueue = pQueue;
    pInfo->queueLock = queueMutex;
    pInfo->queueCond = emptyCheckMutex;
    pInfo->jobsRunning = currJobNum;
    pInfo->jobsHaveFinished = finishedCheck;
    pInfo->threadExit = doneCheck;
}


JobScheduler::JobScheduler(uint8_t threadNumO) {
    threadNum = threadNumO;
    currJobsRunning = 0;
    done = false;

    if(pthread_mutex_init(&queueMutex, nullptr) != 0){
        cout <<"pthread_mutex_t_init failed\n";
    }
    if(pthread_cond_init(&isNotEmpty, nullptr) != 0){
        cout << "pthread_cond_init failed\n";
    }
    if(pthread_cond_init(&jobsHaveFinished, nullptr) != 0){
        cout<< "pthread_cond_init failed\n";
    }

    threadInfo = new ThreadInfo;
    initTI(threadInfo, &jobQueue, &queueMutex,&isNotEmpty,&currJobsRunning, &done, &jobsHaveFinished);
    threads = new pthread_t[threadNum];
    for (int i = 0; i < threadNum; i++) {
        if(pthread_create(&threads[i], nullptr, thread_start, (void *) threadInfo) != 0){cout << "pthread_create failed\n"; threads[i] = 0;}
    }
}

JobScheduler::~JobScheduler() {
    waitUntilAllJobsHaveFinished();
    delete threadInfo;
    done = true;
    if(pthread_cond_broadcast(&isNotEmpty) != 0 ){ cout << "pthread_broadcast failed\n"; }
    for (int i = 0; i < threadNum; i++) {
        if(pthread_join(threads[i], nullptr) != 0 ){ cout << "pthread_join failed\n"; }
    }
    delete[] threads;
    if(pthread_mutex_destroy(&queueMutex) != 0 ){ cout << "pthread_mutex_destroy failed\n"; }
    if(pthread_cond_destroy(&isNotEmpty) != 0 ){ cout << "pthread_cond_destroy failed\n"; }
    if(pthread_cond_destroy(&jobsHaveFinished) != 0 ){ cout << "pthread_cond_destroy failed\n"; }
}

void JobScheduler::schedule(Job *job) {
    if(pthread_mutex_lock(&queueMutex) != 0 ){ cout << "pthread_mutex_lock failed\n"; }
    jobQueue.push(job);
    if(pthread_cond_signal(&isNotEmpty) != 0 ){ cout << "pthread_cond_signal failed\n"; }
    if(pthread_mutex_unlock(&queueMutex) != 0 ){ cout << "pthread_mutex_unlock failed\n"; }
}


void JobScheduler::waitUntilAllJobsHaveFinished() {
    if(pthread_mutex_lock(&queueMutex) != 0 ){ cout << "pthread_mutex_lock failed\n"; }
    while (!jobQueue.empty() || currJobsRunning > 0){
        if((pthread_cond_wait(&jobsHaveFinished, &queueMutex)) != 0) { cout << "pthread_cond_wait failed\n"; }
    }
    if(pthread_mutex_unlock(&queueMutex) != 0 ){ cout << "pthread_mutex_unlock failed\n"; }
}


void *thread_start(void *args) {
    struct thread_Info *argptr = ((struct thread_Info *) args);
    queue<Job *> *job_queue = argptr->jobQueue;
    pthread_mutex_t *queue_lock = argptr->queueLock;
    pthread_cond_t *queue_cond = argptr->queueCond;
    volatile uint8_t *jobs_running_ptr = argptr->jobsRunning;
    volatile bool *threads_must_exit_ptr = argptr->threadExit;
    pthread_cond_t *jobs_finished_cond_ptr = argptr->jobsHaveFinished;

    while (!(*threads_must_exit_ptr)) {
        Job *job;

        if(pthread_mutex_lock(queue_lock) != 0 ){
            cout << "pthread_mutex_lock failed";
            continue;
        }
        while (job_queue->empty() && !(*threads_must_exit_ptr)){
            if(pthread_cond_wait(queue_cond, queue_lock)!=0) {
                cout<< "pthread_cond_wait failed\n";
            }
        }
        if (*threads_must_exit_ptr){
            if(pthread_mutex_unlock(queue_lock) != 0 ){ cout << "pthread_mutex_unlock failed\n"; }
            break;
        }
        job = job_queue->front();
        job_queue->pop();
        (*jobs_running_ptr)++;
        if(pthread_mutex_unlock(queue_lock) != 0 ){ cout << "pthread_mutex_unlock failed\n"; }

        job->run();
        delete job;

        if(pthread_mutex_lock(queue_lock) != 0 ){ cout << "pthread_mutex_lock failed\n"; }
        (*jobs_running_ptr)--;

        if(pthread_cond_broadcast(jobs_finished_cond_ptr) != 0 ){ cout << "pthread_cond_signal failed\n"; }
        if(pthread_mutex_unlock(queue_lock) != 0 ){ cout << "pthread_mutex_unlock failed\n"; }
    }
    pthread_exit((void *) nullptr);
}
