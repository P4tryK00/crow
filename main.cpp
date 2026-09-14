#include "crow.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Header {
  std::string name;
  std::string title;
  std::string description;
  std::string contact_link;
  std::string linkedin_link;
  std::string github_link;
};

struct Project {
  int id;
  std::string title;
  std::string description;
  std::string githubUrl;
  std::vector<std::string> technologies;
  std::string long_description;
  std::string image;
  std::vector<std::string> screenshots;
  std::string liveUrl;
};

struct Experience {
  int id;
  std::string title;
  std::string company;
  std::string dates;
  std::string description;
};

struct SiteData {
  Header header;
  std::vector<Project> projects;
  std::vector<std::string> skills;
  std::vector<Experience> experience;
};

SiteData global_db;

void save_database() {
  crow::json::wvalue x;
  x["header"]["name"] = global_db.header.name;
  x["header"]["title"] = global_db.header.title;
  x["header"]["description"] = global_db.header.description;
  x["header"]["contact_link"] = global_db.header.contact_link;
  x["header"]["linkedin_link"] = global_db.header.linkedin_link;
  x["header"]["github_link"] = global_db.header.github_link;

  for (size_t i = 0; i < global_db.projects.size(); ++i) {
    x["projects"][i]["id"] = global_db.projects[i].id;
    x["projects"][i]["title"] = global_db.projects[i].title;
    x["projects"][i]["description"] = global_db.projects[i].description;
    x["projects"][i]["githubUrl"] = global_db.projects[i].githubUrl;
    x["projects"][i]["long_description"] =
        global_db.projects[i].long_description;
    x["projects"][i]["image"] = global_db.projects[i].image;
    x["projects"][i]["liveUrl"] = global_db.projects[i].liveUrl;
    for (size_t j = 0; j < global_db.projects[i].technologies.size(); ++j) {
      x["projects"][i]["technologies"][j] =
          global_db.projects[i].technologies[j];
    }
    for (size_t j = 0; j < global_db.projects[i].screenshots.size(); ++j) {
      x["projects"][i]["screenshots"][j] = global_db.projects[i].screenshots[j];
    }
  }

  for (size_t i = 0; i < global_db.skills.size(); ++i) {
    x["skills"][i] = global_db.skills[i];
  }

  for (size_t i = 0; i < global_db.experience.size(); ++i) {
    x["experience"][i]["id"] = global_db.experience[i].id;
    x["experience"][i]["title"] = global_db.experience[i].title;
    x["experience"][i]["company"] = global_db.experience[i].company;
    x["experience"][i]["dates"] = global_db.experience[i].dates;
    x["experience"][i]["description"] = global_db.experience[i].description;
  }

  std::ofstream os("db.json");
  os << x.dump();
}

void load_database() {
  std::ifstream is("db.json");
  if (!is.is_open()) {
    global_db.header = {
        "Patryk Jóźwiak",
        "Aspiring Software Engineer / C++ Developer",
        "Hi ! I'm learning to write efficient C++ code, and this is my custom "
        "web server built with the Crow Framework.",
        "/contact",
        "https://www.linkedin.com/in/patryk-j%C3%B3%C5%BAwiak-7711b7306/",
        "https://github.com/P4tryK00"};
    global_db.skills = {"C++",   "STL",  "Crow Framework", "Git",
                        "CMake", "SFML", "LaTeX",          "Python"};
    global_db.projects.push_back(
        {1,
         "Custom C++ Portfolio Server",
         "A custom-built web server acting as my personal portfolio. Developed "
         "in modern C++ using the Crow framework.",
         "https://github.com/P4tryK00/crow",
         {"C++", "STL", "CMake", "Crow"}});
    global_db.experience.push_back(
        {1, "C++ Developer", "Personal Projects", "2023 - Present",
         "Developing various projects using modern C++ features."});
    save_database();
    return;
  }

  std::stringstream ss;
  ss << is.rdbuf();
  auto x = crow::json::load(ss.str());
  if (!x)
    return;

  global_db.projects.clear();
  global_db.skills.clear();
  global_db.experience.clear();

  if (x.has("header")) {
    global_db.header.name = x["header"]["name"].s();
    global_db.header.title = x["header"]["title"].s();
    global_db.header.description = x["header"]["description"].s();
    global_db.header.contact_link = x["header"]["contact_link"].s();
    global_db.header.linkedin_link = x["header"]["linkedin_link"].s();
    global_db.header.github_link = x["header"]["github_link"].s();
  }

  if (x.has("projects")) {
    for (const auto &item : x["projects"]) {
      Project p;
      p.id = item["id"].i();
      p.title = item["title"].s();
      p.description = item["description"].s();
      p.githubUrl = item["githubUrl"].s();
      if (item.has("long_description"))
        p.long_description = item["long_description"].s();
      if (item.has("image"))
        p.image = item["image"].s();
      if (item.has("liveUrl"))
        p.liveUrl = item["liveUrl"].s();
      for (const auto &tech : item["technologies"])
        p.technologies.push_back(tech.s());
      if (item.has("screenshots")) {
        for (const auto &scr : item["screenshots"])
          p.screenshots.push_back(scr.s());
      }
      global_db.projects.push_back(p);
    }
  }

  if (x.has("skills")) {
    for (const auto &item : x["skills"])
      global_db.skills.push_back(item.s());
  }

  if (x.has("experience")) {
    for (const auto &item : x["experience"]) {
      Experience e;
      e.id = item["id"].i();
      e.title = item["title"].s();
      e.company = item["company"].s();
      e.dates = item["dates"].s();
      e.description = item["description"].s();
      global_db.experience.push_back(e);
    }
  }
}

