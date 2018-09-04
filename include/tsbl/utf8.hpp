
#pragma once
#ifndef TSBL_UTF8_HPP
#define TSBL_UTF8_HPP

#include <stdint.h>

namespace tsbl::utf8 {
	typedef int32_t codepoint_t;
	
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
}

#endif
