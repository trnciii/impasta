#include <windows.h>

#include <fstream>
#include <vector>
#include <algorithm>
#include <execution>

#include <ctime>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


std::string now(){
	auto t = std::time(nullptr);
	char s[15] {};
	std::strftime(s, sizeof(s), "%Y%m%d%H%M%S", std::localtime(&t));
	return std::string(s);
}

int main(){
	if (!OpenClipboard(NULL)) return 0;

	if(HGLOBAL hg = GetClipboardData(CF_TEXT)){
		PTSTR strClip = (PTSTR)GlobalLock(hg);

		std::ofstream file;
		file.open(now());
		file <<strClip;
		file.close();

		GlobalUnlock(hg);
	}

	else if(HBITMAP hbmp = (HBITMAP)GetClipboardData(CF_BITMAP)){
		BITMAP bmp{};
		GetObject(hbmp, sizeof(bmp), &bmp);

		std::vector<uint32_t> buffer(bmp.bmHeight * bmp.bmWidth);
		GetBitmapBits(hbmp, buffer.size()*sizeof(uint32_t), buffer.data());

		std::for_each(std::execution::par_unseq, buffer.begin(), buffer.end(), [](auto& _c){
			char* c = reinterpret_cast<char*>(&_c);
			std::swap(c[0], c[2]);
		});

		stbi_write_png((now() + ".png").c_str(), bmp.bmWidth, bmp.bmHeight, 4, buffer.data(), bmp.bmWidthBytes);
	}

	CloseClipboard();
}
