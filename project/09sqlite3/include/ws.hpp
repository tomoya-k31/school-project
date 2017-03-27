// WebSocket
/*
 * Copyright (C) 2016 Shusuke Okamoto, All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice and this list of conditions.
 * 2. Redistributions in binary form must reproduce the above 
 *    copyright notice and this list of conditions in the
 *    documentation and/or other materials provided with the
 *    distribution.
 * 3. All advertising materials mentioning features or use of this 
 *    software must display the following acknowledgement:
 * 
 *       This product includes software developed by Shusuke Okamoto 
 * 
 * 4. The name of Shusuke Okamoto should not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 */
#pragma once
#include <iostream>
#include <random>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <functional>
#include "websvr.hpp"

namespace ws {

// sha1 and base64 encoding,
// which is needed for websocket negotiation

// Integer class to get/set a inner-byte in bigendian way
template<typename T> // T is uint16_t, uint32_t or uint64_t
class bigend_t {
   class sub_t {
      T& i;
      const size_t idx;
   public:
      sub_t(T& ii, size_t x): i(ii),idx(x){}
      uint8_t operator=(uint8_t v) {
         i &= ~((((T)0xFF) << ((sizeof(T)-1-idx)*8)));
         i |=  ((((T)v)    << ((sizeof(T)-1-idx)*8)));
         return v;
      }
      operator uint8_t() { return (uint8_t)(i>>((sizeof(T)-1-idx)*8)); };
   };
   T i;
public:
   bigend_t(T x =T()) :i(x){}
   bigend_t operator+(const bigend_t& a)const{ return bigend_t(this->i+a.i); }
   bigend_t operator^(const bigend_t& a)const{ return bigend_t(this->i^a.i); }
   bigend_t operator&(const bigend_t& a)const{ return bigend_t(this->i&a.i); }
   bigend_t operator|(const bigend_t& a)const{ return bigend_t(this->i|a.i); }
   bigend_t operator~()const{ return bigend_t( ~(this->i) ); }
   bigend_t leftrotate(size_t n) const {
      return bigend_t((this->i << n) | (this->i >> (sizeof(T)*8-n)));
   }
   sub_t operator[](size_t x) { return sub_t(i,x); }
};

typedef bigend_t<uint32_t> b32_t;
typedef bigend_t<uint64_t> b64_t;

// This code is based on https://en.wikipedia.org/wiki/SHA-1
void sha1(b32_t h[5], std::string msg)
{
   h[0] = 0x67452301;
   h[1] = 0xEFCDAB89;
   h[2] = 0x98BADCFE;
   h[3] = 0x10325476;
   h[4] = 0xC3D2E1F0;
   if (msg.empty()) return;
   const size_t nbits = 8;    // the number of bits per a byte
   b64_t ml = (uint64_t)msg.size()*nbits;

   // pre-processing
   msg += '\x80';
   while (((msg.size()*nbits) % 512) != 448)
      msg += '\0';
   for (size_t i=0; i<sizeof(uint64_t); i++)
      msg += ml[i];

   // process the message in successive 512-bit(64 bytes) chunks
   for (size_t q=0; q<msg.size(); q+=64) {
      const uint8_t *chunk = reinterpret_cast<const uint8_t*>(msg.c_str()+q);

      // break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15
      b32_t w[80];
      for (size_t i=0; i<16; i++) {
         w[i][0] = chunk[i*4];
         w[i][1] = chunk[i*4+1];
         w[i][2] = chunk[i*4+2];
         w[i][3] = chunk[i*4+3];
      }

      // extend the sixteen 32-bit word into eighty 32-bit words;
      for (size_t i=16; i<80; i++)
         w[i] = (w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]).leftrotate(1);

      // initialize hash value for this chunk
      b32_t a = h[0];
      b32_t b = h[1];
      b32_t c = h[2];
      b32_t d = h[3];
      b32_t e = h[4];

      // main loop
      for (size_t i=0; i<80; i++) {
         b32_t f, k;
         if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
         } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
         } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
         } else if (i < 80) {
            f = (b ^ c ^ d);
            k = 0xCA62C1D6;
         } 

         b32_t temp = a.leftrotate(5) + f + e + k + w[i];
         e = d;
         d = c;
         c = b.leftrotate(30);
         b = a;
         a = temp;
      }

      // add this chunk's hash to result so far
      h[0] = h[0] + a;
      h[1] = h[1] + b;
      h[2] = h[2] + c;
      h[3] = h[3] + d;
      h[4] = h[4] + e;
   }
}

