#include "dataframe_parser.hpp"
#include "dataframe.hpp"

namespace websocket {

    // For details on a base framing protocol check the following link
    // http://tools.ietf.org/html/rfc6455#section-5.2

    dataframe_parser::dataframe_parser()
        : state_(fin_opcode)
    {
    }

    void dataframe_parser::reset()
    {
        state_ = fin_opcode;
    }

    boost::uint8_t dataframe_parser::get_bits(char b, boost::uint8_t offset, boost::uint8_t count)
    {
        return (b >> offset) & ((1 << count) - 1);
    }

    boost::uint16_t dataframe_parser::ntoh16(boost::uint16_t net16)
    {
        using namespace boost;
        static const int32_t num = 42;

        // Check the endianness.
        if (*reinterpret_cast<const char*>(&num) == num)
        {
            // Convert to the little-endian.
            uint16_t host16 = ((net16 & 0x00FFULL) <<  8) | ((net16 & 0xFF00ULL) >>  8);
            return host16;
        }

        return net16;
    }

    boost::uint16_t dataframe_parser::hton16(boost::uint16_t host16)
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

    boost::uint64_t dataframe_parser::ntoh64(boost::uint64_t net64)
    {
        using namespace boost;
        static const int32_t num = 42;

        // Check the endianness.
        if (*reinterpret_cast<const char*>(&num) == num)
        {
            // Convert to the little-endian.
            uint64_t host64 =
                ((net64 & 0x00000000000000FFULL) << 56) | 
                ((net64 & 0x000000000000FF00ULL) << 40) | 
                ((net64 & 0x0000000000FF0000ULL) << 24) | 
                ((net64 & 0x00000000FF000000ULL) <<  8) | 
                ((net64 & 0x000000FF00000000ULL) >>  8) | 
                ((net64 & 0x0000FF0000000000ULL) >> 24) | 
                ((net64 & 0x00FF000000000000ULL) >> 40) | 
                ((net64 & 0xFF00000000000000ULL) >> 56);

            return host64;
        }

        return net64;
    }

    boost::uint64_t dataframe_parser::hton64(boost::uint64_t host64)
    {
        using namespace boost;
        static const int32_t num = 42;

        // Check the endianness.
        if (*reinterpret_cast<const char*>(&num) == num)
        {
            // Convert to the big-endian.
            uint64_t net64 =
                ((host64 & 0x00000000000000FFULL) << 56) | 
                ((host64 & 0x000000000000FF00ULL) << 40) | 
                ((host64 & 0x0000000000FF0000ULL) << 24) | 
                ((host64 & 0x00000000FF000000ULL) <<  8) | 
                ((host64 & 0x000000FF00000000ULL) >>  8) | 
                ((host64 & 0x0000FF0000000000ULL) >> 24) | 
                ((host64 & 0x00FF000000000000ULL) >> 40) | 
                ((host64 & 0xFF00000000000000ULL) >> 56);

            return net64;
        }

        return host64;
    }

