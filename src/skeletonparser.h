#pragma once
#include <QJsonObject>
#include "scene/joint.h"
#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>



class SkeletonParser
{
public:
    SkeletonParser();
    ~SkeletonParser();
};

Joint* parseSkeleton(const QString &filePath);
Joint* makeChildren(const QJsonObject &obj, Joint* parent);