std::string
base64(uint8_t *src, size_t len)
{
   const char *tab =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   std::string rst;
   for (size_t idx = 0; idx < len; idx += 3) {
      uint32_t tmp = 
         (idx+2 < len) ? ((src[idx]<<16)|(src[idx+1]<<8)|src[idx+2]):
         (idx+1 < len) ? ((src[idx]<<16)|(src[idx+1]<<8)): (src[idx]<<16);
      rst += tab[(tmp>>18)& 0x3F];
      rst += tab[(tmp>>12)& 0x3F];
      if (idx+2 < len) {
         rst += tab[(tmp>> 6)& 0x3F];
         rst += tab[(tmp    )& 0x3F];
      } else if (idx+1 < len) {
         rst += tab[(tmp>> 6)& 0x3F];
         rst += "=";
      } else {
         rst += "==";
      }
   }
   return rst;
}

inline std::string
get_ws_key(std::string msg) {
   msg += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; // magic string for Websocket
   b32_t h[5];
   sha1(h, msg);

   uint8_t *buf = new uint8_t[sizeof(h)];
   for (size_t i=0; i<sizeof(h)/sizeof(h[0]); i++)
      for (size_t j=0; j<sizeof(uint32_t); j++)
         buf[i*sizeof(uint32_t)+j] = h[i][j];

   std::string rst = base64(buf, sizeof(h));
   delete []buf;
   return rst;
}

#if !defined(htonll)
#define wshtonll
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#else
#define htonll(x) (x)
#define ntohll(x) (x)
#endif
#endif

class Socket {
private:
   bool     fin;
   uint8_t  rsv;
   uint16_t opcode;
   bool     maskflg;
   uint8_t  mask[4];
   uint64_t length;
   uint16_t prev_opcode;

public:
   Socket(int x = -1, std::unordered_map<int,ws::Socket>* sl =nullptr)
      :opcode(8),length(0LL),fd(x),idx(0LL),connected(x==-1?false:true),
       socket_list_ptr(sl){}

   Socket& get_next() {
      if (!connected) return *this;
      if (length != 0LL && idx < length)
         clean();
      idx = 0LL;
      prev_opcode = opcode;
      uint8_t hdr[2];
      recv_nbytes(hdr, sizeof(hdr));
      fin     = ((hdr[0]&0x80)>>7) == 1;
      rsv     =  (hdr[0]&0x70)>>4;
      opcode  =   hdr[0]&0x0F;
      maskflg = ((hdr[1]&0x80)>>7) == 1;
      length  =   hdr[1]&0x7F;
      
      if (length == uint64_t(126)) {
         uint16_t buf;
         recv_nbytes(&buf, sizeof(buf));
         length = ntohs(buf);
      } else if (length == uint64_t(127)) {
         recv_nbytes(&length, sizeof(length));
         length = ntohll(length);
      }

      if (maskflg)
         recv_nbytes(mask, sizeof(mask));

      return *this;
   }

   void clean() {
      if (!connected) return;
      while (length - idx > 0) {
         size_t x = length - idx;
         char buf[1024];
         if (x > sizeof(buf)) x = sizeof(buf);
         std::cout << "clean: " << x <<"\n";
         ssize_t n = recv_nbytes(buf, x);
         if (n <= 0) break;
         idx += n;
      }
   }

   ssize_t recv(uint8_t* s, size_t sz) {
      if (!connected || idx >= length) return 0;
      if (idx+sz > length) sz = length - idx;
      ssize_t n = recv_nbytes(s, sz);
      if (n > 0) {
         if (maskflg) {
            for (ssize_t i=0; i<n; i++)
               s[i] = s[i] ^ mask[(idx+i)%sizeof(mask)];
         }
         idx += n;
      }
      return n;
   }

