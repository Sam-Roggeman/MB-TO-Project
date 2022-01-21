#ifndef UABA2_AP_PROJECT_IOBSERVER_H
#define UABA2_AP_PROJECT_IOBSERVER_H

#include <string>

namespace Core {
class IObserver
{
public:
        virtual void handleEvent() = 0;
        virtual void handleEvent(const std::string& event) = 0;
};
} // namespace Core

#endif // UABA2_AP_PROJECT_IOBSERVER_H
