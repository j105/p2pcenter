/*
	Copyright (c) 2004-2005 Cory Nelson

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"

using std::string;
using std::string;
using std::runtime_error;
using boost::filesystem::path;
using boost::filesystem::ifstream;
using boost::filesystem::ofstream;
using libtorrent::entry;

path getmodulepath() {
	char folder[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, folder);

	path p(folder);
	p/="K8Torrent";

	if(!boost::filesystem::exists(p))
		boost::filesystem::create_directory(p);

	return p;
}

entry bdecode(const path &file) {
	ifstream fs(file, ifstream::binary);
	if(fs.is_open()) {
		fs.unsetf(ifstream::skipws);
		return libtorrent::bdecode(std::istream_iterator<char>(fs), std::istream_iterator<char>());
	}
	else return entry();
}

bool bencode(const path &file, const entry &e) {
	ofstream fs(file, ofstream::binary);
	if(!fs.is_open()) return false;

	libtorrent::bencode(std::ostream_iterator<char>(fs), e);
	return true;
}

string loadstring(UINT id) {
	char buf[1024];
	int len=LoadString(GetModuleHandle(NULL), id, buf, 1024);

	return string(buf, len);
}

string loadstringa(UINT id) {
	char buf[1024];
	int len=LoadStringA(GetModuleHandle(NULL), id, buf, 1024);

	return string(buf, len);
}

static void RegWrite(LPCTSTR key, LPCTSTR value) {
	HKEY reg;

	RegCreateKeyEx(HKEY_CLASSES_ROOT, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &reg, NULL);
	RegSetValueEx(reg, NULL, 0, REG_SZ, (LPBYTE)value, (DWORD)((strlen(value)+1)*sizeof(char)));
	RegCloseKey(reg);
}

void Associate() {
	char path[MAX_PATH], path_tmp[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);

	RegWrite("K8Torrent.torrent", "BitTorrent Metadata");

	StringCchPrintf(path_tmp, MAX_PATH, "%s,1", path);
	RegWrite("K8Torrent.torrent\\DefaultIcon", path_tmp);

	RegWrite("K8Torrent.torrent\\shell", "Open");

	RegWrite("K8Torrent.torrent\\shell\\Open", "&Open");

	StringCchPrintf(path_tmp, MAX_PATH, "\"%s\" /dde", path);
	RegWrite("K8Torrent.torrent\\shell\\Open\\command", path_tmp);
	RegWrite("K8Torrent.torrent\\shell\\Open\\ddeexec", "open(\"%1\")");
	RegWrite("K8Torrent.torrent\\shell\\Open\\ddeexec\\application", "K8TorrentTorrent");
	RegWrite("K8Torrent.torrent\\shell\\Open\\ddeexec\\topic", "system");

	RegWrite(".torrent", "K8Torrent.torrent");

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}
