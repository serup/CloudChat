#ifndef WEBSOCKET_DATAFRAME_HPP
#define WEBSOCKET_DATAFRAME_HPP

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace websocket {

    // A structure to hold websocket frame data.
    struct dataframe
    {
        dataframe();

        bool fin;
        enum operation_code { continuation_frame, text_frame, binary_frame, connection_close, ping, pong, reserved } opcode;
        bool mask;
        boost::int8_t fin_opcode;
        boost::int8_t mask_payload_len;
        boost::int8_t payload_len;
        boost::uint16_t extended_payload_len16;
        boost::uint64_t extended_payload_len64;
        boost::array<boost::uint8_t, 4> masking_key;
        std::vector<boost::uint8_t> payload;

        /// helper functions
        /// Convert a uint16_t from the host byte order to the network byte order.
        boost::uint16_t hton16(boost::uint16_t host16)
        {
            using namespace boost;
            static const int32_t num = 42;

            // Check the endianness.
            if (*reinterpret_cast<const char*>(&num) == num)
            {
                // Convert to the big-endian.
                uint16_t net16 = ((host16 & 0x00FFULL) << 8) | ((host16 & 0xFF00ULL) >> 8);
                return net16;
            }

            return host16;
        }

        /// Convert a uint64_t from the host byte order to the network byte order.
        boost::uint64_t hton64(boost::uint64_t host64)
        {
            using namespace boost;
            static const int32_t num = 42;

            // Check the endianness.
            if (*reinterpret_cast<const char*>(&num) == num)
            {
                // Convert to the big-endian.
                boost::uint64_t net64 = \
                    ((host64 & 0x00000000000000FFULL) << 56) | \
                    ((host64 & 0x000000000000FF00ULL) << 40) | \
                    ((host64 & 0x0000000000FF0000ULL) << 24) | \
                    ((host64 & 0x00000000FF000000ULL) <<  8) | \
                    ((host64 & 0x000000FF00000000ULL) >>  8) | \
                    ((host64 & 0x0000FF0000000000ULL) >> 24) | \
                    ((host64 & 0x00FF000000000000ULL) >> 40) | \
                    ((host64 & 0xFF00000000000000ULL) >> 56);

                return net64;
            }

            return host64;
        }
        /// Convert the dataframe into a vector of buffers.
        std::vector<boost::asio::const_buffer> to_buffers();

        /// Put a binary block of data into payload of the dataframe
        void putBinaryInPayload(unsigned char* pBinaryData,int sizeofBinaryData)
        {
            // Put DED/Binary structure in dataframe payload
            std::vector<char> vec(pBinaryData, pBinaryData+sizeofBinaryData);
            std::copy(vec.begin(), vec.end(), std::back_inserter(payload));

            if (payload.size() < 126)
            {
                mask_payload_len = static_cast<boost::uint8_t>(payload.size());
            }
            else if (payload.size() < 65537)
            {
                mask_payload_len = 126;
                extended_payload_len16 = static_cast<boost::uint16_t>(payload.size());
                extended_payload_len16 = hton16(extended_payload_len16);
            }
            else
            {
                mask_payload_len = 127;
                extended_payload_len64 = payload.size();
                extended_payload_len64 = hton64(extended_payload_len64);
            }
        }

    };

} // namespace websocket

#endif // WEBSOCKET_DATAFRAME_HPP
