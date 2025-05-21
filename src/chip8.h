// Include standard headers
#include <bitset>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// A2DD.h
#ifndef chip8_H
#define chip8_H

class chip8 {
  private:
    uint16_t stack[16];
    uint8_t input_pressed[0xf];
    int stack_pointer = -1;
    char op_info[100];
    int program_counter = 512;
    uint16_t index_register;
    uint8_t registers[16];
    uint8_t memory[4096];
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;
    uint8_t display[64][32];

    int pixel_size = 10;

    void load_fonts() {
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
        for (int i = 0; i < sizeof(fonts) / sizeof(fonts[0]); i++) {
            this->memory[counter] = fonts[i];
            counter++;
        }
    }

    void print_stack() {
        for (int i = 0; i < 16; i++) {
            printf("stack %d: %04x", i, this->stack[i]);
        }
    }

    void debug_printout(uint16_t op, uint8_t X, uint8_t Y, uint8_t N, uint8_t NN, uint16_t NNN) {

        printf("PC: %5d |", this->program_counter);
        printf("OP: %04x | X: %02x | Y: %02x | N: %02x | NN: %02x | NNN: %04x |\n", op, X, Y, N, NN, NNN);

        for (int i = 0; i < 16; i++) {
            printf("%2d: %4d | ", i, this->registers[i]);
        }
        printf("idx: %4d | ", this->index_register);
        printf("\n");
    }

    void draw(uint8_t X, uint8_t Y, uint8_t N) {
        int X_coord = this->registers[X] & 63;
        int y_coord = this->registers[Y] & 31;
        this->registers[0xF] = 0;

        for (int byte_shift = 0; byte_shift < N; byte_shift++) // for N rows of sprite data
        {
            uint8_t sprite_data = this->memory[this->index_register + byte_shift];

            for (int bit_shift = 0; bit_shift < 8; bit_shift++) {
                if (X_coord > 70) {
                    break;
                }
                int bit = (sprite_data >> (7 - bit_shift)) % 2;
                // If the current pixel in the sprite row is on and the pixel at
                // coordinates X,Y on the screen is also on, turn off the pixel
                // and set VF to 1
                if (bit == 1 && this->display[X_coord][y_coord] != 0) {
                    this->display[X_coord][y_coord] = 0;
                    this->registers[0xF] = 1;
                }

                // if the current pixel in the sprite row is on and the screen
                // pixel is not, draw the pixel at the X and Y coordinates
                else if (bit == 1 && this->display[X_coord][y_coord] == 0) {
                    this->display[X_coord][y_coord] = 1;
                }

                X_coord++;
            }
            y_coord++;
            X_coord -= 8;
        }
    }

