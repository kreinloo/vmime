//
// VMime library (http://vmime.sourceforge.net)
// Copyright (C) 2002-2004 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef VMIME_TEXT_HPP_INCLUDED
#define VMIME_TEXT_HPP_INCLUDED


#include "base.hpp"
#include "word.hpp"


namespace vmime
{


/** List of encoded-words, as defined in RFC-2047 (basic type).
  */

class text : public component
{
public:

	text();
	text(const text& t);
	text(const string& t, const charset& ch);
	explicit text(const string& t);
	explicit text(const word& w);
	~text();

public:

	const bool operator==(const text& t) const;
	const bool operator!=(const text& t) const;

	text* clone() const;
	void copyFrom(const component& other);
	text& operator=(const component& other);
	text& operator=(const text& other);

	/** Add a word at the end of the list.
	  *
	  * @param w word to append
	  */
	void appendWord(word* w);

	/** Insert a new word before the specified position.
	  *
	  * @param pos position at which to insert the new word (0 to insert at
	  * the beginning of the list)
	  * @param w word to insert
	  */
	void insertWordBefore(const int pos, word* w);

	/** Insert a new word after the specified position.
	  *
	  * @param pos position of the word before the new word
	  * @param w word to insert
	  */
	void insertWordAfter(const int pos, word* w);

	/** Remove the word at the specified position.
	  *
	  * @param pos position of the word to remove
	  */
	void removeWord(const int pos);

	/** Remove all words from the list.
	  */
	void removeAllWords();

	/** Return the number of words in the list.
	  *
	  * @return number of words
	  */
	const int getWordCount() const;

	/** Tests whether the list of words is empty.
	  *
	  * @return true if there is no word, false otherwise
	  */
	const bool isEmpty() const;

	/** Return the word at the specified position.
	  *
	  * @param pos position
	  * @return word at position 'pos'
	  */
	word* getWordAt(const int pos);

	/** Return the word at the specified position.
	  *
	  * @param pos position
	  * @return word at position 'pos'
	  */
	const word* const getWordAt(const int pos) const;

	/** Return the word list.
	  *
	  * @return list of words
	  */
	const std::vector <const word*> getWordList() const;

	/** Return the word list.
	  *
	  * @return list of words
	  */
	const std::vector <word*> getWordList();

	// Decoding
#if VMIME_WIDE_CHAR_SUPPORT
	const wstring getDecodedText() const;
#endif

	/** Return the text converted into the specified charset.
	  * The encoded-words are decoded and then converted in the
	  * specified destination charset.
	  *
	  * @param dest output charset
	  * @return text decoded in the specified charset
	  */
	const string getConvertedText(const charset& dest) const;

	/** This function can be used to make several encoded words from a text.
	  * All the characters in the text must be in the same specified charset.
	  *
	  * <p>Eg: giving:</p>
	  * <pre>   &lt;iso-8859-1> "Linux dans un t'el'ephone mobile"
	  *    ("=?iso-8859-1?Q?Linux_dans_un_t=E9l=E9phone_mobile?=")
	  * </pre><p>it will return:</p>
	  * <pre>   &lt;us-ascii>   "Linux dans un "
	  *    &lt;iso-8859-1> "t'el'ephone "
	  *    &lt;us-ascii>   "mobile"
	  *    ("Linux dans un =?iso-8859-1?Q?t=E9l=E9phone_?= mobile")
	  * </pre>
	  *
	  * @param in input string
	  * @param ch input charset
	  * @param generateInExisting if not NULL, the resulting text will be generated
	  * in the specified object instead of a new created object (in this case, the
	  * function returns the same pointer). Can be used to avoid copying the
	  * resulting object into an existing object.
	  * @return new text object or existing object if generateInExisting != NULL
	*/
	static text* newFromString(const string& in, const charset& ch, text* generateInExisting = NULL);

	/** Flags used by "encodeAndFold" function.
	  */
	enum EncodeAndFoldFlags
	{
		// NOTE: If both "FORCE_NO_ENCODING" and "FORCE_ENCODING" are
		// specified, "FORCE_NO_ENCODING" is used by default.

		FORCE_NO_ENCODING = (1 << 0),    /**< Just fold lines, don't encode them. */
		FORCE_ENCODING = (1 << 1),       /**< Encode lines even if they are plain ASCII text. */
		NO_NEW_LINE_SEQUENCE = (1 << 2)  /**< Use CRLF instead of new-line sequence (CRLF + TAB). */
	};

	/** Encode and fold text in respect to RFC-2047.
	  *
	  * @param os output stream
	  * @param maxLineLength maximum line length for output
	  * @param firstLineOffset the first line length (may be useful if the current output line is not empty)
	  * @param lastLineLength will receive the length of the last line written
	  * @param flags encoding flags (see EncodeAndFoldFlags)
	  */
	void encodeAndFold(utility::outputStream& os, const string::size_type maxLineLength,
		const string::size_type firstLineOffset, string::size_type* lastLineLength, const int flags) const;

	/** Decode and unfold text (RFC-2047).
	  *
	  * @param in input string
	  * @param generateInExisting if not NULL, the resulting text will be generated
	  * in the specified object instead of a new created object (in this case, the
	  * function returns the same pointer). Can be used to avoid copying the
	  * resulting object into an existing object.
	  * @return new text object or existing object if generateInExisting != NULL
	  */
	static text* decodeAndUnfold(const string& in, text* generateInExisting = NULL);


	using component::parse;
	using component::generate;

	// Component parsing & assembling
	void parse(const string& buffer, const string::size_type position, const string::size_type end, string::size_type* newPosition = NULL);
	void generate(utility::outputStream& os, const string::size_type maxLineLength = lineLengthLimits::infinite, const string::size_type curLinePos = 0, string::size_type* newLinePos = NULL) const;

private:

	static void decodeAndUnfold(const string::const_iterator& inStart, const string::const_iterator& inEnd, text& out);

	std::vector <word*> m_words;
};


} // vmime


#endif // VMIME_TEXT_HPP_INCLUDED