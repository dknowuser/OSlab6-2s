//������ �.�. 5307
//����������-������
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
	HANDLE hConnectPipe = NULL; //���������� ������������ ������, � ������� ������ ��������
	HANDLE hEvent = NULL;
	DWORD LastError;
	bool Connected = false;

	hEvent = CreateEventA(NULL, TRUE, FALSE, SomeName);

	if (!hEvent)
	{
		std::cout << "������ ��� �������� ������� " << SomeName << "." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	std::cout << "���� ������� ������� � ������������ " << hEvent << "." << std::endl;

	OVERLAPPED overlap;

	while (Ch != '0')
	{
		ZeroMemory(&overlap, sizeof(overlap));
		overlap.Offset = 0;
		overlap.OffsetHigh = 0;
		overlap.hEvent = hEvent;

		std::cout << "���� ����������-�������:" << std::endl;
		std::cout << "1 - �������� ������������ ������;" << std::endl;
		std::cout << "2 - ������������ ����������;" << std::endl;
		std::cout << "3 - ���������� ����������;" << std::endl;
		std::cout << "4 - ���� ������ � ���������� � �� ����������� ������ � ����������� �����;" << std::endl;
		std::cout << "0 - �����." << std::endl;
		std::cout << ">> ";
		std::cin >> Ch;

		switch (Ch)
		{
		case '0':
			break;

		case '1':
			std::cout << "������� �������� ������������ ������ (\\\\.\\pipe\\pipename):" << std::endl;
			std::cin >> SomeName;

			hConnectPipe = CreateNamedPipe(SomeName, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | WRITE_DAC, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);

			if (hConnectPipe == INVALID_HANDLE_VALUE)
			{
				std::cout << "������ ��� �������� ������������ ������ " << SomeName << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "��� ������ ����������� ����� " << SomeName << " � ������������ " << hConnectPipe << "." << std::endl;
			break;

		case '2':
			std::cout << "������� ���������� ������������ ������, � �������� ����� ����������� �����������:" << std::endl;
			std::cin >> hConnectPipe;

			overlap.Offset = 0;
			overlap.OffsetHigh = 0;
			overlap.hEvent = hEvent;

			Connected = ConnectNamedPipe(hConnectPipe, &overlap);
			LastError = GetLastError();

			if (!Connected && (LastError != ERROR_IO_PENDING) && (LastError != ERROR_PIPE_CONNECTED))
			{
				std::cout << "������ ��� ����������� � ������������ ������ � ������������ " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "����������� � ������������ ������ � ������������ " << hConnectPipe << " ����������� �������." << std::endl;
			break;

		case '3':
			std::cout << "������� ���������� ������������ ������, �� �������� ����� ����������� ����������:" << std::endl;
			std::cin >> hConnectPipe;

			if (!DisconnectNamedPipe(hConnectPipe))
			{
				std::cout << "������ ��� ���������� �� ������������ ������ � ������������ " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "���������� �� ������������ ������ � ������������ " << hConnectPipe << " ����������� �������." << std::endl;
			break;

		case '4':
			ZeroMemory(OutputBuffer, 256);

			std::cout << "������� ������ ��� ������:" << std::endl;
			std::cin >> OutputBuffer;

			//���������� ����� ���� ��� ������
			NumberOfBytesToWrite = strlen(OutputBuffer);
			if(!WriteFile(hConnectPipe, OutputBuffer, 256, &NumberOfBytesWritten, &overlap) && (GetLastError() != ERROR_IO_PENDING))
			{
				std::cout << "������ ��� ������ ������ � ����������� ����� � ������������ " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "������ ������ " << NumberOfBytesToWrite << " ���� ������ �� ������ � ������������ " << hConnectPipe << " ����������� �������." << std::endl;

			if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
			{
				std::cout << "������ ��� �������� ���������� ������ ������ � ����������� ����� � ������������ " << hConnectPipe << "." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			std::cout << "������ " << NumberOfBytesToWrite << " ���� ������ �� ������ � ������������ " << hConnectPipe << " ����������� �������." << std::endl;

			if (!ResetEvent(hEvent))
			{
				std::cout << "������ ��� ������ ������� � ������������ " << hEvent << " � ������������ ���������." << std::endl;
				std::cin.clear();
				std::cin.sync();
				break;
			};

			break;
		default:
			std::cout << "������! ������������ �������� ����." << std::endl;
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