#ifndef WEBSOCKET_APPLICATIONS_CHAT_ROOM_HPP
#define WEBSOCKET_APPLICATIONS_CHAT_ROOM_HPP

#include <string>
#include <algorithm>
#include <deque>
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "dataframe.hpp"
#include "participant.hpp"

namespace websocket {
    namespace applications {
        namespace chat {

            typedef std::deque<dataframe> message_queue;

            struct vipparticipants
            {
                vipparticipants(participant_ptr src, std::string i) : source(src) , dfdid_(i) {}
                participant_ptr source;
                std::string dfdid_;
            };



            /// The chat room class.
            class room
            {
            public:
                /// Join the room.
                void join(participant_ptr participant);

                /// Leave the room.
                void leave(participant_ptr participant);

                /// Deliver a chat message to all participant in the room.
                void deliver(const dataframe& msg, participant_ptr source);

                /// Handle incomming dataframes - investigate dataframe and according to internal DED, deliver it to receipient
                void handleIncomming(const dataframe& msg, participant_ptr source);

                /// Finds the participant based on function name, which usually is the DFD number
                participant_ptr findvipparticipant(std::string& strFunctionName);


            private:
                /// handle dataframe to be delivered from incomming message
                //dataframe frmResponse;

                /// Deliver a message to all participant in the room.
                void deliver(const dataframe& msg);

                /// Deliver a  message to participant in the room
                void deliverTo(const dataframe& msg, participant_ptr source);

                /// Update a number of connected participants.
                void update_participants();

                std::set<participant_ptr> participants_;
                enum { max_recent_msgs = 100 };
                message_queue recent_msgs_;

                std::vector<vipparticipants*> vipparticipants_;

                bool removevipparticipant(std::string& strFunctionName);
                bool removevipparticipant(participant_ptr source);
           };

        } // chat
    } // namespace applications
} // namespace websocket

#endif // WEBSOCKET_APPLICATIONS_CHAT_ROOM_HPP
