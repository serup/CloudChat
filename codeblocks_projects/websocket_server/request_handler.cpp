#include "request_handler.hpp"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include "header.hpp"
#include "reply.hpp"
#include "request.hpp"

// http://www.htmlgoodies.com/html5/tutorials/making-html5-websockets-work.html#fbid=1bdqw5mmbts

#define CHALLENGE_REQUEST_SIZE 8
#define CHALLENGE_RESPONSE_SIZE 16

namespace websocket {
    namespace http {
// https://github.com/iantropov/libws/blob/master/src/web_sockets.c
		std::string request_handler::md5_hash_challenge(int key_1, int key_2, u_char *value)
		{
			u_char *hashed_value = (u_char *)malloc(CHALLENGE_RESPONSE_SIZE);
			if (hashed_value == NULL)
				return NULL;

			int key_1_big_endian = htonl(key_1); // ntohl  is networktohostlong function - it converts a long from bigEndian to littleEndian format
			int key_2_big_endian = htonl(key_2);
			memcpy(hashed_value, (char *)&key_1_big_endian, sizeof(int));
			memcpy(hashed_value + sizeof(int), (char *)&key_2_big_endian, sizeof(int));
			memcpy(hashed_value + CHALLENGE_REQUEST_SIZE, value, CHALLENGE_REQUEST_SIZE);

//			std::string str(CMD5((const char*)hashed_value).GetMD5s());

			//+test -- virker :-)
//			std::string str(CMD5((const char*)"helloworld").GetMD5s());
			//should yield = fc5e038d38a57032085441e7fe7010b0
			//-
			hashed_value[CHALLENGE_RESPONSE_SIZE] = (u_char)NULL;
			std::string strtmp((const char*)hashed_value);
//			std::string str((const char*)CMD5(strtmp.c_str()).GetMD5n()); // return the digest
			std::string str((const char*)CMD5(strtmp.c_str()).GetMD5s());
			//str[CHALLENGE_RESPONSE_SIZE] = NULL;




					int len = str.length();
					std::string md5 = "";
					for(int i=0; i< len; i+=2)
					{
						std::string byte = str.substr(i,2);
						char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
						md5.push_back(chr);
					}


			free(hashed_value);
			//return str;
			return md5;
		}

		int request_handler::get_number_from_string(const char *str)
		{
			int space_num = 0;
			unsigned int raw_num = 0;
			int rank = 1;

			int len = (int)strlen(str);
			int i;
			for (i = len - 1; i >= 0; i--) {
				switch (str[i]) {
					case '0' : raw_num += rank * 0; rank *= 10; break;
					case '1' : raw_num += rank * 1; rank *= 10; break;
					case '2' : raw_num += rank * 2; rank *= 10; break;
					case '3' : raw_num += rank * 3; rank *= 10; break;
					case '4' : raw_num += rank * 4; rank *= 10; break;
					case '5' : raw_num += rank * 5; rank *= 10; break;
					case '6' : raw_num += rank * 6; rank *= 10; break;
					case '7' : raw_num += rank * 7; rank *= 10; break;
					case '8' : raw_num += rank * 8; rank *= 10; break;
					case '9' : raw_num += rank * 9; rank *= 10; break;
					case ' ' : space_num++; break;
				}
			}
			if(raw_num==0)
				return -1;
			if(space_num==0)
				return -1;
			return raw_num / space_num;
		}

