#include <algorithm>
#include <iostream>
#include <string>
#include <windows.h>

#define DEBUG 1 //используется для включения или выключения режима отладки

#define HKCR    HKEY_CLASSES_ROOT //создает удобное сокращение для корневого ключа
#define HKCU    HKEY_CURRENT_USER //содержит настройки текущего пользователя, вошедшего в систему
#define HKLM    HKEY_LOCAL_MACHINE //хранит глобальные системные настройки, применимые ко всем пользователям компьютера
#define HKU     HKEY_USERS //содержит настройки всех пользователей, зарегистрированных в системе
#define HKCC    HKEY_CURRENT_CONFIG //содержит аппаратные настройки, актуальные для текущей сессии работы Windows

HKEY CreateNewKey(HKEY root, const std::string& subDirectory); //создание нового ключа в реестре
void SetKeyValue(HKEY hRegistryKey, const std::string& valueName, const std::string& data); //установка значения в реестре

int main(int argc, char* argv[])
{
	std::string fileDestination	= argv[0];
	std::string rootDirectory	= fileDestination.substr(0, fileDestination.find_last_of("\\"));
	const std::string GUID		= "{01b25495-d2f0-4568-a708-911d380db1be}";

	//CreateNewKey создает или открывает ключ в реестре HKEY_CLASSES_ROOT
	SetKeyValue(CreateNewKey(HKCR, ".abc"), "", "abcfile");
	SetKeyValue(CreateNewKey(HKCR, ".abc\\ShellNew"), "FileName",  //CreateNewKey(HKCR, ".abc\\ShellNew") - cоздает новый ключ ShellNew внутри .abс
		rootDirectory + "\\..\\resources\\BadCloud.png");

	//создает новый ключ в реестре Windows в разделе HKEY_CLASSES_ROOT, связанный с уникальным идентификатором (GUID)
	CreateNewKey(HKCR, "CLSID\\{01b25495-d2f0-4568-a708-911d380db1be}");

	SetKeyValue(CreateNewKey(HKCR, "abcfile"), "", "ABC File Type");//CreateNewKey - cоздает новый ключ abcfile в разделе HKEY_CLASSES_ROOT, этот ключ описывает свойства файлов с этим типом
	SetKeyValue(CreateNewKey(HKCR, "abcfile\\DefaultIcon"), "",//cоздает ключ DefaultIcon внутри abcfile
		rootDirectory + "\\..\\resources\\cloud.ico");//путь к исполняемому файлу программы, а "\\..\\resources\\cloud.ico" указывает на местоположение иконки
	SetKeyValue(CreateNewKey(HKCR, "abcfile\\shell\\open\\command"), "",//cоздает ключ command внутри abcfile\shell\open , этот ключ указывает, какая программа будет открывать файлы этого типа
		"\"C:\\Windows\\system32\\mspaint.exe\" \"%1\"");
	SetKeyValue(CreateNewKey(HKCR, "abcfile\\shellex\\ContextMenuHandlers"), "", //Создает ключ shellex\\ContextMenuHandlers внутри abcfile, этот раздел используется для расширений контекстного меню
		"{01b25495-d2f0-4568-a708-911d380db1be}");

	///добавить контекстное меню
	SetKeyValue(CreateNewKey(HKCR, "*\\shellex\\ContextMenuHandlers\\abc"), "", GUID);

	std::cin.get();
	return 0;
}

//возырат дискриптора ключа реестра для дальнейших операций
HKEY CreateNewKey(HKEY root, const std::string& subDirectory) {
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
		std::cout << "[внимание] ключ \"" << subDirectory << "\" успешно создан." << std::endl;
	else
		std::cout << "[инфо] ключ \"" << subDirectory << "\" успешно открыт." << std::endl;
#endif

	return typeLocation;
}

//HKEY hRegistryKey — дескриптор ключа реестра, в который записываем данные
void SetKeyValue(HKEY hRegistryKey, const std::string& valueName , const std::string& data) {
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
		std::cout << "[ура, победа] значение: \"" << valueName << "" << data << "\" было успешно установлено." << std::endl;
	else
		std::cout << "[Error] ошибка при установке значения." << std::endl;
#endif
}