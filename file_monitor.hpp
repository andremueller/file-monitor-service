#pragma  once

#ifndef FILE_MONITOR_HPP_INCLUDED
#define FILE_MONITOR_HPP_INCLUDED

#include "basic_file_monitor.hpp"
#include "file_monitor_service.hpp"

namespace services
{
    typedef basic_file_monitor< file_monitor_service > file_monitor;
}


#endif // FILE_MONITOR_HPP_INCLUDED
