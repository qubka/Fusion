#include "console_panel.hpp"

using namespace fe;
using namespace std::string_literals;

bitmask::bitmask<MessageLevel> ConsolePanel::MessageBufferRenderFilter = MessageLevel::None;
int32_t ConsolePanel::MessageID = 0;
int16_t ConsolePanel::MessageBufferCapacity = 200;
int16_t ConsolePanel::MessageBufferSize = 0;
int16_t ConsolePanel::MessageBufferBegin = 0;
std::vector<std::unique_ptr<ConsolePanel::Message>> ConsolePanel::MessageBuffer = std::vector<std::unique_ptr<ConsolePanel::Message>>(200);
bool ConsolePanel::AllowScrollingToBottom = true;
bool ConsolePanel::RequestScrollToBottom = false;

ConsolePanel::ConsolePanel() : EditorPanel{ICON_MDI_VIEW_LIST " Console##console", "Console"} {

}

ConsolePanel::~ConsolePanel() {

}

void ConsolePanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowSize(ImVec2{ 640, 480 }, ImGuiCond_FirstUseEver);
    ImGui::Begin(name.c_str(), &enabled, flags);
    {
        renderHeader();
        ImGui::Separator();
        renderMessages();
    }
    ImGui::End();
}

void ConsolePanel::AddMessage(std::unique_ptr<Message>&& message) {
    if (message->level == MessageLevel::None)
        return;

    auto messageStart = MessageBuffer.begin() + MessageBufferBegin;
    if (*messageStart) { // If contains old message here
        for (auto it = messageStart; it != MessageBuffer.end(); it++) {
            if (message->getMessageID() == (*it)->getMessageID()) {
                (*it)->increaseCount();
                return;
            }
        }
    }

    if (MessageBufferBegin != 0) { // Skipped first messages in vector
        for (auto it = MessageBuffer.begin(); it != messageStart; it++) {
            if (*it) {
                if (message->getMessageID() == (*it)->getMessageID()) {
                    (*it)->increaseCount();
                    return;
                }
            }
        }
    }

    *(MessageBuffer.begin() + MessageBufferBegin) = std::move(message);
    if (++MessageBufferBegin == MessageBufferCapacity)
        MessageBufferBegin = 0;

    if (MessageBufferSize < MessageBufferCapacity)
        MessageBufferSize++;

    if (AllowScrollingToBottom)
        RequestScrollToBottom = true;
}

void ConsolePanel::Flush() {
    for (auto& message : MessageBuffer)
        message = nullptr;
    MessageBufferBegin = 0;
}

void ConsolePanel::renderHeader() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    // Button for advanced settings
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f});
        if (ImGui::Button(ICON_MDI_COGS))
            ImGui::OpenPopup("SettingsPopup");
        ImGui::PopStyleColor();
    }

    if (ImGui::BeginPopup("SettingsPopup")) {
        // Checkbox for scrolling lock
        ImGui::Checkbox("Scroll to bottom", &AllowScrollingToBottom);

        // Button to clear the console
        if (ImGui::Button("Clear console"))
            Flush();

        ImGui::EndPopup();
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
    ImGui::SameLine();

    float spacing = style.ItemSpacing.x;
    style.ItemSpacing.x = 2;
    float levelButtonWidth = (ImGui::CalcTextSize(Message::GetLevelIcon(MessageLevel(1))) + style.FramePadding * 2.0f).x;
    float levelButtonWidths = (levelButtonWidth + style.ItemSpacing.x) * 6;

    {
        ImGui::PushFont(io.Fonts->Fonts[1]);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
        Filter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x - (levelButtonWidths));
        ImGuiUtils::DrawItemActivityOutline(2.0f, false);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    ImGui::SameLine(); // ImGui::GetWindowWidth() - levelButtonWidths);

    for (int i = 0; i < 6; i++) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::SameLine();
        auto level = MessageLevel(std::pow(2, i));

        if (MessageBufferRenderFilter & level)
            ImGui::PushStyleColor(ImGuiCol_Text, Message::GetRenderColor(level));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.5f, 0.5, 0.5f, 0.5f});

        if (ImGui::Button(Message::GetLevelIcon(level))) {
            MessageBufferRenderFilter ^= level;
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", Message::GetLevelName(level));
        }
        ImGui::PopStyleColor(2);
    }

    style.ItemSpacing.x = spacing;

    if (!Filter.IsActive()) {
        ImGui::SameLine();
        ImGui::PushFont(io.Fonts->Fonts[1]);
        ImGui::SetCursorPosX(ImGui::GetFontSize() * 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, style.FramePadding.y});
        ImGui::TextUnformatted("Search...");
        ImGui::PopStyleVar();
        ImGui::PopFont();
    }
}

