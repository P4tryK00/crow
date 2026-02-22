#include "crow.h"
crow::mustache::context get_projects_context() {
    crow::mustache::context ctx;
    ctx["project_count"] = 2;

    ctx["projects"][0]["name"] = "Own C++ Server";
    ctx["projects"][0]["description"] = "Card Created in Crow Framework.";

    ctx["projects"][1]["name"] = "MiniGolf Game Developed in C++ with SFML Library.";
    ctx["projects"][1]["description"] = "Final Studies project.";

    return ctx;
}

int main() {
    crow::SimpleApp app; // Tworzymy instancję aplikacji
    crow::mustache::load("cv.html").render();


    // Definiujemy tzw. endpoint, czyli co się stanie po wejściu na stronę główną ("/")
    CROW_ROUTE(app, "/")([](){
        auto ctx = get_projects_context();
        return crow::mustache::load("cv.html").render(ctx);
    });


    CROW_ROUTE(app, "/contact")([](){
    return crow::mustache::load("contact.html").render();
});

    // Uruchamiamy serwer na porcie 8080
    app.port(8080).multithreaded().run();
}