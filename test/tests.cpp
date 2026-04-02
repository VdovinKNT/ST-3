// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;

extern Timer* g_pTimer;
void enableTestMode(bool);

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTest, ConstructorSetsTimeout) {
    TimedDoor door(5);
    EXPECT_EQ(door.getTimeOut(), 5);
}

TEST(TimedDoorTest, InitiallyClosed) {
    TimedDoor door(10);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(3);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(2);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    TimedDoor door(42);
    EXPECT_EQ(door.getTimeOut(), 42);
}

TEST(TimedDoorTest, ThrowStateThrowsRuntimeError) {
    TimedDoor door(1);
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsWhenDoorOpen) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutDoesNotThrowWhenDoorClosed) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapterTest, TimeoutDoesNothingWhenDoorClosed) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, TregisterCallsTimeoutAfterDelay) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(0, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(TimerTest, TregisterDoesNotCallTimeoutBeforeDelay) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(0);
    timer.tregister(1, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(IntegrationTest, LockPreventsExceptionOnTimeout) {
    enableTestMode(true);
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
    enableTestMode(false);
}
