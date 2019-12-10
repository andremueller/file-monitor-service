#pragma  once

#ifndef INOTIFY_FILE_MONITOR_SERVICE_HPP_INCLUDED
#define INOTIFY_FILE_MONITOR_SERVICE_HPP_INCLUDED

#include <sys/inotify.h>
#include <errno.h>
#include <string>
#include <map>
#include <algorithm>

#include <boost/throw_exception.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/assign.hpp>
#include <boost/bimap.hpp>

#include "../file_monitor_event.hpp"

namespace services
{
    namespace detail
    {
        class file_monitor_service
        {
        public:
            class implementation_type :
                private boost::asio::detail::noncopyable
            {
                typedef boost::bimap<int, std::string> watched_files_map;
                int fd_;
                watched_files_map watched_files_;
                boost::shared_ptr<boost::asio::posix::stream_descriptor> input_;
                boost::array<char, 4096> buffer_;
                std::string buffer_str_;

                friend class file_monitor_service;
            };

            explicit file_monitor_service(boost::asio::io_service &io_service)
                : io_service_(io_service)
            {
            }

            virtual ~file_monitor_service()
            {
            }

            void shutdown_service()
            {
            }

            void construct(implementation_type &impl)
            {
                impl.fd_ = init_fd();
                impl.input_.reset(new boost::asio::posix::stream_descriptor(io_service_, impl.fd_));
            }

            void destroy(implementation_type &impl)
            {
                while (impl.watched_files_.size() > 0)
                {
                    implementation_type::watched_files_map::const_iterator iter = impl.watched_files_.begin();
                    boost::system::error_code ec;
                    remove_file(impl, iter->right, ec);
                }
                impl.input_.reset();
            }
            
            bool is_registered(implementation_type &impl, const std::string &file)
            {
                return impl.watched_files_.right.find(file) != impl.watched_files_.right.end();
            }

            void add_file(implementation_type &impl, const std::string &file, boost::system::error_code &ec)
            {
                if (is_registered(impl, file))
                {
                    BOOST_THROW_EXCEPTION(boost::system::system_error(
                        boost::system::error_code(errno, boost::system::system_category()),
                        "file_monitor_service::add_file: file is already registered '"+file+"'"));
                }
                else
                {
                    int wd = inotify_add_watch(impl.fd_, file.c_str(), IN_ALL_EVENTS);

                    if (wd == -1)
                    {
                        ec = boost::system::error_code(errno, boost::system::system_category());
                    }
                    else if (impl.watched_files_.left.find(wd) == impl.watched_files_.left.end())
                    {
                        impl.watched_files_.insert(implementation_type::watched_files_map::value_type(wd, file));
                    }
                }
            }
            
            void remove_file(implementation_type &impl, const std::string &file, boost::system::error_code &ec)
            {
                int wd = impl.watched_files_.right.at(file);
                int status = inotify_rm_watch(impl.fd_, wd);

                if (status == -1)
                {
                    ec = boost::system::error_code(errno, boost::system::system_category());
                }
                else if (impl.watched_files_.left.find(wd) != impl.watched_files_.left.end())
                {
                    impl.watched_files_.left.erase(wd);
                }
            }

            template <typename MonHandler>
            void async_monitor(implementation_type &impl, boost::system::error_code &ec, MonHandler handler)
            {
                impl.input_->async_read_some(
                    boost::asio::buffer(impl.buffer_),
                    boost::bind(
                        &file_monitor_service::handle_monitor<MonHandler>,
                        this,
                        boost::ref(impl),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        handler));
            }

        private:
            template <typename MonHandler>
            void handle_monitor(implementation_type &impl, boost::system::error_code ec, std::size_t bytes_transferred, MonHandler handler)
            {
                if (!ec)
                {
                    impl.buffer_str_.append(impl.buffer_.data(), impl.buffer_.data() + bytes_transferred);

                    while (impl.buffer_str_.size() >= sizeof(inotify_event))
                    {
                        const inotify_event *iev = reinterpret_cast<const inotify_event *>(impl.buffer_str_.data());
                        std::string fname(iev->name);
                        if (fname.size() > 0)
                        {
                            fname = "/" + fname;
                        }
                        event::event_type evt = (event::event_type)(iev->mask & IN_ALL_EVENTS);
                        io_service_.post(
                                boost::asio::detail::bind_handler(
                                    handler,
                                    ec,
                                    event(impl.watched_files_.left.at(iev->wd) + fname, evt)));
                        impl.buffer_str_.erase(0, sizeof(inotify_event) + iev->len);
                    }
                    async_monitor(impl, ec, handler);
                }
            }

            int init_fd()
            {
                int fd = inotify_init1(IN_NONBLOCK);
                if (fd == -1)
                {
                    BOOST_THROW_EXCEPTION(boost::system::system_error(
                            boost::system::error_code(errno, boost::system::system_category()),
                            "file_monitor_service::init_fd: init_inotify failed"));
                }
                return fd;
            }

            boost::asio::io_service &io_service_;
        };
    }
}


#endif // INOTIFY_FILE_MONITOR_SERVICE_HPP_INCLUDED
