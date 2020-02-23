/*
File Name:      decoder.cpp
Author:         Milan Donhowe
Date:           2/22/2020
Description:    Simple .PNG chunk decoder 
*/

#include "png_decode.h"

using namespace std;

// checks if the file has a valid png header
bool valid_png_head(std::ifstream& file){
    const short signature_size = 8;
    uint8_t file_signature[signature_size] = {137, 80, 78, 71, 13, 10, 26, 10};
    uint8_t* this_byte = new uint8_t; 
    
    for (uint8_t index = 0; index < signature_size; index++){
       file.read((char *) this_byte, 1);
       if (*this_byte != file_signature[index]){
           delete this_byte;
           return false;
       }
    }

    delete this_byte;
    return true;
}

namespace png_decode
{

// this function parses the bit pattern to a regular 32 bit unsigned int
// because I don't trust cstdlib c-string parsing functions
// and png uses network byte order over host byte order
uint32_t byte_array_to_number(unsigned char *bytes)
{
    uint32_t parsed_number {0};

     
    // this just shifts the bytes into the correct order
    for (uint8_t byte = 0; byte < 4; byte++){
        parsed_number += (((unsigned int)bytes[byte]) << (8 * (3-byte))); 
    }


    return parsed_number;
};

}


void read_length_section(std::ifstream& file, uint32_t& length){

    const short buff_size {5};
    unsigned char length_buffer[buff_size] = {};
    file.read((char *) length_buffer, buff_size-1);

    length = png_decode::byte_array_to_number(length_buffer);

    // check length parse
    if (length > 2147483647){
        cout << "error: too large of a length requested.";
        exit(EXIT_FAILURE);
    }

    return;    
};

void read_type_section(std::ifstream& file, char* type){
    const short buff_size {5};
    char type_buffer[buff_size] = {};
    file.read(type_buffer, buff_size-1);
    strcpy(type, type_buffer);
    return;
};

void read_crc_section(std::ifstream& file, uint32_t& crc){
    unsigned char crc_buffer[5];
    file.read((char *) crc_buffer, 4);
    //crc = png_decode::byte_array_to_number(crc_buffer);
    crc = 0;
    return;
};

unsigned char* read_data_chunk(std::ifstream& file, uint32_t length){
    if (length == 0) return nullptr;
    unsigned char* data_bytes = new unsigned char [length+1];
    file.read((char *)data_bytes, length); 
    return data_bytes;
};

void read_chunks(std::ifstream& file, std::vector<png_decode::chunk_t>& png_chunks){
    // loop until EOF is true
    while (!file.eof()){
        
        png_decode::chunk_t next_chunk = {};

        //1. read length section
        read_length_section(file, next_chunk.length);
        
        //2. read type section
        read_type_section(file, next_chunk.type);
        
        //3. read data section
        next_chunk.data = read_data_chunk(file, next_chunk.length);
        
        //4. read crc section
        read_crc_section(file, next_chunk.crc);

        png_chunks.push_back(next_chunk);
    }
};


void free_chunks(std::vector<png_decode::chunk_t>& chunks){
    for (auto chunk : chunks){
        delete [] chunk.data;
        chunk.data = nullptr;
    }
    return;
}

int main (int argc, char **argv){

    ifstream image_file;
    image_file.open("meme.png", ios::in | ios::binary);
    /* file validation */ 
    if (image_file.fail()){
        cout << "error: couldn't open file" << endl;
        exit(EXIT_FAILURE);
    }
    if (!valid_png_head(image_file)){
        cout << "error: invalid file header" << endl;
        exit(EXIT_FAILURE);
    }

    std::vector<png_decode::chunk_t> file_chunks;
    cout << "png file correctly loaded and ready for parsing" << endl;

    read_chunks(image_file, file_chunks); 

    for (auto chnk : file_chunks){
        cout << chnk.type << " : " << chnk.length << " bytes long" << endl;
    }

    // close file & free memory
    image_file.close();
    free_chunks(file_chunks);
    return 0;
}

