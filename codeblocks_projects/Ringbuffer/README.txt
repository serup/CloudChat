Project: "Ringbuffer"

Goals:

1]
 Transform ringbuffer.cpp and ringbuffer.h to c++11 standard and remove all reference to packet handling
 Make new functions that can store and retrieve a "packet" from the ringbuffer -- the packet will have
 xml as start / stop tag and CRC check is used to verify that packet is received correctly into the buffer

use boost circular buffer as template
ww.boost.org/doc/libs/1_54_0/libs/circular_buffer/doc/circular_buffer.html

http://www.boost.org/doc/libs/1_54_0/libs/circular_buffer/test/bounded_buffer_comparison.cpp

2]
 make testcases that test above scenario


Info:
 here you can find info about ringbuffers
 http://www.codeproject.com/Articles/2269/Fast-Ring-Buffer-for-Incoming-Winsock-Textual-Data

