#include "MenuList.h"
#include "Form.h"
#include "Button.h"
#include "ModalLayer.h"

using namespace mgp;

MenuList::MenuList()
{
    setPadding(8, 8, 8, 8);
    setLayout(Layout::LAYOUT_VERTICAL);
    setScroll(SCROLL_VERTICAL);
    _className = "MenuList";
    setHeight(0.8, true);
}

MenuList::~MenuList()
{

}

void MenuList::onSerialize(Serializer* serializer) {
    ScrollContainer::onSerialize(serializer);
}

void MenuList::onDeserialize(Serializer* serializer) {
    ScrollContainer::onDeserialize(serializer);
}

void MenuList::controlEvent(Control* control, EventType evt) {
    if (evt == Listener::CLICK) {
        int index = -1;
        for (int i = 0; i < getControlCount(); ++i) {
            Control* c = getControl(i);
            if (control == c) {
                index = i;
                break;
            }
        }

        _selectIndex = index;
        if (index != -1) {
            notifyListeners(Listener::SELECT_CHANGE);
        }

        close();
    }
}

void MenuList::initItems(std::vector<std::string>& items) {
    for (std::string& name : items) {
        UPtr<Button> label = Control::create<Button>((this->_id + "_items").c_str());
        label->setPadding(5, 5, 5, 5);
        label->setStyleName("MenuItem");
        label->setText(name.c_str());
        label->setWidth(1, true);
        label->addListener(this, Listener::CLICK);
        this->addControl(std::move(label));
    }
}

void MenuList::show(Control* any) {
    GP_ASSERT(any);
    any->getTopLevelForm()->getOverlay()->push(this, false);
}

void MenuList::close() {
    this->getTopLevelForm()->getOverlay()->pop();
}