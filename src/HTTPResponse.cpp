/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:48:31 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/12/09 17:39:24 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

/*Private*/

void	HttpResponse::upd_headers()
{
	//Update time
	getHeaders().set_http("Date", getDateHttp());

	//Update Content-legnth
	if (!this->getBody().empty())
	{
		std::ostringstream oss;
		oss << getBody().length();
		this->getHeaders().set_http("Content-Length", oss.str());
	}
}

std::string	HttpResponse::getDateHttp() const
{
	time_t now = time(0);
	struct tm* ptm = gmtime(&now); //covert hour

	struct tm aux = *ptm; //copy 
	char buf[100]; //buffer
	size_t len = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &aux);

	if (len > 0)
		return std::string(buf);
	else
		return "";
}

std::string	HttpResponse::readFile(const std::string& fileP) const
{
	std::ifstream file(fileP.c_str(), std::ios::binary);
	if (!file.is_open())
		return "";
	
	std::ostringstream oss;
	oss << file.rdbuf();  // .rdbuf() returns the pointer to the underlying buffer stream
	return oss.str();
}

/*Public*/
HttpResponse::HttpResponse() : _version("HTTP/1.1"), _statusCode(200), _reason("OK")
{
	this->getHeaders().set_http("Server", "Webserv/1.0");
}

HttpResponse::HttpResponse(int statuscode) : _version("HTTP/1.1"), _statusCode(statuscode)
{
	_reason = HttpStatusCode::getReason(statuscode);
	this->getHeaders().set_http("Server", "Webserv/1.0");
}

HttpResponse::~HttpResponse()
{
}

//Getters
const std::string& HttpResponse::getVersion() const
{
	return _version;
}

int HttpResponse::getStatus() const
{
	return _statusCode;
}

const std::string& HttpResponse::getReason() const
{
	return _reason;
}

const HttpHeaders& HttpResponse::getHeaders() const
{
	return _headers;
}

const std::string& HttpResponse::getBody() const
{
	return _body;
}

HttpHeaders& HttpResponse::getHeaders()
{
	return _headers;
}

//Setters
void	HttpResponse::setVersion(const std::string& version)
{
	_version = version;
}

void	HttpResponse::setStatusCode(int code)
{
	_statusCode = code;
}

void	HttpResponse::setReason(const std::string& phrase)
{
	_reason = phrase;
}

void	HttpResponse::setHeaders(const HttpHeaders& headers)
{
	_headers = headers;
}

void	HttpResponse::setHeader(const std::string& name, const std::string& value)
{
	this->getHeaders().set_http(name, value);
}

void	HttpResponse::setBody(const std::string& body)
{
	_body = body;
}

//Utils
std::string HttpResponse::toString() const
{
	std::ostringstream oss;

	oss << _version << " " << _statusCode << " " << _reason << "\r\n";
	oss << _headers.toString();
	oss << "\r\n";
	if (!_body.empty())
		oss << _body;
	return oss.str();
}

void	HttpResponse::clear()
{
	_version = "HTTP/1.1";
	_statusCode = 200;
	_reason = "OK";
	_headers.clear();
	_body.clear();
}

HttpResponse	HttpResponse::build()
{
	upd_headers();
	return *this;
}

HttpResponse& HttpResponse::setContent(const std::string& type)
{
	this->getHeaders().set_http("Content-Type", type);
	return *this;
}

HttpResponse& HttpResponse::setBodyFile(const std::string& file)
{
	std::string content = readFile(file);
	if (!content.empty())
	{
		if (file.find(".html") != std::string::npos)
			setContent("text/html");
		else if (file.find(".json") != std::string::npos)
			setContent("application/json");
		else if (file.find(".css") != std::string::npos)
			setContent("text/css");
		else if (file.find(".js") != std::string::npos)
			setContent("application/javascript");
		else if (file.find(".png") != std::string::npos)
			setContent("image/png");
		else if (file.find(".jpg") != std::string::npos || file.find(".jpeg") != std::string::npos)
			setContent("image/jpeg");
		else
			setContent("application/octet-stream");
		this->setBody(content);
		return *this;
	}
	else
	{
		_statusCode = 404;
		_reason = HttpStatusCode::getReason(404);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("File not found");
		setContent("text/plain");
		return *this;
	}
}

void	HttpResponse::printResponse()
{
	std::cout << "=== HTTP Response ===" << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Status: " << _statusCode << " " 
				<< _reason << std::endl;
	std::cout << "Headers (" << _headers.size() << "):" << std::endl;
	
	const std::map<std::string, std::string>& allHeaders = 
		_headers.get_all_http();
	std::map<std::string, std::string>::const_iterator it;
	for (it = allHeaders.begin(); it != allHeaders.end(); ++it) {
		std::cout << "  " << it->first << ": " 
					<< it->second << std::endl;
	}
	
	if (!_body.empty()) {
		std::cout << "Body length: " << _body.length() 
					<< " bytes" << std::endl;
		if (_body.length() < 200) {
			std::cout << "Body: " << _body << std::endl;
		}
	}
	std::cout << "====================" << std::endl;
}


