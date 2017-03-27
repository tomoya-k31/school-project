// Web server
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
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>   // std::function
#include <regex>
#include <algorithm> // std::transform

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>     // for close
#include <sys/select.h>

#include <ctime>        // time, strftime
#include <cstring>      // memset, memcpy, strcpy
#include <cstdio>       // perror
#include <cctype>       // isdigit, isupper, islower

namespace http {

using std::string;

class lower_string: public string {
   lower_string& to_lower(const char* p) {
      size_t len = std::strlen(p);
      this->resize(len);
      std::transform(p, p+len, this->begin(), ::tolower);
      return *this;
   }
   lower_string& to_lower(const std::string& s) {
      this->resize(s.size());
      std::transform(s.begin(), s.end(), this->begin(), ::tolower);
      //for (size_t i = 0; i < s.size(); i++) (*this)[i] = ::tolower(s[i]);
      return *this;
   }
public:
   lower_string(){}
   lower_string(const char* p)                   { (void) to_lower(p); }
   lower_string(const std::string& s)            { (void) to_lower(s); }
   lower_string& operator=(const char* p)        { return to_lower(p); }
   lower_string& operator=(const std::string& s) { return to_lower(s); }
   struct hash {
      std::size_t operator() (const lower_string& ls) const {
         return std::hash<std::string>()(ls);
      }
   };
};

inline
bool casecmp(const string& a, const string&b) {
   return strcasecmp(a.c_str(), b.c_str()) == 0;
}


struct Buffer {
   size_t length;
   void*  data;
   ~Buffer(){ std::free(data); }
   Buffer():length(0),data(nullptr){}
   Buffer(size_t x):length(x),data(std::calloc(x,1)){
      if (data == nullptr) { std::perror("calloc"); }
   }
   // copy constructor
   Buffer(const Buffer& a): length(a.length),data(std::malloc(a.length)) {
      if (data == nullptr) { std::perror("malloc"); return; }
      std::memcpy(data, a.data, a.length);
      
   }
   // move constructor
   Buffer(Buffer&& a): length(a.length),data(a.data){
      a.length = 0;
      a.data = nullptr;
   }

   // copy assignment
   Buffer& operator=(const Buffer& a) {
      if (this != &a) {
         if (length != a.length) {
            length = a.length;
            data = std::realloc(data, length);
            if (data == nullptr) { std::perror("realloc"); return *this; }
         }
         std::memcpy(data, a.data, a.length);
      }
      return *this;
   }
   // move assignment
   Buffer& operator=(Buffer&& a) {
      if (this == &a) return a;
      std::free(data);
      data = a.data;
      length = a.length;
      a.data = nullptr;
      a.length = 0;
      return *this;
   }

   // conversion to string
   string str() {
      string s;
      for (size_t i=0; i < length; i++)
         s += ((char*)data)[i];
      return s;
   }

   bool add_space(size_t x, bool clearflg =true) {
      data = std::realloc(data, length+x);
      if (data == nullptr) { std::perror("realloc"); return false; }
      if (clearflg) memset((char*)data+length, 0, x);
      length += x;
      return true;
   }
   Buffer& operator+=(const Buffer& x) {
      size_t orglen = length;
      if (add_space(x.length, false))
         std::memcpy((char*)data+orglen, x.data, x.length);
      return *this;
   }
   void add(const void* d, size_t l) {
      size_t orglen = length;
      if (add_space(l, false))
         std::memcpy((char*)data+orglen, d, l);
   }
   void clear() {
      std::free(data);
      length = 0;
      data = nullptr;
   }
};  // end of struct Buffer


class Response {
 private:
   using hdrs_t = std::vector<std::pair<string,string>>;
   const int sfd; // socket file descriptor
   std::unordered_map<lower_string,string,lower_string::hash> headers;
   bool chunked;
   bool keepalive;

 public:
   bool finished;
   bool headersSet;
   bool headersSent;
   int  statusCode;
   string statusMessage;
 private:
   std::unordered_map<int,string> stCodeMsg;

   Buffer sendbuffer;
   void flush() {
      if (sendbuffer.length == 0) return;
      if (::send(sfd, sendbuffer.data, sendbuffer.length, 0) == -1)
         std::perror("send");
      sendbuffer.clear();
   }

 public:
   string getHeader(lower_string n) const { 
      auto it = headers.find(n);
      return (it == headers.end()) ? string() : it->second;
   }
   Response& setHeader(string name, string value) {
      if (!name.empty()) headers[name] = value;
      return *this;
   }

