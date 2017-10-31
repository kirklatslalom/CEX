#ifndef CEX_CRYPTOCIPHERMODEEXCEPTION_H
#define CEX_CRYPTOCIPHERMODEEXCEPTION_H

#include "CexDomain.h"

NAMESPACE_EXCEPTION

/// <summary>
/// Wraps exceptions thrown within Symmetric cipher mode operations
/// </summary>
struct CryptoCipherModeException : std::exception
{
private:

	std::string m_details;
	std::string m_message;
	std::string m_origin;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	CryptoCipherModeException& operator=(const CryptoCipherModeException&) = delete;

	/// <summary>
	/// Default constructor: default is restricted, this function has been deleted
	/// </summary>
	CryptoCipherModeException() = delete;

	/// <summary>
	/// Constructor: instantiate this class with a message
	/// </summary>
	///
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoCipherModeException(const std::string &Message);

	/// <summary>
	/// Constructor: instantiate this class with an origin and message
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	CryptoCipherModeException(const std::string &Origin, const std::string &Message);

	/// <summary>
	/// Constructor: instantiate this class with an origin, message and inner exception
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	/// <param name="Detail">The inner exception string</param>
	CryptoCipherModeException(const std::string &Origin, const std::string &Message, const std::string &Detail);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~CryptoCipherModeException();

	//~~~Accessors~~~//

	/// <summary>
	/// Read/Write: The inner exception string
	/// </summary>
	std::string &Details();

	/// <summary>
	/// Read/Write: The message associated with the error
	/// </summary>
	std::string &Message();

	/// <summary>
	/// Read/Write: The origin of the exception in the format Class
	/// </summary>
	std::string &Origin();
};

NAMESPACE_EXCEPTIONEND
#endif