crow::mustache::context get_portfolio_context() {
  crow::mustache::context ctx;
  ctx["header"]["name"] = global_db.header.name;
  ctx["header"]["title"] = global_db.header.title;
  ctx["header"]["description"] = global_db.header.description;
  ctx["header"]["contact_link"] = global_db.header.contact_link;
  ctx["header"]["linkedin_link"] = global_db.header.linkedin_link;
  ctx["header"]["github_link"] = global_db.header.github_link;

  ctx["project_count"] = global_db.projects.size();
  for (size_t i = 0; i < global_db.projects.size(); ++i) {
    ctx["projects"][i]["id"] = global_db.projects[i].id;
    ctx["projects"][i]["name"] = global_db.projects[i].title;
    ctx["projects"][i]["title"] = global_db.projects[i].title;
    ctx["projects"][i]["description"] = global_db.projects[i].description;
    ctx["projects"][i]["githubUrl"] = global_db.projects[i].githubUrl;
    ctx["projects"][i]["image"] = global_db.projects[i].image;
    ctx["projects"][i]["liveUrl"] = global_db.projects[i].liveUrl;
    for (size_t j = 0; j < global_db.projects[i].technologies.size(); ++j) {
      ctx["projects"][i]["technologies"][j]["name"] =
          global_db.projects[i].technologies[j];
    }
  }

  for (size_t i = 0; i < global_db.skills.size(); ++i) {
    ctx["skills"][i]["name"] = global_db.skills[i];
  }

  for (size_t i = 0; i < global_db.experience.size(); ++i) {
    ctx["experience"][i]["title"] = global_db.experience[i].title;
    ctx["experience"][i]["company"] = global_db.experience[i].company;
    ctx["experience"][i]["dates"] = global_db.experience[i].dates;
    ctx["experience"][i]["description"] = global_db.experience[i].description;
  }

  return ctx;
}

