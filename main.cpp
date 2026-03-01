#include "crow.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Project {
    int id;
    std::string title;
    std::string description;
    std::string long_description;
    std::string image;
    std::vector<std::string> screenshots;
    std::string githubUrl;
    std::string liveUrl;
    std::vector<std::string> technologies;
};

std::vector<Project> db_projects;

void save_database() {
    crow::json::wvalue x;
    for (size_t i = 0; i < db_projects.size(); ++i) {
        x[i]["id"] = db_projects[i].id;
        x[i]["title"] = db_projects[i].title;
        x[i]["description"] = db_projects[i].description;
        x[i]["long_description"] = db_projects[i].long_description;
        x[i]["image"] = db_projects[i].image;
        x[i]["githubUrl"] = db_projects[i].githubUrl;
        x[i]["liveUrl"] = db_projects[i].liveUrl;
        for (size_t j = 0; j < db_projects[i].technologies.size(); ++j) {
            x[i]["technologies"][j] = db_projects[i].technologies[j];
        }
        for (size_t j = 0; j < db_projects[i].screenshots.size(); ++j) {
            x[i]["screenshots"][j] = db_projects[i].screenshots[j];
        }
    }
    std::ofstream os("projects.json");
    os << x.dump();
}

void load_database() {
    std::ifstream is("projects.json");

    if (!is.is_open()) {
        db_projects.push_back({
             0,
             "Custom C++ Portfolio Server",
             "A custom-built web server acting as my personal portfolio. Developed from scratch in modern C++ using the Crow framework.",
             R"(
<p>Building a personal portfolio usually involves reaching for standard web frameworks or CMS platforms. However,
I wanted to treat my portfolio as a direct demonstration of my C++ skills in non typical task. The goal was to build a
web server completely from scratch using modern C++.</p>
<p>At the core of this project is the <strong>Crow framework</strong>, a fast and lightweight C++ microframework.
Instead of relying on a heavy SQL database, the server utilizes a custom-built JSON data management system to persist
project data efficiently. I implemented dynamic routing, administrative POST endpoints (acting as a custom CMS), and
server-side rendering using the <strong>Mustache template engine</strong>. This ensures a strict separation between
backend logic and frontend presentation.</p>
<p>The user interface is crafted with HTML and <strong>Tailwind CSS</strong>, providing a fully responsive, modern,
and clean design. The entire application is compiled using CMake and deployed on a Linux Virtual Private Server (VPS)
behind a reverse proxy.</p>
            )",
             "static/crow.png",
             {},
             "https://github.com/P4tryK00/crow",
             "https://pjozwiak.dev",
             {"C++", "STL", "CMake", "Crow", "Tailwind CSS", "VPS"}
         });

        save_database();
        return;
    }

    std::stringstream ss;
    ss << is.rdbuf();
    auto x = crow::json::load(ss.str());

    if (!x) return;

    db_projects.clear();

    for (const auto& item : x) {
        Project p;
        p.id = item["id"].i();
        p.title = item["title"].s();
        p.description = item["description"].s();

        if (item.has("long_description")) {
            p.long_description = item["long_description"].s();
        }

        p.image = item["image"].s();
        p.githubUrl = item["githubUrl"].s();
        p.liveUrl = item["liveUrl"].s();

        for (const auto& tech : item["technologies"]) {
            p.technologies.push_back(tech.s());
        }

        if (item.has("screenshots")) {
            for (const auto& img : item["screenshots"]) {
                p.screenshots.push_back(img.s());
            }
        }

        db_projects.push_back(p);
    }
}