   std::string recv() {
      std::string rslt;
      if (!connected || idx >= length) return rslt;
      ssize_t sz = length - idx;
      char *buf = (char*)calloc(sz, 1);
      if (buf == nullptr) { perror("calloc"); return rslt; }
      ssize_t n = recv_nbytes(buf, sz);
      if (n > 0) {
         if (maskflg) {
            for (ssize_t i=0; i<n; i++)
               rslt += buf[i] ^ mask[(idx+i)%sizeof(mask)];
         }
         idx += n;
      }
      free(buf);
      if (n != sz)
         std::cerr << "warning recv: shortage of string\n";
      return rslt;
   }

private:
   void send_pingpong(char p) {
      if (!connected) return;
      if (p != '\x9' && p != '\xA') return; // 0x9 is ping, 0xA is pong
      std::string buf;
      buf += (char)0x80+p; // FIN:1 + opcode
      buf += (char)'\0';   // no mask, 0 Playload len
      ssize_t num = ::send(fd, buf.c_str(), buf.size(), 0);
      if (num == -1) std::perror("send");
   }
public:
   void send_ping() { send_pingpong('\x9'); }
   void send_pong() { send_pingpong('\xA'); }

   //bool write_data(const uint8_t* s, size_t sz) {
   ssize_t send(std::string s) {
      if (!connected) return -1;
      std::uniform_int_distribution<uint32_t> distribution(0,0xFFFFFFFF);
      union {
         uint32_t i;
         uint8_t  m[4];
      } mask = { .i = distribution(generator) };
      std::string buf;
      buf += '\x81';
      size_t sz = s.size();
      if (sz < 126) {
         buf += (char)(0x80+sz);
      } else if (sz <= 0xFFFF) {
         buf += (char)(0x80+126);
         union {
            uint16_t l;
            uint8_t  d[2];
         } len = { .l = htons(sz) };
         buf += (char)len.d[0];
         buf += (char)len.d[1];
      } else {
         buf += (char)(0x80+127);
         union {
            uint64_t l;
            uint8_t  d[8];
         } len = { .l = htonll(sz) };
         for (int i = 0; i<8; i++)
            buf += (char)len.d[i];
      }
      buf += (char)mask.m[0];
      buf += (char)mask.m[1];
      buf += (char)mask.m[2];
      buf += (char)mask.m[3];
      for (size_t i = 0; i < sz; i++)
         buf += s[i]^mask.m[i%4];
      ssize_t num = ::send(fd, buf.c_str(), buf.size(), 0);
      if (num == -1) std::perror("send");
      return num;
   }

   bool is_text()       const { return opcode_check(0x1); }
   bool is_binary()     const { return opcode_check(0x2); }
   bool is_ping()       const { return opcode_check(0x9); }
   bool is_pong()       const { return opcode_check(0xA); }
   bool closed()        const { return opcode_check(0x8) || !connected; }
   bool is_continued()  const { return !fin; }
   uint64_t data_size() const { return length - idx; }
private:
   bool opcode_check(uint16_t x)const {
      if (!connected) return false;
      return opcode==x || (opcode==0 && prev_opcode==x);
   }
   int fd;
   uint64_t idx;
   bool connected;
   std::default_random_engine generator;
   ssize_t recv_nbytes(void* q, size_t sz) {
      ssize_t n = ::recv(fd, q, sz, 0);
      if (n == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK)
            std::cerr << "recv timeout\n";
         else std::perror("recv");
      } else if (n == 0) {
         std::cout << "connection closed\n";
         connected = false;
      }
      return n;
   }
private:
   std::unordered_map<int,ws::Socket>* socket_list_ptr;
public:
   void broadcast(std::string msg)
   {
      if (socket_list_ptr == nullptr) return;
      for (auto& s: *socket_list_ptr)
         if (s.first != fd)
            s.second.send(msg);
   }
};

#ifdef wshtonll
#undef ntohll
#undef htonll
#endif



class Handler{
   std::string path;
   std::unordered_map<int,ws::Socket> socket_list;
public:
   Handler() {}
   Handler(std::string s): path(s){}

