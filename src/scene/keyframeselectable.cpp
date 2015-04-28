#include "keyframeselectable.h"

KeyframeSelectable::KeyframeSelectable(int keyID)
{
    this->keyID = keyID;
}

KeyframeSelectable::KeyframeSelectable(int keyID, QString name)
{
    this->keyID = keyID;
    this->setText(name);
}

KeyframeSelectable::~KeyframeSelectable()
{

}

