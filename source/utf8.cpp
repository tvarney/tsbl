
#include "tsbl/utf8.hpp"

#include <cstdio>
#define UTF8PROC_STATIC
#include "utf8proc.h"

using namespace tsbl;

static const char * _g_category_id[] = {
    "CN", "LU", "LL", "LT", "LM", "LO", "MN", "MC", "ME", "ND", "NL", "NO",
    "PC", "PD", "PS", "PE", "PI", "PF", "PO", "SM", "SC", "SK", "SO", "ZS",
    "ZL", "ZP", "CC", "CF", "CS", "CO"
};
static const char * _g_category_name[] = {
    "Unassigned", "Letter, Uppercase", "Letter, Lowercase",
    "Letter, Titlecase", "Letter, Modifier", "Letter, Other",
    "Mark, Nonspacing", "Mark, Spacing Combining", "Mark, Enclosing",
    "Number, Decimal", "Number, Letter", "Number, Other",
    "Punctuation, Connector", "Punctuation, Dash", "Punctuation, Open",
    "Punctuation, Close", "Punctuation, Initial Quote",
    "Punctuation, Final Quote", "Punctuation, Other", "Symbol, Math",
    "Symbol, Currency", "Symbol, Modifier", "Symbol, Other",
    "Seperator, Space", "Seperator, Line", "Seperator, Paragraph",
    "Control", "Format", "Surrogate", "Private, Other"
};

const utf8::codepoint_t utf8::Codepoint::EndOfFile;
const utf8::codepoint_t utf8::Codepoint::StartOfFile;
const utf8::codepoint_t utf8::Codepoint::Invalid;

utf8::Category utf8::category(utf8::codepoint_t codepoint) {
    return static_cast<utf8::Category>(utf8proc_category(codepoint));
}
const char * utf8::category_id(utf8::Category cat) {
    return _g_category_id[cat];
}
const char * utf8::category_name(utf8::Category cat) {
    return _g_category_name[cat];
}

std::pair<intmax_t, utf8::codepoint_t>
utf8::iterate(const uint8_t *string, int32_t strlen) {
    codepoint_t codepoint = 0;
    auto advance = utf8proc_iterate(string, strlen, &codepoint);
    return std::make_pair(advance, codepoint);
}

//=============================================
// utf8::Reader

utf8::Reader::Reader() :
    m_Current(utf8::Codepoint::StartOfFile)
{ }

utf8::Reader::~Reader() { }

utf8::codepoint_t utf8::Reader::current() const {
    return m_Current;
}

size_t utf8::Reader::write(utf8::codepoint_t * buffer, size_t count) {
    if (bad()) {
        return 0;
    }
    for (size_t i = 0; i < count; ++i) {
        next();
        if (bad()) {
            return i;
        }
        buffer[i] = m_Current;
    }
    return count;
}

bool utf8::Reader::good() const {
    return !bad();
}

bool utf8::Reader::bad() const {
    return (m_Current == utf8::Codepoint::Invalid
        || m_Current == utf8::Codepoint::EndOfFile
    );
}

bool utf8::Reader::eof() const {
    return m_Current == utf8::Codepoint::EndOfFile;
}

bool utf8::Reader::bof() const {
    return m_Current == utf8::Codepoint::StartOfFile;
}

//=============================================
// utf8::FileStream
utf8::FileReader::FileReader(const char * filename, size_t buffsize) :
    m_FilePtr(nullptr), m_Buffer(nullptr), m_BufferIndex(0),
    m_BufferSize(buffsize), m_BufferData(0)
{
    m_FilePtr = (void *)std::fopen(filename, "rb");
    if (m_FilePtr != nullptr) {
        // Turn off the stream buffering
        std::setvbuf((FILE *)m_FilePtr, nullptr, _IONBF, 0);
    }
    else {
        m_Current = utf8::Codepoint::EndOfFile;
    }
}

utf8::FileReader::~FileReader() {
    if (m_FilePtr != nullptr) {
        std::fclose((FILE *)m_FilePtr);
        m_FilePtr = nullptr;
    }
}

utf8::codepoint_t utf8::FileReader::next() {
    // Early exit - if we already are in an ending state (invalid or end of
    // file), don't do any of the other stuff.
    if(bad()) {
        return m_Current;
    }
    
    // This is just so we can reference the file pointer without needing to
    // convert it from a void * every time. 'fp' is much shorter than
    // '(FILE *)m_FilePtr'
    FILE * fp = static_cast<FILE *>(m_FilePtr);

    // If we haven't read anything yet, attempt to populate the buffer
    if(bof()) {
        m_BufferData = std::fread(m_Buffer, m_BufferSize, 1, fp);
        if (m_BufferData == 0) {
            // No data could be read - bad file
            m_Current = utf8::Codepoint::EndOfFile;
            return m_Current;
        }
    }

    // Handle buffer with very little data - we want at least 4 bytes if
    // possible. If that isn't possible, we can still try to process the data
    // in the buffer, but it may be invalid.
    if (m_BufferData - m_BufferIndex < 4) {
        if (m_BufferData == m_BufferSize) {
            // Shift remaining bytes (if any) to the start of the buffer
            size_t shift = m_BufferData - m_BufferIndex;
            for (size_t i = 0; i < shift; ++i) {
                m_Buffer[i] = m_Buffer[m_BufferIndex + i];
            }

            // Read the remainder of the buffer size
            size_t bytes_read = std::fread(m_Buffer + shift,
                m_BufferSize - shift, 1, fp);

            // The final buffer data count includes shift
            if (bytes_read > 0) {
                m_BufferData = shift + bytes_read;
            }
            else {
                m_BufferData = shift;
            }
            m_BufferIndex = 0;
        }
        else {
            // We hit the end of the file last time, no need to read
            m_Current = utf8::Codepoint::EndOfFile;
            return m_Current;
        }
    }

    // At this point, we have our data read from the stream one way or another
    int32_t length = (m_BufferData - m_BufferIndex >= 4 ?
        (int32_t)4 :
        (int32_t)(m_BufferData - m_BufferIndex)
    );
    auto results = utf8::iterate(m_Buffer + m_BufferIndex, length);

    if (results.first > 0) {
        m_BufferIndex += results.first;
        m_Current = results.second;
    }
    else {
        m_Current = utf8::Codepoint::Invalid;
    }
    return m_Current;
}

bool utf8::FileReader::bad() const {
    return m_FilePtr == nullptr || Reader::bad();
}

//====================================
// utf8::StringStream
utf8::StringReader::StringReader(const uint8_t * data) :
    m_Data(data), m_Index(0)
{ }

utf8::StringReader::~StringReader() { }

utf8::codepoint_t utf8::StringReader::next() {
    size_t len = 0;
    for (; len < 4 && m_Data[m_Index + len] != 0; ++len) {}
    if (len == 0) {
        m_Current = utf8::Codepoint::EndOfFile;
        return m_Current;
    }
    // The cast is safe - len is always [0, 4]
    auto results = utf8::iterate(m_Data + m_Index, (int32_t)(len));
    if (results.first > 0) {
        m_Index += results.first;
        m_Current = results.second;
    }
    else {
        m_Current = utf8::Codepoint::Invalid;
    }
    return m_Current;
}
