// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <ios>
#include <iomanip>
#include <iostream>
#include <bitset>


// A2DD.h
#ifndef chip8_H
#define chip8_H



class chip8{
    private:
        uint16_t stack[16];
        uint8_t input_pressed[0xf];
        int stack_pointer = -1;
        int program_counter = 512;
        uint16_t index_register;
        uint8_t registers[16];
        uint8_t memory[4096];
        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;
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


            for (int byte_shift = 0; byte_shift < N; byte_shift++) // for N bytes of sprite data
            {
                uint8_t sprite_data = this->memory[this->index_register + byte_shift];
                printf("%02x \n", this->memory[this->index_register + byte_shift]);

                for (int bit_shift = 0; bit_shift < 8; bit_shift++)
                {
                    if(X_coord == 63){
                        break;
                    }

                    int bit = (sprite_data >> (7-bit_shift)) % 2;
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

        uint16_t get_relative_instruction(int pos){
            return ((uint16_t)this->memory[this->program_counter + pos] << 8 | this->memory[this->program_counter+1 + pos]);
        }

        chip8(char* filepath){
            printf("[+] emulator created\n");
            load_program(filepath);
            load_fonts();
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

            handle_input();

            switch (op >> 12){
                case 0x0: // Clear screen / return
                    if (NN == 0xe0){
                        printf("clear screen\n");
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

                case 0x8: 
                    // printf("%d\n", N);
                    switch (N){
                        case 0x0: // Set Vx = Vy
                            this->registers[X] = this->registers[Y];
                            break;

                        case 0x1: // Binary OR
                            this->registers[X] = this->registers[X] | this->registers[Y];
                            break;
                        
                        case 0x2: // Binary AND
                            this->registers[X] = this->registers[X] & this->registers[Y];
                            break;

                        case 0x3: // Logical XOR
                            this->registers[X] = this->registers[X] ^ this->registers[Y];
                            break; 

                        case 0x4: // Add
                            this->registers[X] = this->registers[X] + this->registers[Y];
                            break; // finish this: does not handle overflow

                        case 0x5: // Subtract
                            if (this->registers[X] > this->registers[Y]){
                                this->registers[0xF] = 1;
                            }
                            else{
                                this->registers[0xF] = 0;
                            }
                            this->registers[X] = this->registers[X] - this->registers[Y];
                            break;

                        case 0x7: // Subtract
                            if (this->registers[X] > this->registers[Y]){
                                this->registers[0xF] = 1;
                            }
                            else{
                                this->registers[0xF] = 0;
                            }
                            this->registers[X] = this->registers[Y] - this->registers[X];
                            break;

                        case 0x6: // Shift
                            
                            if ((this->registers[X] & (1 << 8)) >> 8 == 1){
                                this->registers[0xF] = 1;
                            }
                            else{
                                this->registers[0xF] = 0;
                            }
                            this->registers[X] = this->registers[X] >> 1;
                            break;

                        case 0xE: // Shift
                            
                            if ((this->registers[X] & (1 << 8)) >> 8 == 1){
                                this->registers[0xF] = 1;
                            }
                            else{
                                this->registers[0xF] = 0;
                            }
                            this->registers[X] = this->registers[X] << 1;
                            break;
                    }
                    break;

                case 0x9: // Skip next instruction if Vx != Vy.
                    if (this->registers[X] != this->registers[Y]){
                        this->program_counter += 2;
                    }
                    break;

                case 0xA: // Set I = nnn.
                    this->index_register = NNN;
                    printf("idx set to %03x, %03x\n", this->index_register, NNN);
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
                    switch(NN){
                        case 0x9E:
                            
                            printf("Ex9E\n");
                            
                            if(check_if_input_pressed(X)) {
                                this->program_counter += 2;
                            }
                            break;

                        case 0xA1:
                            printf("ExA1\n");
                            if (!(check_if_input_pressed(X))) {
                                this->program_counter += 2;
                            }
                            break;

                    default:
                        printf("unrecognised 0xE %04x \n", NN);
                    }

                case 0xF:
                    switch(NN){
                        case 0x07:
                        {
                            printf("FX07\n");
                            this->registers[X] = this->delay_timer;
                            break;
                        }
                        case 0x15:
                        {
                            printf("FX15\n");
                            this->delay_timer = this->registers[X];
                            break;
                        }
                        case 0x18:
                        {
                            printf("FX18\n");
                            this->sound_timer = this->registers[X];
                            break;
                        }

                        case 0x1E:
                        {
                            printf("FX1E\n");
                            if(this->index_register + this->registers[X] > 0xFFF)
                                this->registers[0xF] = 1;
                            else
                                this->registers[0xF] = 0;
                            this->index_register += this->registers[X];
                            printf("set idx to %04x, %04x", this->index_register, this->registers[X]);
                            break;
                            }

                        case 0x0A:
                        {
                            printf("FX0A\n");
                            bool input_found = false;
                            for (int i = 0; i < 0xf; i++)
                            {
                                if (this->input_pressed[i]){
                                    printf("%d found\n", i);
                                    input_found = true;
                                    this->registers[X] = i;
                                }                                
                            }
                            if (!(input_found))
                                this->program_counter -= 2;
                            break;
                        }

                        case 0x29:
                        {
                            printf("FX28\n");
                            this->index_register = 80+(5*this->registers[X]);
                            break;
                        }

                        case 0x33:
                            printf("FX33\n");
                            this->memory[this->index_register] = this->registers[X] / 100;
                            this->memory[this->index_register + 1] = (this->registers[X] / 10) % 10;
                            this->memory[this->index_register + 2] = this->registers[X] % 10;
                            break;

                        case 0x55:
                            printf("Fx55\n");
                            for (int i = 0; i < X; i++)
                            {
                                this->memory[this->index_register + i] = this->registers[i];
                            }
                            break;

                        case 0x65:
                            printf("Fx55\n");
                            for (int i = 0; i < X; i++)
                            {
                                this->registers[i] = this->memory[this->index_register + i];
                            }
                            break;

                    default:
                        printf("unrecognised 0xF %04x \n", NN);
                    }
                    break;

                default:
                    printf("unrecognised opcode\n");
                 
            }
            render(draw_list);
            

            if (delay_timer > 0)
            {
                std::cout << delay_timer << std::endl;
                delay_timer--;
            }

            if (sound_timer > 0)
            {
                std::cout << sound_timer << std::endl;
                std::cout << "Beep!" << std::endl;
                sound_timer--;
            }

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
        
        void handle_input(){
            this->input_pressed[0x1] =  ImGui::IsKeyDown(ImGuiKey_1);
            this->input_pressed[0x2] =  ImGui::IsKeyDown(ImGuiKey_2);
            this->input_pressed[0x3] =  ImGui::IsKeyDown(ImGuiKey_3);
            this->input_pressed[0xC] =  ImGui::IsKeyDown(ImGuiKey_4);

            this->input_pressed[0x4] =  ImGui::IsKeyDown(ImGuiKey_Q);
            this->input_pressed[0x5] =  ImGui::IsKeyDown(ImGuiKey_W);
            this->input_pressed[0x6] =  ImGui::IsKeyDown(ImGuiKey_E);
            this->input_pressed[0xD] =  ImGui::IsKeyDown(ImGuiKey_R);

            this->input_pressed[0x7] =  ImGui::IsKeyDown(ImGuiKey_A);
            this->input_pressed[0x8] =  ImGui::IsKeyDown(ImGuiKey_S);
            this->input_pressed[0x9] =  ImGui::IsKeyDown(ImGuiKey_D);
            this->input_pressed[0xE] =  ImGui::IsKeyDown(ImGuiKey_F);

            this->input_pressed[0xA] =  ImGui::IsKeyDown(ImGuiKey_Z);
            this->input_pressed[0x0] =  ImGui::IsKeyDown(ImGuiKey_X);
            this->input_pressed[0xB] =  ImGui::IsKeyDown(ImGuiKey_C);
            this->input_pressed[0xF] =  ImGui::IsKeyDown(ImGuiKey_V);
        }

        bool check_if_input_pressed(uint8_t pos){
            return this->input_pressed[pos];
        }





};

#endif