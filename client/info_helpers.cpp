#include					<Windows.h>
#include					<wbemidl.h>
#include					<iostream>
#include					<conio.h>
#include					<comdef.h>
#include					<wininet.h>
#include					<TlHelp32.h>
#include					<string>
#include                    <Wininet.h>
#include					"info_helpers.h"

#pragma comment				(lib, "wbemuuid.lib")

using namespace				std;

std::string real_ip() {

	HINTERNET net = InternetOpen("--",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);

	HINTERNET conn = InternetOpenUrl(net,
		"https://api.ipify.org/",
		NULL,
		0,
		INTERNET_FLAG_RELOAD,
		0);

	char buffer[4096];
	DWORD read;

	InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
	InternetCloseHandle(net);

	return std::string(buffer, read);
}

DWORD find_process_id(const std::wstring& processName) {
	PROCESSENTRY32W processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32FirstW(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile)) {
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32NextW(processesSnapshot, &processInfo)) {
		if (!processName.compare(processInfo.szExeFile)) {
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}