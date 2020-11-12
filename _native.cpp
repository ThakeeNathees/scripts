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

#include "_var.h"

namespace varh {

NativeClasses* NativeClasses::_singleton = nullptr;

NativeClasses* NativeClasses::singleton() {
	if (_singleton == nullptr) _singleton = new NativeClasses();
	return _singleton;
}

void NativeClasses::_set_singleton(NativeClasses* p_native_classes) {
	if (_singleton == nullptr) { // share singleton across dynamic libraries.
		_singleton = p_native_classes;
	}
}

void NativeClasses::cleanup() {
	delete _singleton;
}

void NativeClasses::bind_data(ptr<BindData> p_bind_data) {
	String class_name = p_bind_data->get_class_name();
	String data_name = p_bind_data->get_name();

	ClassEntries& entries = classes[class_name.hash()];

	if (entries.bind_data[data_name.hash()] != nullptr) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, 
			String::format("entry \"%s\" already exists on class \"%s\".", p_bind_data->get_name(), p_bind_data->get_class_name())
		);
	}
	// check initializer.
	if (class_name == data_name) {
		if (p_bind_data->get_type() != BindData::STATIC_FUNC)
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" can't be the same as type name.", data_name.c_str()));
		const MethodInfo* mi = ptrcast<StaticFuncBind>(p_bind_data)->get_method_info().get();
		if (mi->get_return_type().type != var::_NULL) THROW_VARERROR(VarError::TYPE_ERROR, "constructor initializer must not return anything.");
		if (mi->get_arg_count() < 1 || mi->get_arg_types()[0].type != var::OBJECT) THROW_VARERROR(VarError::TYPE_ERROR, "constructor initializer must take the instance as the first argument.");
		entries.__initializer = ptrcast<StaticFuncBind>(p_bind_data).get();		
	}
	entries.bind_data[data_name.hash()] = p_bind_data;
}

ptr<BindData> NativeClasses::get_bind_data(const String& cls, const String& attrib) {
	if (!is_class_registered(cls))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" not registered on NativeClasses entries.", cls.c_str()));
	return classes[cls.hash()].bind_data[attrib.hash()];
}

ptr<BindData> NativeClasses::find_bind_data(const String& cls, const String& attrib) {
	String class_name = cls;
	while (class_name.size() != 0) {
		ptr<BindData> bind_data = NativeClasses::get_bind_data(class_name, attrib);
		if (bind_data != nullptr) return bind_data;
		class_name = NativeClasses::get_inheritance(class_name);
	}
	return nullptr;
}

const MemberInfo* NativeClasses::get_member_info(const String& p_class_name, const String& attrib) {
	return get_bind_data(p_class_name, attrib)->get_member_info();
}

void NativeClasses::set_inheritance(const String& p_class_name, const String& p_parent_class_name) {
	if (is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" already exists on NativeClasses entries.", p_class_name.c_str()));

	classes[p_class_name.hash()].class_name = p_class_name;
	classes[p_class_name.hash()].parent_class_name = p_parent_class_name;
}

void NativeClasses::set_constructor(const String& p_class_name, __constructor_f p__constructor) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" not registered on NativeClasses entries.", p_class_name.c_str()));
	classes[p_class_name.hash()].__constructor = p__constructor;
}

String NativeClasses::get_inheritance(const String& p_class_name) {
	if (classes[p_class_name.hash()].class_name.size() == 0)
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].parent_class_name;
}

bool NativeClasses::is_class_registered(const String& p_class_name) {
	return classes[p_class_name.hash()].class_name.size() != 0;
}

ptr<Object> NativeClasses::_new(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].__constructor();
}

const StaticFuncBind* NativeClasses::get_initializer(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].__initializer;
}

ptr<Object> NativeClasses::construct(const String& p_class_name, stdvec<var*>& p_args) {
	var instance = _new(p_class_name);
	p_args.insert(p_args.begin(), &instance);
	const StaticFuncBind* initializer = get_initializer(p_class_name);
	if (initializer) initializer->call(p_args);
	return instance;
}

const stdvec<const BindData*> NativeClasses::get_bind_data_list(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	stdvec<const BindData*> ret;
	stdhashtable<size_t, ptr<BindData>>& bind_data_list = classes[p_class_name.hash()].bind_data;
	stdhashtable<size_t, ptr<BindData>>::iterator it = bind_data_list.begin();
	while (it != bind_data_list.end()) {
		const BindData* bd = (*it).second.get();
		ret.push_back(bd);
		it++;
	}
	return ret;
}

const stdvec<const MemberInfo*> NativeClasses::get_member_info_list(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	stdvec<const MemberInfo*> ret;
	stdhashtable<size_t, ptr<BindData>>& bind_data_list = classes[p_class_name.hash()].bind_data;
	stdhashtable<size_t, ptr<BindData>>::iterator it = bind_data_list.begin();
	while (it != bind_data_list.end()) {
		const BindData* bd = (*it).second.get();
		ret.push_back(bd->get_member_info());
		it++;
	}
	return ret;
}

}