    boost::tribool dataframe_parser::consume(dataframe& frame, boost::uint8_t input)
    {
        switch (state_)
        {
        case fin_opcode:
            {
                frame.fin = get_bits(input, 7, 1) == 1;

                switch (get_bits(input, 0, 4))
                {
                case 0:
                    frame.opcode = dataframe::continuation_frame;
                    break;
                case 0x1:
                    frame.opcode = dataframe::text_frame;
                    break;
                case 0x2:
                    frame.opcode = dataframe::binary_frame;
                    break;
                case 0x8:
                    frame.opcode = dataframe::connection_close;
                    break;
                case 0x9:
                    frame.opcode = dataframe::ping;
                    break;
                case 0xA:
                    frame.opcode = dataframe::pong;
                    break;
                default:
                    frame.opcode = dataframe::reserved;
                }

                state_ = mask_payload_len;

                return boost::indeterminate;
            }
        case mask_payload_len:
            {
                frame.mask = get_bits(input, 7, 1) == 1;                    
                frame.payload_len = get_bits(input, 0, 7);

                if (frame.payload_len == 0)
                {
                    return true;
                }
                else if (frame.payload_len == 126 || frame.payload_len == 127)
                {
                    state_ = extended_payload_len1;
                }
                else
                {
                    if (frame.mask)
                    {
                        state_ = masking_key1;
                    }
                    else
                    {
                        state_ = payload;
                    }                     
                }

                return boost::indeterminate;
            }
        case extended_payload_len1:
            {
                if (frame.payload_len == 126)
                {
                    frame.extended_payload_len16 = input;
                }
                else if (frame.payload_len == 127)
                {
                    frame.extended_payload_len64 = input;
                }

                state_ = extended_payload_len2;

                return boost::indeterminate;
            }
        case extended_payload_len2:
            {
                if (frame.payload_len == 126)
                {
                    boost::uint16_t temp = input;
                    temp = temp << 8;
                    frame.extended_payload_len16 |= temp;
                    frame.extended_payload_len16 = ntoh16(frame.extended_payload_len16);

                    if (frame.mask)
                    {
                        state_ = masking_key1;
                    }
                    else
                    {
                        state_ = payload;
                    }
                }
                else if (frame.payload_len == 127)
                {
                    boost::uint64_t temp = input;
                    temp = temp << 8;
                    frame.extended_payload_len64 |= temp;

                    state_ = extended_payload_len3;
                }

                return boost::indeterminate;
            }

        case extended_payload_len3:
            {
                boost::uint64_t temp = input;
                temp = temp << 16;
                frame.extended_payload_len64 |= temp;

                state_ = extended_payload_len4;

                return boost::indeterminate;
            }

        case extended_payload_len4:
            {
                boost::uint64_t temp = input;
                temp = temp << 24;
                frame.extended_payload_len64 |= temp;

                state_ = extended_payload_len5;

                return boost::indeterminate;
            }
        case extended_payload_len5:
            {
                boost::uint64_t temp = input;
                temp = temp << 32;
                frame.extended_payload_len64 |= temp;

                state_ = extended_payload_len6;

                return boost::indeterminate;
            }
        case extended_payload_len6:
            {
                boost::uint64_t temp = input;
                temp = temp << 40;
                frame.extended_payload_len64 |= temp;

                state_ = extended_payload_len7;

                return boost::indeterminate;
            }
        case extended_payload_len7:
            {
                boost::uint64_t temp = input;
                temp = temp << 48;
                frame.extended_payload_len64 |= temp;

                state_ = extended_payload_len8;

                return boost::indeterminate;
            }
        case extended_payload_len8:
            {
                boost::uint64_t temp = input;
                temp = temp << 56;
                frame.extended_payload_len64 |= temp;
                frame.extended_payload_len64 = ntoh64(frame.extended_payload_len64);

                if (frame.mask)
                {
                    state_ = masking_key1;
                }
                else
                {
                    state_ = payload;
                }

                return boost::indeterminate;
            }
        case masking_key1:
            {
                frame.masking_key[0] = input;
                state_ = masking_key2;

                return boost::indeterminate;
            }
        case masking_key2:
            {
                frame.masking_key[1] = input;
                state_ = masking_key3;

                return boost::indeterminate;
            }
        case masking_key3:
            {
                frame.masking_key[2] = input;
                state_ = masking_key4;

                return boost::indeterminate;
            }
        case masking_key4:
            {
                frame.masking_key[3] = input;
                state_ = payload;

                return boost::indeterminate;
            }
        case payload:
            {
                boost::uint8_t mask = frame.masking_key[frame.payload.size() % 4];
                frame.payload.push_back(input ^ mask);

                if (frame.payload_len == 127)
                {
                    if (frame.payload.size() == frame.extended_payload_len64)
                        return true;
                    else
                        return boost::indeterminate;
                }
                else if (frame.payload_len == 126)
                {
                    if (frame.payload.size() == frame.extended_payload_len16)
                        return true;
                    else
                        return boost::indeterminate;
                }
                else
                {
                    if (frame.payload.size() == static_cast<std::vector<char>::size_type>(frame.payload_len))
                        return true;
                    else
                        return boost::indeterminate;
                }
            }

        default:
            return false;
        }
    }

} // namespace websocket