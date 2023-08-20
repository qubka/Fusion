#pragma once

#include "fusion/assets/asset_registry.h"
#include "fusion/core/time.h"
#include "fusion/debug/debug_renderer.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/graphics/graphics.h"
#include "fusion/input/input.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/scripting/script_engine.h"

namespace fe {
    struct ModuleBase {
        /**
         * @brief Represents the stage where the module will be updated in the engine.
         */
        enum class Stage : unsigned char { Never, Pre, /*Main,*/ Post, Render };
    };

    /**
     * @brief A interface used for defining engine modules.
     */
    template<typename ModuleImpl>
    class Module final : public ModuleBase {
        friend class ModuleHolder;
    private:
        Module() = default;
        ~Module() = default;
        NONCOPYABLE(Module);

        /**
         * The start function for the module.
         */
        void onStart() { moduleImpl.onStart(); };

        /**
         * The update function for the module.
         */
        void onUpdate() { moduleImpl.onUpdate(); };

        /**
         * The stop function for the module.
         */
        void onStop() { moduleImpl.onStop(); };

    private:
        ModuleImpl moduleImpl;
        bool started{ false };
    };

    class ModuleHolder {
        //friend class Engine;
    public:
        ModuleHolder() = default;
        ~ModuleHolder() = default;
        NONCOPYABLE(ModuleHolder);

#define ALL_MODULES Time, Input, AssetRegistry, DebugRenderer, FileSystem, Graphics, SceneManager, ScriptEngine

#define PRE_MODULES Time
#define POST_MODULES Input, SceneManager, AssetRegistry
#define RENDER_MODULES Graphics, DebugRenderer

    public:
        void startModules() {
            startModules(ALL_MODULES);
        }

        template<ModuleBase::Stage S>
        void updateModules() {
            if constexpr (S == ModuleBase::Stage::Pre)
                updateModules(PRE_MODULES);
            else if constexpr (S == ModuleBase::Stage::Post)
                updateModules(POST_MODULES);
            else if constexpr (S == ModuleBase::Stage::Render)
                updateModules(RENDER_MODULES);
        }

        void stopModules() {
            stopModules(ALL_MODULES);
        }

    private:
        template<typename... Args>
        void startModules(Args&&...modules) {
            ([&]() {
                modules.onStart();
                modules.started = true;
            }(), ...);
        }

        template<typename... Args>
        void updateModules(Args&&...modules) {
            ([&]() {
                modules.onUpdate();
            }(), ...);
        }

        template<typename... Args>
        void stopModules(Args&&...modules) {
            ([&]() {
                modules.started = false;
                modules.onStop();
            }(), ...);
        }

    private:
        Module<Time> Time;
        Module<Input> Input;
        Module<AssetRegistry> AssetRegistry;
        Module<DebugRenderer> DebugRenderer;
        Module<FileSystem> FileSystem;
        Module<Graphics> Graphics;
        Module<SceneManager> SceneManager;
#if FUSION_SCRIPTING
        Module<ScriptEngine> ScriptEngine;
#endif
    };
}