    void render(ImDrawList *draw_list) {

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 32; j++) {

                // draw_list->AddRectFilled( ImVec2( pos.x  *  1.0f, pos.y
                // *  1.0f ), ImVec2( pos.x *  5.5f , pos.y *  5.5f ), color);
                if (this->display[i][j] == 1) {
                    draw_list->AddRectFilled(
                        ImVec2((pos.x) + (i * this->pixel_size), (pos.y) + (j * pixel_size)),
                        ImVec2((pos.x) + (i * pixel_size) + pixel_size, (pos.y) + (j * pixel_size) + pixel_size),
                        ImColor(255, 255, 255));
                }
            }
        }
    }

  public:
    uint8_t *get_registers() { return this->registers; }

    uint8_t get_index_register() { return this->index_register; }

    int get_program_counter() { return this->program_counter; }

    uint16_t *get_stack() { return this->stack; }

    int get_stack_pointer() { return this->stack_pointer; }

    uint16_t get_relative_instruction(int pos) {
        return ((uint16_t)this->memory[this->program_counter + pos - 2] << 8 |
                this->memory[this->program_counter + 1 + pos - 2]);
    }

    char *get_op_info() { return this->op_info; }

    chip8(char *filepath) {
        printf("[+] emulator created\n");
        load_program(filepath);
        load_fonts();
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 32; j++) {
                this->display[i][j] = 255;
            }
        }
    };

    uint16_t cycle(ImDrawList *draw_list) {
        // handle_input();
        // static int last_run = 0;
        // if (!this->input_pressed[0x1] || last_run == 1) {
        //     last_run = 0;
        //     return 0;
        // }
        // last_run = 1;
        // Fetch - Get current 2 byte instruction

        // combining two bytes to get the opcode -
        // https://stackoverflow.com/questions/14733761/printf-formatting-for-hexadecimal
        uint16_t op = ((uint16_t)this->memory[this->program_counter] << 8 | this->memory[this->program_counter + 1]);
        this->program_counter += 2;

        // Decode / Execute
        uint8_t X = (op & 0x0f00) >> 8; // second nibble
        uint8_t Y = (op & 0x00f0) >> 4; // third nibble
        uint8_t N = (op & 0x000f);      // fourth nibble
        uint8_t NN = (op & 0x00ff);     // second byte
        uint16_t NNN = (op & 0x0fff);   // second, third and fourth nibbles

        // debug_printout(op, X,Y,N,NN,NNN);
        // print_stack();

        handle_input();
        sprintf(this->op_info, "Not implemented");

        switch (op >> 12) {
        case 0x0: // Clear screen / return
            if (NN == 0xe0) {
                for (int i = 0; i < 64; i++) {
                    for (int j = 0; j < 32; j++) {
                        this->display[i][j] = 255;
                    }
                }
                sprintf(this->op_info, "clear display");
            }
            if (NN == 0xee) {
                this->program_counter = this->stack[this->stack_pointer];
                this->stack[this->stack_pointer] = 0;
                this->stack_pointer -= 1;
                sprintf(this->op_info, "return");
            }

            break;

        // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
        case 0x1: // Jump
            this->program_counter = NNN;
            sprintf(this->op_info, "jump to %03x", NNN);
            break;

        case 0x2: // Call
            this->stack_pointer += 1;
            this->stack[this->stack_pointer] = this->program_counter;
            this->program_counter = NNN;
            sprintf(this->op_info, "call %03x", NNN);
            break;

        case 0x3: // Skip next instruction if Vx = NN.
            if (this->registers[X] == NN) {
                this->program_counter += 2;
            }
            sprintf(this->op_info, "skip next if R%x(%02x) == %02x", X, this->registers[X], NN);
            break;

        case 0x4: // Skip next instruction if Vx != NN.
            if (this->registers[X] != NN) {
                this->program_counter += 2;
            }
            sprintf(this->op_info, "skip next if R%x(%02x) != %02x", X, this->registers[X], NN);
            break;

        case 0x5: // Skip next instruction if Vx = Vy.
            if (this->registers[X] == this->registers[Y]) {
                this->program_counter += 2;
            }
            sprintf(this->op_info, "skip next if %02x == %02x", this->registers[X], this->registers[Y]);
            break;

        case 0x6: // Set Vx = NN.
            this->registers[X] = NN;
            sprintf(this->op_info, "set register %01x to %02x", X, NN);
            break;

        case 0x7: // Set Vx = Vx + NN.
            this->registers[X] += NN;
            sprintf(this->op_info, "add %02x to register %01x = %02x", NN, X, this->registers[X]);
            break;

        case 0x8:
            switch (N) {
            case 0x0: // Set Vx = Vy
                this->registers[X] = this->registers[Y];
                sprintf(this->op_info, "set register %01x to register %01x", X, Y);
                break;

            case 0x1: // Binary OR
                this->registers[X] = this->registers[X] | this->registers[Y];
                sprintf(this->op_info, "binary OR register %01x and register %01x", X, Y);
                break;

            case 0x2: // Binary AND
                this->registers[X] = this->registers[X] & this->registers[Y];
                sprintf(this->op_info, "binary AND register %01x and register %01x", X, Y);
                break;

            case 0x3: // Logical XOR
                this->registers[X] = this->registers[X] ^ this->registers[Y];
                sprintf(this->op_info, "logican XOR register %01x and register %01x", X, Y);
                break;

            case 0x4: // Add
                if (this->registers[X] + this->registers[Y] > 255) {
                    this->registers[X] = this->registers[X] + this->registers[Y];
                    this->registers[0xF] = 1;
                } else {
                    this->registers[X] = this->registers[X] + this->registers[Y];
                    this->registers[0xF] = 0;
                }
                sprintf(this->op_info, "add register %01x to register %01x", Y, X);
                break;

            case 0x5: // Subtract X - Y
                if (this->registers[X] >= this->registers[Y]) {
                    this->registers[X] = this->registers[X] - this->registers[Y];
                    this->registers[0xF] = 1;
                } else {
                    this->registers[X] = this->registers[X] - this->registers[Y];
                    this->registers[0xF] = 0;
                }
                sprintf(this->op_info, "subtract register %01x from register %01x", Y, X);
                break;

            case 0x7: // Subtract Y - X
                if (this->registers[Y] >= this->registers[X]) {
                    this->registers[X] = this->registers[Y] - this->registers[X];
                    this->registers[0xF] = 1;
                } else {
                    this->registers[X] = this->registers[Y] - this->registers[X];
                    this->registers[0xF] = 0;
                }
                sprintf(this->op_info, "subtract register %01x from register %01x", Y, X);
                break;

            case 0x6: // Shift right
            {
                uint8_t shifted_bit = this->registers[X] & 1;
                this->registers[X] = this->registers[X] >> 1;
                this->registers[0xF] = shifted_bit;
                sprintf(this->op_info, "Shift right register %01x", X);
            } break;

            case 0xE: // Shift left
            {
                uint8_t shifted_bit = (this->registers[X] & (1 << 7)) >> 7;
                this->registers[X] = this->registers[X] << 1;
                this->registers[0xF] = shifted_bit;
                sprintf(this->op_info, "Shift left register %01x", X);
            } break;
            }
            break;

        case 0x9: // Skip next instruction if Vx != Vy.
            if (this->registers[X] != this->registers[Y]) {
                this->program_counter += 2;
            }
            sprintf(this->op_info, "Shift left register %01x", X);
            break;

        case 0xA: // Set I = nnn.
            this->index_register = NNN;
            sprintf(this->op_info, "idx set to %03x, %03x\n", this->index_register, NNN);
            break;

        case 0xB: // Jump to location nnn + V0.
            this->program_counter = NNN + this->registers[0];
            sprintf(this->op_info, "jump to %03x", NNN + this->registers[0]);
            break;

        case 0xC: // Set Vx = random byte AND nn.
            this->registers[X] = rand() && NN;
            sprintf(this->op_info, "set register %01x to random byte AND %03x", X, NNN);
            break;

        case 0xD: // Display n-byte sprite starting at memory location I at (Vx,
                  // Vy), set VF = collision.
            draw(X, Y, N);
            break;

        case 0xE:
            switch (NN) {
            case 0x9E:

                if (check_if_input_pressed(X)) {
                    this->program_counter += 2;
                    sprintf(this->op_info, "check if input %01x is pressed", X);
                }
                break;

            case 0xA1:
                if (!(check_if_input_pressed(X))) {
                    this->program_counter += 2;
                    sprintf(this->op_info, "check if input %01x is not pressed", X);
                }
                break;

            default:
                break;
            }

        case 0xF:
            switch (NN) {
            case 0x07: {
                this->registers[X] = this->delay_timer;
                sprintf(this->op_info, "set register %01x to delay timer value (%02x)", X, this->delay_timer);
                break;
            }
            case 0x15: {
                this->delay_timer = this->registers[X];
                sprintf(this->op_info, "set delay timer to register %01x value (%02x)", X, this->registers[X]);
                break;
            }
            case 0x18: {
                this->sound_timer = this->registers[X];
                sprintf(this->op_info, "set sound timer to register %01x value (%02x)", X, this->registers[X]);
                break;
            }

            case 0x1E: {
                if (this->index_register + this->registers[X] > 0xFFF)
                    this->registers[0xF] = 1;
                else
                    this->registers[0xF] = 0;
                this->index_register += this->registers[X];
                sprintf(this->op_info, "add register %01x, (%02x) to idx (%04x)\n", X, this->registers[X],
                        this->index_register);
                break;
            }

            case 0x0A: {
                bool input_found = false;
                for (int i = 0; i <= 0xf; i++) {
                    if (this->input_pressed[i]) {
                        input_found = true;
                        this->registers[X] = i;
                    }
                }
                if (!(input_found)) {
                    this->program_counter -= 2;
                }

                sprintf(this->op_info, "wait for input");
                break;
            }

            case 0x29: {
                this->index_register = 80 + (5 * this->registers[X]);
                sprintf(this->op_info, "set index register to character %01x", this->registers[X]);
                break;
            }

            case 0x33:
                this->memory[this->index_register] = this->registers[X] / 100;
                this->memory[this->index_register + 1] = (this->registers[X] / 10) % 10;
                this->memory[this->index_register + 2] = this->registers[X] % 10;
                sprintf(this->op_info, "binary-coded decimal conversion");
                break;

            case 0x55:
                for (int i = 0; i <= X; i++) {
                    this->memory[this->index_register + i] = this->registers[i];
                }
                sprintf(this->op_info, "get memory");
                break;

            case 0x65:
                for (int i = 0; i <= X; i++) {
                    this->registers[i] = this->memory[this->index_register + i];
                }
                sprintf(this->op_info, "set memory");
                break;

            default:
                // printf("unrecognised 0xF %04x \n", NN);
                break;
            }
            break;

        default:
            printf("unrecognised opcode\n");
        }
        render(draw_list);

        if (delay_timer > 0) {
            delay_timer--;
        }

        if (sound_timer > 0) {
            // std::cout << "Beep!" << std::endl;
            sound_timer--;
        }

        // Sleep for 100 ms
        // usleep(500 * 1000);
        return op;
    }

    void load_program(char *filepath) {
        memset(this->memory, 0, sizeof(this->memory));
        printf("[!] loading file [%s]...\n", filepath);
        std::ifstream fin(filepath, std::ios::in | std::ios::binary);

        if (!fin) {
            printf("[-] Failed to read program, exiting...\n");
            exit(1);
        }

        // read from position 512 in memory, The first CHIP-8 interpreter (on
        // the COSMAC VIP computer) was also located in RAM, from address 000 to
        // 1FF. It would expect a CHIP-8 program to be loaded into memory after
        // it, starting at address 200
        fin.read(reinterpret_cast<char *>(&memory[512]), 3896);
        printf("[!] %ld bytes read\n", fin.gcount());
        printf("[+] Successfuly read program\n");
    }

    void print_memory() {
        int counter = 0;
        for (char i : this->memory) {
            counter++;
            std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(unsigned char)i;
            // printf("%c/n", i);
            if (counter == 16) {
                std::cout << std::endl;
                counter = 0;
            }
        }
        std::cout << std::endl;
    }

    void handle_input() {
        this->input_pressed[0x1] = ImGui::IsKeyPressed(ImGuiKey_1);
        this->input_pressed[0x2] = ImGui::IsKeyPressed(ImGuiKey_2);
        this->input_pressed[0x3] = ImGui::IsKeyPressed(ImGuiKey_3);
        this->input_pressed[0xC] = ImGui::IsKeyPressed(ImGuiKey_4);

        this->input_pressed[0x4] = ImGui::IsKeyPressed(ImGuiKey_Q);
        this->input_pressed[0x5] = ImGui::IsKeyPressed(ImGuiKey_W);
        this->input_pressed[0x6] = ImGui::IsKeyPressed(ImGuiKey_E);
        this->input_pressed[0xD] = ImGui::IsKeyPressed(ImGuiKey_R);

        this->input_pressed[0x7] = ImGui::IsKeyPressed(ImGuiKey_A);
        this->input_pressed[0x8] = ImGui::IsKeyPressed(ImGuiKey_S);
        this->input_pressed[0x9] = ImGui::IsKeyPressed(ImGuiKey_D);
        this->input_pressed[0xE] = ImGui::IsKeyPressed(ImGuiKey_F);

        this->input_pressed[0xA] = ImGui::IsKeyPressed(ImGuiKey_Z);
        this->input_pressed[0x0] = ImGui::IsKeyPressed(ImGuiKey_X);
        this->input_pressed[0xB] = ImGui::IsKeyPressed(ImGuiKey_C);
        this->input_pressed[0xF] = ImGui::IsKeyPressed(ImGuiKey_V);
    }

    bool check_if_input_pressed(uint8_t pos) {
        handle_input();
        return this->input_pressed[pos];
    }
};

#endif