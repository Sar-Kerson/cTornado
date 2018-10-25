#include "../../include/server_http.hpp"
//Added for the json-example:
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;
//Added for the json-example:
using namespace boost::property_tree;
int main() {
    //HTTP-server at port 8080 using 4 threads
    HTTPServer httpserver(8080, 1);
    
    //Add resources using regular expressions for path and method
    //POST-example for the path /string, responds the posted string
    httpserver.resources["^/string/?$"]["^POST$"]=[](ostream& response, const Request& request, const smatch& path_match) {
        //Retrieve string from istream (*request.content)
        stringstream ss;
        *request.content >> ss.rdbuf();
        string content=move(ss.str());
        response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    };
    
    //POST-example for the path /json, responds firstName+" "+lastName from the posted json
    //Responds with an appropriate error message if the posted json is not correct, or if firstName or lastName is missing
    httpserver.resources["^/json/?$"]["^POST$"]=[](ostream& response, const Request& request, const smatch& path_match) {
        ptree pt;
        try {
            read_json(*request.content, pt);
             string name=pt.get<string>("firstName")+" "+pt.get<string>("lastName");
             response << "HTTP/1.1 200 OK\r\nContent-Length: " << name.length() << "\r\n\r\n" << name;
        }
        catch(exception& e) {
            response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
    };
    
    //GET-example for the path /
    httpserver.resources["^/$"]["^GET$"]=[](ostream& response, const Request& request, const smatch& path_match) {
        response <<  "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nRoot resource";
    };
    
    //GET-example for the path /test, responds with the matched string in path (test) 
    httpserver.resources["^/(test)/?$"]["^GET$"]=[](ostream& response, const Request& request, const smatch& path_match) {
        response << "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\n" << path_match[1];
    };
    
    //Start HTTP-server
    httpserver.start();
    
    return 0;
}