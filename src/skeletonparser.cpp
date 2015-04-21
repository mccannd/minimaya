#include "skeletonparser.h"

SkeletonParser::SkeletonParser()
{

}

SkeletonParser::~SkeletonParser()
{

}

// return the root node of a fully connected skeleton of joints
// these joints are based on a json file
Joint* parseSkeleton(const QString &filePath)
{
    // Read JSON file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray rawData = file.readAll();

    // Parse document
    QJsonDocument doc(QJsonDocument::fromJson(rawData));

    // Get JSON object
    QJsonObject json = doc.object();

    // Access properties
    QJsonObject root =  json["root"].toObject();

    // Send into recursive tree constructor
    Joint* rj = makeChildren(root, NULL);

    return rj;
}

// recursively create children for a node
Joint* makeChildren(const QJsonObject &obj, Joint* parent)
{
    // extract json object information
    QString name = obj["name"].toString();
    QJsonArray pos = obj["pos"].toArray();
    QJsonArray rot = obj["rot"].toArray();

    Joint* j;
    if (parent == NULL) {
        // root node has no parent pointer
        j = new Joint((float) pos.at(0).toDouble(),
                      (float) pos.at(1).toDouble(),
                      (float) pos.at(2).toDouble());
        j->rotate((float) rot.at(0).toDouble(),
                  (float) rot.at(1).toDouble(),
                  (float) rot.at(2).toDouble(),
                  (float) rot.at(3).toDouble());
    } else {
        j = new Joint(parent,
                      (float) pos.at(0).toDouble(),
                      (float) pos.at(1).toDouble(),
                      (float) pos.at(2).toDouble(),
                      (float) rot.at(0).toDouble(),
                      glm::vec3((float) rot.at(1).toDouble(),
                                (float) rot.at(2).toDouble(),
                                (float) rot.at(3).toDouble()));
    }


    j->rename(name);

    // for each child, create a new, parented node
    QJsonArray children = obj["children"].toArray();
    foreach (const QJsonValue & value, children) {
        QJsonObject newobj = value.toObject();
        makeChildren(newobj, j);
    }

    return j;
}
