// Web Application Handlers
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
#include <cstdio>
#include <utility> // pair
#include <unordered_map>
#include <regex>
#include <vector>
#include <functional>
#include <ctime>
#include "websvr.hpp"
#include "ws.hpp"

namespace http {

class Mime {
   std::unordered_map<std::string,std::string> tbl;
public:
   Mime() :tbl({
{"htm","text/html"},
{"html","text/html"},
{"txt","text/plain"},
{"text","text/plain"},
{"c","text/plain"},
{"h","text/plain"},
{"cpp","text/plain"},
{"conf","text/plain"},
{"cfg","text/plain"},
{"log","text/plain"},
{"jpg","image/jpeg"},
{"jpeg","image/jpeg"},
{"png","image/png"},
{"bmp","image/bmp"},
{"gif","image/gif"},
{"mp3","audio/mpeg"},
{"m4a","audio/m4a"},
{"oga","audio/ogg"},
{"ogg","audio/ogg"},
{"ps","application/postscript"},
{"eps","application/postscript"},
{"zip","application/zip"},
{"doc","application/msword"},
{"xls","application/vnd.ms-excel"},
{"ppt","application/vnd.ms-powerpoint"},
{"pdf","application/pdf"},
{"xlsx","application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
{"pptx","application/vnd.openxmlformats-officedocument.presentationml.presentation"},
{"docx","application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
}
){}
   std::string lookup(const std::string& p) {
      return extention(p.substr(p.find_last_of('.')+1));
   }
   std::string extention(const std::string& s) {
      auto it = tbl.find(s);
      if (it == tbl.end())
         return "application/octet-stream";
      return it->second; 
  }
};

class Cookie {
   std::string cn, cv;
   std::unordered_map<std::string,std::string> av;
   std::string
   cookie_time(time_t t)
   {
      const int sz = 32; // 30 is enough for this purpose
      char a[sz]; 
      strftime(a, sz, "%a, %d %b %Y %T %Z", localtime(&t));
      return a;
   }

public:
   Cookie(std::string n, std::string v):cn(n),cv(v) {}
   Cookie& maxage(int v) { av["max-age"] = std::to_string(v); return *this; }
   Cookie& expires(time_t t) { av["expires"] = cookie_time(t); return *this; }
   Cookie& secure() { av["secure"] = "true"; return *this; }
   Cookie& httponly() { av["httponly"] = "true"; return *this; }
   Cookie& path(std::string p) { av["path"] = p; return *this; }
   Cookie& domain(std::string d) { av["domain"] = d; return *this; }
   std::string serialize() const {
      std::string cs = cn + "=" + cv;
      for (auto a:av) {
         if (a.first == "secure" || a.first == "httponly")
            cs += "; " + a.first;
         else
            cs += "; " + a.first + "=" + a.second;
      }
      return cs;
   }
};

class AppRequest: public Request {
   std::unordered_map<std::string,std::string> pathparams;
   std::unordered_map<std::string,std::string> cookies;

   std::pair<std::string,std::vector<std::string>>
   path2regex(std::string path)
   {
      std::string x;
      std::vector<std::string> k;
      k.push_back("");
      for (size_t i = 0; i < path.size(); i++) {
         if (path[i] == ':') {
            ++i;
            std::string key;
            while (i < path.size() && path[i] != '/' && path[i] != '(') {
               key += path[i];
               ++i;
            }
            k.push_back(key);
            x += R"(([^\/]+?))";
            if (i >= path.size())
               break;
         }
         x += path[i];
      }
      return std::make_pair(x, k);
   }

   bool pathmatch(std::string pattern, std::string route) {
      auto ptn = path2regex(pattern);
      std::regex regexp(ptn.first);
      std::smatch match;
      if (!std::regex_match(route, match, regexp))
         return false;
      const auto& key = ptn.second;
      for (size_t i = 1; i < match.size(); i++)
         if (i < key.size())
            pathparams[key[i]] = match[i];
      return true;
   }
   friend class WebApp;

public:
   AppRequest(const Request& r) :Request(r){
      using std::string;
      auto it = r.headers.find("cookie");
      if (it == r.headers.end()) return;
      string c = it->second;
      while (!c.empty()) {
         auto idx = c.find("=");
         if (idx == string::npos)
            break;
         string n = c.substr(0, idx);
         c = c.substr(idx+1);
         idx = c.find("; ");
         string v = c.substr(0, idx);
         if (!n.empty() && !v.empty())
            cookies[n] = v;
         if (idx   == string::npos
          || idx+1 == string::npos)
            break;
         c = c.substr(idx+2);
      }
   }
   std::string params(const std::string& s) const {
      auto it = pathparams.find(s);
      return (it == pathparams.end()) ? std::string() : it->second;
   }
   std::string cookie(const std::string& s) const {
      auto it = cookies.find(s);
      return (it == cookies.end()) ? std::string() : it->second;
   }
};

class AppResponse: public Response{
public:
   AppResponse(const Response& r) :Response(r){}
   AppResponse& status(int x) {
      this->writeHead(x);
      return *this;
   }
   AppResponse& send(int x, std::string s) {
      std::string m = "<!DOCTYPE html>\r\n"
                      "<html>\r\n<head><meta charset=\"UTF-8\"></head>\r\n"
                      "<body>\r\n" + s +"\r\n</body></html>";
      this->writeHead(x, {{"Content-Length", std::to_string(m.size())}}).end(m);
      return *this;
   }
   AppResponse& send(std::string s) {
      this->send(200, s);
      return *this;
   }
   AppResponse& sendStatus(int x) {
      this->status(x).end();
      return *this;
   }
   AppResponse& sendFile(const std::string& path) {
      if (path.empty()) return *this;
      std::string basename = path.substr(path.find_last_of('/')+1);
      if (basename.empty() || basename[0] == '.') {
         this->send(403,"You don\'t have permission to access");
         return *this;
      }

      std::FILE *fp = std::fopen(path.c_str(), "r");
      if (fp == nullptr) {
         this->send(404,"404 Not Found **");
         return *this;
      }

      this->writeHead(200,{{"Content-Type", Mime().lookup(path)},
                           {"Transfer-Encoding", "chunked"}});
      const size_t bsize = 1024;
      Buffer b(bsize);
      size_t len;
      do {
         len = std::fread(b.data, 1, bsize, fp);
         this->write(b.data, len);
      } while( len == b.length );
      this->end();
      std::fclose(fp);
      return *this;
   }
   AppResponse& setCookie(const Cookie& c) {
      this->setHeader("Set-Cookie", c.serialize());
      return *this;
   }
};

class WebApp {
   using handler_t  = std::function<bool(AppRequest&,AppResponse&)>;
   using handlers_t = std::vector<std::pair<std::string, handler_t>>;
   handlers_t get_handlers;
   handlers_t post_handlers;
public:
   WebApp& get(std::string path, handler_t x) {
      get_handlers.push_back(std::make_pair(path, x));
      return *this;
   }

   template<typename C, typename M>
   WebApp& get(std::string path, const C& obj, M method) {
      using namespace std::placeholders;
      return this->get(path, std::bind(method, std::ref(obj), _1, _2));
   }

   WebApp& post(std::string path, handler_t x) {
      post_handlers.push_back(std::make_pair(path, x));
      return *this;
   }

   template<typename C, typename M>
   WebApp& post(std::string path, const C& obj, M method) {
      using namespace std::placeholders;
      return this->post(path, std::bind(method, std::ref(obj), _1, _2));
   }

   void handler(Request& req0, Response& res0) {
      AppRequest req(req0);
      AppResponse res(res0);
      std::string pathname = url::parse(req.url)["pathname"];
      handlers_t  handlers = (req.method == "GET") ? get_handlers :
                             (req.method == "POST")? post_handlers: 
                             handlers_t();
      for (auto &x: handlers) {
         if (req.pathmatch(x.first, pathname)) {
            if (x.second(req, res))
               return;
         }
      }
      res.send(404, "404 Not Found");
   }
};

using namespace std::placeholders;

inline
WebSvr createServer(WebApp& app) {
   return WebSvr(std::bind(&WebApp::handler, std::ref(app), _1, _2),
               nullptr, nullptr);
}

inline
WebSvr createServer(WebApp& app, ws::Handler& h) {
   return WebSvr(std::bind(&WebApp::handler, std::ref(app), _1, _2),
                 std::bind(&ws::Handler::upgrade, std::ref(h), _1, _2),
                 std::bind(&ws::Handler::handler, std::ref(h), _1) );
}


}; // end of namespace http