   Response& setHeader(string name, std::vector<string> vec) {
      if (name.empty()) return *this;
      bool flag = true;
      for (auto& v: vec) {
         headers[name] += (flag ? "":"; ") + v;
         flag = false;
      }
      return *this;
   }

   Response& removeHeader(string n){
      headers.erase(n);
      return *this;
   }

   Response& writeContinue() { return writeHead(100); }
   Response& writeHead(int code) { return writeHead(code, hdrs_t()); }

   Response& writeHead(int code, string msg) {
         statusMessage = msg;
         return writeHead(code, hdrs_t());
   }

   Response& writeHead(int code, const hdrs_t& vec) {
         if (statusMessage.empty())
            statusMessage = stCodeMsg[code];
         if (statusMessage.empty())
            statusMessage = "Unknown Code";
         return writeHead(code, statusMessage, vec);
   }


   Response& writeHead(int code, string msg, const hdrs_t& v) {
         if (headersSent) {
            std::cerr << "send headers in multiple times\n";
            return *this;
         } 

         hdrs_t vec(v);
         for (auto& x: headers) vec.push_back(x);
         headers.clear();

         // building up a set of headers
         string buf = "HTTP/1.1 "+std::to_string(code)+" "+msg+"\r\n";
         bool teflag = false, dateflag = false, connflag = false, lenflag=false;
         for (auto& x:vec) {
            buf += x.first+": "+x.second+"\r\n";
            lower_string f(x.first);
            lower_string v(x.second);
            if (f == "transfer-encoding") {
               teflag = true;
               if (v == "chunked")
                  chunked = true;
            }
            if (f == "content-length") {
               lenflag = true; 
               chunked = false;
            }
            else if (f == "date")
               dateflag = true;
            else if (f == "connection") {
               connflag = true;
               keepalive = (v == "close")      ? false:
                           (v == "upgrade")    ? false:
                           (v == "keep-alive") ? true: keepalive;
            }
         }

         if (!lenflag && !teflag && chunked)
            buf += "Transfer-Encoding: chunked\r\n";
         if (!dateflag) {
            char a[64]; // 30 is enough for this purpose
            struct tm tm;
            std::time_t t = std::time(nullptr);
            std::strftime(a,sizeof(a),"%a, %d %b %Y %T %Z",::gmtime_r(&t,&tm));
            buf += "Date: " + string(a) + "\r\n";
         }
         if (!connflag) 
            buf += "Connection: "+string(keepalive?"keep-alive":"close")+"\r\n";

         buf += "\r\n"; // end of header section

         // debug info
         std::cout <<"\n"<< buf;
         if (chunked) std::cout << "chunked response\n";

         sendbuffer.add(buf.c_str(), buf.size());
         statusCode = code;
         statusMessage = msg;
         headersSent = true;
         return *this;
   }

   bool write(string s) {
         if (s.empty()) return true;
         return this->write(reinterpret_cast<const void*>(s.c_str()), s.size());
   }

   bool write(Buffer b) {
         if (b.length == 0) return true;
         return this->write(b.data, b.length);
   }

 protected:
   bool write(const void* buf, size_t sz) {
         if (finished) {
            std::cerr << "already finished\n";
            return true;
         }
         if (!headersSent) 
            writeHead(statusCode);

         return send_body(buf, sz);
   }

 public:
   Response& end(string s =string()) {
         if (!s.empty())
            this->write(s);
         if (chunked)
            send_body(nullptr, 0);

         finished = true;
         this->flush();
         return *this;
   }

 private:
   bool send_body(const void* buf, size_t sz) {
         if (chunked) {
            char hex[2*sizeof(size_t)+3]; // 3 for \r\n\0
            int n = std::snprintf(hex, sizeof(hex), "%lx\r\n", sz);
            sendbuffer.add(hex, n);
         }

         if (sz > 0)
            sendbuffer.add(buf, sz);

         if (chunked)
            sendbuffer.add("\r\n", 2);

         return true;
   }

