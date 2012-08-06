/* 
 * Copyright 2012, Ilya Tsvetkov
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal 
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software,  and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * The software is provided as is, without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose and noninfringement. In no event shall the
 * authors or copyright holders be liable for any claim, damages or other
 * liability, whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other dealings in
 * the software.
 */

#ifndef __ITSVETKOV__XML_STREAMER_H__
#define __ITSVETKOV__XML_STREAMER_H__

#include <sstream>
#include <stack>
#include <string>

namespace xmls {

struct tag {
	tag(const std::wstring &tag_name) : tag_name_(tag_name) {}
	std::wstring tag_name_;
};

struct attr {
	attr(const std::wstring &attr_name) : attr_name_(attr_name) {}
	std::wstring attr_name_;
};

struct close {

};

class xmlstreamer {
public:
	xmlstreamer(std::wostream &stream) :
		indent_(L"\t"),
		stream_(stream),
		state_(STATE_NONE)
	{
		write_prolog();
	}

	template<typename T>
	xmlstreamer &operator <<(const T &data) {
		switch (state_) {
		case STATE_TAG:
			end_tag();
			// Fall through
		case STATE_NONE:
			state_ = STATE_DATA;
			// Fall through
		case STATE_DATA:
			buffer_ << data;
			break;
		case STATE_ATTR:
			buffer_ << data;
			flush_buffer();
			stream_ << L'"';
			state_ = STATE_TAG;
			break;
		default:
			break;
		}
		return *this;
	}

	xmlstreamer &operator <<(const tag &val) {
		start_tag(val.tag_name_);
		return *this;
	}

	xmlstreamer &operator <<(const attr &val) {
		start_attr(val.attr_name_);
		return *this;
	}

	xmlstreamer &operator <<(const close &) {
		close_tag();
		return *this;
	}

private:
	enum WriterState {
		STATE_NONE,
		STATE_TAG,
		STATE_ATTR,
		STATE_DATA
	};

	void indent() {
		stream_ << L'\n';
		size_t size = tag_stack_.size();
		for (size_t i = 1; i < size; ++i) {
			stream_ << indent_;
		}
	}

	void flush_buffer() {
		std::wstring buffer = buffer_.str();
		for (std::wstring::const_iterator iter = buffer.begin();
				iter != buffer.end(); ++iter) {
			switch (*iter) {
			case L'&':
				stream_ << L"&amp;";
				break;
			case L'<':
				stream_ << L"&lt;";
				break;
			case L'>':
				stream_ << L"&gt;";
				break;
			case L'"':
				stream_ << L"&quot;";
				break;
			case L'\'':
				stream_ << L"&apos;";
				break;
			default:
				stream_ << *iter;
				break;
			}
		}
		buffer_.str(L"");
		state_ = STATE_NONE;
	}

	void write_prolog() {
		stream_ << L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>";
	}

	void start_tag(const std::wstring &tag_name) {
		bool flushed = false;
		switch (state_) {
		case STATE_TAG:
			// Fall through
		case STATE_ATTR:
			end_tag();
			break;
		case STATE_DATA:
			flush_buffer();
			flushed = true;
			break;
		default:
			break;
		}

		tag_stack_.push(tag_name);
		if (!flushed) {
			indent();
		}
		stream_ << L'<' << tag_name;
		state_ = STATE_TAG;
	}

	void end_tag() {
		switch (state_) {
		case STATE_ATTR:
			stream_ << L'"';
			// Fall through
		case STATE_TAG:
			stream_ << L'>';
			state_ = STATE_NONE;
			break;
		default:
			break;
		}
	}

	void close_tag() {
		bool flushed = false;
		switch (state_) {
		case STATE_DATA:
			flush_buffer();
			flushed = true;
			// Fall through
		case STATE_NONE:
			if (!tag_stack_.empty()) {
				if (!flushed) {
					indent();
				}
				stream_ << "</" << tag_stack_.top() << ">";
				tag_stack_.pop();
			}
			break;
		case STATE_ATTR:
			stream_ << L"\"";
			// Fall through
		case STATE_TAG:
			stream_ << L" />";
			state_ = STATE_NONE;
			tag_stack_.pop();
			break;
		default:
			break;
		}
	}

	void start_attr(const std::wstring &attr_name) {
		switch (state_) {
		case STATE_DATA:
			flush_buffer();
			// Fall through
		case STATE_NONE:
			start_tag(L"!--");
			// Fall through
		case STATE_TAG:
			state_ = STATE_ATTR;
			break;
		case STATE_ATTR:
			stream_ << L'"';
			break;
		default:
			break;
		}
		stream_ << L' ' << attr_name << L"=\"";
	}

	std::stack<std::wstring> tag_stack_;
	std::wostringstream buffer_;
	const std::wstring indent_;
	std::wostream &stream_;
	WriterState state_;
};

} // namespace xmls

#endif // __ITSVETKOV__XML_STREAMER_H__
