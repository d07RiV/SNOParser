#include "server.h"
#include "logger.h"
#include "path.h"
#include "regexp.h"
#include <algorithm>

class SocketBuffer : public FileBuffer {
public:
  SocketBuffer(mg_connection* con)
    : con_(con)
  {}

  uint64 tell() const { return 0; }
  void seek(int64 pos, int mode) {}
  uint64 size() { return 0; }
  size_t read(void* ptr, size_t size) { return 0; }

  size_t write(void const* ptr, size_t size) {
    mg_send_data(con_, ptr, size);
    return size;
  }

private:
  mg_connection* con_;
};

Server::Server(int port)
  : port_(port)
{
  server_ = mg_create_server(this, static_handler);
  mg_set_option(server_, "listening_port", fmtstring("%d", port).c_str());

  File icons(path::work() / "icons.wgz");
  if (icons) icons_.load(icons, false);

  json::parse(File(path::work() / "versions.js"), versions_);
}

int Server::static_handler(mg_connection* con, mg_event ev) {
  Server* srv = (Server*) con->server_param;
  return srv->handler(con, ev);
}

int Server::handler(mg_connection* con, mg_event ev) {
  switch (ev) {
  case MG_AUTH:
    return MG_TRUE;
  case MG_REQUEST:
    handle_uri(con, File(new SocketBuffer(con)), con->uri);
    return MG_TRUE;
  default:
    return MG_FALSE;
  }
}

void Server::run() {
  Logger::log("Server started at localhost:%d", port_);
  Logger::log("Press Ctrl+C or close the window to stop");
  ShellExecute(NULL, "open", fmtstring("http://localhost:%d/game/items", port_).c_str(), NULL, NULL, SW_SHOWNORMAL);
  while (true) {
    mg_poll_server(server_, 100);
  }
}

