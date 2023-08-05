#pragma once

namespace fe {
    /**
     * @brief Class used to describe the gamepad input state.
     */
    struct GamepadState {
        /// The states of each [gamepad button](@ref gamepad_buttons), `Action::Press` or `Action::Release`.
        InputAction buttons[15];
        /// The states of each [gamepad axis](@ref gamepad_axes), in the range -1.0 to 1.0 inclusive.
        float axes[6];
    };

    /**
     * @brief Class used to represent a joystick.
     */
    class FUSION_API Joystick {
        friend class DeviceManager;
    public:
        Joystick() = default;
        virtual ~Joystick() = default;
        NONCOPYABLE(Joystick);

        /**
         * Gets the port of the joystick.
         * @return The joysticks port.
         */
        virtual uint8_t getPort() const = 0;

        /**
         * Gets if the joystick is connected.
         * @return If the joystick is connected.
         */
        virtual bool isConnected() const = 0;

        /**
         * Gets if the joystick is gamepad.
         * @return If the joystick is gamepad.
         */
        virtual bool isGamePad() const = 0;

        /**
         * Gets the name of the joystick.
         * @return The joysticks name.
         */
        virtual const std::string& getName() const = 0;

        /**
         * Gets the number of axes the joystick contains.
         * @return The number of axes the joystick contains.
         */
        virtual size_t getAxisCount() const = 0;

        /**
         * Gets the number of buttons the joystick contains.
         * @return The number of buttons the joystick contains.
         */
        virtual size_t getButtonCount() const = 0;

        /**
         * Gets the number of hats the joystick contains.
         * @return The number of hats the joystick contains.
         */
        virtual size_t getHatCount() const = 0;

        /**
         * Gets the value of a joysticks axis.
         * @param axis The axis id to get the value from.
         * @return The value of the joystick's axis.
         */
        virtual float getAxis(uint8_t axis) const = 0;

        /**
         * Gets the whether a button on a joystick is pressed.
         * @param button The button id to get the value from.
         * @return Whether a button on a joystick is pressed.
         */
        virtual InputAction getButton(uint8_t button) = 0;

        /**
         * Gets the value of a joysticks hat.
         * @param hat The hat id to get the value from.
         * @return The value of the joystick's hat.
         */
        virtual bitmask::bitmask<JoystickHat> getHat(uint8_t hat) const = 0;

        /**
         * This function retrieves the state of the specified joystick remapped to an Xbox-like gamepad.
         * @return The gamepad input state of the joystick.
         */
        virtual const GamepadState& getGamepadState() const = 0;

        /**
         * Gets the current joystick object.
         * @return The object.
         */
        virtual void* getNativeJoystick() const = 0;

    protected:
        /**
         * Run every frame as long as the joystick has work to do.
         */
        virtual void onUpdate() = 0;

    public:
        /**
         * Event when a joystick buttons changes state.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnButton() { return entt::sink{onButton}; }

        /**
         * Event when a joystick axis moves.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnAxis() { return entt::sink{onAxis}; }

        /**
         * Event when a joystick had changes state.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnHat() { return entt::sink{onHat}; }

    protected:
        entt::sigh<void(uint8_t, InputAction)> onButton;
        entt::sigh<void(uint8_t, float)> onAxis;
        entt::sigh<void(uint8_t, bitmask::bitmask<JoystickHat>)> onHat;
    };
}
