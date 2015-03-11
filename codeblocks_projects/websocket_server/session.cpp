#include "session.hpp"
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
// http://en.wikipedia.org/wiki/Proactor_pattern

#include "../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"

/// how to remove unwanted CRLF
// sed -i 's/\r\n/\n/' session.cpp
namespace websocket {
    namespace applications {
        namespace chat {

            session::session(boost::asio::io_service& io_service, room& room)
                : state_(undefined_state), strand_(io_service),
                socket_(io_service), room_(room)
            {
            }

            boost::asio::ip::tcp::socket& session::socket()
            {
                return socket_;
            }

            void session::start()
            {
                state_ = reading_handshake;
                socket_.async_read_some(boost::asio::buffer(buff_),
                    strand_.wrap(
                    boost::bind(&session::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)));
            }

            char* session::bufferToCString(char *buff, int buffSize, char *str)
            {
                memset(str, '\0', buffSize + 1);
                return(strncpy(str, buff, buffSize));
            }


            std::string& session::bufferToString(char* buffer, int bufflen, std::string& str)
            {
                char temp[bufflen];

                memset(temp, '\0', bufflen + 1);
                strncpy(temp, buffer, bufflen);

                return(str.assign(temp));
            }


            void session::handle_read(const boost::system::error_code& error,
                std::size_t bytes_transferred)
            {
                if (!error)
                {
                    if (state_ == reading_handshake)
                    {
                        boost::tribool result;
                        boost::tie(result, boost::tuples::ignore) = req_parser_.parse(
                            req_, buff_.data(), buff_.data() + bytes_transferred);


                        std::vector<unsigned char> vecchallenge(buff_.data()+bytes_transferred-8, buff_.data()+bytes_transferred);

						req_.challenge="found";
						req_.vecchallenge=vecchallenge;

                        if (result)
                        {
                            http::request_handler::handle_request(req_, reply_);

                            state_ = writing_handshake;
                            boost::asio::async_write(socket_, reply_.to_buffers(),
                                strand_.wrap(
                                boost::bind(&session::handle_write, shared_from_this(),
                                boost::asio::placeholders::error)));
                        }
                        else if (!result)
                        {
                            state_ = bad_request;
                            reply_ = http::reply::stock_reply(http::reply::bad_request);
                            boost::asio::async_write(socket_, reply_.to_buffers(),
                                strand_.wrap(
                                boost::bind(&session::handle_write, shared_from_this(),
                                boost::asio::placeholders::error)));
                        }
                        else
                        {
                            socket_.async_read_some(boost::asio::buffer(buff_),
                                strand_.wrap(
                                boost::bind(&session::handle_read, shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));
                        }
                    }
                    else if (state_ == pumping_messages)
                    {
                        dataframe empty_dataframe;
                        boost::tribool result;
                        boost::tie(result, boost::tuples::ignore) = frame_parser_.parse(
                            read_frame_, buff_.data(), buff_.data() + bytes_transferred);

                        if (result)
                        {
                            if (read_frame_.opcode == dataframe::text_frame)
                            {
                                std::string strPayload = "";
                                std::string strType = "";
                                std::string strDED = "";
                                dataframe frm = read_frame_;
                                std::copy(frm.payload.begin(), frm.payload.end(), std::back_inserter(strPayload));
                                std::copy(strPayload.begin(), strPayload.begin()+5,std::back_inserter(strType));
                                if(strType == "DED:[") { /// deprecated, since javascript can send binary dataframe if data is in Uint8Array
                                    std::copy(strPayload.begin()+5, strPayload.end(),std::back_inserter(strDED));
                                    dataframe frame;
                                    frame.opcode = dataframe::binary_frame;
                                    std::vector<unsigned char> vecdata(strDED.begin(), strDED.end());
                                    /// TODO: MAKE SURE IT IS BINARY - so far it is in ascii -- so it does NOT work when it comes from javascript -- find a way!!!!
                                    frame.putBinaryInPayload(&vecdata[0],vecdata.size()); // Put DED structure in dataframe payload


                                    /// handle a text dataframe, in this context a text dataframe is a dataframe with a DED object in its payload
                                    room_.handleIncomming(frame, shared_from_this());


                                }


                                strPayload = "";
                                strType = "";
                                strDED = "";
                                read_frame_.payload.clear();
                                frame_parser_.reset();

                                socket_.async_read_some(boost::asio::buffer(buff_),
                                    strand_.wrap(
                                    boost::bind(&session::handle_read, shared_from_this(),
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)));
                            }
                            else if (read_frame_.opcode == dataframe::pong ||
                                read_frame_.opcode == dataframe::ping)
                            {
                                read_frame_.payload.clear();
                                frame_parser_.reset();

                                socket_.async_read_some(boost::asio::buffer(buff_),
                                    strand_.wrap(
                                    boost::bind(&session::handle_read, shared_from_this(),
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)));
                            }
                            else if (read_frame_.opcode == dataframe::binary_frame)
                            {
                                /// handle a binary dataframe, in this context a binary dataframe is a dataframe with a DED object in its payload
                                room_.handleIncomming(read_frame_, shared_from_this());


                                read_frame_.payload.clear();
                                //read_frame_ = empty_dataframe;
                                frame_parser_.reset();

                                socket_.async_read_some(boost::asio::buffer(buff_),
                                    strand_.wrap(
                                    boost::bind(&session::handle_read, shared_from_this(),
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)));
                            }
                            else
                            {
                                room_.leave(shared_from_this());
                            }
                        }
                        else if (!result)
                        {
                            room_.leave(shared_from_this());
                        }
                        else
                        {
                            socket_.async_read_some(boost::asio::buffer(buff_),
                                strand_.wrap(
                                boost::bind(&session::handle_read, shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));
                        }
                    }
                }
                else
                {
                    if (error != boost::asio::error::eof)
                        std::cerr << error.message() << std::endl;

                    room_.leave(shared_from_this());
                }

                // If an error occurs then no new asynchronous operations are started. This
                // means that all shared_ptr references to the connection object will
                // disappear and the object will be destroyed automatically after this
                // handler returns. The connection class's destructor closes the socket.
            }

            void session::handle_write(const boost::system::error_code& error)
            {
                if (!error)
                {
                    if (state_ == writing_handshake)
                    {
                        state_ = pumping_messages;

                        room_.join(shared_from_this());

                        socket_.async_read_some(boost::asio::buffer(buff_),
                                    strand_.wrap(
                                    boost::bind(&session::handle_read, shared_from_this(),
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)));
                    }
                    else if (state_ == pumping_messages)
                    {
                        if (!write_msgs_.empty())
                        {
                            write_msgs_.pop_front();
                            if (!write_msgs_.empty())
                            {
                                boost::asio::async_write(socket_,
                                    write_msgs_.front().to_buffers(),
                                    strand_.wrap(boost::bind(&session::handle_write, shared_from_this(),
                                    boost::asio::placeholders::error)));
                            }
                        }
                    }
                }
                else
                {
                    if (error != boost::asio::error::eof)
                        std::cerr << error.message() << std::endl;

                    room_.leave(shared_from_this());
                }
            }

            void session::deliver(dataframe msg)
            {
                bool write_in_progress = !write_msgs_.empty();
                //write_msgs_.clear(); // do not send more than one dataframe
                write_msgs_.push_back(msg);
                if (!write_in_progress)
                {
                    boost::asio::async_write(socket_,
                        write_msgs_.front().to_buffers(),
                        strand_.wrap(boost::bind(&session::handle_write, shared_from_this(),
                        boost::asio::placeholders::error)));
                }
            }

            std::string session::get_id()
            {
                return socket_.is_open() ? "user" + boost::lexical_cast<std::string>(socket_.remote_endpoint().port()) : "user0";
            }

        } // namespace chat
    } // namespace applications
} // namespace websocket