        void request_handler::handle_request(const request& req, reply& rep)
        {
            std::string key;
//            for (std::vector<header>::const_iterator i = req.headers.cbegin();
//                i != req.headers.cend(); ++i)
            for (std::vector<header>::const_iterator i = req.headers.begin();
                i != req.headers.end(); ++i)
            {
                if (i->name == "Sec-WebSocket-Key")
                {
					/*  http://tools.ietf.org/id/draft-ietf-hybi-thewebsocketprotocol-01.html
					    http://tools.ietf.org/id/draft-ietf-hybi-thewebsocketprotocol-06.html

						As of HyBi 06, the client sends a Sec-WebSocket-Key which is base64 encoded.
						To this key the magic string "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" is appended, hashed with SHA1 and then base64 encoded.
						The result is then replied in the header "Sec-WebSocket-Accept".
						For instance, a string of "x3JJHMbDL1EzLkh9GBhXDw==258EAFA5-E914-47DA-95CA-C5AB0DC85B11" hashed by SHA1 yields a hexadecimal value of "1d29ab734b0c9585240069a6e4e3e91b61da1969".
						Here's what the version 6 handshake looks like:

						GET /ws HTTP/1.1
						Host: gravelleconsulting.com
						Upgrade: websocket
						Connection: Upgrade
						Sec-WebSocket-Version: 6
						Sec-WebSocket-Origin: http://gravelleconsulting.com
						Sec-WebSocket-Extensions: deflate-stream
						Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==

						...to which the server responds:

						HTTP/1.1 101 Switching Protocols
						Upgrade: websocket
						Connection: Upgrade
						Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=
					*/
                    key = i->value;
                    break;
                }
            }

            if (key.empty())
            {
				std::string key1;
				std::string key2;
				std::string origin;
				std::string host;
				std::string challenge;
				std::string md5Challenge;
				std::string md5ChallengeASCII;
				std::string location;
				std::string protocol;
				int key_1;
				int key_2;

				// check for old HyBi 00 version of websockets [http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-00]
				/*
					GET /demo HTTP/1.1
					Upgrade: WebSocket
					Connection: Upgrade
					Host: gravelleconsulting.com
					Origin: http://gravelleconsulting.com
					Sec-WebSocket-Key1: 4 @1  46546xW%0l 1 5
					Sec-WebSocket-Key2: 12998 5 Y3 1  .P00

					^n:ds[4U

					...to which the server responds:

					HTTP/1.1 101 WebSocket Protocol Handshake
					Upgrade: WebSocket
					Connection: Upgrade
					Sec-WebSocket-Origin: http://gravelleconsulting.com
					Sec-WebSocket-Location: ws://gravelleconsulting.com/socket/server/socketDaemon.php
					Sec-WebSocket-Protocol: forum

					8jKS'y:G*Co,Wxa-

				*/
				for (std::vector<header>::const_iterator i = req.headers.begin(); i != req.headers.end(); ++i)
				{
					if (i->name == "Sec-WebSocket-Key1")
					{
						key1 = i->value;
					}
					if (i->name == "Sec-WebSocket-Key2")
					{
						key2 = i->value;
					}
					if (i->name == "Origin")
					{
						origin = i->value;
					}
					if (i->name == "Host")
					{
						host = i->value;
					}

				}
				if (key1.empty())
				{
					// old version protocol was also not found - the request is BAD
					rep = reply::stock_reply(reply::bad_request);
					return;
				}
				else
				{
				    //std::vector<header>::const_iterator i = req.headers.end();
				    //challenge = i->name; // should be last line, so when finished this should contain the challenge
					challenge = req.challenge;

//+ test
/*					challenge ="";
					key1 = "P388 O503D&ul7 {K%gX( %7  15";
					key2 = "1 N ?|k UT0or 3o  4 I97N 5-S3O 31";
					challenge.push_back(0x47);
					challenge.push_back(0x30);
					challenge.push_back(0x22);
					challenge.push_back(0x2D);
					challenge.push_back(0x5A);
					challenge.push_back(0x3F);
					challenge.push_back(0x47);
					challenge.push_back(0x58);
*/
//-

					// old version found, finish parsing it
					if (challenge.empty())
					{
						rep = reply::stock_reply(reply::bad_request);
						return;
					}
					// prepare handshake response
					key_1 = get_number_from_string(key1.c_str()); // raw number / num of spaces ( remove spaces to create raw number, then divide with the amount of spaces found )
					key_2 = get_number_from_string(key2.c_str());
					if(key_1<=0 || key_2 <=0)
					{
						rep = reply::stock_reply(reply::bad_request);
						return;
					}

//					md5ChallengeASCII = md5_hash_challenge(key_1, key_2, (u_char*)challenge.c_str()); // returns the digest
					md5ChallengeASCII = md5_hash_challenge(key_1, key_2, (u_char*)&req.vecchallenge[0]); // returns the digest
//// should yield 0st3Rl&q-2ZU^weu

					location = req.uri;
					protocol = "chat";

					rep.status = reply::handshake;
					rep.headers.resize(7);
//					rep.headers.resize(6);
					rep.headers[0].name = "Upgrade";
					rep.headers[0].value = "WebSocket";
					rep.headers[1].name = "Connection";
					rep.headers[1].value = "Upgrade";
					rep.headers[2].name = "Sec-WebSocket-Origin";
					rep.headers[2].value = origin;
					rep.headers[3].name = "Sec-WebSocket-Location";
					rep.headers[3].value = location;
					rep.headers[4].name = "Sec-WebSocket-Protocol";
					rep.headers[4].value = protocol;
					rep.headers[5].name = "";
					rep.headers[5].value = "";
//					rep.headers[6].name = "Sec-WebSocket-Key3";
					rep.headers[6].name = "";
					rep.headers[6].value = md5ChallengeASCII;

					return; // finished old version protocol
				}
            }

            const std::string magic_guid("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
            std::string buffer(key + magic_guid);
            std::string accept(to_base64(to_sha1(buffer)));

            rep.status = reply::switching_protocols;
            rep.headers.resize(3);
            rep.headers[0].name = "Upgrade";
            rep.headers[0].value = "websocket";
            rep.headers[1].name = "Connection";
            rep.headers[1].value = "Upgrade";
            rep.headers[2].name = "Sec-WebSocket-Accept";
            rep.headers[2].value = accept;
        }

        std::vector<unsigned char> request_handler::to_sha1(const std::string& s)
        {
            boost::uuids::detail::sha1 cipher;
            cipher.process_bytes(s.c_str(), s.length());
            boost::uint32_t digest[5];
            cipher.get_digest(digest);

            std::vector<unsigned char> hash(20, 0);
            for (std::size_t i = 0; i < hash.size(); ++i)
            {
                hash[i] = (unsigned char)(digest[i >> 2] >> 8 * (3 - (i & 0x03)));
            }

            return hash;
        }

        std::string request_handler::to_base64(const std::vector<unsigned char>& data)
        {
            using namespace boost::archive::iterators;

            typedef
                insert_linebreaks<         // insert line breaks every 72 characters
                    base64_from_binary<    // convert binary values ot base64 characters
                        transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
                            const char *,
                            6,
                            8
                        >
                    >
                    ,72
                >
                base64_iterator;

            std::vector<unsigned char> buff(data);
            size_t number_of_padd_chars = (buff.size() % 3 != 0) ? 3 - (buff.size() % 3) : 0;
            buff.insert(buff.end(), number_of_padd_chars, '\0');

            base64_iterator begin(&buff[0]), end(&buff[0] + data.size());
            std::string result(begin, end);
            result.insert(result.end(), number_of_padd_chars, '=');

            return result;
        }

    } // namespace http
} // namespace websocket
