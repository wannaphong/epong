/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#include <QApplication>

#include "miniiapqtclient.h"
#include "drmfile.h"
#include <QDir>
#include <QDebug>

Q_DECLARE_METATYPE(IAPClient::ProductDataList)  //to be able to transfer data this type with Qt::QueuedConnection


MiniIAPQtClient::MiniIAPQtClient(QObject *parent) :
    QObject(parent),
    MiniIAPClientApi(),
    m_requestId(-1),
    m_productId(),
    m_IAPClient(0),
    isPurchaseRunning(false)
{
}

MiniIAPQtClient::~MiniIAPQtClient()
{
    if (m_IAPClient) {
        delete m_IAPClient;
        m_IAPClient = 0;
    }
}


bool MiniIAPQtClient::isProductPurchased(const char *productId, const char *fileName)
{
    QString path = QApplication::applicationDirPath();
    path.append(QString("/drm/data/resourceid_%1/%2").arg(productId)
                                                         .arg(fileName));
    return canReadDRM(path);
}

bool MiniIAPQtClient::canReadDRM(const QString& fileName) const
{
#ifdef IAP_DEBUG
    qDebug() << "IAPImpl::canReadDRM" << fileName;
#endif
    bool canRead = false;
#ifdef Q_OS_SYMBIAN
    // try to access it
    DRMFile file;
    int error = file.open(fileName);

    if(!error) {
        // if you can, read the file
#ifdef IAP_DEBUG
        qDebug() << "IAPImpl::canReadDRM: We can read the DRM file";
#endif
        canRead = true;
        file.close();
    }
    else {
        file.close();
        // opening failed due to DRM protection?
        if(file.isDRMError(error)) {
#ifdef IAP_DEBUG
            qDebug() << "IAPImpl::canReadDRM: DRM error occured" << error;
#endif
        }
        else {
#ifdef IAP_DEBUG
            qDebug() << "IAPImpl::canReadDRM: Unknown error occured" << error;
#endif
        }
    }
#endif // Q_OS_SYMBIAN
    return canRead;
}

bool MiniIAPQtClient::purchaseProduct(const char *productId)
{
    qDebug() << "MiniIAPQtClient::purchaseProduct >>>";
    prepare();

    if (!m_miniIAPReceiver) return false;
    if (isPurchaseRunning) {
        qDebug() << "already running";
        return false;
    }
    isPurchaseRunning = true;
    m_productId = productId;
    m_requestId = m_IAPClient->purchaseProduct(m_productId, IAPClient::ForcedAutomaticRestoration);
    bool result = false;
    if (m_requestId > 0) {
        qDebug() << "MiniIAPQtClient::purchaseProduct INITIATED" << m_requestId << m_productId;
        result = true;
    }
    else {
        clearPurchasing();
        qDebug() << "MiniIAPQtClient::purchaseProduct FAILED" << m_requestId;
    }
    return result;
}

void MiniIAPQtClient::purchaseCompleted( int requestId, QString status, QString purchaseTicket )
{
    qDebug() << "purchaseCompleted";
    if (!m_miniIAPReceiver) return;
    if (m_requestId != requestId) return;
    m_miniIAPReceiver->purchaseCompleted(m_productId.toLatin1().constData(), status.toLatin1().constData());
    Q_UNUSED(purchaseTicket)
}

void MiniIAPQtClient::restorationCompleted( int requestId, QString status, QString purchaseTicket )
{
    qDebug() << "restorationCompleted";
    if (!m_miniIAPReceiver) return;
    if (m_requestId != requestId) return;
    m_miniIAPReceiver->purchaseCompleted(m_productId.toLatin1().constData(), status.toLatin1().constData());
    Q_UNUSED(purchaseTicket)
}


void MiniIAPQtClient::purchaseFlowFinished( int requestId )
{
    qDebug() << "purchaseFlowFinished";
    if (!m_miniIAPReceiver) return;
    if (m_requestId != requestId) return;
    m_miniIAPReceiver->purchaseFlowFinished(m_productId.toLatin1().constData());
    clearPurchasing();
}

void MiniIAPQtClient::clearPurchasing()
{
    m_requestId = -1;
    m_productId.clear();
    isPurchaseRunning = false;
}

void MiniIAPQtClient::prepare()
{
    if (!m_IAPClient) {
        // required so that IAPClient::ProductData can be queued in the signal
        //qRegisterMetaType<IAPClient::ProductDataHash>("IAPClient::ProductDataHash");

        m_IAPClient = new IAPClient(this);
        connect(m_IAPClient, SIGNAL(purchaseCompleted(int,QString,QString)),
            SLOT(purchaseCompleted(int,QString,QString)));
        connect(m_IAPClient, SIGNAL(purchaseFlowFinished(int)),
            SLOT(purchaseFlowFinished(int)));
        connect(m_IAPClient, SIGNAL(restorationCompleted(int, QString, QString)), this,
                SLOT(restorationCompleted(int,QString,QString)));
        qDebug() << "MiniIAPQtClient::purchaseProduct >>> prepared";
    }
}
