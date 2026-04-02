// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

static Timer* g_pTimer = nullptr;

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    if (g_pTimer == nullptr) {
        g_pTimer = new Timer();
    }
    g_pTimer->tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is still open after timeout");
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
    std::thread([this, timeout, client]() {
        sleep(timeout);
        client->Timeout();
        }).detach();
}