crow::mustache::context get_all_projects_context() {
    crow::mustache::context ctx;
    ctx["project_count"] = db_projects.size();

    for (size_t i = 0; i < db_projects.size(); ++i) {
        ctx["projects"][i]["id"] = db_projects[i].id;
        ctx["projects"][i]["name"] = db_projects[i].title;
        ctx["projects"][i]["title"] = db_projects[i].title;
        ctx["projects"][i]["description"] = db_projects[i].description;
        ctx["projects"][i]["image"] = db_projects[i].image;
        ctx["projects"][i]["githubUrl"] = db_projects[i].githubUrl;
        if (!db_projects[i].liveUrl.empty()) ctx["projects"][i]["liveUrl"] = db_projects[i].liveUrl;

        for (size_t j = 0; j < db_projects[i].technologies.size(); ++j) {
            ctx["projects"][i]["technologies"][j]["name"] = db_projects[i].technologies[j];
        }
    }

    std::vector<std::string> my_skills = {"C++", "STL", "Crow Framework", "Git", "CMake", "SFML", "LaTeX", "Python"};
    for (size_t i = 0; i < my_skills.size(); ++i) {
        ctx["skills"][i]["name"] = my_skills[i];
    }
    return ctx;
}

crow::mustache::context get_single_project_context(int id) {
    crow::mustache::context ctx;
    for (const auto& p : db_projects) {
        if (p.id == id) {
            ctx["id"] = p.id;
            ctx["title"] = p.title;
            ctx["description"] = p.description;
            ctx["long_description"] = p.long_description;
            ctx["image"] = p.image;
            ctx["githubUrl"] = p.githubUrl;
            if (!p.liveUrl.empty()) ctx["liveUrl"] = p.liveUrl;

            for (size_t i = 0; i < p.technologies.size(); ++i) {
                ctx["technologies"][i]["name"] = p.technologies[i];
            }
            for (size_t i = 0; i < p.screenshots.size(); ++i) {
                ctx["screenshots"][i]["url"] = p.screenshots[i];
            }

            std::string tech_joined = "";
            for (size_t i = 0; i < p.technologies.size(); ++i) {
                tech_joined += p.technologies[i];
                if (i < p.technologies.size() - 1) tech_joined += ", ";
            }
            ctx["technologies_joined"] = tech_joined;

            std::string screens_joined = "";
            for (size_t i = 0; i < p.screenshots.size(); ++i) {
                screens_joined += p.screenshots[i];
                if (i < p.screenshots.size() - 1) screens_joined += ", ";
            }
            ctx["screenshots_joined"] = screens_joined;
            break;
        }
    }
    return ctx;
}

