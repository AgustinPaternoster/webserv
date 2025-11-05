/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPStatusCode.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:33:45 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/04 14:19:55 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPSTATUSCODE_HPP_
# define _HTTPSTATUSCODE_HPP_

# include <map>
# include <string>

/**
 * HttpStatusCode
 * Gestionar códigos de estado HTTP 
 * y sus reason phrases con funciones helper
 */
class HttpStatusCode
{
	public:
			// 1xx Informational
			static const int CONTINUE = 100;
			static const int SWITCHING_PROTOCOLS = 101;

			// 2xx Success
			static const int OK = 200;
			static const int CREATED = 201;
			static const int ACCEPTED = 202;
			static const int NO_CONTENT = 204;
			static const int PARTIAL_CONTENT = 206;

			// 3xx Redirection
			static const int MOVED_MERMANENTLY = 301;
			static const int FOUND = 302;
			static const int SEE_OTHER = 303;
			static const int NOT_MODIFIED = 304;
			static const int TEMPORARY_REDICT = 307;
			static const int PERMANENT_REDICT = 308;

			// 4xx Client Error
			static const int BAD_REQUEST = 400;
			static const int UNAUTHORIZED = 401;
			static const int FORBIDDEN = 403;
			static const int NOT_FOUND = 404;
			static const int METHOD_NOT_ALLOWED = 405;
			static const int REQUEST_TIMEOUT = 408;
			static const int CONFLICT = 409;
			static const int GONE = 410;
			static const int LENGTH_REQUIRED = 411;
			static const int PAYLOAD_TOO_LARGE = 413;
			static const int URI_TOO_LONG = 414;
			static const int UNSUPPORTED_MEDIA_TYPE = 415;
			static const int TOO_MANY_REQUEST = 429;

			// 5xx Server Error
			static const int INTERNAL_SERVER_ERROR = 500;
			static const int NOT_IMPLEMENTED = 501;
			static const int BAD_GETAVAY = 502;
			static const int SERVICE_UNAVAILABLE = 503;
			static const int GETAWAY_TIMEOUT = 504;
			static const int HTTP_VERSION_NOT_SUPPORTED = 505;

			static std::string getReason(int code);
			static bool isInformational(int code);
			static bool isSuccess(int code);
			static bool isRedirection(int code);
			static bool isClientError(int code);
			static bool isServerError(int code);
			static bool isError(int code);

	private:
			static std::map<int, std::string> initReason();
};

#endif