 public:
   ~Response() { flush(); }
   Response(int f, bool k): sfd(f),
         chunked(k),
         keepalive(k),
         finished(false),
         headersSet(false),
         headersSent(false),
         statusCode(200),
         stCodeMsg({
    {100,"Continue"}, {101,"Switching Protocols"}, {102,"Processing"},
    {200,"OK"}, {201,"Created"}, {202,"Accepted"},
    {203,"Non-Authoritative Information"}, {204,"No Content"},
    {205,"Reset Content"}, {206,"Partial Content"}, {207,"Multi-Status"},
    {208,"Already Reported"}, {226,"IM Used"},
    {300,"Multiple Choices"}, {301,"Moved Permanently"}, {302,"Found"},
    {303,"See Other"}, {304,"Not Modified"}, {305,"Use Proxy"},
    {307,"Temporary Redirect"}, {308,"Permanent Redirect"},
    {400,"Bad Request"}, {401,"Unauthorized"}, {403,"Forbidden"},
    {404,"Not Found"}, {405,"Method Not Allowed"}, {406,"Not Acceptable"},
    {407,"Proxy Authentication Required"}, {408,"Request Timeout"},
    {409,"Conflict"}, {410,"Gone"}, {411,"Length Required"},
    {412,"Precondition Failed"}, {413,"Payload Too Large"},
    {414,"URI Too Long"}, {415,"Unsupported Media Type"},
    {416,"Range Not Satisfiable"}, {417,"Expectation Failed"},
    {422,"Unprocessable Entity"}, {423,"Locked"}, {424,"Failed Dependency"},
    {426,"Upgrade Required"}, {451,"Unavailable For Legal Reasons"},
    {500,"Internal Server Error"}, {501,"Not Implemented"}, {502,"Bad Gateway"},
    {503,"Service Unavailable"}, {504,"Gateway Timeout"},
    {505,"HTTP Version Not Supported"}, {506,"Variant Also Negotiates"},
    {507,"Insufficient Storage"}, {509,"Bandwidth Limit Exceeded"},
    {510,"Not Extended"}
         }) {}
   int get_socket_fd() const { return sfd; }
   friend class WebSvr;
};  // end of struct Response

struct RecvBuffer {
   int sfd; // file descriptor
   char ptr[1024];
   size_t num;
   bool closed;
   RecvBuffer(int f): sfd(f),num(0),closed(false){}
   ~RecvBuffer() { std::cout<<"RecvBuffer: deleted\n"; }

   void l_shift_buf(size_t n) {
      for (size_t j = n; j < num; j++)
         ptr[j-n] = ptr[j];
      num -= n;
   }

   bool append_a_line(string& a) {
      for (size_t i = 0; i < num; i++) {
         a += ptr[i];
         if (ptr[i] == '\r' && i+1 < num && ptr[i+1] == '\n') {
            a += '\n';
            l_shift_buf(i+2);
            return true;
         }
      }
      num = 0;
      return false;
   }

   bool set_recv_timeout(time_t sec) {
      struct timeval tv;
      tv.tv_sec = sec;
      tv.tv_usec = 0;
      if (::setsockopt(sfd, SOL_SOCKET,SO_RCVTIMEO,(void*)&tv,sizeof(tv))!=0) {
         perror("setsockopt");
         return false;
      }
      return true;
   }

   bool refill_buf() {
      int n = ::recv(sfd, ptr+num, sizeof(ptr)-num, 0);
      if (n == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK)
            std::cerr << "recv timeout\n";
         else std::perror("recv");
         return false;
      } else if (n == 0) {
         std::cout << "connection closed\n";
         closed = true;
         return false;
      }
      num += n;
      return true;
   }

   bool recv_a_line(string& a) {
      const int linelimit = 8192;
      a.clear();
      while (!append_a_line(a)) {
         if (a.size() >= linelimit) {
            // the standard impose no limits to the size 
            // but it is necessary for security reasons
            std::cerr << "too long header field\n";
            return false;
         }
         if (!refill_buf())
            return false;
      }
      return true;
   }

   size_t recv_nbytes(void* q, size_t n) {
      size_t x = 0;
      while (n - x > 0) {
         if (num == 0 && !refill_buf())
            break;
         size_t y = (n - x) > num ? num : (n-x);
         std::memcpy((char*)q+x, ptr, y);
         l_shift_buf(y);
         x += y;
      }
      return x;
   }

   size_t recv_nbytes(string& s, size_t n) {
      size_t x = 0;
      while (n - x > 0) {
         if (num == 0 && !refill_buf())
            break;
         size_t y = (n - x) > num ? num : (n-x);
         for (size_t i = 0; i < y; i++)
            s += ptr[i];
         l_shift_buf(y);
         x += y;
      }
      return x;
   }

   bool is_completely_consumed() {
      if (num > 0) return false;
      if (refill_buf()) return false;
      return true;
   }

   class UpStream {
   public:
      virtual UpStream& operator<<(std::string) =0;
      virtual UpStream& operator<<(char) =0;
   };

   class UpStrStr: public UpStream {
      std::stringstream os;
   public:
      virtual UpStrStr& operator<<(std::string s){
         os << s;
         return *this;
      }
      virtual UpStrStr& operator<<(char ch) {
         os << ch;
         return *this;
      }
      virtual std::string str() { return os.str(); }
   };

