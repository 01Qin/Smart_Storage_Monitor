//
// Created by Quinn on 8/18/2026.
//

#ifndef MONITOR_PICO_COUNRDOWN_H
#define MONITOR_PICO_COUNRDOWN_H

#include "pico/time.h"


class Countdown
{
public:
    Countdown();
    explicit Countdown(int ms);
    bool expired();
    void countdown_ms(int ms);
    void countdown(int seconds);
    int left_ms();
private:
    absolute_time_t target_time;
};

#endif //MONITOR_PICO_COUNRDOWN_H
