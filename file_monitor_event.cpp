#include <ostream>

#include "file_monitor_event.hpp"
#include "file_monitor_service.hpp"
#include "inotify/file_monitor_service.hpp"
#include <boost/lexical_cast.hpp>

namespace services
{
    std::ostream &operator <<(std::ostream &os, event::event_type eventType)
    {
        std::string buf;
        switch (eventType)
        {
        case event::null:
            buf = "null";
            break;

        case event::access:
            buf = "access";
            break;

        case event::attrib:
            buf = "attrib";
            break;

        case event::close_write:
            buf = "close_write";
            break;

        case event::close_nowrite:
            buf = "close_nowrite";
            break;

        case event::create:
            buf = "create";
            break;

        case event::file_delete:
            buf = "file_delete";
            break;

        case event::delete_self:
            buf = "delete_self";
            break;

        case event::modify:
            buf = "modify";
            break;

        case event::move_self:
            buf = "move_self";
            break;

        case event::moved_from:
            buf = "moved_from";
            break;

        case event::moved_to:
            buf = "moved_to";
            break;

        case event::open:
            buf = "open";
            break;

        default:
            buf = "unknown[" + boost::lexical_cast<std::string>((int)eventType) + "]";
        }
        os << buf;
        return os;
    }
    
    boost::asio::io_service::id file_monitor_service::id;

} // namespace services