   class UpStrFile: public UpStream {
      FILE *fp;
      char *path;
   public:
      UpStrFile(std::string dir =std::string(".")) :fp(nullptr) {
         std::string p = dir + "/uploaded_file_XXXXXX";
         path = (char*)std::malloc(p.size()+1);
         if (path == nullptr) { std::perror("malloc"); return; }
         strcpy(path, p.c_str());
         int fd = ::mkstemp(path);
         if (fd == -1) { std::perror("mkstemp"); return; }
         if ((fp = ::fdopen(fd, "wb")) == nullptr) std::perror("fdopen");
      }
      ~UpStrFile() {
         if (fp != nullptr && std::fclose(fp) != 0)
               std::perror("fclose");
         std::free(path);
      }
      bool operator!() const { return fp == nullptr; }
      virtual UpStrFile& operator<<(std::string s){
         if (fp != nullptr &&
            fwrite(s.c_str(), sizeof(char), s.size(), fp) != s.size())
            std::cerr << "write error\n";
         return *this;
      }
      virtual UpStrFile& operator<<(char ch) {
         if (fp != nullptr &&
            fwrite(&ch, sizeof(char), 1, fp) != 1)
            std::cerr << "write error\n";
         return *this;
      }
      std::string filepath() { return (fp != nullptr) ? string(path) : ""; }
   };

   size_t recv_and_write_to(UpStream& os,
                     const string& boundary, size_t max) {
      size_t x=0, match=0;
      bool success = true;
      while (true) {
         if (num == 0 && !refill_buf())
            break;
         size_t i=0;
         while (i<num) {
            if (ptr[i] == boundary[match]) {
               ++i; ++match;
               if (match == boundary.size()) {
                  l_shift_buf(i);
                  return (success ? x:(~0U));
               }
            } else {
               if (match > 0 && success)
                  os << boundary.substr(0,match);
               os << ptr[i++];
               x += match + 1;
               match = 0;
               if (x > max)
                  success = false;
            }
         }
         num = 0;
      }
      return ~0U; // failed
   }
}; // end of struct RecvBuffer


class Request {
 private:
   using data_str_t   = std::function<void(string)>;
   using data_chunk_t = std::function<void(Buffer)>;
   using voidfunc_t   = std::function<void(void)>;
   using closed_t     = std::function<void(void)>;

 public:
   string method;  // GET, PUT, POST, ...
   string url;
   string version;     // HTTP/1.0 or HTTP/1.1
   string httpVersion; // 1.0 or 1.1
   std::unordered_map<lower_string,string,lower_string::hash> headers;
   size_t bodysize;
   Request(RecvBuffer& b)
      :bodysize(0),chunked(false),buf(b),mode(PAUSED),prev(FLOWING){}

   Request& on(string label, const data_str_t& cb) {
      if (label != "data") return *this;
      callback_data_str = cb;
      mode = FLOWINGSTR;
      consume_body();
      return *this;
   }
   Request& on(string label, const data_chunk_t& cb) {
      if (label != "data") return *this;
      callback_data_chunk = cb;
      mode = FLOWING;
      consume_body();
      return *this;
   }
   Buffer body() {
      Buffer b;
      callback_data_chunk = [&b](Buffer chunk){ b += chunk; };
      mode = FLOWING;
      consume_body();
      return b;
   }
   Request& on(string label, const voidfunc_t& cb) {
      if (label == "end") {
         callback_end = cb;
         end_of_body();
      } else if (label == "close")
         callback_closed = cb;
      return *this;
   }
   Request& resume() {
      mode = prev;
      consume_body();
      return *this;
   }
   bool isPaused() { return mode == PAUSED; }
   Request& pause() {
      if (mode != PAUSED) {
         prev = mode;
         mode = PAUSED;
      }
      return *this;
   }