void Server::handle_uri(mg_connection* con, File& output, std::string const& uri) {
  re::Prog re_icons("/webgl/icons/(\\d+)");
  re::Prog re_game("/game/(items|itemsets){/(\\d*)}?");
  re::Prog re_diff("/diff/(items|itemsets|powers){/{(\\d+).(\\d+)}?}?");
  std::vector<std::string> sub;

  if (uri == "/external/jquery.min.js") {
    File input(path::work() / "jquery.min.js");
    mg_send_status(con, 200);
    mg_send_header(con, "Content-Type", "application/javascript; charset=utf-8");
    mg_send_header(con, "Content-Length", fmtstring("%I64d", input.size()).c_str());
    output.copy(input);
    return;
  }
  
  if (re_icons.match(uri, &sub)) {
    uint32 id;
    sscanf(sub[1].c_str(), "%u", &id);
    File file = icons_.open(id);
    if (file) {
      file.seek(0);
      mg_send_status(con, 200);
      mg_send_header(con, "Content-Type", "image/png");
      mg_send_header(con, "Content-Length", fmtstring("%I64d", file.size()).c_str());
      output.copy(file);
      return;
    }
  }

  if (re_game.match(uri, &sub)) {
    std::string type = sub[1];
    std::vector<uint32> versions;
    for (auto const& kv : versions_.getMap()) {
      uint32 ver = std::stoi(kv.first);
      if (File::exists(path::root() / fmtstring("game/%s.%d.html", type.c_str(), ver))) {
        versions.push_back(ver);
      }
    }
    if (versions.empty()) {
      mg_send_status(con, 200);
      mg_send_header(con, "Content-Type", "text/plain; charset=utf-8");
      output.printf("No game data has been extracted");
      return;
    }
    std::sort(versions.begin(), versions.end(), [](uint32 a, uint32 b) { return a > b; });

    uint32 version;
    if (sub.size() > 2 && sub[2].size()) {
      version = std::stoi(sub[2]);
      if (!File::exists(path::root() / fmtstring("game/%s.%d.html", type.c_str(), version))) {
        version = versions[0];
      }
    } else {
      version = versions[0];
    }

    File input(path::root() / fmtstring("game/%s.%d.html", type.c_str(), version));
    std::string content;
    content.resize(input.size());
    input.read(&content[0], content.size());

    size_t navpos = content.find("{NAVBAR}");
    if (navpos != std::string::npos) {
      std::string navbar = (type == "items" ? "" : fmtstring("<a href=\"/game/items/%d\">", version)) + "Items" + (type == "items" ? "" : "</a>");
      navbar += " | " + (type == "itemsets" ? "" : fmtstring("<a href=\"/game/itemsets/%d\">", version)) + "Sets" + (type == "itemsets" ? "" : "</a>");
      navbar += fmtstring(" | <a href=\"/diff/%s\">Diff</a>", type.c_str());
      navbar += "<br/>Version <select class=\"version\">";
      for (uint32 v : versions) {
        navbar += fmtstring("<option value=\"%d\"%s>%s</option>", v, v == version ? " selected=\"selected\"" : "", versions_[fmtstring("%d", v)].getString().c_str());
      }
      navbar += "</select>";

      content.replace(navpos, 8, navbar);
    }

    mg_send_status(con, 200);
    mg_send_header(con, "Content-Type", "text/html; charset=utf-8");
    mg_send_header(con, "Content-Length", fmtstring("%d", content.size()).c_str());
    output.write(content.data(), content.size());
    return;
  }

  if (re_diff.match(uri, &sub)) {
    std::string type = sub[1];

    typedef std::pair<uint32, uint32> vpair;
    std::vector<vpair> versions;

    re::Prog re_name(type + "\\.(\\d+)\\.(\\d+)\\.html");
    WIN32_FIND_DATA fdata;
    HANDLE hFind = FindFirstFile((path::root() / fmtstring("diff/%s.*.html", type.c_str())).c_str(), &fdata);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        std::vector<std::string> sub2;
        if (re_name.match(fdata.cFileName, &sub2)) {
          versions.push_back(vpair(std::stoi(sub2[1]), std::stoi(sub2[2])));
        }
      } while (FindNextFile(hFind, &fdata));
      FindClose(hFind);
    }
    if (versions.empty()) {
      mg_send_status(con, 200);
      mg_send_header(con, "Content-Type", "text/plain; charset=utf-8");
      output.printf("No diff files found");
      return;
    }
    std::sort(versions.begin(), versions.end(), [](vpair const& a, vpair const& b) { return a > b; });

    vpair version;
    if (sub.size() >= 4 && sub[2].size() && sub[3].size()) {
      version = vpair(std::stoi(sub[2]), std::stoi(sub[3]));
      if (!File::exists(path::root() / fmtstring("diff/%s.%d.%d.html", type.c_str(), version.first, version.second))) {
        version = versions[0];
      }
    } else {
      version = versions[0];
    }

    File input(path::root() / fmtstring("diff/%s.%d.%d.html", type.c_str(), version.first, version.second));
    std::string content;
    content.resize(input.size());
    input.read(&content[0], content.size());

    size_t navpos = content.find("{NAVBAR}");
    if (navpos != std::string::npos) {
      std::string navbar = (type == "items" ? "" : fmtstring("<a href=\"/diff/items/%d/%d\">", version.first, version.second)) + "Items" + (type == "items" ? "" : "</a>");
      navbar += " | " + (type == "itemsets" ? "" : fmtstring("<a href=\"/diff/itemsets/%d/%d\">", version.first, version.second)) + "Sets" + (type == "itemsets" ? "" : "</a>");
      navbar += " | " + (type == "powers" ? "" : fmtstring("<a href=\"/diff/powers/%d/%d\">", version.first, version.second)) + "Skills" + (type == "powers" ? "" : "</a>");
      if (type != "powers") {
        navbar += fmtstring(" | <a href=\"/game/%s/%d\">%s</a>", type.c_str(), version.first, versions_[fmtstring("%d", version.first)].getString().c_str());
        navbar += fmtstring(" | <a href=\"/game/%s/%d\">%s</a>", type.c_str(), version.second, versions_[fmtstring("%d", version.second)].getString().c_str());
      }
      navbar += "<br/>Version <select class=\"version\">";
      for (vpair const& v : versions) {
        navbar += fmtstring("<option value=\"%d/%d\"%s>%s to %s</option>", v.first, v.second, v == version ? " selected=\"selected\"" : "",
          versions_[fmtstring("%d", v.first)].getString().c_str(), versions_[fmtstring("%d", v.second)].getString().c_str());
      }
      navbar += "</select>";

      content.replace(navpos, 8, navbar);
    }

    mg_send_status(con, 200);
    mg_send_header(con, "Content-Type", "text/html; charset=utf-8");
    mg_send_header(con, "Content-Length", fmtstring("%d", content.size()).c_str());
    output.write(content.data(), content.size());
    return;
  }

  mg_send_status(con, 404);
  mg_send_header(con, "Content-Type", "text/plain; charset=utf-8");
  output.printf("Document at %s not found", uri.c_str());
}
