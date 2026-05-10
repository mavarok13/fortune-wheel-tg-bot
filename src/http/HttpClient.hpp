#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#include <string>

class HttpClient {
public:
    explicit HttpClient(std::string host);

    HttpResponse send(HttpRequest request);

private:
    std::string host_;
};
