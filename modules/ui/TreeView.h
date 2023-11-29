#ifndef TREE_VIEW_H_
#define TREE_VIEW_H_

#include "ScrollContainer.h"
#include "Label.h"
#include "Theme.h"
#include "base/Properties.h"

namespace mgp
{

class TreeView : public ScrollContainer {
public:

    static UPtr<TreeView> create(const char* id, Style* style = NULL);

    const char* getTypeName() const;

    static Control* create(Style* style, Properties* properties = NULL);
protected:
    TreeView();
    ~TreeView();
    
    void initialize(const char* typeName, Style* style, Properties* properties);
};

}


#endif