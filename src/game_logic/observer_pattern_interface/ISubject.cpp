#include "ISubject.h"

void Core::ISubject::addObserver(const std::shared_ptr<IObserver>& observer) { _observers.insert(observer); }

void Core::ISubject::removeObserver(const std::shared_ptr<IObserver>& observer) { _observers.erase(observer); }

void Core::ISubject::notifyObservers()
{
        for (const auto& observer : _observers) {
                observer->handleEvent();
        }
}

void Core::ISubject::notifyObservers(const std::string& event)
{
        for (const auto& observer : _observers) {
                observer->handleEvent(event);
        }
}
