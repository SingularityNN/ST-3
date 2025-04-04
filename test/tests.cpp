// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <thread>

using ::testing::_;
using ::testing::Mock;
using ::testing::InSequence;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    MockTimerClient* mockClient;
    void SetUp() override {
        door = new TimedDoor(5);
        mockClient = new MockTimerClient();
    }
    void TearDown() override {
        delete door;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutWhenOpenThrowsException) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutWhenClosedDoesNotThrow) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenOpen) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenClosed) {
    TimedDoor door(5);
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}

TEST_F(TimedDoorTest, TimeoutAfterLockDoesNotThrow) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, TestTimeoutAfterOpening) {
    door->unlock();
    EXPECT_CALL(*mockClient, Timeout())
        .Times(1);
    Timer timer;
    timer.tregister(5, mockClient);
}
