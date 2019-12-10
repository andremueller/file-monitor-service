#pragma  once

#ifndef BASIC_FILE_MONITOR_HPP_INCLUDED
#define BASIC_FILE_MONITOR_HPP_INCLUDED

#include <string>

#include <boost/asio.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/error.hpp>

namespace services
{
    template <typename Service>
        class basic_file_monitor :
            public boost::asio::basic_io_object< Service >
        {
        public:

            explicit basic_file_monitor(boost::asio::io_service& io_service) :
                boost::asio::basic_io_object< Service >(io_service)
            {
            }
            
            virtual ~basic_file_monitor()
            {                
            }
            
            bool is_registered(const std::string& filename)
            {
                return this->get_service().is_registered(this->get_implementation(), filename);
            }

            void add_file(const std::string& filename)
            {
                boost::system::error_code ec;
                this->get_service().add_file(this->get_implementation(), filename, ec);
                boost::asio::detail::throw_error(ec, "add_file");
            }
            
            void remove_file(const std::string& filename)
            {
                boost::system::error_code ec;
                this->get_service().remove_file(this->get_implementation(), filename, ec);
                boost::asio::detail::throw_error(ec, "remove_file");                
            }

            template <typename MonHandler>
            void async_monitor(BOOST_ASIO_MOVE_ARG(MonHandler) handler)
            {
                boost::system::error_code ec;
                this->get_service().async_monitor(this->get_implementation(), ec, BOOST_ASIO_MOVE_CAST(MonHandler)(handler));
                boost::asio::detail::throw_error(ec, "async_monitor");
            }
        };
}

#endif // BASIC_FILE_MONITOR_HPP_INCLUDED

