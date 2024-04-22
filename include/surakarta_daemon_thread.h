#pragma once

#include <QThread>
#include "surakarta.h"

class SurakartaDaemonThread : public QThread {
    Q_OBJECT

   public:
    SurakartaDaemonThread(std::unique_ptr<SurakartaDaemon> daemon)
        : daemon_(std::move(daemon)) {}

    class AgentFactoryApplicator {
       public:
        std::unique_ptr<SurakartaAgentBase> CreateAgentFromFactory(
            SurakartaDaemon::AgentFactory& factory,
            PieceColor color) {
            return daemon_->CreateTemporaryGameAgent(factory, color);
        }

       private:
        AgentFactoryApplicator(std::shared_ptr<SurakartaDaemon> daemon)
            : daemon_(daemon) {}

        std::shared_ptr<SurakartaDaemon> daemon_;
        friend class SurakartaDaemonThread;
    };

    std::shared_ptr<AgentFactoryApplicator> CreateAgentFactoryApplicator() {
        auto applicator_ptr = new AgentFactoryApplicator(daemon_);
        auto applicator = std::shared_ptr<AgentFactoryApplicator>(applicator_ptr);
        applicators_.push_back(applicator);
        return applicator;
    }

   protected:
    void run() override {
        daemon_->Execute();
    }

   private:
    std::shared_ptr<SurakartaDaemon> daemon_;
    std::vector<std::shared_ptr<AgentFactoryApplicator>> applicators_;
    friend class AgentFactoryApplicator;
};
