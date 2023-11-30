﻿
#include <iostream>
#include "mgp.h"

using namespace mgp;

class MainApp : public Game, Control::Listener {
    Label* _label = NULL;
    //Button* button;

    void initialize() {
        UPtr<Theme> theme = Theme::create("res/ui/default.theme");
        Theme::setDefault(theme.get());
        
        UPtr<Form> form = Form::create();
        form->getContent()->setSize(600, 600);
        form->getContent()->setPadding(20, 20, 20, 20);
        form->getContent()->setLayout(Layout::LAYOUT_VERTICAL);
#if 1
        UPtr<Label> label = Label::create("testLabel");
        //label->setPosition(50, 50);
        //label->setSize(200, 50);
        label->setText("Label");
        _label = label.get();
        _label->addRef();
        form->getContent()->addControl(std::move(label));

        UPtr<Button> button = Button::create("testButton");
        //button->setPosition(45, 100);
        //button->setSize(200, 100);
        button->setText("Button");
        button->addListener(this, Control::Listener::CLICK);
        form->getContent()->addControl(std::move(button));


        UPtr<CheckBox> checkbox = CheckBox::create("checkbox");
        //checkbox->setPosition(45, 200);
        checkbox->setText("CheckBox");
        form->getContent()->addControl(std::move(checkbox));
        //SAFE_RELEASE(checkbox);

        UPtr<RadioButton> radio = RadioButton::create("radio");
        radio->setGroupId("radioGroup");
        radio->setText("RadioButton");
        form->getContent()->addControl(std::move(radio));
        //SAFE_RELEASE(radio);

        UPtr<RadioButton> radio2 = RadioButton::create("radio");
        radio2->setGroupId("radioGroup");
        radio2->setText("RadioButton");
        form->getContent()->addControl(std::move(radio2));
        //SAFE_RELEASE(radio2);

        UPtr<Slider> slider = Slider::create("slider");
        slider->setText("Slider");
        slider->setWidth(1.0, true);
        form->getContent()->addControl(std::move(slider));
        //SAFE_RELEASE(slider);

        UPtr<TextBox> text = TextBox::create("text");
        text->setText("input");
        text->setWidth(300);
        form->getContent()->addControl(std::move(text));
        //SAFE_RELEASE(text);


        //test scroll
        UPtr<ScrollContainer> container = ScrollContainer::create("container");
        container->setSize(200, 100);
        container->setScroll(ScrollContainer::SCROLL_BOTH);
        UPtr<Button> button2 = Button::create("testButton2");
        button2->setText("test\nButton");
        button2->setSize(200, 100);
        container->addControl(std::move(button2));
        form->getContent()->addControl(std::move(container));
        //SAFE_RELEASE(button2);
        //SAFE_RELEASE(container);


        UPtr<ImageControl> image = ImageControl::create("image");
        image->setImage("res/image/logo.png");
        image->setSize(50, 50);
        form->getContent()->addControl(std::move(image));
        //SAFE_RELEASE(image);


        UPtr<JoystickControl> joystick = JoystickControl::create("joystick");
        joystick->setSize(100, 100);
        form->getContent()->addControl(std::move(joystick));
        //SAFE_RELEASE(joystick);
#endif
        UPtr<ComboBox> combobox = ComboBox::create("combobox");
        combobox->setWidth(100);
        combobox->setText("Combobox");
        combobox->getItems() = { "Apple", "Google" };
        form->getContent()->addControl(std::move(combobox));

        //UPtr<MenuList> menuList = MenuList::create("MenuList");
        //menuList->setSize(100, 100);
        //form->getContent()->addControl(std::move(menuList));
        
#if 0
        UPtr<Font> font = Font::create("res/ui/sans.ttf");
        set button style
        button->overrideStyle();
        button->getStyle()->setFont(font);
        
        ThemeImage* image = theme->getImage("button");
        BorderImage* border = new BorderImage(image->getRegion(), Border(20,20,20,20));
        button->getStyle()->setBgImage(border);
        border->release();
        button->getStyle()->setBgColor(Vector4(1.0, 0, 0, 1.0), Style::OVERLAY_HOVER);

        font->release();
#endif

        getFormManager()->add(std::move(form));
        //SAFE_RELEASE(theme);
    }

    void render(float elapsedTime) override {
        Renderer::cur()->clear(Renderer::CLEAR_COLOR_DEPTH_STENCIL);
        Game::render(elapsedTime);
    }

    void finalize() {
        SAFE_RELEASE(_label);
        //SAFE_RELEASE(button);
    }

    void controlEvent(Control* control, EventType evt)
    {
        if (evt == CLICK)
        {
            if (strcmp("testButton", control->getId()) == 0)
            {
                if (_label) _label->setText("clicked");
            }
        }
    }

};

int main() {
    printf("main start\n");
    
    #if __EMSCRIPTEN__
        MainApp* instance = new MainApp();
    #else
        MainApp instance;
    #endif
    return Platform::run();
}