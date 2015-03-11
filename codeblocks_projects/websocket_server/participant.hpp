#ifndef WEBSOCKET_APPLICATIONS_CHAT_PARTICIPANT_HPP
#define WEBSOCKET_APPLICATIONS_CHAT_PARTICIPANT_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include "dataframe.hpp"

namespace websocket {
    namespace applications {
        namespace chat {

            class participant
            {
            public:
                virtual ~participant() {}
                virtual void deliver(dataframe msg) = 0;
                virtual std::string get_id() = 0;
            };

            typedef boost::shared_ptr<participant> participant_ptr;
            
        } // chat
    } // namespace applications
} // namespace websocket

#endif // WEBSOCKET_APPLICATIONS_CHAT_PARTICIPANT_HPP