#pragma once
#include <string>
#include <vector>
#include <map> 

struct ParsedHeader {
    std::string key;
    std::string value;
};

struct ParsedRequest {
    std::string method;
    std::string protocol;
    std::string host;
    std::string port;
    std::string path;
    std::string version;

    std::vector<ParsedHeader> headers;
};

class ProxyRequestParser {
public:
    static bool parse(const std::string& raw, ParsedRequest& request);
    static std::string unparse(const ParsedRequest& request);
    static std::string unparseHeaders(const ParsedRequest& request);
    static std::string getHeader(const ParsedRequest& request, const std::string& key);
    static void setHeader(ParsedRequest& request, const std::string& key, const std::string& value);
    static void removeHeader(ParsedRequest& request, const std::string& key);
};
