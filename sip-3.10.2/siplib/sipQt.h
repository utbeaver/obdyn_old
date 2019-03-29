// The SIP library interface for Qt support.
//
// Copyright (c) 2004
// 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
// 
// This file is part of SIP.
// 
// This copy of SIP is licensed for use under the terms of the SIP License
// Agreement.  See the file LICENSE for more details.
// 
// SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


#ifndef _SIPQT_H
#define _SIPQT_H

#include <Python.h>
#include <qobject.h>
#include <sip.h>


// The base proxy slot handler class.

class SIP_EXTERN sipProxy : public QObject {
	Q_OBJECT

public:
	sipProxy();
	~sipProxy();

	static const QObject *sipSender;	// Saved QObject::sender().

	virtual char *getProxySlot(char *) = 0;

	sipSlot sipRealSlot;			// The Python slot.
	sipThisType *sipTxThis;			// The transmitter.
	char *sipTxSig;				// The transmitting signal.
	char *sipRxSlot;			// The receiving slot.
	sipProxy *sipNext;			// Next in linked list.
	sipProxy *sipPrev;			// Previous in linked list.

protected:
	char *searchProxySlotTable(char **,char *);
};


// These are public support functions specifically for signals/slots that can
// be called by handwritten code and will be supported in SIP v4.

extern SIP_EXTERN int sipEmitSignal Py_PROTO((PyObject *,char *,PyObject *));
extern SIP_EXTERN PyObject *sipConnectRx Py_PROTO((PyObject *,char *,PyObject *,char *));
extern SIP_EXTERN PyObject *sipDisconnectRx Py_PROTO((PyObject *,char *,PyObject *,char *));
extern SIP_EXTERN const void *sipGetSender Py_PROTO((void));


// These are private support functions specifically for signals/slots that
// should only be called by generated code and may not exist in SIP v4.

extern SIP_EXTERN void sipEmitToSlot Py_PROTO((sipSlot *,char *,...));

#endif
