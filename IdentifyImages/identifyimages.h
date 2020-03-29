#ifndef IDENTIFYIMAGES_H
#define IDENTIFYIMAGES_H

#include "identifyimages_global.h"
#include "recognizerinterface.h"

class IDENTIFYIMAGESSHARED_EXPORT IdentifyImages:public RecognizerInterface
{
    Q_OBJECT
    Q_INTERFACES(RecognizerInterface)
    Q_PLUGIN_METADATA(IID RecognizerInterfaceIID)

public:
    IdentifyImages(QObject* parent=nullptr);
    ~IdentifyImages()Q_DECL_OVERRIDE;

public:

    ///
    /// \brief pictureStreamSlot 图片流
    /// \param jpgStream 图片流
    /// \param imgNumber 图片编号
    ///
    void pictureStreamSlot(const QByteArray &jpgStream,const int &imgNumber)Q_DECL_OVERRIDE;

};

#endif // IDENTIFYIMAGES_H