void ConsolePanel::renderMessages() {
    ImGui::BeginChild("ScrollRegion", ImVec2{0, 0}, false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        // ImGuiUtils::AlternatingRowsBackground();

        auto messageStart = MessageBuffer.begin() + MessageBufferBegin;
        if (*messageStart) {// If contains old message here
            for (auto it = messageStart; it != MessageBuffer.end(); it++) {
                if (Filter.IsActive()) {
                    if (Filter.PassFilter((*it)->message.c_str())) {
                        (*it)->onImGui();
                    }
                } else {
                    (*it)->onImGui();
                }
            }
        }

        if (MessageBufferBegin != 0) { // Skipped first messages in vector
            for (auto it = MessageBuffer.begin(); it != messageStart; it++) {
                if (*it) {
                    if (Filter.IsActive()) {
                        if (Filter.PassFilter((*it)->message.c_str())) {
                            (*it)->onImGui();
                        }
                    } else {
                        (*it)->onImGui();
                    }
                }
            }
        }

        if (RequestScrollToBottom && ImGui::GetScrollMaxY() > 0) {
            ImGui::SetScrollHereY(1.0f);
            RequestScrollToBottom = false;
        }
    }
    ImGui::EndChild();
}

ConsolePanel::Message::Message(std::string message, MessageLevel level, std::string source)
    : message{std::move(message)}
    , level{level}
    , source{std::move(source)}
    , messageID{MessageID++} {
}

void ConsolePanel::Message::onImGui() {
    if (MessageBufferRenderFilter & level) {
        ImGui::PushID(messageID);
        ImGui::PushStyleColor(ImGuiCol_Text, GetRenderColor(level));
        ImGui::TextUnformatted(GetLevelIcon(level));
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(message.c_str());
        if (ImGui::BeginPopupContextItem(message.c_str())) {
            if (ImGui::MenuItem("Copy")) {
                ImGui::SetClipboardText(message.c_str());
            }

            ImGui::EndPopup();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", source.c_str());
        }

        if (count > 1) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (count > 99 ? ImGui::GetFontSize() * 1.7f : ImGui::GetFontSize() * 1.5f));
            ImGui::Text("%d", count);
        }

        ImGui::PopID();
    }
}

const char* ConsolePanel::Message::GetLevelIcon(MessageLevel level) {
    switch (level) {
        case MessageLevel::Trace:
            return ICON_MDI_MESSAGE_TEXT;
        case MessageLevel::Info:
            return ICON_MDI_INFORMATION;
        case MessageLevel::Debug:
            return ICON_MDI_BUG;
        case MessageLevel::Warn:
            return ICON_MDI_ALERT;
        case MessageLevel::Error:
            return ICON_MDI_CLOSE_OCTAGON;
        case MessageLevel::Critical:
            return ICON_MDI_ALERT_OCTAGRAM;
        default:
            return "Unknown name";
    }
}

const char* ConsolePanel::Message::GetLevelName(MessageLevel level) {
    switch (level) {
        case MessageLevel::Trace:
            return ICON_MDI_MESSAGE_TEXT " Trace";
        case MessageLevel::Info:
            return ICON_MDI_INFORMATION " Info";
        case MessageLevel::Debug:
            return ICON_MDI_BUG " Debug";
        case MessageLevel::Warn:
            return ICON_MDI_ALERT " Warning";
        case MessageLevel::Error:
            return ICON_MDI_CLOSE_OCTAGON " Error";
        case MessageLevel::Critical:
            return ICON_MDI_ALERT_OCTAGRAM " Critical";
        default:
            return "Unknown name";
    }
}

glm::vec4 ConsolePanel::Message::GetRenderColor(MessageLevel level) {
    switch (level) {
        case MessageLevel::Trace:
            return { 0.75f, 0.75f, 0.75f, 1.00f }; // Gray
        case MessageLevel::Info:
            return { 0.40f, 0.70f, 1.00f, 1.00f }; // Blue
        case MessageLevel::Debug:
            return { 0.00f, 0.50f, 0.50f, 1.00f }; // Cyan
        case MessageLevel::Warn:
            return { 1.00f, 1.00f, 0.00f, 1.00f }; // Yellow
        case MessageLevel::Error:
            return { 1.00f, 0.25f, 0.25f, 1.00f }; // Red
        case MessageLevel::Critical:
            return { 0.6f, 0.2f, 0.8f, 1.00f }; // Purple
        default:
            return { 1.00f, 1.00f, 1.00f, 1.00f };
    }
}