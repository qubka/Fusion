#pragma once

#include "editor_panel.hpp"

namespace fe {
    enum class MessageLevel : uint8_t {
        None = 0,
        Trace = 1,
        Debug = 2,
        Info = 4,
        Warn = 8,
        Error = 16,
        Critical = 32,
    };
    BITMASK_DEFINE_MAX_ELEMENT(MessageLevel, Critical);

    class ConsolePanel : public EditorPanel {
    public:
        class Message {
        public:
            explicit Message(std::string message = "", MessageLevel level = MessageLevel::Trace, std::string source = "");

            void onImGui();

            void increaseCount() { count++; }
            int32_t getMessageID() const { return messageID; }

            static const char* GetLevelName(MessageLevel level);
            static const char* GetLevelIcon(MessageLevel level);
            static glm::vec4 GetRenderColor(MessageLevel level);

        public:
            const std::string message;
            const MessageLevel level;
            const std::string source;
            uint32_t count{ 1 };
            int32_t messageID;
        };

    public:
        ConsolePanel();
        ~ConsolePanel() override;

        void onImGui() override;

        static void Flush();

        static void AddMessage(std::unique_ptr<Message>&& message);

    private:
        void renderHeader();
        void renderMessages();

    private:
        static int32_t MessageID;
        static int16_t MessageBufferCapacity;
        static int16_t MessageBufferSize;
        static int16_t MessageBufferBegin;
        static std::vector<std::unique_ptr<Message>> MessageBuffer;
        static bool AllowScrollingToBottom;
        static bool RequestScrollToBottom;
        static bitmask::bitmask<MessageLevel> MessageBufferRenderFilter;
        ImGuiTextFilter Filter;
    };
}
