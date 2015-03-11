#ifndef WEBSOCKET_DATAFRAME_PARSER_HPP
#define WEBSOCKET_DATAFRAME_PARSER_HPP

#include <boost/cstdint.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace websocket {

        struct dataframe;

        /// Parser for incoming dataframes.
        class dataframe_parser
        {
        public:
            /// Construct ready to parse the dataframe.
            dataframe_parser();

            /// Reset to initial parser state.
            void reset();

            /// Parse some data. The tribool return value is true when a complete dataframe
            /// has been parsed, false if the data is invalid, indeterminate when more
            /// data is required. The InputIterator return value indicates how much of the
            /// input has been consumed.
            template <typename InputIterator>
            boost::tuple<boost::tribool, InputIterator> parse(dataframe& frame,
                InputIterator begin, InputIterator end)
            {
                while (begin != end)
                {
                    boost::tribool result = consume(frame, *begin++);
                    if (result || !result)
                        return boost::make_tuple(result, begin);
                }
                boost::tribool result = boost::indeterminate;
                return boost::make_tuple(result, begin);
            }

            /// Convert a uint16_t from the network byte order to the host byte order.
            static boost::uint16_t ntoh16(boost::uint16_t net16);

            /// Convert a uint16_t from the host byte order to the network byte order.
            static boost::uint16_t hton16(boost::uint16_t net16);

            /// Convert a uint64_t from the network byte order to the host byte order.
            static boost::uint64_t ntoh64(boost::uint64_t net64);

            /// Convert a uint64_t from the host byte order to the network byte order.
            static boost::uint64_t hton64(boost::uint64_t net64);

        private:
            /// Handle the next character of input.
            boost::tribool consume(dataframe& frame, boost::uint8_t input);

            /// Get a number of bits at the specified offset.
            boost::uint8_t get_bits(char b, boost::uint8_t offset, boost::uint8_t count);

            /// The current state of the parser.
            enum state
            {
                fin_opcode,
                mask_payload_len,
                extended_payload_len1,
                extended_payload_len2,
                extended_payload_len3,
                extended_payload_len4,
                extended_payload_len5,
                extended_payload_len6,
                extended_payload_len7,
                extended_payload_len8,
                masking_key1,
                masking_key2,
                masking_key3,
                masking_key4,
                payload
            } state_;
        };

} // namespace websocket

#endif // WEBSOCKET_DATAFRAME_PARSER_HPP