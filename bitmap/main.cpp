#include <iostream>
#include <fstream>

static bool load_file_bytes(const char* path, unsigned char* headers, const int size) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { return false; }
    file.read(reinterpret_cast<char*>(headers), size);
    return file.gcount() == size;
}

int bitwiseShift_16(const unsigned char* arr, int bytes) {
    return arr[bytes] | arr[bytes + 1] << 8;
}

int bitwiseShift_32(const unsigned char* arr, int bytes) {
    return arr[bytes] | arr[bytes + 1] << 8 | arr[bytes + 2] << 16 | arr[bytes + 3] << 24;
}

int main(const int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "image.bmp";

    unsigned char headers[54];

    if (!load_file_bytes(path, headers, 54)) {
        std::cout << "Невозможно открыть файл " << path << '\n';
        return 1;
    }

    if (headers[0] != 'B' || headers[1] != 'M') {
        std::cout << "Это не BMP файл\n";
        return 1;
    }

    std::cout << "2b headers.bitmap_signature : " << headers[0] << headers[1] << "\n";
    std::cout << "4b headers.bitmap_file_size : " << bitwiseShift_32(headers, 2) << '\n';
    std::cout << "4b headers.bitmap_data_offset : " << bitwiseShift_32(headers, 10) << '\n';
    std::cout << "4b headers.bitmap_info_header_size : " << bitwiseShift_32(headers, 14) << '\n';
    std::cout << "4b headers.bitmap_width : " << bitwiseShift_32(headers, 18) << '\n';
    std::cout << "4b headers.bitmap_height : " << bitwiseShift_32(headers, 22) << '\n';
    std::cout << "2b headers.bitmap_planes : " << bitwiseShift_16(headers, 26) << '\n';
    std::cout << "2b headers.bitmap_bits_per_pixel : " << bitwiseShift_16(headers, 28) << '\n';
    std::cout << "4b headers.bitmap_compression : " << bitwiseShift_32(headers, 30) << '\n';
    std::cout << "4b headers.bitmap_image_size : " << bitwiseShift_32(headers, 34) << '\n';
    std::cout << "4b headers.bitmap_XpixelsPerM : " << bitwiseShift_32(headers, 38) << '\n';
    std::cout << "4b headers.bitmap_YpixelsPerM : " << bitwiseShift_32(headers, 42) << '\n';
    std::cout << "4b headers.bitmap_ColorsUsed : " << bitwiseShift_32(headers, 46) << '\n';
    std::cout << "4b headers.bitmap_ColorsImportant : " << bitwiseShift_32(headers, 50) << '\n';

    int offset = bitwiseShift_32(headers, 10);
    int width = bitwiseShift_32(headers, 18);
    int height = bitwiseShift_32(headers, 22);
    int bpp = bitwiseShift_16(headers, 28);
    int compression = bitwiseShift_32(headers, 30);
    
    int row_size = width * bpp / 8;
    if (row_size % 4 != 0) { 
        row_size += 2; 
    }  

    std::ifstream file(path, std::ios::binary);
    if (!file) { 
        return 1; 
    }

    int xs[4] = {0, width - 1, 0, width - 1};
    int ys[4] = {0, 0, height - 1, height - 1};

    for (int i = 0; i < 4; i++) {
        unsigned char p[3];

        file.seekg(offset + ys[i] * row_size + xs[i] * (bpp / 8));
        file.read(reinterpret_cast<char*>(p), (bpp / 8));

        int r, g, b;

        if (bpp == 16) {
            int pixel = p[0] | p[1] << 8;
            if (compression == 3) {                    
                r = (pixel >> 11) * 255 / 31;
                g = ((pixel >> 5) & 63) * 255 / 63;
                b = (pixel & 31) * 255 / 31;
            } 
            else {                                  
                r = ((pixel >> 10) & 31) * 255 / 31;
                g = ((pixel >> 5) & 31) * 255 / 31;
                b = (pixel & 31) * 255 / 31;
            }
        }
        else if (bpp == 24) {
            b = p[0]; 
            g = p[1]; 
            r = p[2];
        }

        if(i == 0) {
            std::cout << "Левый нижний:\n";
        }
        else if(i == 1) {
            std::cout << "Правый нижний:\n";
        }
        else if(i == 2) {
            std::cout << "Левый верхний:\n";
        }
        else if(i == 3) {
            std::cout << "Правый верхний:\n";
        }
        std::cout << "R: " << r << " G: " << g << " B: " << b << std::endl;
    }
    return 0;
}

