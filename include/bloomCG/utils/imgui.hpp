#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <bloomCG/core/common.hpp>

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

static void TextCentered(std::string text) {
  auto windowWidth = ImGui::GetWindowSize().x;
  auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

  // Bold the text if it is the currently selected item
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

  ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
  ImGui::Text("%s", text.c_str());

  ImGui::PopStyleColor(1);
}

static void selectableButton(std::string name, bool* selected) {
  // create a copy of the initial selected value
  bool initialSelected = *selected;
  if (*selected) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.00f, 0.00f, 1.00f));
  }
  if (ImGui::Button(name.c_str())) {
    *selected = !*selected;
  }

  if (initialSelected) ImGui::PopStyleColor();

  ImGui::SameLine();
}