std::string	HttpResponse::execute_response(HttpRequest par, t_server server)
{
	if (par.getMethod() == "GET" || par.getMethod() == "POST" || par.getMethod() == "DELETE")
	{
		if (!isvalidmethod(par, server))
			return generateError(405, server, "Method Not Allowed");
		if (par.getMethod() == "GET")
			return handle_get(par, server, 0);
		else if (par.getMethod() == "POST")
			return handle_post(par, server, 0);
		else if (par.getMethod() == "DELETE")
			return handle_delete(par, server, 0);
	}
	return generateError(501, server, "Method not implemented");
}

std::string	HttpResponse::handle_get(HttpRequest par, t_server server, int flag)
{
	std::string uri = par.getUri();
	std::string root = server.root;
	std::string path;

	if (!server.locations.empty())
		flag = 1;
	if (flag && !server.locations[0].root.empty())
	{
		root = server.locations[0].root;
		std::string aux = server.locations[0].path;
		if (aux.find(aux) == 0)
			path = joinPaths(root, uri.substr(aux.size() - 1));
		else
			path = joinPaths(root, uri);
	}
	else
		path = joinPaths(root, uri);
	if (isFile(path))
	{
		_statusCode = 200;
		_reason = HttpStatusCode::getReason(200);
		getHeaders().set_http("Server", "Webserver/1.0");
		setBodyFile(path);
		return build().toString();
	}
	if (!isDir(path))
		return generateError(404, server, "404 Not found");
	if (path[path.size() - 1] != '/') //if not found / -> redirect
	{
		_statusCode = 301;
		_reason = HttpStatusCode::getReason(301);
		getHeaders().set_http("Location", uri + "/");
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Moved Permanently");
		setContent("text/plain");
		return build().toString();
	}
	if (haveIndex(path))
	{
		std::string indexFile = getIndexFile(path);
		_statusCode = 200;
		_reason = HttpStatusCode::getReason(200);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBodyFile(indexFile);
		return build().toString();
	}

	std::string autoindex = "off";
	if(flag)
		autoindex = server.locations[0].autoindex;
	
	if (autoindex == "on")
	{
		std::string listen = autoIndexDir(path);
		_statusCode = 200;
		_reason = HttpStatusCode::getReason(200);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody(listen);
		setContent("text/html");
		return build().toString();
	}
	else
		return generateError(403, server, "403 Forbidden: Directory access is disabled");
}

