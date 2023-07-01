// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <ios>
#include <iomanip>
#include <iostream>

// A2DD.h
#ifndef chip8_H
#define chip8_H



class chip8{
    private:
        uint16_t stack[16];
        int stack_pointer = -1;
        int program_counter = 512;
        uint8_t index_register;
        uint8_t registers[16];
        uint8_t memory[4096];
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t display[64][32];
        int pixel_size = 10;


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
                this->memory[counter] = i;
                counter++;
            }
            
        }

        void print_stack(){
            for (int i = 0; i < 16; i++)
            {
                printf("stack %d: %04x", i, this->stack[i]);
            }
        }


        void debug_printout(uint16_t op, uint8_t X, uint8_t Y, uint8_t N, uint8_t NN, uint16_t NNN){

            printf("PC: %5d |", this->program_counter);
            printf("OP: %04x | X: %02x | Y: %02x | N: %02x | NN: %02x | NNN: %04x |\n",op, X, Y, N, NN, NNN);

            for (int i = 0; i < 16; i++)
            {
                printf("%2d: %4d | ", i,this->registers[i]);
            }
            printf("idx: %4d | ", this->index_register);
            printf("\n");
            
        }

        void draw(uint8_t X, uint8_t Y, uint8_t  N){
            int X_coord = this->registers[X] % 64;
            int y_coord = this->registers[Y] % 32;
            this->registers[0xF] = 0;

            printf("drawing..., start at %d, %d\n", X_coord, y_coord);

            for (int byte_shift = 0; byte_shift < N; byte_shift++) // for N bytes of sprite data
            {
                uint16_t op = ((uint16_t)this->memory[this->program_counter] << 8 | this->memory[this->program_counter+1]);
                uint8_t sprite_data = this->memory[this->index_register + byte_shift + 512];
                printf("%02x ", sprite_data);

                for (int bit_shift = 0; bit_shift < 8; bit_shift++)
                {
                    if(X_coord == 63){
                        break;
                    }

                    int bit = (sprite_data >> (7-bit_shift)) % 2;
                    printf("%d", bit);
                    // If the current pixel in the sprite row is on and the pixel at coordinates X,Y on the screen is also on,
                    // turn off the pixel and set VF to 1
                    if (bit == 1 && this->display[X_coord][y_coord] != 0){
                        this->display[X_coord][y_coord] = 0;
                        this->registers[0xF] = 1;
                    }

                    // if the current pixel in the sprite row is on and the screen pixel is not, draw the pixel at the X and Y coordinates

                    if (bit == 1 && this->display[X_coord][y_coord] == 0){
                        this->display[X_coord][y_coord] = 1;
                    }

                    X_coord++;
                }
                printf("\n");
                if(y_coord == 31){
                        break;
                }
                y_coord++;
                X_coord -= 8;
            
            }
        }

        void render(ImDrawList* draw_list){


            const ImVec2 pos = ImGui::GetCursorScreenPos();
            for (int i = 0; i < 64; i++ )
            	{
            		for (int j = 0; j < 32; j++ )
            		{

            			// draw_list->AddRectFilled( ImVec2( pos.x  *  1.0f, pos.y *  1.0f ), ImVec2( pos.x *  5.5f , pos.y *  5.5f ), color);
                        if (this->display[i][j] == 1){
                            draw_list->AddRectFilled( ImVec2( (pos.x) + (i * this->pixel_size), (pos.y) +  (j * pixel_size) ),
                                                    ImVec2( (pos.x) + (i * pixel_size) + pixel_size, (pos.y) +  (j * pixel_size)  + pixel_size), 
                                                    ImColor(255,255,255));
                        }
            		}
            	}
            
        }
        

        

    public:
        uint8_t* get_registers(){
            return this->registers;
        }

        uint8_t get_index_register(){
            return this->index_register;
        }

        int get_program_counter(){
            return this->program_counter;
        }
        
        uint16_t* get_stack(){
            return this->stack;
        }

        int get_stack_pointer(){
            return this->stack_pointer;
        }

        chip8(char* filepath){
            printf("[+] emulator created\n");
            load_program(filepath);
            load_fonts();
            print_memory();
            for (int i = 0; i < 64; i++)
            {
                for (int j = 0; j < 32; j++)
                {
                    this->display[i][j] = 255;
                }
            }
     
        };

        uint16_t cycle(ImDrawList* draw_list){
            // Fetch - Get current 2 byte instruction

            // combining two bytes to get the opcode - https://stackoverflow.com/questions/14733761/printf-formatting-for-hexadecimal
            uint16_t op = ((uint16_t)this->memory[this->program_counter] << 8 | this->memory[this->program_counter+1]);
            this->program_counter += 2;


            // Decode / Execute
            uint8_t X = (op & 0x0f00) >> 8;             // second nibble
            uint8_t Y = (op & 0x00f0) >> 4;             // third nibble
            uint8_t N = (op & 0x000f);                  // fourth nibble
            uint8_t NN = (op & 0x00ff);                 // second byte
            uint16_t NNN = (op & 0x0fff) ;              // second, third and fourth nibbles

            // debug_printout(op, X,Y,N,NN,NNN);
            // print_stack();

            switch (op >> 12){
                case 0x0: // Clear screen / return
                    if (NN == 0xe0){
                        for (int i = 0; i < 64; i++)
                        {
                            for (int j = 0; j < 32; j++)
                            {
                                this->display[i][j] = 255;
                            }
                        }
                    }
                    if (NN == 0xee){
                        this->program_counter = this->stack[this->stack_pointer];
                        this->stack[this->stack_pointer] = 0;
                        this->stack_pointer -= 1;
                    }
                    
                    break;

                // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
                case 0x1: // Jump
                    this->program_counter = NNN;

                    break;
                
                case 0x2: // Call 
                    this->stack_pointer += 1;
                    this->stack[this->stack_pointer] = this->program_counter;
                    this->program_counter = NNN;
                    break;

                case 0x3: // Skip next instruction if Vx = NN.
                    if (this->registers[X] == NN){
                        this->program_counter += 2;
                    }
                    break;

                case 0x4: // Skip next instruction if Vx != NN.
                    if (this->registers[X] != NN){
                        this->program_counter += 2;
                    }
                    break;

                case 0x5: // Skip next instruction if Vx = Vy.
                    if (this->registers[X] == this->registers[Y]){
                        this->program_counter += 2;
                    }
                    break;

                case 0x6: // Set Vx = NN.
                    this->registers[X] = NN;

                    break;

                case 0x7: // Set Vx = Vx + NN.
                    this->registers[X] += NN;
                    break;

                case 0x8: // Set Vx = Vy.
                    // TODO: not done
                    this->registers[X] = this->registers[Y];
                    break;

                case 0x9: // Skip next instruction if Vx != Vy.
                    if (this->registers[X] != this->registers[Y]){
                        this->program_counter += 2;
                    }
                    break;

                case 0xA: // Set I = nnn.
                    this->index_register = NNN;
                    break;

                case 0xB: // Jump to location nnn + V0.
                    this->program_counter = NNN + this->registers[0];
                    break;

                case 0xC: // Set Vx = random byte AND nn.
                    this->registers[X] = rand() && NN;
                    break;

                case 0xD: // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
                    draw(X,Y,N);
                    break;

                case 0xE:
                    break;

                case 0xF:
                    break;

                default:
                    printf("unrecognised opcode\n");
                 
            }
            render(draw_list);


            // int a = std::cin.get();
            return op;
        }

        void load_program(char* filepath){
            memset(this->memory, 0, sizeof(this->memory));
            printf("[!] loading file [%s]...\n", filepath);
            std::ifstream fin(filepath, std::ios::in | std::ios::binary );
             
            if (!fin){
                printf("[-] Failed to read program, exiting...\n");
                exit(1);
            }

            // read from position 512 in memory, The first CHIP-8 interpreter (on the COSMAC VIP computer) was also located in RAM, from address 000 to 1FF. It would expect a CHIP-8 program to be loaded into memory after it, starting at address 200
            fin.read(reinterpret_cast<char*>(&memory[512]), 3896);
            printf("[!] %ld bytes read\n", fin.gcount());
            printf("[+] Successfuly read program\n");
            
        }

        void print_memory(){
            int counter = 0;
            for (char i : this->memory){
                counter ++;
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(unsigned char)i ;
                // printf("%c/n", i);
                if (counter == 16){
                    std::cout  << std::endl;
                    counter = 0;
                }
            }
            std::cout << std::endl;
        }





};

#endif