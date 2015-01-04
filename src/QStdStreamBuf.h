#ifndef QSTDSTREAMBUF_H
#define QSTDSTREAMBUF_H

#include <ios>
#include <stdexcept>

#include <QIODevice>

class QStdStreamBuf : public std::streambuf
{
public:
	QStdStreamBuf(QIODevice *dev) : std::streambuf(), m_dev(dev)
	{
		// Initialize get pointer.  This should be zero so that underflow is called upon first read.
		this->setg(0, 0, 0);
	}

protected:
	virtual std::streamsize xsgetn(std::streambuf::char_type *str, std::streamsize n)
	{
		return m_dev->read(str, n);
	}

	virtual std::streamsize xsputn(const std::streambuf::char_type *str, std::streamsize n)
	{
		return m_dev->write(str, n);
	}

	virtual std::streambuf::pos_type seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode /*__mode*/)
	{
		switch(dir)
		{
		case std::ios_base::beg:
			break;
		case std::ios_base::end:
			off = m_dev->size() - off;
			break;
		case std::ios_base::cur:
			off = m_dev->pos() + off;
			break;
		default:
			throw std::runtime_error("Operation not implemented");
		}
		if(m_dev->seek(off))
			return m_dev->pos();
		else
			return std::streambuf::pos_type(std::streambuf::off_type(-1));
	}
	virtual std::streambuf::pos_type seekpos(std::streambuf::pos_type off, std::ios_base::openmode /*__mode*/)
	{
		if(m_dev->seek(off))
			return m_dev->pos();
		else
			return std::streambuf::pos_type(std::streambuf::off_type(-1));
	}

	virtual std::streambuf::int_type underflow()
	{
		// Read enough bytes to fill the buffer.
		std::streamsize len = sgetn(m_inbuf, sizeof(m_inbuf)/sizeof(m_inbuf[0]));

		// Since the input buffer content is now valid (or is new)
		// the get pointer should be initialized (or reset).
		setg(m_inbuf, m_inbuf, m_inbuf + len);

		// If nothing was read, then the end is here.
		if(len == 0)
			return traits_type::eof();

		// Return the first character.
		return traits_type::not_eof(m_inbuf[0]);
	}


private:
	static const std::streamsize BUFFER_SIZE = 1024;
	std::streambuf::char_type m_inbuf[BUFFER_SIZE];
	QIODevice *m_dev;
};

class QStdIStream : public std::istream
{
public:
	QStdIStream(QIODevice* dev) : std::istream(m_buf = new QStdStreamBuf(dev)) {}
	virtual ~QStdIStream()
	{
		rdbuf(0);
		delete m_buf;
	}

private:
	QStdStreamBuf * m_buf;
};

#endif // QSTDSTREAMBUF_H

