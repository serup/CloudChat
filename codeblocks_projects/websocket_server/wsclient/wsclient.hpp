#ifndef WSCLIENT_3f61fa9204524a8d9bc5e27ae6938a43
#define WSCLIENT_3f61fa9204524a8d9bc5e27ae6938a43

#include <string>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

namespace wsclient {

class CDED
{
public:
    int sizeofDED;
    void* pDEDarray;
    std::string status;
    void* pOwner;
};

// A structure to hold websocket frame data.
struct dataframe
{
//    dataframe();
    dataframe() : fin(true),
        opcode(text_frame), mask(false), fin_opcode(0),
        mask_payload_len(0), payload_len(0), extended_payload_len16(0), extended_payload_len64(0) {}

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

    /// Convert the dataframe into a vector of buffers.
    std::vector<boost::asio::const_buffer> to_buffers()
    {
        std::vector<boost::asio::const_buffer> buffers;


        if (fin)
        {
            fin_opcode |= 0x80;
        }

        switch (opcode)
        {
        case continuation_frame:    fin_opcode |= 0x0; break;
        case text_frame:            fin_opcode |= 0x1; break;
        case binary_frame:          fin_opcode |= 0x2; break;
        case connection_close:      fin_opcode |= 0x8; break;
        case ping:                  fin_opcode |= 0x9; break;
        case pong:                  fin_opcode |= 0xA; break;
        default:                    fin_opcode |= 0xF; break;
        }

        buffers.push_back(boost::asio::buffer(static_cast<const void*>(&fin_opcode), sizeof(fin_opcode)));

        if (payload.size() < 126)
        {
            mask_payload_len = static_cast<boost::uint8_t>(payload.size());
            buffers.push_back(boost::asio::buffer(static_cast<const void*>(&mask_payload_len), sizeof(mask_payload_len)));
            buffers.push_back(boost::asio::buffer(payload));
        }
        else if (payload.size() < 65537)
        {
            mask_payload_len = 126;
            extended_payload_len16 = static_cast<boost::uint16_t>(payload.size());
//            extended_payload_len16 = dataframe_parser::hton16(extended_payload_len16);
            extended_payload_len16 = hton16(extended_payload_len16);

            buffers.push_back(boost::asio::buffer(static_cast<const void*>(&mask_payload_len), sizeof(mask_payload_len)));
            buffers.push_back(boost::asio::buffer(static_cast<const void*>(&extended_payload_len16), sizeof(extended_payload_len16)));
            buffers.push_back(boost::asio::buffer(payload));
        }
        else
        {
            mask_payload_len = 127;
            extended_payload_len64 = static_cast<boost::uint64_t>(payload.size());
//            extended_payload_len64 = dataframe_parser::hton64(extended_payload_len64);
            extended_payload_len64 = hton64(extended_payload_len64);

            buffers.push_back(boost::asio::buffer(static_cast<const void*>(&mask_payload_len), sizeof(mask_payload_len)));
            buffers.push_back(boost::asio::buffer(static_cast<const void*>(&extended_payload_len64), sizeof(extended_payload_len64)));
            buffers.push_back(boost::asio::buffer(payload));
        }

        return buffers;
    }

    void putBinaryInPayload(unsigned char* pBinaryData,int sizeofBinaryData)
    {
            // Put DED/Binary structure in dataframe payload
            std::vector<char> vec(pBinaryData, pBinaryData+sizeofBinaryData);
            std::copy(vec.begin(), vec.end(), std::back_inserter(payload));
            //payload_len = (boost::int8_t)sizeofBinaryData;
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
                extended_payload_len64 = static_cast<boost::uint64_t>(payload.size());
                extended_payload_len64 = hton64(extended_payload_len64);
            }

        }
};

struct ClientWebSocket {
    typedef ClientWebSocket * pointer;
    typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;

    // Factories:
    static pointer create_dummy();
    static pointer from_url(std::string url);
    static pointer from_url(std::string url,void* pParent);
    static void* pOwner;

    // Interfaces:
    virtual ~ClientWebSocket() { }
    virtual bool insertDataframeIn_txbuf(dataframe frm) = 0;
    virtual void poll() = 0;
    virtual void close() = 0;
    virtual readyStateValues getReadyState() = 0;
    virtual void start(void (*pt2Function)(void* pParent)) = 0;
    virtual void join() = 0;
    virtual void processQueue() = 0;
    virtual void setOwner(void* pParent) = 0;

    template<class Callable>
    void dispatch(Callable callable) { // N.B. this is compatible with both C++11 lambdas, functors and C function pointers
        struct _Callback : public Callback {
            Callable & callable;
            _Callback(Callable & callable) : callable(callable) { }
            void operator()(void * pParent) { callable(pParent); }
        };
        _Callback callback(callable);
        _dispatch(callback);
    }

  protected:
    void (*pt2YourFunction)(void* pParent);
    struct Callback { virtual void operator()(void * pParent) = 0; };
    virtual bool _dispatch(Callback & callable) = 0;
};

} // namespace wsclient

#endif /* WSCLIENT_3f61fa9204524a8d9bc5e27ae6938a43 */
