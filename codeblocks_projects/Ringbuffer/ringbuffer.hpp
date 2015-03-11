
#define BOOST_CB_DISABLE_DEBUG

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/call_traits.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

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
            extended_payload_len64 = payload.size();
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
    }
};


class CDED
{
public:
    int sizeofDED;
    void* pDEDarray;
    std::string status;
    void* pOwner;
};

struct Callback { virtual void operator()(void * pParent) = 0; }; //TODO: change to fit DED type, since data will be inside a DED structure


#if 0
http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol

 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
#endif
struct wsheader_type {
    int header_size;
    bool fin;
    bool mask;
    enum opcode_type {
        CONTINUATION = 0x0,
        TEXT_FRAME = 0x1,
        BINARY_FRAME = 0x2,
        CLOSE = 8,
        PING = 9,
        PONG = 0xa,
    } opcode;
    int N0;
    uint64_t N;
    uint8_t masking_key[4];
};

template<class Buffer>
class ConsumeFromRingbuffer {

    bool bRunning;

    typedef typename Buffer::value_type value_type;
    Buffer* m_container;
    value_type m_item;
    void *ptrCaller;
    CDED* ded; // this object will be instantiated by user
    std::vector<uint8_t> rxbuf;

    void (*pt2YourFunction)(void* pParent);
    virtual bool _dispatch(Callback & callable)
    {
        bool bResult = false; // Result will be true if dataframe is read
        while (true) {
            wsheader_type ws;
            if (rxbuf.size() < 2) { return bResult; /* Need at least 2 */ }
            const uint8_t * data = (uint8_t *) &rxbuf[0]; // peek, but don't consume
            ws.fin = (data[0] & 0x80) == 0x80;
            ws.opcode = (wsheader_type::opcode_type) (data[0] & 0x0f);
            ws.mask = (data[1] & 0x80) == 0x80;
            ws.N0 = (data[1] & 0x7f);
            ws.header_size = 2 + (ws.N0 == 126? 2 : 0) + (ws.N0 == 127? 6 : 0) + (ws.mask? 4 : 0);
            if (rxbuf.size() < (unsigned int)ws.header_size) { return bResult; /* Need: ws.header_size - rxbuf.size() */ }
            int i;
            if (ws.N0 < 126) {
                ws.N = ws.N0;
                i = 2;
            }
            else if (ws.N0 == 126) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 8;
                ws.N |= ((uint64_t) data[3]) << 0;
                i = 4;
            }
            else if (ws.N0 == 127) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 56;
                ws.N |= ((uint64_t) data[3]) << 48;
                ws.N |= ((uint64_t) data[4]) << 40;
                ws.N |= ((uint64_t) data[5]) << 32;
                ws.N |= ((uint64_t) data[6]) << 24;
                ws.N |= ((uint64_t) data[7]) << 16;
                ws.N |= ((uint64_t) data[8]) << 8;
                ws.N |= ((uint64_t) data[9]) << 0;
                i = 10;
            }
            if (ws.mask) {
                ws.masking_key[0] = ((uint8_t) data[i+0]) << 0;
                ws.masking_key[1] = ((uint8_t) data[i+1]) << 0;
                ws.masking_key[2] = ((uint8_t) data[i+2]) << 0;
                ws.masking_key[3] = ((uint8_t) data[i+3]) << 0;
            }
            else {
                ws.masking_key[0] = 0;
                ws.masking_key[1] = 0;
                ws.masking_key[2] = 0;
                ws.masking_key[3] = 0;
            }
            if (rxbuf.size() < ws.header_size+ws.N) { return bResult; /* Need: ws.header_size+ws.N - rxbuf.size() */ }

            // We got a whole message, now do something with it:
            if (false) { }
            else if (ws.opcode == wsheader_type::TEXT_FRAME && ws.fin) {
                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
                std::vector<uint8_t> data;
                std::copy(rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+ws.N, std::back_inserter(data));
                data.push_back(0);
                ded->pDEDarray = &data[0];
                ded->sizeofDED = data.size()-1;
                ded->pOwner=this;
                callable(ded);
                bResult=true;
            }
            else if (ws.opcode == wsheader_type::PING) { }
            else if (ws.opcode == wsheader_type::PONG) { }
            else if (ws.opcode == wsheader_type::CLOSE) {  }
            else { fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n");  }

            rxbuf.erase(rxbuf.begin(), rxbuf.begin() + ws.header_size+ws.N);
        }
        return bResult;
    }

    template<class Callable>
    bool dispatch(Callable callable) { // N.B. this is compatible with both C++11 lambdas, functors and C function pointers
        struct _Callback : public Callback {
            Callable & callable;
            _Callback(Callable & callable) : callable(callable) { }
            void operator()(void * pParent) { callable(pParent); }
        };
        _Callback callback(callable);
        return _dispatch(callback);
    }

    void SetCallback(void (*pt2Function)(void* pParent))
    {
        pt2YourFunction = pt2Function;
    }

