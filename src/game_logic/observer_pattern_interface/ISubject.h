#ifndef UABA2_AP_PROJECT_ISUBJECT_H
#define UABA2_AP_PROJECT_ISUBJECT_H

#include "IObserver.h"
#include <memory>
#include <set>

namespace Core {
class ISubject
{
private:
        std::set<std::shared_ptr<Core::IObserver>> _observers;

public:
        void addObserver(const std::shared_ptr<Core::IObserver>& observer);

        void removeObserver(const std::shared_ptr<Core::IObserver>& observer);

        virtual void notifyObservers();

        virtual void notifyObservers(const std::string& event);
};
} // namespace Core
#endif // UABA2_AP_PROJECT_ISUBJECT_H