 private:
   void end_of_body() {
      if (callback_end != nullptr && bodysize == 0)
         callback_end();
      if (callback_closed != nullptr && buf.closed)
         callback_closed();
   }
   void consume_body() {
      if (method != "POST" && method != "PUT")
         std::cerr << "\ninvalid method("+method+")to read body\n";
      if (bodysize == 0 && !chunked) {
         std::cerr << "\nnull body\n";
         return;
      }

      while (!buf.is_completely_consumed() && mode != PAUSED) {
         std::cout << "\nconsume_body: "<< buf.num <<"\n";
         size_t num = buf.num;
         if (chunked) {
            string size;
            if (!buf.recv_a_line(size)) {
               std::cerr << "Bad Request\n";
               return;
            }
            num = std::stoi(size);
            if (num == 0) {
               string crlf;
               while (!buf.is_completely_consumed() && buf.recv_a_line(crlf)){
                  if (crlf == "\r\n") break;
                  std::cout << "Trailer(not implemented) --> " << crlf;
               }
               chunked = false;
               bodysize = 0;
               break;
            }
         } else {
            bodysize -= num;
         }

         if (mode == FLOWINGSTR) {
            string s;
            buf.recv_nbytes(s, num);
            callback_data_str(s);
         } else {
            Buffer c(num);
            buf.recv_nbytes(c.data, c.length);
            if (callback_data_chunk != nullptr)
               callback_data_chunk(c);
         } 

         if (chunked) { // read "\r\n" after raw data
            string s;
            buf.recv_nbytes(s, 2);
            std::cout<<"ok\n";
         } else if (bodysize == 0)
            break;
      }
      end_of_body();
   }

   bool chunked;
   RecvBuffer& buf; 
   enum {PAUSED, FLOWING, FLOWINGSTR} mode, prev;
   data_str_t   callback_data_str;
   data_chunk_t callback_data_chunk;
   voidfunc_t   callback_end;
   voidfunc_t   callback_closed;
   friend class WebSvr;
   friend class Upload;
}; // end of class Request


