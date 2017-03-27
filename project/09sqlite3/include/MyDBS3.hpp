// version 20160211
/* Copyright (C) 2013-2016 Shusuke Okamoto, All rights reserved.
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
#include <sqlite3.h>
#include <string>
#include <utility> // pair

class MyDBS {
   private:
      void clear_stmt() {
         sqlite3_reset( stmt );
         sqlite3_clear_bindings( stmt );  /* optional */
         sqlite3_finalize(stmt);
         stmt = nullptr;
      }
   public:
      using Blob = std::pair<void*,int>;

      MyDBS(std::string file):stmt(nullptr) {
         sqlite3_initialize();
         rc = sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE |
                                                 SQLITE_OPEN_CREATE, nullptr);
         if (rc != SQLITE_OK) {
            sqlite3_close(db);
            db = nullptr;
         }
      }

      ~MyDBS() {
         if (stmt) clear_stmt();
         if (db) sqlite3_close(db);
         sqlite3_shutdown();
      }
 
      std::string error()      const { return sqlite3_errstr(rc); }
      std::string error(int x) const { return sqlite3_errstr(x); }
      bool operator!()         const { return db == nullptr; }

      int exec(std::string s) {
         if (db == nullptr) return rc; 
         return (rc = sqlite3_exec(db, s.c_str(), nullptr, nullptr, nullptr));
      }

      template <typename... Args>
      int exec(std::string s, Args... args) {
         if (db == nullptr) return rc; 
         if (stmt) clear_stmt();

         rc = sqlite3_prepare_v2(db, s.c_str(), -1, &stmt, nullptr);
         if (rc != SQLITE_OK || stmt == nullptr) return rc;

         rc = bind(1, args...);
         if (rc != SQLITE_OK) return rc;
         while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
            continue;
         clear_stmt();
         if (rc != SQLITE_DONE) return rc;
         return SQLITE_OK;
      }

   private:
      int bind(int) const { return SQLITE_OK; }

   public:
      template <typename... Args>
      int bind(int idx, int n, Args... args) {
         rc = sqlite3_bind_int(stmt, idx, n);
         return (rc != SQLITE_OK) ? rc : bind(idx+1, args...);
      }

      template <typename... Args>
      int bind(int idx, double n, Args... args) {
         rc = sqlite3_bind_double(stmt, idx, n);
         return (rc != SQLITE_OK) ? rc : bind(idx+1, args...);
      }

      template <typename... Args>
      int bind(int idx, std::string n, Args... args) {
         rc = sqlite3_bind_text(stmt, idx,n.c_str(),n.size(), SQLITE_TRANSIENT);
         return (rc != SQLITE_OK) ? rc : bind(idx+1, args...);
      }

      template <typename... Args>
      int bind(int idx, Blob n, Args... args) {
         rc = sqlite3_bind_blob(stmt, idx, n.first, n.second, SQLITE_TRANSIENT);
         return (rc != SQLITE_OK) ? rc : bind(idx+1, args...);
      }

      int bind(const char* s, int n) {
         int idx = sqlite3_bind_parameter_index(stmt, s);
         if (idx == 0) return SQLITE_ERROR;
         rc = sqlite3_bind_int(stmt, idx, n);
         return rc;
      }

      int bind(const char* s, double n) {
         int idx = sqlite3_bind_parameter_index(stmt, s);
         if (idx == 0) return SQLITE_ERROR;
         rc = sqlite3_bind_double(stmt, idx, n);
         return rc;
      }

      int bind(const char* s, std::string n) {
         int idx = sqlite3_bind_parameter_index(stmt, s);
         if (idx == 0) return SQLITE_ERROR;
         rc = sqlite3_bind_text(stmt, idx,n.c_str(),n.size(), SQLITE_TRANSIENT);
         return rc;
      }

      int bind(const char* s, Blob n) {
         int idx = sqlite3_bind_parameter_index(stmt, s);
         if (idx == 0) return SQLITE_ERROR;
         rc = sqlite3_bind_blob(stmt, idx, n.first, n.second, SQLITE_TRANSIENT);
         return rc;
      }


      int prepare(std::string s) {
         if (stmt) clear_stmt();
         rc = sqlite3_prepare_v2(db, s.c_str(), s.size(), &stmt, nullptr);
         return rc;
      }
  
      template <typename... Args>
      int prepare(std::string s, Args... args) {
         rc = prepare(s);
         return (rc != SQLITE_OK) ? rc : bind(1, args...);
      }

      int reset() { return (rc = sqlite3_reset( stmt )); }
      int clear_bindings() { return (rc = sqlite3_clear_bindings( stmt )); }
      std::string sql() { return stmt ? sqlite3_sql(stmt) : ""; }

      int step() {
         if (db == nullptr || stmt == nullptr) return SQLITE_ERROR; 
         return (rc = sqlite3_step(stmt));
      }

      template <typename... Args>
      int step(Args... args) {
         rc = step();
         if (rc != SQLITE_ROW && rc != SQLITE_DONE) return rc;
         int n = sqlite3_column_count(stmt);
         if (rc == SQLITE_ROW && n > 0) fetch(0, n, args...);
         return rc;
      }

    private:
      void fetch(int,int) {}

      template <typename... Args>
      void fetch(int i, int n, int* x, Args... args) {
         if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER)
            *x = sqlite3_column_int(stmt, i);
         if (i+1 < n) fetch(i+1, n, args...);
      }

      template <typename... Args>
      void fetch(int i, int n, std::string* x, Args... args) {
         if (sqlite3_column_type(stmt, i) == SQLITE_TEXT) {
            const char* pt = (const char*)sqlite3_column_text(stmt, i);
            *x = std::string(pt?pt:"");
         }
         if (i+1 < n) fetch(i+1, n, args...);
      }

      template <typename... Args>
      void fetch(int i, int n, double* x, Args... args) {
         if (sqlite3_column_type(stmt, i) == SQLITE_FLOAT)
            *x = sqlite3_column_double(stmt, i);
         if (i+1 < n) fetch(i+1, n, args...);
      }

    private:
      sqlite3       *db;
      sqlite3_stmt  *stmt;
      int            rc;
};
