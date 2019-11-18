/***************************************************************************
                           definitions.cpp  -  description
                             -------------------
    begin                : Mon Mai 12 2003
    copyright            : (C) 2003 by Dirk Henrici
    email                : henrici@informatik.uni-kl.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of license found in "COPYING".                     *
 *                                                                         *
 ***************************************************************************/

#include "definitions.h"

string errcode2str(tERROR error) {
//Gibt die Erklärung zu einem Fehlercode zurück
  switch (error) {
    case ERR_OK:             return "ERR_OK: Kein Fehler aufgetreten.";
    case ERR_Mutex:          return "ERR_Mutex: Fehler bei Arbeit mit einem Mutex oder einer Semaphore.";
    case ERR_NotImplemented: return "ERR_NotImplemented: Aufruf einer nicht-implementieren Funktion/Methode.";
    default:
      ostringstream errornumber;
      errornumber << error;
      return "ERR_Unknown[" + errornumber.str() + "]: Unbekannter Fehler.";
  }
};
