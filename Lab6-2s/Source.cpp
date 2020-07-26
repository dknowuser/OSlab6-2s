//Марчук Л.Б. 5307
//Приложение-сервер
#include <conio.h>
#include <iostream>
#include <Windows.h>

void main(void)
{
	setlocale(LC_ALL, "Russian");

	char Ch = 0;
	char* SomeName = new char[80];
	char* OutputBuffer = new char[256];
	DWORD NumberOfBytesToWrite = 0;
	DWORD NumberOfBytesWritten = 0;
	HANDLE hConnectPipe = NULL; //Дескриптор именованного канала, с которым обычно работаем
	HANDLE hEvent = NULL;
	DWORD LastError;
	bool Connected = false;

	hEvent = CreateEventA(NULL, TRUE, FALSE, SomeName);

	if (!hEvent)
	{
		std::cout << "Ошибка при создании события " << SomeName << "." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	std::cout << "Было создано событие с дескриптором " << hEvent << "." << std::endl;

	OVERLAPPED overlap;

	while (Ch != '0')
	{
		ZeroMemory(&overlap, sizeof(overlap));
		overlap.Offset = 0;
		overlap.OffsetHigh = 0;
		overlap.hEvent = hEvent;

		std::cout << "Меню приложения-сервера:" << std::endl;
		std::cout << "1 - создание именованного канала;" << std::endl;
		std::cout << "2 - установление соединения;" << std::endl;
		std::cout << "3 - отключение соединения;" << std::endl;
		std::cout << "4 - ввод данных с клавиатуры и их асинхронная запись в именованный канал;" << std::endl;
		std::cout << "0 - выход." << std::endl;
		std::cout << ">> ";
		std::cin >> Ch;

		switch (Ch)
		{
		case '0':
			break;

		case '1':
			std::cout << "Введите название именованного канала (\\\\.\\pipe\\pipename):" << std::endl;
			std::cin >> SomeName;

			hConnectPipe = CreateNamedPipe(SomeName, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | WRITE_DAC, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);

			if (hConnectPipe == INVALID_HANDLE_VALUE)
			{
				std::cout << "Ошибка при создании именованного канала " << SomeName << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "Был создан именованный канал " << SomeName << " с дескриптором " << hConnectPipe << "." << std::endl;
			break;

		case '2':
			std::cout << "Введите дескриптор именованного канала, к которому будет произведено подключение:" << std::endl;
			std::cin >> hConnectPipe;

			overlap.Offset = 0;
			overlap.OffsetHigh = 0;
			overlap.hEvent = hEvent;

			Connected = ConnectNamedPipe(hConnectPipe, &overlap);
			LastError = GetLastError();

			if (!Connected && (LastError != ERROR_IO_PENDING) && (LastError != ERROR_PIPE_CONNECTED))
			{
				std::cout << "Ошибка при подключении к именованному каналу с дескриптором " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "Подключение к именованному каналу с дескриптором " << hConnectPipe << " произведено успешно." << std::endl;
			break;

		case '3':
			std::cout << "Введите дескриптор именованного канала, от которого будет произведено отключение:" << std::endl;
			std::cin >> hConnectPipe;

			if (!DisconnectNamedPipe(hConnectPipe))
			{
				std::cout << "Ошибка при отключении от именованного канала с дескриптором " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "Отключение от именованного канала с дескриптором " << hConnectPipe << " произведено успешно." << std::endl;
			break;

		case '4':
			ZeroMemory(OutputBuffer, 256);

			std::cout << "Введите данные для записи:" << std::endl;
			std::cin >> OutputBuffer;

			//Определяем число байт для записи
			NumberOfBytesToWrite = strlen(OutputBuffer);
			if(!WriteFile(hConnectPipe, OutputBuffer, 256, &NumberOfBytesWritten, &overlap) && (GetLastError() != ERROR_IO_PENDING))
			{
				std::cout << "Ошибка при записи данных в именованный канал с дескриптором " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "Запрос записи " << NumberOfBytesToWrite << " байт данных по каналу с дескриптором " << hConnectPipe << " произведена успешно." << std::endl;

			if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
			{
				std::cout << "Ошибка при ожидании завершения записи данных в именованный канал с дескриптором " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "Запись " << NumberOfBytesToWrite << " байт данных по каналу с дескриптором " << hConnectPipe << " произведена успешно." << std::endl;

			if (!ResetEvent(hEvent))
			{
				std::cout << "Ошибка при сбросе события с дескриптором " << hEvent << " в несигнальное состояние." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			break;
		default:
			std::cout << "Ошибка! Недопустимый параметр меню." << std::endl;
			std::cin.clear();
			std::cin.sync();
			break;
		};

		std::cout << std::endl;
	};

	CloseHandle(hConnectPipe);
	CloseHandle(hEvent);

	getch();
};