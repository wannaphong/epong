/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef MINIIAPQTCLIENT_H
#define MINIIAPQTCLIENT_H

#include <QString>
#include <QObject>
#include "miniiapclientapi.h"
#include "iapclient.h"

const char IAP_API_FAILED[] = "IAPApiFailed";

class MiniIAPQtClient : public QObject, public MiniIAPClientApi
{
    Q_OBJECT
public:
    MiniIAPQtClient(QObject *parent = 0);
    ~MiniIAPQtClient();

    virtual void prepare();
    virtual bool purchaseProduct(const char * productId);
    virtual bool isProductPurchased(const char *productId, const char *fileName = 0);

public slots:
    void purchaseCompleted( int requestId, QString status, QString purchaseTicket );
    void restorationCompleted(int requestId, QString status, QString purchaseTicket);
    void purchaseFlowFinished( int requestId );

private:
    bool canReadDRM(const QString &fileName) const;
    void clearPurchasing();
    bool isPurchaseRunning;
    IAPClient *m_IAPClient;
    int m_requestId;
    QString m_productId;

};

#endif // MINIIAPQTCLIENT_H
