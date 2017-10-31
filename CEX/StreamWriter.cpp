#include "StreamWriter.h"

NAMESPACE_IO

//~~~Constructor~~~//

StreamWriter::StreamWriter(size_t Length)
	:
	m_streamState(Length),
	m_streamPosition(0)
{
}

StreamWriter::StreamWriter(const std::vector<byte> &DataArray)
	:
	m_streamState(DataArray),
	m_streamPosition(0)
{
}

StreamWriter::StreamWriter(MemoryStream &DataStream)
	:
	m_streamState(DataStream.ToArray()),
	m_streamPosition(0)
{
}

StreamWriter::~StreamWriter()
{
	m_streamPosition = 0;
	Utility::MemUtils::Clear(m_streamState, 0, m_streamState.size());
}

//~~~Accessors~~~//

const size_t StreamWriter::Length() 
{
	return m_streamState.size();
}

const size_t StreamWriter::Position() 
{ 
	return m_streamPosition;
}

//~~~Public Functions~~~//

std::vector<byte> &StreamWriter::GetBytes()
{
	return m_streamState;
}

MemoryStream* StreamWriter::GetStream()
{
	m_streamState.resize(m_streamPosition);
	return new MemoryStream(m_streamState);
}

NAMESPACE_IOEND
