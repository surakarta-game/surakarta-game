#pragma once
#include "surakarta.h"

/// @brief A high performance implementation of Surakarta animation.
class SurakartaAnimation {
   public:
    struct Point {
        double x;
        double y;
    };

    SurakartaAnimation();
    SurakartaAnimation(
        const std::vector<SurakartaMovePathFragment>& trace,
        int total_milliseconds = 1000);
    SurakartaAnimation(SurakartaAnimation&&);
    SurakartaAnimation& operator=(SurakartaAnimation&&);
    ~SurakartaAnimation();

    // Each call should have milliseconds no less than the previous call.
    Point PositionAt(int milliseconds);

    bool Finished() const {
        return index_ == total_;
    }

    bool Empty() const {
        return total_ == 0;
    }

   private:
    int total_;
    SurakartaMovePathFragment* trace_;
    int* length_;
    int total_length_;  // ideal * 1000
    int* time_;
    int* start_time_;
    int index_;
    bool delta_cached;
    double dx, dy, dtheta;
};