int main() {
  load_database();
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() {
    return crow::mustache::load("cv.html").render(get_portfolio_context());
  });

  // Preserve some legacy routes if they were just serving static templates
  CROW_ROUTE(app, "/projects")([]() {
    return crow::mustache::load("projects.html")
        .render(get_portfolio_context());
  });

  CROW_ROUTE(app, "/contact")(
      []() { return crow::mustache::load("contact.html").render(); });

  // Dynamic project detail route
  CROW_ROUTE(app, "/project/<int>")
  ([](int id) -> crow::response {
    for (const auto &p : global_db.projects) {
      if (p.id == id) {
        crow::mustache::context ctx;
        ctx["id"] = p.id;
        ctx["title"] = p.title;
        ctx["description"] = p.description;
        ctx["long_description"] = p.long_description;
        ctx["image"] = p.image;
        ctx["githubUrl"] = p.githubUrl;
        ctx["liveUrl"] = p.liveUrl;

        for (size_t j = 0; j < p.technologies.size(); ++j) {
          ctx["technologies"][j]["name"] = p.technologies[j];
        }
        for (size_t j = 0; j < p.screenshots.size(); ++j) {
          ctx["screenshots"][j]["url"] = p.screenshots[j];
        }
        return crow::response(
            crow::mustache::load("project_detail.html").render(ctx));
      }
    }
    return crow::response(404, "Project not found");
  });

  // New Admin Panel route
  CROW_ROUTE(app, "/admin")(
      []() { return crow::mustache::load("admin.html").render(); });

  // API Route to return State
  CROW_ROUTE(app, "/api/get_data")([]() {
    crow::json::wvalue x;
    for (size_t i = 0; i < global_db.projects.size(); ++i) {
      x["projects"][i]["id"] = global_db.projects[i].id;
      x["projects"][i]["title"] = global_db.projects[i].title;
      x["projects"][i]["description"] = global_db.projects[i].description;
      x["projects"][i]["githubUrl"] = global_db.projects[i].githubUrl;
      for (size_t j = 0; j < global_db.projects[i].technologies.size(); ++j) {
        x["projects"][i]["technologies"][j] =
            global_db.projects[i].technologies[j];
      }
    }
    for (size_t i = 0; i < global_db.experience.size(); ++i) {
      x["experience"][i]["id"] = global_db.experience[i].id;
      x["experience"][i]["title"] = global_db.experience[i].title;
      x["experience"][i]["company"] = global_db.experience[i].company;
      x["experience"][i]["dates"] = global_db.experience[i].dates;
      x["experience"][i]["description"] = global_db.experience[i].description;
    }
    return crow::response(x);
  });

  // API Route to Save Project
  CROW_ROUTE(app, "/api/save_project")
      .methods(crow::HTTPMethod::POST)(
          [](const crow::request &req) -> crow::response {
            auto body = crow::json::load(req.body);
            if (!body)
              return crow::response(400, "Invalid JSON");
            if (!body.has("password") || body["password"].s() != "test123") {
              return crow::response(403, "Access Denied: Invalid password.");
            }

            Project p;
            p.id = body.has("id") ? body["id"].i()
                                  : (global_db.projects.empty()
                                         ? 1
                                         : global_db.projects.back().id + 1);
            p.title = body.has("title") ? std::string(body["title"].s()) : "";
            p.description = body.has("description")
                                ? std::string(body["description"].s())
                                : "";
            p.githubUrl =
                body.has("githubUrl") ? std::string(body["githubUrl"].s()) : "";

            if (body.has("technologies")) {
              std::string tech_str = std::string(body["technologies"].s());
              std::stringstream ss(tech_str);
              std::string item;
              while (std::getline(ss, item, ',')) {
                size_t start = item.find_first_not_of(" ");
                size_t end = item.find_last_not_of(" ");
                if (start != std::string::npos && end != std::string::npos) {
                  p.technologies.push_back(item.substr(start, end - start + 1));
                }
              }
            }

            bool updated = false;
            for (auto &proj : global_db.projects) {
              if (proj.id == p.id) {
                proj = p;
                updated = true;
                break;
              }
            }
            if (!updated)
              global_db.projects.push_back(p);

            save_database();
            return crow::response(200, "Project saved");
          });

  // API Route to Save Experience
  CROW_ROUTE(app, "/api/save_experience")
      .methods(crow::HTTPMethod::POST)(
          [](const crow::request &req) -> crow::response {
            auto body = crow::json::load(req.body);
            if (!body)
              return crow::response(400, "Invalid JSON");
            if (!body.has("password") || body["password"].s() != "test123") {
              return crow::response(403, "Access Denied: Invalid password.");
            }

            Experience e;
            e.id = body.has("id") ? body["id"].i()
                                  : (global_db.experience.empty()
                                         ? 1
                                         : global_db.experience.back().id + 1);
            e.title = body.has("title") ? std::string(body["title"].s()) : "";
            e.company =
                body.has("company") ? std::string(body["company"].s()) : "";
            e.dates = body.has("dates") ? std::string(body["dates"].s()) : "";
            e.description = body.has("description")
                                ? std::string(body["description"].s())
                                : "";

            bool updated = false;
            for (auto &exp : global_db.experience) {
              if (exp.id == e.id) {
                exp = e;
                updated = true;
                break;
              }
            }
            if (!updated)
              global_db.experience.push_back(e);

            save_database();
            return crow::response(200, "Experience saved");
          });
  
  // API Route to Delete Project
  CROW_ROUTE(app, "/api/delete_project").methods(crow::HTTPMethod::POST)([](const crow::request &req) -> crow::response {
      auto body = crow::json::load(req.body);
      if (!body) return crow::response(400, "Invalid JSON");
      if (!body.has("password") || body["password"].s() != "test123") return crow::response(403, "Access Denied: Invalid password.");
      if (!body.has("id")) return crow::response(400, "Missing ID");

      int target_id = body["id"].i();
      auto it = std::remove_if(global_db.projects.begin(), global_db.projects.end(), [target_id](const Project& p) {
          return p.id == target_id;
      });

      if (it != global_db.projects.end()) {
          global_db.projects.erase(it, global_db.projects.end());
          save_database();
          return crow::response(200, "Project deleted");
      }
      return crow::response(404, "Project not found");
  });

  // API Route to Delete Experience
  CROW_ROUTE(app, "/api/delete_experience").methods(crow::HTTPMethod::POST)([](const crow::request &req) -> crow::response {
      auto body = crow::json::load(req.body);
      if (!body) return crow::response(400, "Invalid JSON");
      if (!body.has("password") || body["password"].s() != "test123") return crow::response(403, "Access Denied: Invalid password.");
      if (!body.has("id")) return crow::response(400, "Missing ID");

      int target_id = body["id"].i();
      auto it = std::remove_if(global_db.experience.begin(), global_db.experience.end(), [target_id](const Experience& e) {
          return e.id == target_id;
      });

      if (it != global_db.experience.end()) {
          global_db.experience.erase(it, global_db.experience.end());
          save_database();
          return crow::response(200, "Experience deleted");
      }
      return crow::response(404, "Experience not found");
  });

  app.port(54607).multithreaded().run();
}