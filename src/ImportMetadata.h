#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#define METADATA_GENERATION_N_THREADS 1
#else //macos

#define METADATA_GENERATION_N_THREADS 1
#endif

#include "ofMain.h"
#include "ofApp.h"
#include <atomic>
#include "ofAppData.h"
using namespace cv;
using namespace ofxCv;

// This is a simple example of a ThreadedObject created by extending ofThread.
// It contains data (count) that will be accessed from within and outside the
// thread and demonstrates several of the data protection mechanisms (aka
// mutexes).
class ImportMetadata : public ofThread {
    
    public:
        // On destruction wait for the thread to finish
        // so we don't destroy the pixels while they are
        // being used. Otherwise we would crash
        ~ImportMetadata() {
            stop();
            waitForThread(false);
        }

        ofAppData* appData;

        void setup(ofAppData* appData) {
            this->appData = appData;
            start();
        }

        // Start the thread.
        void start() {
            startThread();
        }

        // Signal the thread to stop.  After calling this method,
        // isThreadRunning() will return false and the while loop will stop
        // next time it has the chance to.
        // In order for the thread to actually go out of the while loop
        // we need to notify the condition, otherwise the thread will
        // sleep there forever.
        // We also lock the mutex so the notify_all call only happens
        // once the thread is waiting. We lock the mutex during the
        // whole while loop but when we call condition.wait, that
        // unlocks the mutex which ensures that we'll only call
        // stop and notify here once the condition is waiting
        void stop() {
            std::unique_lock<std::mutex> lck(mutex);
            stopThread();
            condition.notify_all();
        }

    protected:
        std::condition_variable condition;
};
