/**
 * Copyright (c) 2011-2014 Microsoft Mobile and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#include "miniiapinterface.h"

MiniIAPInterface::MiniIAPInterface() {}
void MiniIAPInterface::purchaseCompleted(const char *productId, const char *status) {}
void MiniIAPInterface::purchaseFlowFinished(const char *productId) {}
