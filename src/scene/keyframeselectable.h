#ifndef KEYFRAMESELECTABLE_H
#define KEYFRAMESELECTABLE_H
#include "QListWidgetItem"


class KeyframeSelectable : public QListWidgetItem
{
public:
    KeyframeSelectable(int keyID);
    KeyframeSelectable(int keyID, QString name);
    int keyID;
    ~KeyframeSelectable();
};

#endif // KEYFRAMESELECTABLE_H
