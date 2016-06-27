#include "common.h"
#include "json.h"
#include "mongoose.h"

class Server {
public:
  Server(int port = 5757);
  void run();
private:
  int port_;
  mg_server* server_;
  static int static_handler(mg_connection* con, mg_event ev);
  int handler(mg_connection* con, mg_event ev);

  void handle_uri(mg_connection* con, File& output, std::string const& uri);

  json::Value versions_;
  Archive icons_;
};