/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 21:07:31 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/04 19:05:40 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//first headers

#include "../include/HTTP/HTTPHeaders.hpp"
#include "../include/HTTP/HTTPRequest.hpp"
#include "../include/HTTP/HTTPResponse.hpp"
#include "../include/HTTP/HTTPStatusCode.hpp"
#include "../include/HTTP/HTTPRequestParser.hpp"
#include <iostream>

int main() {
    std::cout << "\n=== TEST 1: Simple GET Request ===" << std::endl;
    {
        RequestParser parser;

        std::string request = 
            "GET /api/users HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "User-Agent: TestClient/1.0\r\n"
            "\r\n";

        parser.feedData(request);

        if (parser.isComplete()) {
            std::cout << "✓ Parsing successful!" << std::endl;
            parser.getRequest().print_http();
        } else if (parser.hasError()) {
            std::cout << "✗ Parsing error: " << parser.getErrorMsg() << std::endl;
        }
    }

    std::cout << "\n=== TEST 2: POST Request with JSON Body ===" << std::endl;
    {
        RequestParser parser;

        std::string request = 
            "POST /api/users HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 25\r\n"
            "\r\n"
            "{\"name\":\"John\",\"age\":30}"
            "\r\n";

        parser.feedData(request);

        if (parser.isComplete()) {
            std::cout << "✓ Parsing successful!" << std::endl;
            const HttpRequest& req = parser.getRequest();
            std::cout << "Body: " << req.getBody() << std::endl;
        }
    }

    std::cout << "\n=== TEST 3: Chunked Data (Streaming) ===" << std::endl;
    {
        RequestParser parser;

        // Simular recepción en 3 chunks
        parser.feedData("GET /api");
        parser.printState();

        parser.feedData("/data HTTP/1.1\r\nHost: localhost\r\n");
        parser.printState();

        parser.feedData("\r\n");
        parser.printState();

        if (parser.isComplete()) {
            std::cout << "✓ Successfully parsed from 3 chunks!" << std::endl;
            parser.getRequest().print_http();
        }
    }

    std::cout << "\n=== TEST 4: Invalid Request ===" << std::endl;
    {
        RequestParser parser;

        parser.feedData("INVALID REQUEST\r\n\r\n");

        if (parser.hasError()) {
            std::cout << "✓ Error detected correctly: " << parser.getErrorMsg() << std::endl;
        }
    }

    return 0;
}