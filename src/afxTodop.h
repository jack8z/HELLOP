#pragma once

#define WIN32_LEAN_AND_MEAN

#include <UIlib.h>
#include <Windows.h>

#include <functional>
#include <string>
#include <codecvt>

#include <stdio.h>
#include <process.h>

#include <sstream>
#include <list>
#include <fstream>

#define LITEHTML_UTF8

typedef std::wstring TString;

// 将std:wstring转换为std::string
inline std::string todop_to_string(std::wstring inStr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(inStr);
}

// 将std:string转换为std::wstring
inline std::wstring todop_to_wstring(std::string inStr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(inStr);
}

inline wchar_t* utf8_to_wchar( const char* src ) {
	if(!src) return NULL;

	int len = (int) strlen(src);
	wchar_t* ret = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, src, -1, ret, len + 1);
	return ret;
}

inline char* wchar_to_utf8( const wchar_t* src ) {
	if(!src) return NULL;

	int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
	char* ret = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, src, -1, ret, len, NULL, NULL);
	return ret;
}

int read_file(std::wstring filePath, std::vector<std::wstring> &data) {
	data.clear();

	std::wifstream fs(filePath.c_str(), std::ios::in);

	const int bufsize = 1024;
	wchar_t strbuff[bufsize];

	while(!fs.eof()) {
		fs.getline(strbuff, bufsize);
		std::wstring tmp = strbuff;
		if(tmp.empty()) continue;
		data.push_back(tmp);
	}
	fs.close();

	if(data.size()<1){
		return -1;
	}

	return 0;
}

int read_file(std::wstring filePath, std::wstring &data) {
	std::wstringstream ss;
	std::vector<std::wstring> v;
	int ret = read_file(filePath, v);
	if(ret == 0){
		for(size_t i=0; i<v.size(); i++){
			if(i>0) ss << "\n";
			ss << v[i];
		}
	}
	data = ss.str();
	return ret;
}