class WebSvr {
#define RCVTIMEOUT0 15
#define RCVTIMEOUT1 60

public:

void listen(unsigned short int port) {
   std::cout << "port = " << port << "\n";

   int sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
   if (sockfd == -1) return std::perror("socket");

   sockaddr_in svr;
   std::memset(&svr, 0, sizeof(svr));
   svr.sin_family      = AF_INET;
   svr.sin_addr.s_addr = htonl(INADDR_ANY);
   svr.sin_port        = htons(port);
   const int on = 1;
   if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
      return std::perror("setsockopt");
   if (::bind(sockfd, (sockaddr*)&svr, sizeof(svr)) == -1)
      return std::perror("bind");

   const int listenq = 5;
   if (::listen(sockfd, listenq) == -1) return std::perror("listen");

   select_and_doit(sockfd);
}

private:
void select_and_doit(int sockfd)
{
   fd_set readfds;
   FD_ZERO(&readfds);
   FD_SET(sockfd, &readfds);

   int nfds = sockfd + 1;

   auto disconnect = [&](int i) {
               ::close(i);
               FD_CLR(i, &readfds);
               delete recvbuf[i];
               recvbuf.erase(i);
   };
   while (true) {
      fd_set av; // available
      std::memcpy(&av, &readfds, sizeof(readfds));
      struct timeval tv = { .tv_sec = RCVTIMEOUT0, .tv_usec = 0 };

      int n = ::select(nfds, &av, nullptr, nullptr, &tv);
      if (n == -1) perror("select");
      else if (n == 0) {
         // std::cout << "time out\n";
         for (int j = nfds-1; j >= 0; --j) {
            if (FD_ISSET(j, &readfds) && recvbuf[j] != nullptr) {
               disconnect(j);
               if (j+1 == nfds)
                  -- nfds;
               std::cout << "closed " << j <<"/"<< nfds <<"\n";
            }
         }
         continue;
      }

      // n I/Os are available
      for (int i = 0, max = nfds; i < max; i++) {
         if (FD_ISSET(i, &av)) {
            if (i == sockfd) {  // new connection
               sockaddr_in cli;
               socklen_t clen = sizeof(cli);
               int fd = ::accept(sockfd, (sockaddr*)&cli, &clen);
               if (fd < 0) { perror("accept"); continue; }
               FD_SET(fd, &readfds);
               if (nfds <= fd+1) nfds = fd+1;
               std::cout << "accept " << fd <<"/"<< nfds <<"\n";
               recvbuf[fd] = new RecvBuffer(fd);
            } else if (!http_request(i)) {
               disconnect(i);
               if (i+1 == nfds) {
                  for (int j = i; j > sockfd && !FD_ISSET(j, &readfds); j--)
                     --nfds;
               }
               std::cout << "closed " << i <<"/"<< nfds <<"\n";
            }
         }
      }
   }
}

inline
string trim(string a)
{
   if (a.empty()) return a;
   size_t sidx = 0;
   size_t eidx = a.size()-1;
   while (sidx <= eidx && a[sidx] == ' ') ++sidx;
   while (sidx <= eidx && a[eidx] == ' ') --eidx;
   return a.substr(sidx, eidx-sidx+1);
}


bool http_request(int sfd)
{
   RecvBuffer* bufptr = recvbuf[sfd];
   if (bufptr == nullptr)
      return (upgrade_callback != nullptr) ? upgrade_callback(sfd) : false;

   RecvBuffer& buf = *bufptr;
   Request req(buf);
   bool statusline = true;
   string prev_key;
   buf.set_recv_timeout(RCVTIMEOUT0);
   while (true) { // retrieve a status line and header fields
      string line;
      if (!buf.recv_a_line(line))
         return false;

      size_t idx = line.find("\r\n");
      if (idx == 0) break;   // end of header section
      if (idx == string::npos) {
         std::cerr << "premature end of script headers:"<<line.size()<<"\n";
         std::cerr << line <<"\n";
         return false;
      }

      line = line.substr(0, idx);

      if (statusline) { // first line includes method/url/version
         std::istringstream iss(line);
         if (!(iss >> req.method >> req.url >> req.version)) {
            std::cerr << "method not found\n";
            return false;
         }
         statusline = false;
         buf.set_recv_timeout(RCVTIMEOUT1);

         std::cout <<req.method<<" "<<req.url<<" "<<req.version<<"\n";

      } else {
         idx = line.find(":");
         if (idx == string::npos) {
            if (line.size() > 0
            && (line[0] == ' ' || line[0] == '\t')
            && !prev_key.empty()) {
               // line of continuation
               req.headers[prev_key] += " " + trim(line);
            } else
               std::cerr << "invalid field: " << line << ":\n";
            continue;
         }
         lower_string key   = trim(line.substr(0, idx));
         string value = trim(line.substr(idx+1));
         if (key.empty()) {
            std::cerr << "invalid field: " << line << ":\n";
            continue;
         }
         if (req.headers.find(key) != req.headers.end()) {
            // Multiple message-header fields with the same field-name
            std::cout << "Multiple message-header: "<< key << "\n";
            req.headers[key] += ", "+value;
            continue;
         }
         req.headers[key] = value;
         prev_key = key;
         if (key == "content-length")
            req.bodysize = std::stoi(value);
         else
         if (key == "transfer-encoding" && value != "identity") {
            req.chunked = true;
            req.bodysize = 0;
         }

         std::cout << key << ": " << value <<"\n";
      }
   }
   std::cout <<"\n";

   double version = 0.0;
   if (req.version.find("HTTP/") == string::npos)
      std::cerr << "invalid version\n";
   else {
      req.httpVersion = req.version.substr(5);
      version = std::stof(req.httpVersion);
   }

   bool keepalive = (version >= 1.1 && 
                     casecmp(req.headers["Connection"],"keep-alive"));
   Response res(sfd, keepalive);

   if (upgrade_check != nullptr && upgrade_check(req,res)) {
      delete recvbuf[sfd];
      recvbuf[sfd] = nullptr;
      return true;
   }

   http_callback(req, res);
   return res.keepalive;
}

public:
   using http_callback_t = std::function<void(Request&,Response&)>;
   using upgrade_check_t = std::function<bool(Request&,Response&)>;
   using upgrade_callback_t = std::function<bool(int)>;
   http_callback_t http_callback;
   upgrade_check_t upgrade_check;
   upgrade_callback_t upgrade_callback;
   std::unordered_map<int,RecvBuffer*> recvbuf;
   WebSvr(http_callback_t c, upgrade_check_t u, upgrade_callback_t w)
      :http_callback(c),upgrade_check(u),upgrade_callback(w){}

#undef RCVTIMEOUT0
#undef RCVTIMEOUT1
}; // end of WebSvr

// factory function
inline
WebSvr createServer(WebSvr::http_callback_t c){
         return WebSvr(c, nullptr, nullptr);
};

inline
WebSvr createServer(WebSvr::http_callback_t c,
                    WebSvr::upgrade_check_t u,
                    WebSvr::upgrade_callback_t w){
         return WebSvr(c, u, w);
};

struct UploadFile {
   size_t size;
   string path;
   string name;
   string mimetype;
};

using Fields = std::unordered_map<string,string>;
using Files  = std::unordered_map<string,UploadFile>;


