//
// sqlite3ライブラリ関数の基本的な使い方
//
#include <iostream>
#include <sqlite3.h>
using namespace std;

int do_query(sqlite3* db)
{
   string sql = "select sid,name,age from student where age <= ?";

   /* create a statement from an SQL string */
   sqlite3_stmt *stmt = NULL;
   sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );

   cout << sqlite3_sql(stmt) << "\n";

   int age;
   /* use the statement as many times as required */
   while(cout<<"age:" && cin >> age && age > 0)
   {
      /* bind any parameter values */
      int param_idx = 1;
      int param_value = age;
      sqlite3_bind_int(stmt, param_idx, param_value);

       /* execute statement and step over each row of the result set */
       while ( sqlite3_step( stmt ) == SQLITE_ROW )
       {
           /* extract column values from the current result row */
           cout << sqlite3_column_int(stmt, 0)  << ", ";
           cout << sqlite3_column_text(stmt, 1) << ",\t";
           cout << sqlite3_column_int(stmt, 2)  << "\n";
       }

       /* reset the statement so it may be used again */
       sqlite3_reset( stmt );
       sqlite3_clear_bindings( stmt );  /* optional */
   }

   /* destroy and release the statement */
   sqlite3_finalize( stmt );
}

int main(int argc, char *argv[])
{
   const char *file = "student.db";
   sqlite3    *db = NULL;
   if (argc > 1)
      file = argv[1];

   sqlite3_initialize(); // ライブラリ全体の初期化
   int rc = sqlite3_open_v2(file, &db, SQLITE_OPEN_READWRITE |
                                       SQLITE_OPEN_CREATE, NULL);
   if (rc != SQLITE_OK) {
      sqlite3_close(db);
      return 1;
   }

   do_query(db);

   sqlite3_close(db);
   sqlite3_shutdown(); // ライブラリ全体の後片付け
}
