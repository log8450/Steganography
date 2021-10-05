#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

const char* originFile = "origin.bmp";
const char* stegoFile = "stego.bmp";

int main(int argc, char* argv[]) {
	FILE* origin;		// �������� ���� ������
	FILE* stego;		// ���װ� ���� ������
	int count = 0;		// ���� ���翡 ����� ����
	int interval = 0;	// ������ ������ �ȼ� ����
	int offset = 0;		// ù ���� ���� �ּҸ� �����ϴ� ����
	char buf16[16];	// ����
	unsigned char buffer[1] = "";	// �����͸� �о�� �� ����� ����

	char data[10000] = "";	// �Է¹޴� ������

	int fileSize = 0;		// ���� ũ��
	short dataSize = 0;		// �Է��ϴ� ������ ����
	
	char control;			// ���

	cout << "Welcome to Steganography Program!" << endl << endl;

	cout << "Please enter the function you want to use" << endl
		<< "+ Encode words : e" << endl
		<< "| Decode words : d" << endl
		<< "| Help : h " << endl
		<< "+ Exit : any key" << endl << endl
		<< "function : ";

	cin >> control;
	cin.ignore();
	cout << endl;

	if (control == 'e') {	// e �ɼ�
		cout << "Enter the words you want to hide in the file:" << endl;
		cin.getline(data, 10000);			// �̹����� ������ ���ڸ� �޾ƿɴϴ�
		dataSize = (short)strlen(data);		// ������ ������ �����մϴ�

		if (fopen_s(&origin, originFile, "rb") != 0) {
			cout << "File Open Error" << endl;
			return 1;
		}

		if (fopen_s(&stego, stegoFile, "w+b") != 0) {
			cout << "File Open Error" << endl;
			return 1;
		}
		while ((count = fread(buf16, sizeof(char), 16, origin)) != 0) {
			fwrite(buf16, sizeof(char), count, stego);
		}	// ���� ���� & ���� & ����

		fseek(stego, 0x02, SEEK_SET);
		fread(buf16, sizeof(char), sizeof(buf16), stego);	// �̹����� ũ�⸦ �����ϴ� ����� �н��ϴ�

		fileSize = (unsigned char)buf16[0] +
			(unsigned char)buf16[1] * 16 * 16 +
			(unsigned char)buf16[2] * 16 * 16 * 16 * 16 +
			(unsigned char)buf16[3] * 16 * 16 * 16 * 16 * 16 * 16;	// �о�� ���� ũ�⸦ �� ������ �����մϴ�

		fseek(stego, 0x36, SEEK_SET);
		buffer[0] = (char)(dataSize % 16);
		fwrite(buffer, 1, 1, stego);

		fseek(stego, 0x39, SEEK_SET);
		buffer[0] = (char)(dataSize / 16);
		fwrite(buffer, 1, 1, stego);			// 0x36(���� ��Ʈ ����)�� 0x39(���� ��Ʈ ����)�� ���ڿ��� ���̸� ���� �����մϴ�

		offset = fileSize / dataSize / 2;		// �ȼ� ������ ���� ũ��� ������ ũ�⸦ �̿��� �����ϰ� �����մϴ�.
		interval = offset;

		for (int i = 0; i < dataSize; i++) {
			fseek(stego, offset, SEEK_SET);		// ���������� Ŀ���� �̵��մϴ�
			fread(buffer, 1, 1, stego);			// �ش� �������� ������ �о�ɴϴ�
			buffer[0] = (char)((int)buffer[0] / 16) * 16 + ((int)data[i] / 16);		// �ش� �������� �������� ���� ��Ʈ�� ���� ������(���� ��Ʈ)�� �ٲ��ݴϴ�
			fseek(stego, offset, SEEK_SET);		// �ٽ� �ش� ���������� ���ƿɴϴ�
			fwrite(buffer, 1, 1, stego);		// ������ ���� �����͸� �� �ݴϴ�

			offset += interval;					// �������� ���ݸ�ŭ �����ݴϴ�

			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			buffer[0] = (char)((int)buffer[0] / 16) * 16 + ((int)data[i] % 16);		// �ش� �������� �������� ���� ��Ʈ�� ���� ������(���� ��Ʈ)�� �ٲ��ݴϴ�
			fseek(stego, offset, SEEK_SET);
			fwrite(buffer, 1, 1, stego);

			offset += interval;
		}

		cout << "The output to the \"stego.bmp\" file with the words you entered is complete." << endl;

		return 0;
	}

	else if (control == 'd') {	// option d
		if (fopen_s(&stego, stegoFile, "rb") != 0) {
			cout << "File Open Error" << endl;
			return 1;
		}	// ���� ����

		fseek(stego, 0x02, SEEK_SET);
		fread(buf16, sizeof(char), sizeof(buf16), stego);

		fileSize = (unsigned char)buf16[0] +
			(unsigned char)buf16[1] * 16 * 16 +
			(unsigned char)buf16[2] * 16 * 16 * 16 * 16 +
			(unsigned char)buf16[3] * 16 * 16 * 16 * 16 * 16 * 16;	// ������� ���� ũ�⸦ �о�ɴϴ�

		fseek(stego, 0x36, SEEK_SET);
		fread(buffer, 1, 1, stego);
		dataSize = (int)buffer[0];

		fseek(stego, 0x39, SEEK_SET);
		fread(buffer, 1, 1, stego);
		dataSize += (int)buffer[0] * 16;		// 0x36�� 0x39���� ���� �������� ���� ������ �����ɴϴ�

		offset = fileSize / dataSize / 2;
		interval = offset;

		char readChar;		// �о�� ���ڸ� ������ ����

		cout << "The hidden words from the Stego.bmp file are:" << endl;

		for (int i = 0; i < dataSize; i++) {
			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			readChar = ((int)buffer[0] % 16) * 16;		// ���� �������� ���� ��Ʈ�� �����ɴϴ�

			offset += interval;

			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			readChar = readChar + buffer[0] % 16;		// ���� �������� ���� ��Ʈ�� �����ݴϴ�

			offset += interval;

			cout << readChar;		// �����͸� ����մϴ�
		}

		cout << endl;

		return 0;
	}

	else if (control == 'h') {
		cout << "Steganography is \"techniques to hide data inside common media files\"" << endl
			<< "You can hide text on your picture (origin.bmp)" << endl
			<< "And you can import text from encoded pictures (stego.bmp)" << endl;
	}
}