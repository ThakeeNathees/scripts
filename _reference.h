//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#ifndef REFERENCE_H
#define REFERENCE_H

#include "varhcore.h"

namespace varh {

class Ref
{
private:
	Ptr<Object> _data;

public:
	Ref(){}

	template <typename T>
	Ref(const Ptr<T>& p_ptr) { _data = p_ptr; }


	bool is_null() const { return _data == nullptr; }
	Ptr<Object> ptr() const { return _data; }
	// const Object* raw_ptr() const { return &(*_data); }

	String to_string() const;
	operator String() const;
	Ref& operator=(const Ptr<Object>& p_obj) { _data = p_obj; return *this; }

#define REF_CMP_OP(m_op) \
	bool operator m_op (const Ref& p_other) const { return *_data m_op *p_other._data; }
	REF_CMP_OP(==);
	REF_CMP_OP(!=);
	REF_CMP_OP(<=);
	REF_CMP_OP(>=);
	REF_CMP_OP(<);
	REF_CMP_OP(>);

};

}

#endif // REFERENCE_H