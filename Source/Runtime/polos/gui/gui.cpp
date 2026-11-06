///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/gui/gui.hpp"

#include "polos/gui/backend.hpp"

#include <imgui.h>

#include <limits>

namespace polos::gui
{
namespace
{
auto ToImGui(Vec2 tValue) -> ImVec2
{
    return {tValue.x, tValue.y};
}

auto ToImGui(Color tValue) -> ImVec4
{
    return {tValue.r, tValue.g, tValue.b, tValue.a};
}

auto ToImGui(Condition tValue) -> ImGuiCond
{
    switch (tValue)
    {
        case Condition::kAlways: return ImGuiCond_Always;
        case Condition::kOnce: return ImGuiCond_Once;
        case Condition::kFirstUse: return ImGuiCond_FirstUseEver;
        case Condition::kAppearing: return ImGuiCond_Appearing;
    }
    return ImGuiCond_None;
}

auto ToImGui(StyleColor tValue) -> ImGuiCol
{
    switch (tValue)
    {
        case StyleColor::kText: return ImGuiCol_Text;
        case StyleColor::kWindowBackground: return ImGuiCol_WindowBg;
        case StyleColor::kBorder: return ImGuiCol_Border;
        case StyleColor::kFrameBackground: return ImGuiCol_FrameBg;
        case StyleColor::kButton: return ImGuiCol_Button;
        case StyleColor::kButtonHovered: return ImGuiCol_ButtonHovered;
        case StyleColor::kButtonActive: return ImGuiCol_ButtonActive;
        case StyleColor::kHeader: return ImGuiCol_Header;
    }
    return ImGuiCol_Text;
}

template<typename E>
auto HasFlag(E tFlags, E tFlag) -> bool
{
    return (static_cast<std::uint32_t>(tFlags) & static_cast<std::uint32_t>(tFlag)) != 0U;
}

auto TextData(std::string_view tText) -> char const*
{
    return tText.empty() ? "" : tText.data();
}

auto ToImGui(WindowFlags tFlags) -> ImGuiWindowFlags
{
    ImGuiWindowFlags result{0};
    if (HasFlag(tFlags, WindowFlags::kNoTitleBar))
    {
        result |= ImGuiWindowFlags_NoTitleBar;
    }
    if (HasFlag(tFlags, WindowFlags::kNoResize))
    {
        result |= ImGuiWindowFlags_NoResize;
    }
    if (HasFlag(tFlags, WindowFlags::kNoMove))
    {
        result |= ImGuiWindowFlags_NoMove;
    }
    if (HasFlag(tFlags, WindowFlags::kNoScrollbar))
    {
        result |= ImGuiWindowFlags_NoScrollbar;
    }
    if (HasFlag(tFlags, WindowFlags::kNoCollapse))
    {
        result |= ImGuiWindowFlags_NoCollapse;
    }
    if (HasFlag(tFlags, WindowFlags::kAutoResize))
    {
        result |= ImGuiWindowFlags_AlwaysAutoResize;
    }
    if (HasFlag(tFlags, WindowFlags::kNoBackground))
    {
        result |= ImGuiWindowFlags_NoBackground;
    }
    if (HasFlag(tFlags, WindowFlags::kMenuBar))
    {
        result |= ImGuiWindowFlags_MenuBar;
    }
    if (HasFlag(tFlags, WindowFlags::kNoInputs))
    {
        result |= ImGuiWindowFlags_NoInputs;
    }
    if (HasFlag(tFlags, WindowFlags::kNoDocking))
    {
        result |= ImGuiWindowFlags_NoDocking;
    }
    return result;
}

auto ToImGui(TableFlags tFlags) -> ImGuiTableFlags
{
    ImGuiTableFlags result{0};
    if (HasFlag(tFlags, TableFlags::kResizable))
    {
        result |= ImGuiTableFlags_Resizable;
    }
    if (HasFlag(tFlags, TableFlags::kReorderable))
    {
        result |= ImGuiTableFlags_Reorderable;
    }
    if (HasFlag(tFlags, TableFlags::kHideable))
    {
        result |= ImGuiTableFlags_Hideable;
    }
    if (HasFlag(tFlags, TableFlags::kStripedRows))
    {
        result |= ImGuiTableFlags_RowBg;
    }
    if (HasFlag(tFlags, TableFlags::kBorders))
    {
        result |= ImGuiTableFlags_Borders;
    }
    if (HasFlag(tFlags, TableFlags::kScrollY))
    {
        result |= ImGuiTableFlags_ScrollY;
    }
    return result;
}
}// namespace

auto operator|(WindowFlags tLeft, WindowFlags tRight) -> WindowFlags
{
    return static_cast<WindowFlags>(static_cast<std::uint32_t>(tLeft) | static_cast<std::uint32_t>(tRight));
}

auto operator|(TableFlags tLeft, TableFlags tRight) -> TableFlags
{
    return static_cast<TableFlags>(static_cast<std::uint32_t>(tLeft) | static_cast<std::uint32_t>(tRight));
}

auto Begin(char const* tTitle, bool* tOpen, WindowFlags tFlags) -> bool
{
    return ImGui::Begin(tTitle, tOpen, ToImGui(tFlags));
}

auto End() -> void
{
    ImGui::End();
}

auto BeginChild(char const* tId, Vec2 tSize, bool tBorder, WindowFlags tFlags) -> bool
{
    return ImGui::BeginChild(
        tId,
        ToImGui(tSize),
        tBorder ? ImGuiChildFlags_Borders : ImGuiChildFlags_None,
        ToImGui(tFlags));
}

auto EndChild() -> void
{
    ImGui::EndChild();
}

auto SetNextWindowPosition(Vec2 tPosition, Condition tCondition, Vec2 tPivot) -> void
{
    ImGui::SetNextWindowPos(ToImGui(tPosition), ToImGui(tCondition), ToImGui(tPivot));
}

auto SetNextWindowSize(Vec2 tSize, Condition tCondition) -> void
{
    ImGui::SetNextWindowSize(ToImGui(tSize), ToImGui(tCondition));
}

auto SetNextWindowFocus() -> void
{
    ImGui::SetNextWindowFocus();
}

auto SetNextWindowBackgroundAlpha(float tAlpha) -> void
{
    ImGui::SetNextWindowBgAlpha(tAlpha);
}

auto IsWindowHovered() -> bool
{
    return ImGui::IsWindowHovered();
}

auto IsWindowFocused() -> bool
{
    return ImGui::IsWindowFocused();
}

auto GetWindowSize() -> Vec2
{
    auto const size = ImGui::GetWindowSize();
    return {size.x, size.y};
}

auto DockSpaceOverMainViewport(bool tPassthroughBackground) -> void
{
    ImGui::DockSpaceOverViewport(
        0U,
        ImGui::GetMainViewport(),
        tPassthroughBackground ? ImGuiDockNodeFlags_PassthruCentralNode : ImGuiDockNodeFlags_None);
}

auto ShowDemoWindow(bool* tOpen) -> void
{
    ImGui::ShowDemoWindow(tOpen);
}

auto Separator() -> void
{
    ImGui::Separator();
}

auto Spacing() -> void
{
    ImGui::Spacing();
}

auto NewLine() -> void
{
    ImGui::NewLine();
}

auto BeginGroup() -> void
{
    ImGui::BeginGroup();
}

auto EndGroup() -> void
{
    ImGui::EndGroup();
}

auto PopID() -> void
{
    ImGui::PopID();
}

auto Unindent() -> void
{
    ImGui::Unindent();
}

auto SetItemDefaultFocus() -> void
{
    ImGui::SetItemDefaultFocus();
}

auto SameLine(float tOffset, float tSpacing) -> void
{
    ImGui::SameLine(tOffset, tSpacing);
}

auto Indent(float tWidth) -> void
{
    ImGui::Indent(tWidth);
}

auto Dummy(Vec2 tSize) -> void
{
    ImGui::Dummy(ToImGui(tSize));
}

auto SetNextItemWidth(float tWidth) -> void
{
    ImGui::SetNextItemWidth(tWidth);
}

auto PushID(char const* tId) -> void
{
    ImGui::PushID(tId);
}

auto PushID(int tId) -> void
{
    ImGui::PushID(tId);
}

auto GetContentRegionAvailable() -> Vec2
{
    auto const size = ImGui::GetContentRegionAvail();
    return {size.x, size.y};
}

auto GetCursorScreenPosition() -> Vec2
{
    auto const position = ImGui::GetCursorScreenPos();
    return {position.x, position.y};
}

auto SetCursorScreenPosition(Vec2 tPosition) -> void
{
    ImGui::SetCursorScreenPos(ToImGui(tPosition));
}

auto Text(std::string_view tText) -> void
{
    auto const* start = TextData(tText);
    ImGui::TextUnformatted(start, start + tText.size());
}

auto TextColored(Color tColor, std::string_view tText) -> void
{
    ImGui::PushStyleColor(ImGuiCol_Text, ToImGui(tColor));
    Text(tText);
    ImGui::PopStyleColor();
}

auto TextWrapped(std::string_view tText) -> void
{
    ImGui::PushTextWrapPos(0.0F);
    Text(tText);
    ImGui::PopTextWrapPos();
}

auto SeparatorText(char const* tLabel) -> void
{
    ImGui::SeparatorText(tLabel);
}

auto Button(char const* tLabel, Vec2 tSize) -> bool
{
    return ImGui::Button(tLabel, ToImGui(tSize));
}

auto SmallButton(char const* tLabel) -> bool
{
    return ImGui::SmallButton(tLabel);
}

auto Checkbox(char const* tLabel, bool& tValue) -> bool
{
    return ImGui::Checkbox(tLabel, &tValue);
}

auto RadioButton(char const* tLabel, bool tActive) -> bool
{
    return ImGui::RadioButton(tLabel, tActive);
}

auto Selectable(char const* tLabel, bool tSelected) -> bool
{
    return ImGui::Selectable(tLabel, tSelected);
}

auto SliderFloat(char const* tLabel, float& tValue, float tMin, float tMax, char const* tFormat) -> bool
{
    return ImGui::SliderFloat(tLabel, &tValue, tMin, tMax, tFormat);
}

auto SliderInt(char const* tLabel, int& tValue, int tMin, int tMax) -> bool
{
    return ImGui::SliderInt(tLabel, &tValue, tMin, tMax);
}

auto DragFloat(char const* tLabel, float& tValue, float tSpeed, float tMin, float tMax) -> bool
{
    return ImGui::DragFloat(tLabel, &tValue, tSpeed, tMin, tMax);
}

auto DragInt(char const* tLabel, int& tValue, float tSpeed, int tMin, int tMax) -> bool
{
    return ImGui::DragInt(tLabel, &tValue, tSpeed, tMin, tMax);
}

auto InputInt(char const* tLabel, int& tValue) -> bool
{
    return ImGui::InputInt(tLabel, &tValue);
}

auto InputFloat(char const* tLabel, float& tValue) -> bool
{
    return ImGui::InputFloat(tLabel, &tValue);
}

auto InputText(char const* tLabel, std::span<char> tBuffer) -> bool
{
    return !tBuffer.empty() && ImGui::InputText(tLabel, tBuffer.data(), tBuffer.size());
}

auto InputTextMultiline(char const* tLabel, std::span<char> tBuffer, Vec2 tSize) -> bool
{
    return !tBuffer.empty() && ImGui::InputTextMultiline(tLabel, tBuffer.data(), tBuffer.size(), ToImGui(tSize));
}

auto ColorEdit(char const* tLabel, Color& tColor) -> bool
{
    float channels[]{tColor.r, tColor.g, tColor.b, tColor.a};
    if (!ImGui::ColorEdit4(tLabel, channels))
    {
        return false;
    }
    tColor = {channels[0], channels[1], channels[2], channels[3]};
    return true;
}

auto ProgressBar(float tFraction, Vec2 tSize, char const* tOverlay) -> void
{
    ImGui::ProgressBar(tFraction, ToImGui(tSize), tOverlay);
}

auto PlotLines(char const* tLabel, std::span<float const> tValues, Vec2 tSize) -> void
{
    if (!tValues.empty() && tValues.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        ImGui::PlotLines(
            tLabel,
            tValues.data(),
            static_cast<int>(tValues.size()),
            0,
            nullptr,
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            ToImGui(tSize));
    }
}

auto Image(utils::string_id tTexture, Vec2 tSize, Vec2 tUvMin, Vec2 tUvMax) -> bool
{
    auto const texture = backend::GetImageTexture(tTexture);
    if (texture == 0U)
    {
        return false;
    }

    ImGui::Image(static_cast<ImTextureID>(texture), ToImGui(tSize), ToImGui(tUvMin), ToImGui(tUvMax));
    return true;
}

auto BeginCombo(char const* tLabel, char const* tPreview) -> bool
{
    return ImGui::BeginCombo(tLabel, tPreview);
}

auto EndCombo() -> void
{
    ImGui::EndCombo();
}

auto BeginListBox(char const* tLabel, Vec2 tSize) -> bool
{
    return ImGui::BeginListBox(tLabel, ToImGui(tSize));
}

auto EndListBox() -> void
{
    ImGui::EndListBox();
}

auto BeginTabBar(char const* tId) -> bool
{
    return ImGui::BeginTabBar(tId);
}

auto EndTabBar() -> void
{
    ImGui::EndTabBar();
}

auto BeginTabItem(char const* tLabel, bool* tOpen) -> bool
{
    return ImGui::BeginTabItem(tLabel, tOpen);
}

auto EndTabItem() -> void
{
    ImGui::EndTabItem();
}

auto TreeNode(char const* tLabel) -> bool
{
    return ImGui::TreeNode(tLabel);
}

auto TreePop() -> void
{
    ImGui::TreePop();
}

auto CollapsingHeader(char const* tLabel) -> bool
{
    return ImGui::CollapsingHeader(tLabel);
}

auto SetNextItemOpen(bool tOpen, Condition tCondition) -> void
{
    ImGui::SetNextItemOpen(tOpen, ToImGui(tCondition));
}

auto BeginTable(char const* tId, int tColumns, TableFlags tFlags, Vec2 tOuterSize) -> bool
{
    return ImGui::BeginTable(tId, tColumns, ToImGui(tFlags), ToImGui(tOuterSize));
}

auto EndTable() -> void
{
    ImGui::EndTable();
}

auto TableSetupColumn(char const* tLabel) -> void
{
    ImGui::TableSetupColumn(tLabel);
}

auto TableHeadersRow() -> void
{
    ImGui::TableHeadersRow();
}

auto TableNextRow() -> void
{
    ImGui::TableNextRow();
}

auto TableNextColumn() -> bool
{
    return ImGui::TableNextColumn();
}

auto TableSetColumnIndex(int tIndex) -> bool
{
    return ImGui::TableSetColumnIndex(tIndex);
}

auto BeginMainMenuBar() -> bool
{
    return ImGui::BeginMainMenuBar();
}

auto EndMainMenuBar() -> void
{
    ImGui::EndMainMenuBar();
}

auto BeginMenuBar() -> bool
{
    return ImGui::BeginMenuBar();
}

auto EndMenuBar() -> void
{
    ImGui::EndMenuBar();
}

auto BeginTooltip() -> bool
{
    return ImGui::BeginTooltip();
}

auto EndTooltip() -> void
{
    ImGui::EndTooltip();
}

auto BeginMenu(char const* tLabel, bool tEnabled) -> bool
{
    return ImGui::BeginMenu(tLabel, tEnabled);
}

auto EndMenu() -> void
{
    ImGui::EndMenu();
}

auto MenuItem(char const* tLabel, char const* tShortcut, bool tSelected, bool tEnabled) -> bool
{
    return ImGui::MenuItem(tLabel, tShortcut, tSelected, tEnabled);
}

auto OpenPopup(char const* tId) -> void
{
    ImGui::OpenPopup(tId);
}

auto BeginPopup(char const* tId, WindowFlags tFlags) -> bool
{
    return ImGui::BeginPopup(tId, ToImGui(tFlags));
}

auto BeginPopupModal(char const* tTitle, bool* tOpen, WindowFlags tFlags) -> bool
{
    return ImGui::BeginPopupModal(tTitle, tOpen, ToImGui(tFlags));
}

auto EndPopup() -> void
{
    ImGui::EndPopup();
}

auto CloseCurrentPopup() -> void
{
    ImGui::CloseCurrentPopup();
}

auto IsItemHovered() -> bool
{
    return ImGui::IsItemHovered();
}

auto IsItemActive() -> bool
{
    return ImGui::IsItemActive();
}

auto IsItemEdited() -> bool
{
    return ImGui::IsItemEdited();
}

auto IsItemDeactivatedAfterEdit() -> bool
{
    return ImGui::IsItemDeactivatedAfterEdit();
}

auto IsItemClicked() -> bool
{
    return ImGui::IsItemClicked();
}

auto BeginDisabled(bool tDisabled) -> void
{
    ImGui::BeginDisabled(tDisabled);
}

auto EndDisabled() -> void
{
    ImGui::EndDisabled();
}

auto PushStyleColor(StyleColor tTarget, Color tColor) -> void
{
    ImGui::PushStyleColor(ToImGui(tTarget), ToImGui(tColor));
}

auto PopStyleColor(int tCount) -> void
{
    ImGui::PopStyleColor(tCount);
}

auto SetDarkTheme() -> void
{
    ImGui::StyleColorsDark();
}

auto SetLightTheme() -> void
{
    ImGui::StyleColorsLight();
}

auto WantsMouse() -> bool
{
    return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
}

auto WantsKeyboard() -> bool
{
    return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard;
}

auto WantsTextInput() -> bool
{
    return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantTextInput;
}

}// namespace polos::gui
