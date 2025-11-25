/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:48:31 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/25 20:03:38 by camurill         ###   ########.fr       */
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


std::string	HttpResponse::execute_response(HttpRequest par, Config &config)
{
	size_t num = get_port_www(config, par);
	const std::vector<t_server> & servers = config.getServers();
	const t_server &server = servers[num];
	if (!isvalidmethod(par, server))
	{
		_statusCode = 405;
		_reason = HttpStatusCode::getReason(405);
		this->getHeaders().set_http("Server", "Webserv/1.0");
		this->setBody("File not found");
		this->setContent("/text/plain");

		return build().toString();
	}
	if (par.getMethod() == "GET")
		return handle_get(par, server, 1);
	else
	{
		_statusCode = 501;
		_reason = HttpStatusCode::getReason(501);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
		return build().toString();
	}
	/*if (isCGI())
			execute(par, config); //only flag*/
	/*switch (par.getMethod()) //location status code
	{
	case GET:
		return handle_get(par, server);
	case POST:
		return handle_post(par, config);
	case DELETE:
		return handle_delete(par, config);
	default:
		_statusCode = 501;
		_reason = HttpStatusCode::getReason(501);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
		return ;
	}*/
}

std::string	HttpResponse::handle_get(HttpRequest par, t_server server, int flag)
{
	std::string uri = par.getUri();
	std::string root;

	if (flag && !server.locations[0].root.empty())
	{
		root = server.locations[0].root;
		uri = uri.substr(server.locations[0].path.size() - 1);
	}
	else
		root = server.root;
	std::string path = root + uri;
	std::cout << path << std::endl;
	if (isFile(path))
	{
		_statusCode = 200;
		_reason = HttpStatusCode::getReason(200);
		getHeaders().set_http("Server", "Webserver/1.0");
		setBodyFile(path);
		return build().toString();
	}
	if (!isDir(path))
	{
		_statusCode = 404;
		_reason = HttpStatusCode::getReason(404);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Not found");
		setContent("text/plain");
		return build().toString();
	}
	if (path[path.size() - 1] != '/') //red
	{
		_statusCode = 301;
		_reason = HttpStatusCode::getReason(301);
		getHeaders().set_http("Location", path + "/");
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
	if (server.locations[0].autoindex == "off")
	{
		_statusCode = 403;
		_reason = HttpStatusCode::getReason(403);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
		return build().toString();
	}
	std::string listen = autoIndexDir(path);
	_statusCode = 200;
	_reason = HttpStatusCode::getReason(200);
	getHeaders().set_http("Server", "Webserv/1.0");
	setBody(listen);
	setContent("text/html");
	return build().toString();
}

/*std::string	HttpResponse::handle_post(HttpRequest par, Config &config)
{
	if (isdirectory() && has_perm())
	{
		return upload_file;
	}
	else
	{
		_statusCode = 403;
		_reason = HttpStatusCode::getReason(403);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
	}
}

std::string HttpReponse::handle_delete(HttpRequest par, Config &config)
{
	if (isfile(uri))
		return file_delete(uri);
	if (!isdirectory() && ismidpath())
	{
		_statusCode = 409;
		_reason = HttpStatusCode::getReason(409);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
	}
	if (!has_perm())
	{
		_statusCode = 403;
		_reason = HttpStatusCode::getReason(403);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented");
		setContent("text/plain");
		return toString()
	}
	if (is_delete())
	{
		_statuscode = 204;
		_reason = HttpStatusCode::getReason(204);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method implemented"); //delete succs
		setContent("text/plain");
		return toString()
	}
	else
	{
		_statuscode = 500;
		_reason = HttpStatusCode::getReason(500);
		getHeaders().set_http("Server", "Webserv/1.0");
		setBody("Method not implemented"); //delete succs
		setContent("text/plain");
		return toString();
	}
}*/


//Aux
bool			HttpResponse::isvalidmethod(HttpRequest par, t_server server)
{
	std::string input = par.getMethod();
	int nbr_input = stringToMethod(input);
	if (nbr_input == -1)
		return false;
	const std::vector <int> &allo = server.locations[0].methods;
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