#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

const char* originFile = "origin.bmp";
const char* stegoFile = "stego.bmp";

int main(int argc, char* argv[]) {
	FILE* origin;		// 오리지널 파일 포인터
	FILE* stego;		// 스테고 파일 포인터
	int count = 0;		// 파일 복사에 사용할 변수
	int interval = 0;	// 정보를 저장할 픽셀 간격
	int offset = 0;		// 첫 정보 시작 주소를 저장하는 변수
	char buf16[16];	// 버퍼
	unsigned char buffer[1] = "";	// 데이터를 읽어올 때 사용할 버퍼

	char data[10000] = "";	// 입력받는 데이터

	int fileSize = 0;		// 파일 크기
	short dataSize = 0;		// 입력하는 문자의 개수
	
	char control;			// 기능

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

	if (control == 'e') {	// e 옵션
		cout << "Enter the words you want to hide in the file:" << endl;
		cin.getline(data, 10000);			// 이미지에 저장할 문자를 받아옵니다
		dataSize = (short)strlen(data);		// 문자의 개수를 저장합니다

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
		}	// 파일 열기 & 생성 & 복사

		fseek(stego, 0x02, SEEK_SET);
		fread(buf16, sizeof(char), sizeof(buf16), stego);	// 이미지의 크기를 저장하는 헤더를 읽습니다

		fileSize = (unsigned char)buf16[0] +
			(unsigned char)buf16[1] * 16 * 16 +
			(unsigned char)buf16[2] * 16 * 16 * 16 * 16 +
			(unsigned char)buf16[3] * 16 * 16 * 16 * 16 * 16 * 16;	// 읽어온 파일 크기를 한 변수에 저장합니다

		fseek(stego, 0x36, SEEK_SET);
		buffer[0] = (char)(dataSize % 16);
		fwrite(buffer, 1, 1, stego);

		fseek(stego, 0x39, SEEK_SET);
		buffer[0] = (char)(dataSize / 16);
		fwrite(buffer, 1, 1, stego);			// 0x36(하위 비트 저장)과 0x39(상위 비트 저장)에 문자열의 길이를 나눠 저장합니다

		offset = fileSize / dataSize / 2;		// 픽셀 간격을 파일 크기와 데이터 크기를 이용해 적당하게 설정합니다.
		interval = offset;

		for (int i = 0; i < dataSize; i++) {
			fseek(stego, offset, SEEK_SET);		// 오프셋으로 커서를 이동합니다
			fread(buffer, 1, 1, stego);			// 해당 오프셋의 정보를 읽어옵니다
			buffer[0] = (char)((int)buffer[0] / 16) * 16 + ((int)data[i] / 16);		// 해당 오프셋의 정보에서 하위 비트만 숨길 데이터(상위 비트)로 바꿔줍니다
			fseek(stego, offset, SEEK_SET);		// 다시 해당 오프셋으로 돌아옵니다
			fwrite(buffer, 1, 1, stego);		// 정보가 숨긴 데이터를 써 줍니다

			offset += interval;					// 오프셋을 간격만큼 벌려줍니다

			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			buffer[0] = (char)((int)buffer[0] / 16) * 16 + ((int)data[i] % 16);		// 해당 오프셋의 정보에서 하위 비트만 숨길 데이터(하위 비트)로 바꿔줍니다
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
		}	// 파일 열기

		fseek(stego, 0x02, SEEK_SET);
		fread(buf16, sizeof(char), sizeof(buf16), stego);

		fileSize = (unsigned char)buf16[0] +
			(unsigned char)buf16[1] * 16 * 16 +
			(unsigned char)buf16[2] * 16 * 16 * 16 * 16 +
			(unsigned char)buf16[3] * 16 * 16 * 16 * 16 * 16 * 16;	// 헤더에서 파일 크기를 읽어옵니다

		fseek(stego, 0x36, SEEK_SET);
		fread(buffer, 1, 1, stego);
		dataSize = (int)buffer[0];

		fseek(stego, 0x39, SEEK_SET);
		fread(buffer, 1, 1, stego);
		dataSize += (int)buffer[0] * 16;		// 0x36과 0x39에서 숨긴 데이터의 문자 개수를 꺼내옵니다

		offset = fileSize / dataSize / 2;
		interval = offset;

		char readChar;		// 읽어온 문자를 저장할 변수

		cout << "The hidden words from the Stego.bmp file are:" << endl;

		for (int i = 0; i < dataSize; i++) {
			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			readChar = ((int)buffer[0] % 16) * 16;		// 숨긴 데이터의 상위 비트를 가져옵니다

			offset += interval;

			fseek(stego, offset, SEEK_SET);
			fread(buffer, 1, 1, stego);
			readChar = readChar + buffer[0] % 16;		// 숨긴 데이터의 하위 비트와 합쳐줍니다

			offset += interval;

			cout << readChar;		// 데이터를 출력합니다
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