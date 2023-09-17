// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// needed as we're passing through window from main.cppq
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
#include "../imgui/imgui.h"
#include <string>

#include "chip8.h"


void render_loop( GLFWwindow* window, chip8* chip_8){
	// setup game window
	ImGui::SetNextWindowPos(ImVec2(0,0));
	ImGui::SetNextWindowSize(ImVec2(660,380));
	bool is_open = NULL; // ensure we cant close the window
	ImGui::Begin( "Display", &is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
	
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const ImVec2 pos = ImGui::GetCursorScreenPos();

	uint16_t op = chip_8->cycle(draw_list);

	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(780,0));
	ImGui::SetNextWindowSize(ImVec2(200,200));
	ImGui::Begin( "Gamepad" );
	ImGui::Text( "(%.1f FPS)", ImGui::GetIO().Framerate );

	char* layout[] = {"1","2","3","C",
					 "4","5","6","D",
					 "7","8","9","E",
					 "A","0","B","F"
					};

	ImGui::BeginTable("table_padding", 4);
	for (int row = 0; row < 4; row++)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 4; column++)
		{
			ImGui::TableSetColumnIndex(column);

				if (ImGui::Button(layout[(row*4)+column])){
					chip_8->input(layout[(row*4)+column]);
				}
				
			
			if (ImGui::TableGetColumnFlags() & ImGuiTableColumnFlags_IsHovered)
			   ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
		}
	}
	ImGui::EndTable();
	ImGui::End();


	ImGui::SetNextWindowPos(ImVec2(0,380));
	ImGui::SetNextWindowSize(ImVec2(660,120));
	ImGui::Begin( "Registers" );
	uint8_t* registers = chip_8->get_registers();
	if (ImGui::BeginTable("table1", 4))
	{
		for (int row = 0; row < 4; row++)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 4; column++)
			{
				ImGui::TableSetColumnIndex(column);
				ImGui::Text("Register %d: %d", row + column*4, registers[row + column*4]);
				
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();


	ImGui::SetNextWindowPos(ImVec2(0,500));
	ImGui::SetNextWindowSize(ImVec2(150,200));
	ImGui::Begin( "Instruction" );
	ImGui::Text("Op:   %02x", op >> 12);
	ImGui::Text("X:    %02x", (op & 0x0f00) >> 8);
	ImGui::Text("Y:    %02x", (op & 0x00f0) >> 4);
	ImGui::Text("N:    %02x", (op & 0x000f));
	ImGui::Text("NN:   %02x", (op & 0x00ff));
	ImGui::Text("NNN:  %04x", (op & 0x0fff));
	ImGui::Text("PC:   %04x", chip_8->get_program_counter());
	ImGui::Text("IDX:  %04x", chip_8->get_index_register());
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(660,0));
	ImGui::SetNextWindowSize(ImVec2(100,350));
	ImGui::Begin( "Stack" );
	uint16_t* stack = chip_8->get_stack();
	uint8_t stack_pointer = chip_8->get_stack_pointer();


	for (int i = 0; i <= 16; i++)
	{
		if (i == stack_pointer){
			ImGui::Text("%04x<---", stack[i]);
		}
		else {
			ImGui::Text("%04x", stack[i]);
		}		
	}
	ImGui::End();
}