inline
std::unordered_map<string,string>
parse_prop_value(string s)
{
   size_t i = 0;
   std::unordered_map<string,string> p;
   while (i < s.size()) {
      string tmp1, tmp2;
      while (i < s.size() && s[i]==' ') ++i;
      while (i < s.size() && s[i]!='=' && s[i]!=';' && std::isgraph(s[i]))
         tmp1 += s[i++];
      while (i < s.size() && std::isblank(s[i])) ++i;
      if (i == s.size() || std::iscntrl(s[i])) {
         p[""] = tmp1;
         break;
      }
      if (s[i] == ';') {
         p[""] = tmp1;
         ++ i;
         continue;
      }
      if (s[i] != '=') break; // error
      ++i;
      if (s[i]!='\"') {
         while (i < s.size() && !std::isspace(s[i]) && s[i]!=';')
            tmp2 += s[i++];
      } else {
         ++i;
         while (i < s.size() && s[i]!='\"' && std::isprint(s[i]))
            tmp2 += s[i++];
         if (i == s.size() || s[i]!='\"') break; //error
         ++i;
      }
      p[tmp1] = tmp2;
      while (i < s.size() && std::isspace(s[i])) ++i;
      if (i < s.size() && s[i]==';') ++i;
      else break;
   }
   return p;
}

inline
std::pair<string,std::unordered_map<string,string>>
parse_property(string s)
{
   string label;
   size_t idx = 0;
   while (idx < s.size() && s[idx]==' ') ++idx;
   while (idx < s.size() && s[idx]!=' ' && s[idx]!=':')
      label += s[idx++];
   while (idx < s.size() && s[idx]==' ') ++idx;
   if (s[idx]!=':')
      return std::make_pair(label, std::unordered_map<string,string>());
   ++idx;
   return std::make_pair(label, parse_prop_value(s.substr(idx)));
}

class Upload { // formidable-like
   using field_t = std::function<void(string,string)>;
   using file_t  = std::function<void(string,const UploadFile&)>;
   using err_t   = std::function<void(string)>;
   using end_t   = std::function<void(void)>;
   using parse_t = std::function<void(string,const Fields&,const Files&)>;
 public:
   Upload& on(string label, const field_t& cb) {
      if (label == "field") callback_field = cb;
      return *this;
   }
   Upload& on(string label, const file_t& cb) {
      if (label == "file") callback_file  = cb;
      return *this;
   }
   Upload& on(string label, const err_t& cb) {
      if (label == "error") callback_err = cb;
      return *this;
   }
   Upload& on(string label, const end_t& cb) {
      if (label == "end") callback_end = cb;
      return *this;
   }
   Upload& parse(Request& req, const parse_t& cb =nullptr) {
      // retrieve boundary
      string s = req.headers["Content-Type"];
      if (s.empty()) return error("no Content-Type");
      auto m = parse_prop_value(s);
      if (m[""] != "multipart/form-data")
         return error("not multipart/form-data: "+m[""]);
      s = m["boundary"];
      if (s.empty()) return error("no boundary");

      Files files;
      Fields fields;
      string err;

      if (!process_each_part(0, files, fields, req.buf, s, err))
         error(err);

      if (cb != nullptr)
         cb(err, fields, files);

      if (callback_end != nullptr)
         callback_end();

      return *this;
   }
   static const size_t defaultmax = 20*1024*1024;
   Upload(size_t m ,string s) :maxsize(m),upload_dir(s){}
   Upload(string s) :maxsize(defaultmax),upload_dir(s){}
   Upload(size_t m =defaultmax) :maxsize(m),upload_dir(string("/tmp")){}

 private:
   
