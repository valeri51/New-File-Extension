#define  _WIN32_WINNT  0x0600 //определяет минимальную версию windows api

#include <algorithm> //библиотека алгоритмов
#include <iostream>
#include <regex>
#include <string>
#include <windows.h>

#define DEBUG 1

#define HKCR    HKEY_CLASSES_ROOT
#define HKCU    HKEY_CURRENT_USER
#define HKLM    HKEY_LOCAL_MACHINE
#define HKU     HKEY_USERS
#define HKCC    HKEY_CURRENT_CONFIG

HKEY CreateNewKey(HKEY root, const std::string& subDirectory);
void SetKeyValue(HKEY hRegistryKey, const std::string& valueName, const std::string& data);
void DeleteKey(HKEY root, const std::string& subDirectory); // удаление конкретного ключа
void DeleteTree(HKEY root, const std::string& subDirectory); // рекурсивное удаление ключа и всех его подключец

int main(int argc, char* argv[])
{
	DeleteKey(HKCR, "*\\shellex\\ContextMenuHandlers\\abc"); //удаление контекстного меню
    DeleteKey(HKCR, "CLSID\\{01b25495-d2f0-4568-a708-911d380db1be}"); //удаление CLSID
    DeleteTree(HKCR, "abcfile"); // удаление пользовательского расширения файла и связанного с ним расширения
    DeleteTree(HKCR, ".abc");

	std::cin.get();
	return 0;
}

HKEY CreateNewKey(HKEY root, const std::string& subDirectory)
{
	HKEY typeLocation = {};
	DWORD status = {};

	RegCreateKeyExA(
		root,
		const_cast<char *>(subDirectory.c_str()),
		NULL,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS | KEY_WOW64_64KEY,
		NULL,
		&typeLocation,
		&status
	);

#if DEBUG == 1
	std::cout << std::endl;
	if (status == REG_CREATED_NEW_KEY)
		std::cout << "[внимание] местоположение \"" << subDirectory << "\" успешно создано." << std::endl;
	else
		std::cout << "[инфа] местоположение \"" << subDirectory << "\" успешно открыто." << std::endl;
#endif

	return typeLocation;
}

void SetKeyValue(HKEY hRegistryKey, const std::string& valueName , const std::string& data) //установка значения в ключ реестра
{
	auto status = RegSetValueExA(
		hRegistryKey,
		const_cast<char *>(valueName.c_str()),
		NULL,
		REG_SZ,
		(LPBYTE)const_cast<char *>(data.c_str()),
		data.size() + 1
	);

#if DEBUG == 1
	if (status == ERROR_SUCCESS)
		std::cout << "[ура, победа] значение \"" << valueName << ": " << data << "\" было успешно утсановлено." << std::endl;
	else
		std::cout << "[Error] ошибка при установке значения." << std::endl;
#endif
}

void DeleteKey(HKEY root, const std::string& subDirectory) //удаление конкретного ключа без подключей
{
	auto status = RegDeleteKeyExA( //функция WinAPI, удаляет только ключ, если в нём нет вложенных подключей. Если они есть — вернёт ошибку
		root,
		const_cast<char *>(subDirectory.c_str()),
		KEY_WOW64_64KEY,
		NULL
	);

#if DEBUG == 1
	if (status == ERROR_SUCCESS)
		std::cout << "[ура, победа] ключ \"" << subDirectory << "\" был успешно удалён." << std::endl;
	else
		std::cout << "[Error] ошибка при удалении ключа." << std::endl;
#endif
}

void DeleteTree(HKEY root, const std::string& subDirectory) //рекуривное удаление ключа и всех его подузлов
{
	auto status = RegDeleteTreeA( //функция, удаляет указанный ключ вместе со всем его содержимым (вложенные папки, значения и тд)
		root,
		const_cast<char *>(subDirectory.c_str())
	);

#if DEBUG == 1
	if (status == ERROR_SUCCESS)
		std::cout << "[ура, победа] ключ \"" << subDirectory << "\" был успешно удалён." << std::endl;
	else
		std::cout << "[Error] ошибка при удалении ключа." << std::endl;
#endif
}