std::string	HttpResponse::handle_post(HttpRequest par, t_server server, int flag)
{
	size_t max_bytes = parseSize(server.client_max_body_size);
	if (!server.locations.empty())
		flag = 1;

	if (max_bytes > 0 && par.getBody().size() > max_bytes) //check client body size
		return generateError(413, server, "Request Entity Too Large");

	std::string base_path;
	std::string uploadPath;
	std::string uri = par.getUri();
	if (flag)
		uploadPath = server.locations[0].upload_store;

	if (!uploadPath.empty())
	{
		if (uploadPath[0] == '/' || (uploadPath.size() > 2 && uploadPath[0] == '.' && uploadPath[1] == '/'))
			base_path = uploadPath;
		else
			base_path = joinPaths(server.locations[0].root, uploadPath);
	}
	else
	{
		std::string aux = uri;
		if (flag && aux.find(server.locations[0].path) == 0)
			aux = aux.substr(server.locations[0].path.size() - 1);
		base_path = joinPaths(server.root, uri);
	}
	std::string finalPath;
	std::string filename;
	
	if (isDir(base_path)) //directory
	{
		std::string uri_name = "";
		size_t pos_2 = uri.find_last_of('/');
		if (pos_2 != std::string::npos && pos_2 != uri.size() - 1)
			uri_name = uri.substr(pos_2 + 1);
		if (!uri_name.empty())
			filename = uri_name;
		else
		{
			std::stringstream ss;
			ss << time(0);
			filename = "post_" + ss.str() + ".txt";
		}
		finalPath = joinPaths(base_path, filename);
	}
	else //file
	{
		finalPath = base_path;
		size_t pos = finalPath.find_last_of('/');
		if (pos != std::string::npos)
			filename = finalPath.substr(pos + 1);
		else
			filename = "uploaded_file";	
	}

	std::ofstream file(finalPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (file.is_open())
	{
		file << par.getBody();
		file.close();
		_statusCode = 201;
		_reason = HttpStatusCode::getReason(201);

		std::string uri_aux = uri;
        if (!uri.empty() && uri_aux[uri_aux.length() - 1] != '/')
            uri_aux += '/';
        if (isDir(base_path))
            getHeaders().set_http("Location", uri_aux + filename);
		else
			getHeaders().set_http("Location", uri);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Resource created successfully");
		setContent("text/plain");
		return build().toString();
	}
	else
		return generateError(500, server, "Error: Could not save file. Check permissions or path existence");
}


std::string HttpResponse::handle_delete(HttpRequest par, t_server server, int flag)
{
	std::string uri = par.getUri();
	std::string root = server.root;
	std::string path;
	if (!server.locations.empty())
		flag = 1;
	if (flag && !server.locations[0].root.empty())
	{
		root = server.locations[0].root;
		std::string loc_path = server.locations[0].path;
		if (uri.find(loc_path) == 0)
			path = joinPaths(root, uri.substr(loc_path.size()));
		else
			path = joinPaths(root, uri);
	}
	else
		path = joinPaths(root, uri);
	if (!isFile(path) && !isDir(path))
		return generateError(404, server, "404 Not found: File does not exist");
	if (isDir(path)) //For security dont delete directories
		return generateError(44030, server, "403 Forbidden: Directory access is disabled");
	if (access(path.c_str(), W_OK) != 0)
		return generateError(403, server, "403 Forbidden: Permission denied");
	std::cout << "Deleting file: " << path << std::endl;
	if (std::remove(path.c_str()) == 0) //Success no content
	{
		_statusCode = 204;
		_reason = HttpStatusCode::getReason(204);
		getHeaders().set_http("Server", "Webserv/1.0");
		return build().toString();
	}
	else
		return generateError(500, server, "500 Internal Server Error: Could not delete file");
}


//Aux
bool			HttpResponse::isvalidmethod(HttpRequest par, t_server server)
{
	std::string input = par.getMethod();
	int nbr_input = stringToMethod(input);
	if (nbr_input == -1)
		return false;
	if (server.locations.empty())
		return true;
	const std::vector <int> &allo = server.locations[0].methods;
	if (allo.empty())
		return true;
	std::vector <int>::const_iterator it = std::find(allo.begin(), allo.end(), nbr_input);
	return it != allo.end();
}


bool	HttpResponse::isFile(const std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
		return false;
	return S_ISREG(st.st_mode);
}

bool	HttpResponse::isDir(const std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
		return false;
	return S_ISDIR(st.st_mode);
}

bool	HttpResponse::haveIndex(const std::string &path)
{
	std::string indexPath = path + "index.html";
	return isFile(indexPath);
}

std::string	HttpResponse::getIndexFile(const std::string &path)
{
	std::string indexPath = path + "index.html";
	return indexPath;
}

std::string HttpResponse::autoIndexDir(const std::string &path)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir(path.c_str());
	if (!dir)
		return "<html><body><h1>Error opening directory</h1></body></html>";
	
	std::string html;
	html += "<html><head><title>Index of ";
    html += path;
    html += "</title></head><body>";
    html += "<h1>Index of " + path + "</h1><ul>";

	while  ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;

		if (name == "." || name == "..")
			continue;
		html += "<li><a href=\"";
        html += name;
		html += "\">";
        html += name;
        html += "</a></li>";
	}
	html += "</ul></body></html>";
	closedir(dir);
	return html;
}

std::string HttpResponse::generateError(int code, const t_server& server, const std::string& msg)
{
    _statusCode = code;
    _reason = HttpStatusCode::getReason(code);
	getHeaders().set_http("Server", "Webserv/1.0");
    
	bool server_custom = false;
	std::string body;

	std::string codeStr = intToString(code);

	if (server.error_page.count(codeStr) > 0)
	{
		std::string relativePath = server.error_page.find(codeStr)->second;
		std::string fullPath = joinPaths(server.root, relativePath);
		std::string file_content = readFile(fullPath);

		if (!file_content.empty())
		{
			body = file_content;
			setContent("text/html");
			server_custom = true;
		}
	}
	if (!server_custom)
    {
        std::stringstream ss;
        ss << "<!DOCTYPE html><html><head><title>" << code << " " << _reason << "</title></head>";
        ss << "<body style=\"font-family: Arial, sans-serif; text-align: center; margin-top: 50px;\">";
        ss << "<h1>Error " << code << "</h1>";
        ss << "<h2>" << _reason << "</h2>";
        
        if (!msg.empty())
            ss << "<p>" << msg << "</p>";
        else
            ss << "<p>The requested resource could not be found or processed.</p>";
            
        ss << "<hr><p><em>Webserv/1.0</em></p>";
        ss << "</body></html>";
        
        body = ss.str();
        setContent("text/html");
    }

    setBody(body);
    return build().toString();
}