public:

    void SetResultDED(CDED& dedobj)
    {
        ded = &dedobj;
    }

    void SetDEDstatus(std::string text)
    {
        ded->status = text;
    }

    ConsumeFromRingbuffer(Buffer* buffer,void (*pt2Function)(void* pParent)) : m_container(buffer),ptrCaller(this) { SetCallback(pt2Function); }
    void operator()() {bRunning = true; FetchDataframe(); }

    /** \brief FetchDataframe will continue as long as there is a dataframe inside Ringbuffer
     *
     *
     * \return void
     *
     */
    void FetchDataframe()
    {
        m_container->ReadBinaryFromRingBuffer(rxbuf);
        while(bRunning) // as long as thread holding this is not killed
        {
            if(dispatch(pt2YourFunction)==false) //TODO: make test for multiple dataframes in ringbuffer
            {
                if (m_container->ReadBinaryFromRingBuffer(rxbuf) <= 0) // Perhaps more frames have arrived
                    break; // no more dataframes in ringbuffer - thus exit
            }
        }
    }

private:

public:
//    void operator()(void (&)(const std::string & message)) {bRunning = true; /*FetchDataframe(handle_data); */}
};


template<class Buffer>
class ProduceToRingbuffer {

    typedef typename Buffer::value_type value_type;
    Buffer* m_container;

public:
    ProduceToRingbuffer(Buffer* buffer) : m_container(buffer) {}
    bool StoreDataframe(value_type *pDataframe, int iSizeOfDataframe)
    {
        bool bResult = false;
        int iBytesWritten = m_container->WriteBinaryIntoRingBuffer(pDataframe,iSizeOfDataframe);
        if(iBytesWritten == iSizeOfDataframe)
            bResult=true;
        return bResult;
    }

    bool StoreDataframe(dataframe frame)
    {
        bool bResult = false;
        std::vector<boost::asio::const_buffer> buffers = frame.to_buffers();
        std::vector<uint8_t> rxbuf(boost::asio::buffer_size(buffers));
        boost::asio::buffer_copy(boost::asio::buffer(rxbuf), buffers);
        std::size_t iSizeOfDataframe = rxbuf.size();

        int iBytesWritten = m_container->WriteBinaryIntoRingBuffer((value_type*)&rxbuf[0],iSizeOfDataframe);
        if(iBytesWritten == (int)iSizeOfDataframe)
            bResult=true;
        return bResult;
    }
    void operator()() {}
};


template <class T>
class RingBuffer {
public:

    typedef boost::circular_buffer_space_optimized<T> container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::value_type value_type;
    typedef typename boost::call_traits<value_type>::param_type param_type;

    explicit RingBuffer(size_type capacity) : m_container(capacity)
    {
    }

    int WriteBinaryIntoRingBuffer(value_type * pBuf, int iBufLen)
    {
        int item = 0;
        for(item = 0; item < iBufLen; item++)
        {
            if(is_not_full()==true)
                push_front(pBuf[item]);
            else
                return item; // Ringbuffer not large enough to contain the item
                // Should return the amount of items transfered
        }
        return item; // zero calculated as first item
    }

    int ReadBinaryFromRingBuffer(value_type * pBuf, int iBufLen)
    {
        int iElementsInBuffer = (m_container.capacity()) - m_container.reserve();
        if (iBufLen <= iElementsInBuffer)
        {
            int item = 0;
            for(item = 0; item < iBufLen; item++)
            {
                if(is_not_empty()==true)
                    pop_back(&pBuf[item]);
                else
                    return item; // return amount read
            }
            return item;
        }
        return 0;
    }

    int ReadBinaryFromRingBuffer(std::vector<uint8_t>& buf)
    {
        int iElementsInBuffer = (m_container.capacity()) - m_container.reserve();
        int iBufLen = buf.size();
        if (iBufLen <= iElementsInBuffer)
        {
            int item = 0;
            for(item = 0; item < iElementsInBuffer; item++)
            {
                if(is_not_empty()==true)
                {
                    value_type Element;
                    pop_back(&Element); // Fetch element from ringbuffer
                    buf.push_back(Element); // Store element in buf
                }
                else
                    return item; // return amount read
            }
            return item;
        }
        return 0;
    }

private:

    RingBuffer(const RingBuffer&);              // Disabled copy constructor
    RingBuffer& operator = (const RingBuffer&); // Disabled assign operator

    bool is_not_empty() const { return m_container.size() > 0; }
    bool is_not_full() const { return m_container.size() < m_container.capacity(); }

    container_type m_container;
    boost::mutex m_mutex;
    boost::condition m_not_empty;
    boost::condition m_not_full;

    void push_front(param_type item) {
        boost::mutex::scoped_lock lock(m_mutex);
        m_not_full.wait(lock, boost::bind(&RingBuffer<value_type>::is_not_full, this));
        m_container.push_front(item);
        lock.unlock();
        m_not_empty.notify_one();
    }

    void pop_back(value_type* pItem) {
        boost::mutex::scoped_lock lock(m_mutex);
        m_not_empty.wait(lock, boost::bind(&RingBuffer<value_type>::is_not_empty, this));
        *pItem = m_container.back();
        m_container.pop_back();
        lock.unlock();
        m_not_full.notify_one();
    }
};

