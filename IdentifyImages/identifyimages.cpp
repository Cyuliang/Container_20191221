#include "identifyimages.h"

IdentifyImages::IdentifyImages(QObject *parent)
{
    this->setParent(parent);
}

IdentifyImages::~IdentifyImages()
{

}

void IdentifyImages::pictureStreamSlot(const QByteArray &jpgStream, const int &imgNumber)
{

}