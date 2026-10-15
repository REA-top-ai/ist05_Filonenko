#include <iostream>
#include <fstream>

int bytes_to_int32(unsigned char* bytes) {
    return bytes[0] + bytes[1] * 256 + bytes[2] * 256 * 256 + bytes[3] * 256 * 256 * 256;
}

int bytes_to_int2(unsigned char* bytes) {
    return bytes[0] + bytes[1] * 256;
}

void transform_to_RGB_cout(unsigned char* bytes) {
    unsigned short pixel = *reinterpret_cast<unsigned short*>(bytes);


    auto R = 0B1111100000000000;
    auto G = 0B0000011111100000;
    auto B = 0B0000000000011111;

    int R_out = ((pixel & R) >> 11);
    int G_out = ((pixel & G) >> 5);
    int B_out = (pixel & B);

    std::cout << "B: " << B_out * (255/31) <<std::endl;
    std::cout << "G " << G_out * (255/63) <<std::endl;
    std::cout << "R: " << R_out * (255/31) <<std::endl;
}

static bool load_file_bytes(const char* path, unsigned char* headers, const int size) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { return false; }
    file.read(reinterpret_cast<char*>(headers), size);
    return true;
}

int main(int argc, char** argv) {

    const char* path = argc > 1 ? argv[1] : "../../bitmap/images/Rufous-Hummingbird_16.bmp";

    unsigned char headers[54];
    
    load_file_bytes(path, headers, 54);

    if (headers[0] == 'B' && headers[1] == 'M') {
        std::cout << "Это BMP файл." <<std::endl;

        int file_size = bytes_to_int32(headers + 2);
        int data_offset = bytes_to_int32(headers + 10);
        int header_size = bytes_to_int32(headers + 14);
        int width = bytes_to_int32(headers + 18);
        int height = bytes_to_int32(headers + 22);
        int planes = bytes_to_int2(headers + 26);
        int bits_per_pixel = bytes_to_int2(headers + 28);
        int compression = bytes_to_int32(headers + 30);
        int image_size = bytes_to_int32(headers + 34);
        int XpixelsM = bytes_to_int32(headers + 38);
        int YpixelsM = bytes_to_int32(headers + 42);
        int colors_used = bytes_to_int32(headers + 46);
        int colors_important = bytes_to_int32(headers + 50);

        std::cout << "Размер файла " << file_size <<std::endl;
        std::cout << "Сколько байт до первого пикселя картинки " << data_offset <<std::endl;
        std::cout << "Ширина Файла " << width <<std::endl;
        std::cout << "Плоскости " << planes <<std::endl;
        std::cout << "Высота файла " << height <<std::endl;
        std::cout << "Биты на пиксель " << bits_per_pixel <<std::endl;
        std::cout << "Сжат ли файл " << compression <<std::endl;
        std::cout << "Размер картинки без заголовка " << image_size <<std::endl;
        std::cout << "Разрешение по X " << XpixelsM <<std::endl;
        std::cout << "Разрешение по Y " << YpixelsM <<std::endl;
        std::cout << "Количество цветов в палитре " << colors_used <<std::endl;
        std::cout << "Сколько цветов критически необходимы " << colors_important <<std::endl;

        unsigned char pixel[2];

        std::ifstream file(path, std::ios::binary);
        file.seekg(data_offset, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixel), 2);
        std::cout << "Левый нижний пиксель: " <<std::endl;
        transform_to_RGB_cout(pixel);

        file.seekg(data_offset + (width-1)*2, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixel), 2);
        std::cout << "Правый нижний пиксель: " <<std::endl;
        transform_to_RGB_cout(pixel);

        file.seekg(data_offset + (height-1) * (width*2), std::ios::beg);
        file.read(reinterpret_cast<char*>(pixel), 2);
        std::cout << "Левый верхний пиксель: " <<std::endl;
        transform_to_RGB_cout(pixel);

        
        file.seekg(data_offset + (height-1) * (width*2) + (width-1)*2, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixel), 2);
        std::cout << "Правый верхний пиксель: " <<std::endl;
        transform_to_RGB_cout(pixel);
    }

    else {
        std::cout << "Не BMP файл" << std::endl;

    return 0;
}

}


