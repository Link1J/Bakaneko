
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/memory_mapped_file.hpp"
#include "ljh/system_info.hpp"

#if defined(LJH_TARGET_Windows)
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#    include <memoryapi.h>
#    include <cstring>
#else
#    include <unistd.h>
#    include <fcntl.h>
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <sys/mman.h>
#    include <string.h>
#endif

ljh::memory_mapped::file::file(std::filesystem::path&& filename, permissions permissions)
{
	if ((permissions & permissions::rw) == permissions::w) { throw invalid_permissions{}; }
	if ((permissions & permissions::rwx) == permissions::x) { throw invalid_permissions{}; }

#ifdef _WIN32
	DWORD access    = 0;
	DWORD shareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;
	DWORD protect   = 0;

	if ((permissions & permissions::r) == permissions::r) { access |= GENERIC_READ   ; }
	if ((permissions & permissions::w) == permissions::w) { access |= GENERIC_WRITE  ; }
	if ((permissions & permissions::x) == permissions::x) { access |= GENERIC_EXECUTE; }

	switch (permissions)
	{
	case permissions::r  : protect = PAGE_READONLY         ; break;
	case permissions::x  : protect = PAGE_EXECUTE          ; break;
	case permissions::rw : protect = PAGE_READWRITE        ; break;
	case permissions::rx : protect = PAGE_EXECUTE_READ     ; break;
	case permissions::rwx: protect = PAGE_EXECUTE_READWRITE; break;
	}

	file_handle = CreateFileW(filename.c_str(), access, shareMode, NULL, OPEN_EXISTING, 0, NULL);
	if (file_handle == invalid_handle) { throw invalid_file{}; }
	file_descriptor = CreateFileMappingW(file_handle, NULL, protect, 0, 0, NULL);
	if (file_descriptor == invalid_handle) { throw invalid_file{}; }
	
	LARGE_INTEGER size_help;
	GetFileSizeEx(file_handle, &size_help);
	memcpy(&filesize, &size_help, sizeof(filesize));
#else
	int mode = 0;
	switch (permissions)
	{
	case permissions::r  : mode = O_RDONLY; break;
	case permissions::w  : mode = O_WRONLY; break;
	case permissions::rw : mode = O_RDWR  ; break;
	case permissions::rx : mode = O_RDONLY; break;
	case permissions::rwx: mode = O_RDWR  ; break;
	}

	struct stat st;
	if (stat(filename.c_str(), &st)) { throw invalid_file{}; }
	file_descriptor = open(filename.c_str(), mode);
	if (file_descriptor == invalid_handle) { throw invalid_file{}; }
	filesize = st.st_size;
#endif
}

ljh::memory_mapped::file::~file()
{
	if (!is_open()) { return; }
#ifdef _WIN32
	CloseHandle(file_descriptor);
	CloseHandle(file_handle);
#else
	close(file_descriptor);
#endif
}

ljh::memory_mapped::file::file(file&& other)
{
	file_descriptor = other.file_descriptor;
	file_handle     = other.file_handle    ;
	filesize        = other.filesize       ;
	other.file_descriptor = invalid_handle ;
	other.file_handle     = invalid_handle ;
	other.filesize        = 0              ;
}

ljh::memory_mapped::file& ljh::memory_mapped::file::operator=(file&& other)
{
	file_descriptor = other.file_descriptor;
	file_handle     = other.file_handle    ;
	filesize        = other.filesize       ;
	other.file_descriptor = invalid_handle ;
	other.file_handle     = invalid_handle ;
	other.filesize        = 0              ;
	return *this;
}

size_t ljh::memory_mapped::file::size() const noexcept
{
	return filesize;
}

bool ljh::memory_mapped::file::is_open() const noexcept
{
	return file_descriptor != invalid_handle;
}

