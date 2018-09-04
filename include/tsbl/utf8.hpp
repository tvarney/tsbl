
#pragma once
#ifndef TSBL_UTF8_HPP
#define TSBL_UTF8_HPP

#include <stdint.h>
#include <utility>

namespace tsbl::utf8 {
	typedef int32_t codepoint_t;
	struct Codepoint {
		static const codepoint_t EndOfFile = -1;
		static const codepoint_t StartOfFile = -2;
		static const codepoint_t Invalid = -3;
	};
	
	enum Category : int32_t {
		CN = 0,  //< Not assigned
		LU = 1,  //< Letter, Uppercase
		LL = 2,  //< Letter, Lowercase
		LT = 3,  //< Letter, Titlecase
		LM = 4,  //< Letter, Modifier
		LO = 5,  //< Letter, Other
		MN = 6,  //< Mark, Nonspacing
		MC = 7,  //< Mark, Spacing Combining
		ME = 8,  //< Mark, Enclosing
		ND = 9,  //< Number, Decimal Digit
		NL = 10, //< Number, Letter
		NO = 11, //< Number, Other
		PC = 12, //< Punctuation, Connector
		PD = 13, //< Punctuation, Dash
		PS = 14, //< Punctuation, Open
		PE = 15, //< Punctuation, Close
		PI = 16, //< Punctuation, Initial Quote
		PF = 17, //< Punctuation, Final Quote
		PO = 18, //< Punctuation, Other
		SM = 19, //< Symbol, Math
		SC = 20, //< Symbol, Currency
		SK = 21, //< Symbol, Modifier
		SO = 22, //< Symbol, Other
		ZS = 23, //< Seperator, Space
		ZL = 24, //< Seperator, Line
		ZP = 25, //< Seperator, Paragraph
		CC = 26, //< Other, Control
		CF = 27, //< Other, Format
		CS = 28, //< Other, Surrogate
		CO = 29, //< Other, Private Use

		Unassigned = CN,
		LetterUppercase = LU,
		LetterLowercase = LL,
		LetterTitlecase = LT,
		LetterModifier = LM,
		LetterOther = LO,
		MarkNonspacing = MN,
		MarkSpacingCombining = MC,
		MarkEnclosing = ME,
		NumberDecimal = ND,
		NumberLetter = NL,
		NumberOther = NO,
		PunctuationConnector = PC,
		PunctuationDash = PD,
		PunctuationOpen = PO,
		PunctuationClose = PE,
		PunctuationQuoteInit = PI,
		PunctuationQuoteEnd = PF,
		PunctutationOther = PO,
		SymbolMath = SM,
		SymbolCurrency = SC,
		SymbolModifier = SK,
		SymbolOther = SO,
		SeperatorSpace = ZS,
		SeperatorLine = ZL,
		SeperatorParagraph = ZP,
		Control = CC,
		Format = CF,
		Surrogate = CS,
		Other = CO
	};

	Category category(codepoint_t);
	std::pair<intmax_t, codepoint_t>
		iterate(const uint8_t *string, int32_t strlen);

	class Reader {
	public:
		Reader();
		virtual ~Reader();

		virtual codepoint_t current() const;
		virtual codepoint_t next() = 0;
		virtual size_t write(codepoint_t * buffer, size_t count);

		virtual bool good() const;
		virtual bool bad() const;
		virtual bool eof() const;
		virtual bool bof() const;
	protected:
		codepoint_t m_Current;
	};

	// TODO: Look into memory mapped files instead of FILE * streams
	class FileReader : public Reader {
	public:
		FileReader(const char * filename, size_t buffsize = 4096);
		virtual ~FileReader();

		virtual codepoint_t next();

		virtual bool bad() const;
	protected:
		void * m_FilePtr; //< FILE *, handles the buffering for us
		uint8_t * m_Buffer;
		size_t m_BufferIndex, m_BufferSize, m_BufferData;
	};

	class StringReader : public Reader {
	public:
		StringReader(const uint8_t * data);
		virtual ~StringReader();

		virtual codepoint_t next();
	protected:
		const uint8_t * m_Data;
		size_t m_Index;
	};
}

#endif
