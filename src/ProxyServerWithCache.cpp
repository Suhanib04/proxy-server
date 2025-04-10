#include "ProxyRequestParser.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
 
bool ProxyRequestParser::parse(const std::string& raw, ParsedRequest& req) {
    std::istringstream stream(raw);
    std::string line;

    // First line: Method URL Version
    if (!std::getline(stream, line)) return false;
    std::istringstream reqLine(line);
    if (!(reqLine >> req.method)) return false;

    std::string fullURL;
    if (!(reqLine >> fullURL)) return false;

    if (!(reqLine >> req.version)) return false;

    auto protocolEnd = fullURL.find("://");
    if (protocolEnd == std::string::npos) return false;
    req.protocol = fullURL.substr(0, protocolEnd);

    auto hostStart = protocolEnd + 3;
    auto pathStart = fullURL.find("/", hostStart);
    std::string hostPort = fullURL.substr(hostStart, pathStart - hostStart);
    req.path = pathStart != std::string::npos ? fullURL.substr(pathStart) : "/";

    auto colonPos = hostPort.find(":");
    if (colonPos != std::string::npos) {
        req.host = hostPort.substr(0, colonPos);
        req.port = hostPort.substr(colonPos + 1);
    } else {
        req.host = hostPort;
        req.port = "80"; // default
    }

    // Parse headers
    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty()) break;
        auto colon = line.find(":");
        if (colon != std::string::npos) {
            ParsedHeader header;
            header.key = line.substr(0, colon);
            header.value = line.substr(colon + 2); // skip ": "
            req.headers.push_back(header);
        }
    }

    return true;
}

std::string ProxyRequestParser::unparse(const ParsedRequest& req) {
    std::ostringstream oss;
    oss << req.method << " " << req.protocol << "://" << req.host;
    if (!req.port.empty() && req.port != "80")
        oss << ":" << req.port;
    oss << req.path << " " << req.version << "\r\n";
    oss << unparseHeaders(req);
    oss << "\r\n";
    return oss.str();
}

std::string ProxyRequestParser::unparseHeaders(const ParsedRequest& req) {
    std::ostringstream oss;
    for (const auto& h : req.headers)
        oss << h.key << ": " << h.value << "\r\n";
    return oss.str();
}

std::string ProxyRequestParser::getHeader(const ParsedRequest& req, const std::string& key) {
    for (const auto& h : req.headers)
        if (h.key == key) return h.value;
    return "";
}

void ProxyRequestParser::setHeader(ParsedRequest& req, const std::string& key, const std::string& value) {
    for (auto& h : req.headers) {
        if (h.key == key) {
            h.value = value;
            return;
        }
    }
    req.headers.push_back({key, value});
}

void ProxyRequestParser::removeHeader(ParsedRequest& req, const std::string& key) {
    req.headers.erase(std::remove_if(req.headers.begin(), req.headers.end(),
        [&](const ParsedHeader& h) { return h.key == key; }), req.headers.end());
}