ljh::memory_mapped::view::view(file& fd, permissions permissions, size_t start, size_t length)
{
	if (!fd.is_open())
	{
		throw invalid_file{};
	}
	if (start < 0 || start + length > fd.size() || length < 0)
	{
		throw invalid_position{};
	}
	if ((permissions & permissions::rw ) == permissions::w) { throw invalid_permissions{}; }
	if ((permissions & permissions::rwx) == permissions::x) { throw invalid_permissions{}; }

	if ((permissions & (permissions::write | permissions::copy_on_write)) == (permissions::write | permissions::copy_on_write))
	{
		throw invalid_permissions{};
	}
#ifdef _WIN32
	DWORD access = 0;

	switch (permissions)
	{
	case permissions::r  : access = FILE_MAP_READ                    ; break;
	case permissions::rw : access = FILE_MAP_WRITE                   ; break;
	case permissions::rx : access = FILE_MAP_READ  | FILE_MAP_EXECUTE; break;
	case permissions::rwx: access = FILE_MAP_WRITE | FILE_MAP_EXECUTE; break;
	}

	if ((permissions & permissions::copy_on_write) == permissions::copy_on_write)
	{
		access |= FILE_MAP_COPY;
	}
	
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);

	offset = start % sys_info.dwAllocationGranularity;
	start = floor(start / sys_info.dwAllocationGranularity) * sys_info.dwAllocationGranularity;
	length += offset;

	data = MapViewOfFile(fd.file_descriptor, access, start >> 32, start & 0xFFFFFFFF, length);
	if (data == nullptr) { throw invalid_file{}; }
	this->length = length - offset;
#else
	int prot  = 0;
	int flags = 0;

	if ((permissions & permissions::r) == permissions::r) { prot |= PROT_READ ; }
	if ((permissions & permissions::w) == permissions::w) { prot |= PROT_WRITE; }
	if ((permissions & permissions::x) == permissions::x) { prot |= PROT_EXEC ; }

	if ((permissions & permissions::copy_on_write) == permissions::copy_on_write)
	{
		prot |= PROT_WRITE;
		flags |= MAP_PRIVATE;
	}
	else
	{
		flags |= MAP_SHARED;
	}
	
	auto page_size = sysconf(_SC_PAGESIZE);

	offset = start % page_size;
	start = floor(start / page_size) * page_size;
	length += offset;

	data = mmap(nullptr, length, prot, flags, fd.file_descriptor, start);
	if (data == MAP_FAILED) { throw invalid_file{}; }
	this->length = length;
#endif
}

ljh::memory_mapped::view::~view()
{
	if (!valid()) { return; }
	flush();
#ifdef _WIN32
	UnmapViewOfFile(data);
#else
	munmap(data, length);
#endif
	data = nullptr;
	length = 0;
}

ljh::memory_mapped::view::view(view&& other)
{
	data   = other.data   ;
	length = other.length ;
	other.data   = nullptr;
	other.length = 0      ;
}

ljh::memory_mapped::view& ljh::memory_mapped::view::operator=(view&& other)
{
	data   = other.data   ;
	length = other.length ;
	other.data   = nullptr;
	other.length = 0      ;
	return *this;
}

void ljh::memory_mapped::view::flush() noexcept
{
#ifdef _WIN32
	FlushViewOfFile(data, length);
#else 
	msync(data, length, MS_ASYNC);
#endif
}

bool ljh::memory_mapped::view::valid() const noexcept
{
	return data != nullptr;
}

ljh::memory_mapped::io_error::io_error()
{
#ifdef _WIN32
	_error_code = GetLastError();
#else 
	_error_code = errno;
#endif
}

const uint32_t ljh::memory_mapped::io_error::error_code() const noexcept
{
	return _error_code;
}

const char* ljh::memory_mapped::io_error::what() const noexcept
{
	return "unknown io_error";
}

const char* ljh::memory_mapped::invalid_position::what() const noexcept
{
	return "invalid_position: index into file, out of bounds";
}

const char* ljh::memory_mapped::invalid_file::what() const noexcept
{
	return "invalid_file: could not open file";
}

const char* ljh::memory_mapped::invalid_permissions::what() const noexcept
{
	return "invalid_permissions";
}

const char* ljh::memory_mapped::io_error::error_string() const noexcept
{
#ifdef _WIN32
	static char buffer[300];
	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, _error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL
	);
	return buffer;
#else 
	return strerror(_error_code);
#endif
}