int main() {
    load_database();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return crow::mustache::load("cv.html").render(get_all_projects_context());
    });

    CROW_ROUTE(app, "/projects")([](){
        return crow::mustache::load("projects.html").render(get_all_projects_context());
    });

    CROW_ROUTE(app, "/project/<int>")([](int id) -> crow::response {
        auto ctx = get_single_project_context(id);
        if (ctx["title"].dump() == "\"\"" || ctx["title"].dump() == "null") {
            return crow::response(404, "Project not found!");
        }
        return crow::response(crow::mustache::load("project_detail.html").render(ctx));
    });

    CROW_ROUTE(app, "/contact")([](){
        return crow::mustache::load("contact.html").render();
    });

    CROW_ROUTE(app, "/admin/add").methods(crow::HTTPMethod::GET)([](){
        return crow::mustache::load("add_project.html").render();
    });

    CROW_ROUTE(app, "/admin/add").methods(crow::HTTPMethod::POST)([](const crow::request& req) -> crow::response {
        std::string body = req.body;
        for (char& c : body) { if (c == '+') c = ' '; }

        crow::query_string post_data("?" + body);

        std::string password = post_data.get("secret_password") ? post_data.get("secret_password") : "";
        if (password != "test123") {
            return crow::response(403, "Access Denied: Invalid password.");
        }

        Project p;
        p.id = db_projects.empty() ? 0 : db_projects.back().id + 1;
        p.title = post_data.get("title") ? post_data.get("title") : "";
        p.description = post_data.get("description") ? post_data.get("description") : "";
        p.long_description = post_data.get("long_description") ? post_data.get("long_description") : "";
        p.image = post_data.get("image") ? post_data.get("image") : "";
        p.githubUrl = post_data.get("github") ? post_data.get("github") : "";
        p.liveUrl = post_data.get("live_url") ? post_data.get("live_url") : "";

        std::string tech_str = post_data.get("technologies") ? post_data.get("technologies") : "";
        std::stringstream ss(tech_str);
        std::string item;
        while (std::getline(ss, item, ',')) {
            size_t start = item.find_first_not_of(" ");
            size_t end = item.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                p.technologies.push_back(item.substr(start, end - start + 1));
            }
        }

        std::string screens_str = post_data.get("screenshots") ? post_data.get("screenshots") : "";
        std::stringstream ss_screens(screens_str);
        while (std::getline(ss_screens, item, ',')) {
            size_t start = item.find_first_not_of(" ");
            size_t end = item.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                p.screenshots.push_back(item.substr(start, end - start + 1));
            }
        }

        db_projects.push_back(p);
        save_database();

        crow::response res(302);
        res.set_header("Location", "/projects");
        return res;
    });

    CROW_ROUTE(app, "/admin/delete/<int>").methods(crow::HTTPMethod::POST)([](const crow::request& req, int id) -> crow::response {
        std::string body = req.body;
        for (char& c : body) { if (c == '+') c = ' '; }
        crow::query_string post_data("?" + body);

        std::string password = post_data.get("secret_password") ? post_data.get("secret_password") : "";
        if (password != "test123") return crow::response(403, "Access Denied: Invalid password.");

        for (auto it = db_projects.begin(); it != db_projects.end(); ++it) {
            if (it->id == id) {
                db_projects.erase(it);
                break;
            }
        }
        save_database();

        crow::response res(302);
        res.set_header("Location", "/projects");
        return res;
    });

    CROW_ROUTE(app, "/admin/edit/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        auto ctx = get_single_project_context(id);
        return crow::mustache::load("edit_project.html").render(ctx);
    });

    CROW_ROUTE(app, "/admin/edit/<int>").methods(crow::HTTPMethod::POST)([](const crow::request& req, int id) -> crow::response {
        std::string body = req.body;
        for (char& c : body) { if (c == '+') c = ' '; }
        crow::query_string post_data("?" + body);

        std::string password = post_data.get("secret_password") ? post_data.get("secret_password") : "";
        if (password != "test123") return crow::response(403, "Access Denied: Invalid password.");

        for (auto& p : db_projects) {
            if (p.id == id) {
                p.title = post_data.get("title") ? post_data.get("title") : "";
                p.description = post_data.get("description") ? post_data.get("description") : "";
                p.long_description = post_data.get("long_description") ? post_data.get("long_description") : "";
                p.image = post_data.get("image") ? post_data.get("image") : "";
                p.githubUrl = post_data.get("github") ? post_data.get("github") : "";
                p.liveUrl = post_data.get("live_url") ? post_data.get("live_url") : "";

                p.technologies.clear();
                std::string tech_str = post_data.get("technologies") ? post_data.get("technologies") : "";
                std::stringstream ss(tech_str);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    size_t start = item.find_first_not_of(" ");
                    size_t end = item.find_last_not_of(" ");
                    if (start != std::string::npos && end != std::string::npos) {
                        p.technologies.push_back(item.substr(start, end - start + 1));
                    }
                }

                p.screenshots.clear();
                std::string screens_str = post_data.get("screenshots") ? post_data.get("screenshots") : "";
                std::stringstream ss_screens(screens_str);
                while (std::getline(ss_screens, item, ',')) {
                    size_t start = item.find_first_not_of(" ");
                    size_t end = item.find_last_not_of(" ");
                    if (start != std::string::npos && end != std::string::npos) {
                        p.screenshots.push_back(item.substr(start, end - start + 1));
                    }
                }
                break;
            }
        }
        save_database();

        crow::response res(302);
        res.set_header("Location", "/project/" + std::to_string(id));
        return res;
    });

    app.port(54607).multithreaded().run();
}