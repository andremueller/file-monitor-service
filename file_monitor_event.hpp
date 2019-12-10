#pragma  once


#ifndef FILE_MONITOR_EVENT_HPP_INCLUDED
#define FILE_MONITOR_EVENT_HPP_INCLUDED

#include <string>
#include <iosfwd>
#include <sys/inotify.h>

namespace services
{
    struct event
    {
        enum event_type
        {
            null = 0,
            access = IN_ACCESS,
            attrib = IN_ATTRIB,
            close_write = IN_CLOSE_WRITE,
            close_nowrite = IN_CLOSE_NOWRITE,
            create = IN_CREATE,
            file_delete = IN_DELETE,
            delete_self = IN_DELETE_SELF,
            modify = IN_MODIFY,
            move_self = IN_MOVE_SELF,
            moved_from = IN_MOVED_TO,
            moved_to = IN_MOVED_FROM,
            open = IN_OPEN,
        };

        std::string filename;
        event_type type;

        event (const std::string& f = "", event_type t = event::null) :
            filename(f), type(t)
        {
        }        
    };
    
    std::ostream & operator <<(std::ostream &os, event::event_type eventType);
}


#endif // FILE_MONITOR_EVENT_HPP_INCLUDED