   static bool check_origin(int fd, std::string host, std::string port)
   {
      if (host.empty()) return false;
      if (port.empty()) port = "80"; // default http port
      sockaddr_in svr;
      socklen_t len = sizeof(svr);
      if (getsockname(fd, (sockaddr*)&svr, &len) == -1) {
         perror("getsockname");
         return false;
      }
      // network byte order
      uint32_t svraddr = svr.sin_addr.s_addr;
      uint16_t svrport = svr.sin_port;

      std::cout <<"org: "<< std::hex << ntohl(svraddr)
                << " : " << std::dec << ntohs(svrport) <<"\n";

      addrinfo hints, *svr2;
      memset(&hints, 0, sizeof(hints));
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_family = AF_INET;
      hints.ai_flags = AI_NUMERICSERV;
      if (int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &svr2)) {
         std::cout << "Error: " << gai_strerror(err) << "\n";
         return false;
      }
      bool match = false;
      for (addrinfo* x = svr2; x != nullptr; x = x->ai_next) {
         struct sockaddr_in* p = (struct sockaddr_in*)(x->ai_addr);
         std::cout <<"chk: "<< std::hex << ntohl(p->sin_addr.s_addr)
                   << " : " << std::dec << ntohs(p->sin_port) << "\n";
         if (svraddr == p->sin_addr.s_addr && svrport == p->sin_port) {
            std::cout << "Origin check OK\n";
            match = true;
            break;
         }
      }
      freeaddrinfo(svr2);

      return match;
   }

   bool upgrade(http::Request& req, http::Response& res)
   {
      std::string pathname = url::parse(req.url)["pathname"];
      if (req.method == "GET" 
      &&  (this->path.empty() || pathname == this->path)
      &&  req.headers["Upgrade"] == "websocket"
      &&  req.headers["Connection"].find("Upgrade") != std::string::npos
      &&  req.headers["Sec-WebSocket-Version"] == "13"
      && !req.headers["Sec-WebSocket-Key"].empty()
      && !req.headers["Origin"].empty()){

         int fd = res.get_socket_fd();
         auto url = url::parse(req.headers["Origin"]);
         if (!check_origin(fd, url["hostname"], url["port"]))
            return false;
         std::string key = ws::get_ws_key(req.headers["Sec-WebSocket-Key"]);
         if (key.empty()) return false;

         res.writeHead(101, {{"Upgrade","websocket"},
                             {"Connection","Upgrade"},
                             {"Sec-WebSocket-Accept", key},
                             {"Sec-WebSocket-Protocol", "chat"}});
         res.end();
         socket_list[fd] = ws::Socket(fd, &socket_list);

         std::cout << "Upgrade to websocket: "<< fd << "\n";
         connect(socket_list[fd]); // user defined callback

         return true;
      }
      return false;
   }


   bool handler(int fd)
   {
      ws::Socket &socket = socket_list[fd];
      socket.get_next();
      std::cout << "handler for "<<fd <<": "<<socket.data_size()<<"\n";
      if (socket.is_text()) {
         std::string msg = socket.recv();
         std::cout << "get a text: "<<msg<<"\n";
         receive(socket, msg);
      } else
      if (socket.is_ping()) {
         std::cout << "get ping: "<<socket.data_size()<<"\n";
         socket.send_pong();
      } else
      if (socket.is_pong()) {
         std::cout << "get pong: "<<socket.data_size()<<"\n";
      }

      if (socket.closed()) {
         std::cout << "closed on "<<fd <<"\n";
         closed(socket_list[fd]);
         socket_list.erase(fd);
         return false; // want to disconnect
      }
      return true; // to be continued
   }

   virtual void connect(ws::Socket& socket) = 0;
   virtual void receive(ws::Socket& socket, std::string msg) = 0;
   virtual void closed(ws::Socket& socket){}
};


};

namespace http {

using namespace std::placeholders;

inline
WebSvr createServer(WebSvr::http_callback_t c, ws::Handler& h) {
   return WebSvr(c, std::bind(&ws::Handler::upgrade, std::ref(h), _1, _2),
                    std::bind(&ws::Handler::handler, std::ref(h), _1) );
}

};
