#if !defined(SQL_ODBCXX_FILE_STREAM_BUFFER_HH)
#define SQL_ODBCXX_FILE_STREAM_BUFFER_HH

#include <windef.h>
#include <WinBase.h>
#include <fileapi.h>

#include <cstdlib>
#include <string>
#include <streambuf>
#include <span>
#include <utility>
#include <algorithm>
#include <execution>

#include "WindowsCategory.hh"

namespace odbc
{
    template <typename CharT, class CharTraitsT = std::basic_streambuf<CharT>::traits_type>
	class FileStreamBuffer: public std::basic_streambuf<CharT, CharTraitsT>
    {
    protected:
	using ios_base = std::ios_base;

	ios_base::openmode openMode { };
	bool closeHandle = false;
	HANDLE hFileHandle = INVALID_HANDLE_VALUE;
	std::unique_ptr<CharT> bufferPtr;
	std::span<CharT> readBuffer { }, writeBuffer { };

	// 29.6.3.5.2, buffer management and positioning
	virtual std::basic_streambuf<CharT, CharTraitsT> *setbuf(CharT *s, std::streamsize n) override;
	virtual typename CharTraitsT::pos_type seekoff(typename CharTraitsT::off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out) override;
	virtual typename CharTraitsT::pos_type seekpos(typename CharTraitsT::pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out) override;
	virtual int sync() override;

	// 29.6.3.5.3, get area
	virtual std::streamsize showmanyc() override;
	virtual std::streamsize xsgetn(CharT *s, std::streamsize n) override;
	virtual typename CharTraitsT::int_type underflow() override;

	// 29.6.3.5.4, putback
	virtual typename CharTraitsT::int_type pbackfail(typename CharTraitsT::int_type c = CharTraitsT::eof()) override;

	// 29.6.3.5.5, put area
	virtual std::streamsize xsputn(const CharT *s, std::streamsize n) override;
	virtual typename CharTraitsT::int_type overflow(typename CharTraitsT::int_type c = CharTraitsT::eof()) override;

    public:
	std::size_t DEFAULT_BUFFER_SIZE;

	FileStreamBuffer() = default;
	FileStreamBuffer(std::string const &fileName, ios_base::openmode mode);
	FileStreamBuffer(char const *fileName, ios_base::openmode mode);
	FileStreamBuffer(HANDLE fileHandle, ios_base::openmode mode);
	FileStreamBuffer(FileStreamBuffer const &other);
	FileStreamBuffer(FileStreamBuffer &&moved);

	FileStreamBuffer &operator =(FileStreamBuffer const &other);
	FileStreamBuffer &operator =(FileStreamBuffer &&moved);

	void open(std::string const &fileName, ios_base::openmode mode);
	void open(char const *fileName, ios_base::openmode mode);
	void open(HANDLE fileHandle, ios_base::openmode mode);

	HANDLE native_handle() const;

	void close();

	~FileStreamBuffer();
    };

    // extern template FileStreamBuffer<char>;
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT>::FileStreamBuffer(std::string const &fileName, ios_base::openmode mode)
{
    open(fileName, mode);
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT>::FileStreamBuffer(char const *fileName, ios_base::openmode mode)
{
    open(fileName, mode);
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT>::FileStreamBuffer(HANDLE fileHandle, ios_base::openmode mode)
{
    open(fileHandle, mode);
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT>::FileStreamBuffer(FileStreamBuffer const &other)
{
    if (other.hFileHandle != INVALID_HANDLE_VALUE)
    {
	HANDLE hNewHandle = INVALID_HANDLE_VALUE;

	if (!::DuplicateHandle(GetCurrentProcess(), other.hFileHandle, GetCurrentProcess(), &hNewHandle, 0, TRUE, DUPLICATE_SAME_ACCESS))
	    throw windows_error_code();

	open(hNewHandle, other.openMode);
    }
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT>::FileStreamBuffer(FileStreamBuffer &&moved):
	openMode { moved.openMode }, closeHandle { moved.closeHandle }, hFileHandle { moved.hFileHandle }, bufferPtr(std::move(moved.bufferPtr)),
	readBuffer { moved.readBuffer }, writeBuffer { moved.writeBuffer }
{
    moved.openMode = ios_base::openmode { };
    moved.closeHandle = false;
    moved.hFileHandle = INVALID_HANDLE_VALUE;
    moved.readBuffer = std::span<CharT> { };
    moved.writeBuffer = std::span<CharT> { };
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT> &odbc::FileStreamBuffer<CharT, CharTraitsT>::operator =(odbc::FileStreamBuffer<CharT, CharTraitsT> const &other)
{
    close();

    if (other.hFileHandle != INVALID_HANDLE_VALUE)
    {
	HANDLE hNewHandle = INVALID_HANDLE_VALUE;

	if (!::DuplicateHandle(GetCurrentProcess(), other.hFileHandle, GetCurrentProcess(), &hNewHandle, 0, TRUE, DUPLICATE_SAME_ACCESS))
	    throw windows_error_code();

	open(hNewHandle, other.openMode);
    }
}

template <typename CharT, class CharTraitsT>
    inline odbc::FileStreamBuffer<CharT, CharTraitsT> &odbc::FileStreamBuffer<CharT, CharTraitsT>::operator =(odbc::FileStreamBuffer<CharT, CharTraitsT> &&moved)
{
    close();

    openMode	= moved.openMode;
    closeHandle = moved.closeHandle;
    hFileHandle = moved.hFileHandle;
    bufferPtr	= std::move(moved.bufferPtr);
    readBuffer	= moved.readBuffer;
    writeBuffer = moved.writeBuffer;

    moved.openMode = ios_base::openmode { };
    moved.closeHandle = false;
    moved.hFileHandle = INVALID_HANDLE_VALUE;
    moved.readBuffer = std::span<CharT> { };
    moved.writeBuffer = std::span<CharT> { };
}

template <typename CharT, class CharTraitsT>
    inline  HANDLE odbc::FileStreamBuffer<CharT, CharTraitsT>::native_handle() const
{
    return hFileHandle;
}

template <typename CharT, class CharTraitsT>
    inline void odbc::FileStreamBuffer<CharT, CharTraitsT>::close()
{
    if (hFileHandle != INVALID_HANDLE_VALUE)
    {
	sync();

	if (!::CloseHandle(hFileHandle))
	    throw windows_error_code();

	hFileHandle = INVALID_HANDLE_VALUE;
    }
}

#endif	    // !defined(SQL_ODBCXX_FILE_STREAM_BUFFER_HH)
