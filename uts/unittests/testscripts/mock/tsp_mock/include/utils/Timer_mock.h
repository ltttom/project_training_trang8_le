#ifndef TIMER_MOCK_H
#define TIMER_MOCK_H
class MockTimer {
public:
    MockTimer(){}

    MOCK_METHOD2(setDuration, void(uint32_t value, uint32_t interval));
    MOCK_METHOD2(setDurationMs, void(uint32_t value, uint32_t interval));
    MOCK_METHOD0(start, void());
    MOCK_METHOD0(restart, void());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD0(getRemainingTimeMs, uint32_t());
    MOCK_METHOD0(callbackWrapper, void());

private:
    MOCK_METHOD2(createTimer, int32_t(timer_t *timerid, TimerHandlerFunc handler_cb));
    MOCK_METHOD5(startTimer, void(const timer_t timerid, const time_t start_sec, const long start_nsec, const time_t cyclic_sec, const long cyclic_nsec));
    MOCK_METHOD1(stopTimer, void(timer_t timerid));
    MOCK_METHOD1(deleteTimer, void(timer_t timerid));
};

MockTimer* M_Timer;

Timer::Timer(TimerTimeoutHandler * timeHandler, const int32_t timerId){}

Timer::~Timer(){}

void Timer::setDurationMs(uint32_t value, uint32_t interval) {
    M_Timer->setDurationMs(value, interval);
}

void Timer::start() {
    M_Timer->start();
}

void Timer::stop() {
    M_Timer->stop();
}

void Timer::setDuration(uint32_t value, uint32_t interval) {
    M_Timer->setDuration(value, interval);
}

uint32_t Timer::getRemainingTimeMs() {
    return M_Timer->getRemainingTimeMs();
}

void Timer::callbackWrapper() {
    M_Timer->callbackWrapper();
}

#endif // TIMER_MOCK_H