   bool process_each_part(int nestedlevel, Files& files, Fields& fields,
         RecvBuffer& buf, string bv, string& err) {
      string boundary = "--" + bv + "\r\n";
      if (nestedlevel > 1) {
         err = "too much nested level\n";
         return false;
      }
      // read each part
      string s;
      if (!buf.recv_a_line(s) || s != boundary) {
         if (s == ("--" + bv + "--\r\n")) {
            err = "no part is found; probably missing name field in input tag";
         } else {
            std::cerr<<s.size() << ": " << boundary.size()<<"\n";
            std::cerr<<s<<"\n";
            std::cerr<<boundary<<"\n";
            err = "there is no start-boundary";
         }
         return false;
      }
      while (true) {
         bool fileflg=false;
         UploadFile up;
         string field;

         // read Content-xxxx part
         while (true) {
            if (!buf.recv_a_line(s)) {
               err = "Content-XXX part error"; return false;
            }
            if (s == "\r\n") break;
            auto x = parse_property(s);
            if (x.first == "Content-Disposition") {
               auto& y = x.second;
               if (y[""] != "form-data") { err = "no form-data"; return false; }
               if (y["name"].empty()) { err = "no name field"; return false; }
               field = y["name"];
               if (!y["filename"].empty()) {
                  fileflg = true;
                  up.name = y["filename"];
               }
            } else
            if (x.first == "Content-Type") {
               up.mimetype = x.second[""];
            } else {
               std::cerr << x.first << " is ignored\n";
            } 
         }

         // read a body of multipart content
         string eboundary = "\r\n--" + bv;
         if (fileflg) { // for uploading a file
            RecvBuffer::UpStrFile os(upload_dir);
            if (!os) {
               err = "file error"; return false;
            }
            up.path = os.filepath();
            up.size = buf.recv_and_write_to(os, eboundary, maxsize);
            if (up.size == ~0U) { // error
               err = "upload file content error"; return false;
            }
            files[field] = up;
            if (callback_file != nullptr)
               callback_file(field, up);
         } else { // for sending a string
            RecvBuffer::UpStrStr os;
            size_t x = buf.recv_and_write_to(os, eboundary, maxsize);
            if (x == ~0U) { // error
               err = "upload string content error"; return false;
            }
            fields[field] = os.str();
            if (callback_field != nullptr)
               callback_field(field, os.str());
         }

         // final boundary check: "\r\n" or "--\r\n"
         string ends;
         if (buf.recv_nbytes(ends, 2) != 2) {
            err = "boundary error"; return false;
         }
         if (ends == "\r\n")
            continue;
         if (ends != "--") {
            err = "boundary error"; return false;
         }
         ends.clear();
         if (buf.recv_nbytes(ends, 2) != 2) {
            err = "boundary error"; return false;
         }
         if (ends != "\r\n") {
            err = "boundary error"; return false;
         }
         break;
      }
      return true; // success
   }
   Upload& error(string e) {
      std::cerr << e << "\n";
      if (callback_err != nullptr) callback_err(e);
      return *this;
   }
   size_t maxsize;
   string upload_dir;
   field_t callback_field;
   file_t  callback_file;
   err_t   callback_err;
   end_t   callback_end;
};

}; // end of namespace http


/////////////////// utilities
namespace url {

inline
std::unordered_map<std::string,std::string> 
parse(std::string url)
{
   std::regex regexp (
      R"(^(([^:\/?#]+):)?)"
      R"((//((([a-zA-Z0-9_]*)(:[^@]*)?)@)?(([^\/?#:]*)(:([0-9]+))?))?)"
      R"(([^?#]*)(\?([^#]*))?(#(.*))?)",   
      std::regex::extended
   );
   std::smatch match;
   std::unordered_map<std::string,std::string> p;

   if (!std::regex_match(url, match, regexp)) {
      std::cerr << "Malformed URL.\n";
      return p;
   }
   int cnt = 0; 
   for (const auto& resp : match) {
      std::string res = resp;
      //std::cout << cnt <<":"<< resp << "\n";
      switch (cnt) {
      case 0:  p["href"]     = res; break;
      case 1:  p["protocol"] = res; break;
      case 5:  p["auth"]     = res; break;
      case 8:  p["host"]     = res; break;
      case 9:  p["hostname"] = res; break;
      case 11: p["port"]     = res; break;
      case 12: p["pathname"] = ((!res.empty() && res[0]=='/')?res:("/"+res));
               break;
      case 13: p["search"]   = res; break;
      case 14: p["query"]    = res; break;
      case 15: p["hash"]     = res; break;
      }  
      ++cnt;
   }  
   p["path"] = p["pathname"]+p["search"];
   return p;
}

}; // end of namespace URL



namespace querystring {

inline
int getnum(char c1, char c2) {
   int a = std::isdigit(c1) ? (c1 - '0')      :
           std::isupper(c1) ? (c1 - 'A' + 10) :
           std::islower(c1) ? (c1 - 'a' + 10) : 0;
   int b = std::isdigit(c2) ? (c2 - '0')      :
           std::isupper(c2) ? (c2 - 'A' + 10) :
           std::islower(c2) ? (c2 - 'a' + 10) : 0;
   return 16*a + b;
}

inline
std::string decode(std::string s) {
   std::string x;
   for (size_t i = 0; i < s.size(); i++) {
      switch (s[i]) {
      case '+': x += " "; break;
      case '%': if (i+2 < s.size()) {
                   x += getnum(s[i+1],s[i+2]);
                   i += 2;
                   break;
                }
      default:  x += s[i];
      }
   }
   return x;
}

inline
std::unordered_map<std::string, std::string>
parse(std::string qs) {
   std::istringstream in(qs);
   std::string line;
   std::unordered_map<std::string, std::string> m;
   while (std::getline(in, line, '&')) {
      if (line.size()==0) continue;
      size_t p = line.find('=');
      if (p == std::string::npos) {
         m[line] = "";
         continue;
      }
      std::string key   = decode(line.substr(0,p));
      std::string value = decode(line.substr(p+1));
      m[key] = value;
   }
   return m;
}

}; // end of namespace QueryString
