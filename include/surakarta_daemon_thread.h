#pragma once

#include <QThread>
#include "surakarta.h"

class SurakartaDaemonThread : public QThread {
    Q_OBJECT

   public:
    SurakartaDaemonThread(std::unique_ptr<SurakartaDaemon> daemon)
        : daemon_(std::move(daemon)) {}

   protected:
    void run() override { daemon_->Execute(); }
    std::unique_ptr<SurakartaDaemon> daemon_;
};
