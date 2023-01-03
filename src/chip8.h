// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <ios>

// A2DD.h
#ifndef chip8_H
#define chip8_H


class chip8{
    private:
        short stack[16];
        int program_counter;
        short index_register;
        char registers[16];
        char memory[4096];
        char delay_timer;
        char sound_timer;

        void load_fonts(){
            int fonts[] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };

            int counter = 80;
            for (int i : fonts)
            {
                printf("%d\n", i);
                memory[counter] = i;
            }
            
        }

    public:
        int test;

        chip8(char* filepath){
            printf("[+] emulator created\n");
            load_program(filepath);
            load_fonts();
        };

        void load_program(char* filepath){
            printf("[!] loading file [%s]...\n", filepath);
            std::ifstream fin(filepath, std::ios::in | std::ios::binary );
             
            if (!fin){
                printf("[-] Failed to read program, exiting...\n");
                exit(1);
            }

            // read from position 512 in memory, The first CHIP-8 interpreter (on the COSMAC VIP computer) was also located in RAM, from address 000 to 1FF. It would expect a CHIP-8 program to be loaded into memory after it, starting at address 200
            fin.read(&memory[512], 3896);
            printf("[!] %ld bytes read\n", fin.gcount());
            printf("[+] Successfuly read program\n");
            printf("%c\n", this->memory[514]);
            
            
        }





};

#endif