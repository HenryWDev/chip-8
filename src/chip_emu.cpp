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




void render_loop( GLFWwindow* window, chip8* chip_8 ){
	// setup game window
	ImGui::SetNextWindowPos(ImVec2(0,0));
	ImGui::SetNextWindowSize(ImVec2(660,380));
	bool is_open = NULL; // ensure we cant close the window
	ImGui::Begin( "Display", &is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
	ImGui::Text( "(%.1f FPS)", ImGui::GetIO().Framerate );

    int pixel_size = 10;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    int display[64][32];
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			display[i][j] = 255;
		}
	}
	display[15][15] = 0;
	uint16_t op = chip_8->cycle(draw_list);

	draw_list->AddRectFilled( ImVec2( (pos.x) + (1 * pixel_size), (pos.y) +  (1 * pixel_size) ),
										  ImVec2( (pos.x) + (1 * pixel_size) + pixel_size, (pos.y) +  (1 * pixel_size)  + pixel_size), 
										  ImColor(255,255,255));

	ImGui::End();

	// ImGui::SetNextWindowPos(ImVec2(660,0));
	// ImGui::SetNextWindowSize(ImVec2(200,200));
	// ImGui::Begin( "Gamepad" );
	// ImGui::BeginTable("table_padding", 3);
	// for (int row = 0; row < 3; row++)
	// {
	// 	ImGui::TableNextRow();
	// 	for (int column = 0; column < 3; column++)
	// 	{
	// 		ImGui::TableSetColumnIndex(column);

	// 			char buf[32];
	// 			sprintf(buf, "A");
	// 			ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
			
	// 		//if (ImGui::TableGetColumnFlags() & ImGuiTableColumnFlags_IsHovered)
	// 		//    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
	// 	}
	// }
	// ImGui::EndTable();
	// ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0,380));
	ImGui::SetNextWindowSize(ImVec2(660,250));
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
	ImGui::Text("Op:   %02x", op >> 12);
	ImGui::Text("X:    %02x", (op & 0x0f00) >> 8);
	ImGui::Text("Y:    %02x", (op & 0x00f0) >> 4);
	ImGui::Text("N:    %02x", (op & 0x000f));
	ImGui::Text("NN:   %02x", (op & 0x00ff));
	ImGui::Text("NNN:  %04x", (op & 0x0fff));
	ImGui::Text("PC:   %04x", chip_8->get_program_counter());
	ImGui::Text("IDX:  %04x", chip_8->get_index_register());


	ImGui::End();
        
}