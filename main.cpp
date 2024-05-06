/*
http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html
https://ru.wikipedia.org/wiki/BMP
*/

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>


using namespace std;

struct header_struct {
    short signature;
    int fileSize;
    int reserved;
    int dataOffset;
} header_s;

struct info_header {
    int size;
    int width;
    int height;
    short planes;
    short bitcount;
    int compression;
    int imageSize;
    int XpixelsPerM;
    int YpixelsPerM;
    int colorsUsed;
    int colorsImportant;
} info_h;

struct pixel_struct {
    char b;
    char g;
    char r;
    char a;
};

void delete_pixel_array(
        vector<vector<pixel_struct*>>& array) {
    for (vector<pixel_struct*> v : array) {
        for (pixel_struct* p : v) {
            delete p;
        }
    }
}

vector<vector<pixel_struct*>> right_rotate(
        vector<vector<pixel_struct*>>& array,
         int w, int h) {

    vector<
        vector<pixel_struct*>
    > array90_r;
    for (int i=0; i<w; i++) {
        vector<pixel_struct*> column;
        for (int j=0; j<h; j++) {
            // convert column into row
            column.insert(column.begin(), array[j][i]);
        }
        array90_r.push_back(column);
    }
    return array90_r;
}

vector<vector<pixel_struct*>> left_rotate(
        vector<vector<pixel_struct*>>& array,
        int w, int h) {

    vector<
        vector<pixel_struct*>
    > array90_l;
    for (int i=w-1; i>=0; i--) {
        vector<pixel_struct*> column;
        for (int j=0; j<h; j++) {
            // convert column into row
            column.push_back(array[j][i]);
        }
        array90_l.push_back(column);
    }
    return array90_l;
}


int main() {

    ifstream file ("image.bmp", ios::binary);
    if (!file) {
        cout << "Error\n";
        return 0;
    }

    /* start of parsing */
    int header_size = 14;
    char* header = new char[header_size];
    file.read(header, header_size);
    memcpy(&header_s, header, header_size); // serialize raw data into structure

    int infoheader_size = 40;
    char* infoheader = new char[infoheader_size];
    file.read(infoheader, infoheader_size);
    memcpy(&info_h, infoheader, infoheader_size); // serialize raw data into structure

    char* infoheader_tail = new char[info_h.size - 40];
    file.read(infoheader_tail, info_h.size - 40);

    char* dataBuffer = new char[
        info_h.imageSize
    ];
    file.read(dataBuffer, info_h.imageSize);
    /* end of parsing */


    printf("size : %d\n", info_h.size);
    printf("width : %d\n", info_h.width);
    printf("height : %d\n", info_h.height);
    printf("bitcount : %d\n", info_h.bitcount);
    printf("compression : %d\n", info_h.compression);
    printf("imageSize : %d\n", info_h.imageSize);
    printf("colorsUsed : %d\n", info_h.colorsUsed);
    printf("colorsImportant : %d\n", info_h.colorsImportant);


    vector<
        vector<pixel_struct*>
    > array;


    int index = 0;
    int offset = (info_h.bitcount == 32) ? 4 : 3;
    // convert row image data into 2-dim vector
    for (int i=0; i<info_h.height; i++) {
        vector<pixel_struct*> row;
        for (int j=0; j<info_h.width; j++) {
            pixel_struct* pixel = new pixel_struct;

            memcpy(pixel, &(dataBuffer[index]), offset);

            index += offset;
            row.push_back(pixel);
        }
        array.insert(array.begin(), row);
    }
    // end of convert



    //---------------------------------------
    vector<
        vector<pixel_struct*>
    > array90 = right_rotate(array, info_h.width, info_h.height);
    int ww = info_h.width, hh = info_h.height;
    swap(info_h.width, info_h.height);

    // convert 2-dim vector into raw bmp data
    index = 0;
    char* outBuffer = new char[info_h.imageSize];
    for (int i=array90.size()-1; i>=0; i--) {
        vector<pixel_struct*> row = array90[i];
        for (pixel_struct* pixel : row) {
            memcpy(&(outBuffer[index]), pixel, offset);
            index += offset;
        }
    }
    // end of convert

    ofstream outFile ("right.bmp", ios::binary);
    outFile.write(header, header_size);
    outFile.write((char*)(&info_h), sizeof(info_h));
    outFile.write(infoheader_tail, info_h.size - 40);

    outFile.write(outBuffer, info_h.imageSize);
    outFile.close();
    //---------------------------------------




    //---------------------------------------
    array90 = left_rotate(array, ww, hh);

    // convert 2-dim vector into raw bmp data
    index = 0;
    for (int i=array90.size()-1; i>=0; i--) {
        vector<pixel_struct*> row = array90[i];
        for (pixel_struct* pixel : row) {
            memcpy(&(outBuffer[index]), pixel, offset);
            index += offset;
        }
    }
    // end of convert

    ofstream outFile1 ("left.bmp", ios::binary);
    outFile1.write(header, header_size);
    outFile1.write((char*)(&info_h), sizeof(info_h));
    outFile1.write(infoheader_tail, info_h.size - 40);

    outFile1.write(outBuffer, info_h.imageSize);
    outFile1.close();
    //---------------------------------------



    delete header;
    delete infoheader;
    delete infoheader_tail;
    delete dataBuffer;
    delete outBuffer;

    delete_pixel_array(array);

    file.close();
    outFile.close();
}