#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>

#define smpEx(msg) Exception(__FILE__, __LINE__, msg)
#define fmtEx(fmt, ...) Exception(__FILE__, __LINE__, fmt, __VA_ARGS__)

class Exception : public std::runtime_error {
public:
	Exception(const char* file, int line, const std::string& message) : std::runtime_error(message) {
		std::ostringstream o;
		o << file << ":" << line << ": " << message;
		this->m_sMsg = o.str();
	}

	template<typename ... Arguments> Exception(const char* file, unsigned int line, const std::string& fmt, Arguments ... args) : std::runtime_error("error") {
		unsigned int stringSize = std::snprintf(nullptr, 0, fmt.c_str(), args ...) + strlen(file) + std::to_string(line).length() + 4;
		if(stringSize <= 0) throw smpEx("Formatting error!");
		auto size = static_cast<size_t>(stringSize);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, fmt.c_str(), args ...);
		std::ostringstream o;
		o << file << ":" << line << ": " << buf.get();
		this->m_sMsg = o.str();
	}

	~Exception() throw() {}
	const char *what() const throw() {
		return this->m_sMsg.c_str();
	}

private:
	std::string m_sMsg;
};

#endif // EXCEPTION_HPP
