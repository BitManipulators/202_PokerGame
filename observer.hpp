#pragma once

class Observer {
    public:
        virtual ~Observer() = default;
        virtual void onGameStateChanged() = 0;
};
