/**
 * Copyright (c) 2011-2014 Microsoft Mobile and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#include "miniiapclientapi.h"

MiniIAPClientApi::MiniIAPClientApi()
{ 
    m_miniIAPReceiver = 0;
}

MiniIAPClientApi::~MiniIAPClientApi() 
{

}

void MiniIAPClientApi::prepare() 
{
}

bool MiniIAPClientApi::purchaseProduct(const char * productId) {
	return false;
}

bool MiniIAPClientApi::isProductPurchased(const char *productId, const char *fileName) {
	return false;
}

MiniIAPInterface *MiniIAPClientApi::miniIAPReceiver() const 
{ 
	return m_miniIAPReceiver; 
}

void MiniIAPClientApi::setMiniIAPReceiver(MiniIAPInterface *receiver) 
{ 
	m_miniIAPReceiver = receiver; 
}

