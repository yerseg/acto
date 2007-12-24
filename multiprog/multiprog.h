﻿///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   The multiprog Library                                       //
//                                                                                               //
//-----------------------------------------------------------------------------------------------//
// Copyright © 2007                                                                              //
//     Pavel A. Artemkin (acto.stan@gmail.com)                                                   //
// ----------------------------------------------------------------------------------------------//
// License:                                                                                      //
//     Code covered by the MIT License.                                                          //
//     The authors make no representations about the suitability of this software                //
//     for any purpose. It is provided "as is" without express or implied warranty.              //
//-----------------------------------------------------------------------------------------------//
// File: multiprog.h                                                                             //
//     Главный файл библиотеки.                                                                  //
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined ( __multiprog_h__ )
#define __multiprog_h__


// Первым идет конфигурационный заголовок
#include "config.h"

// Реализация делегатов
#include "system/delegates.h"

// Обертка над API операционных систем
#ifdef MSWINDOWS
#	include "system/act_mswin.h"
#endif


///////////////////////////////////////////////////////////////////////////////
//                         КОМПОНЕНТЫ БИБЛИОТЕКИ                             //
///////////////////////////////////////////////////////////////////////////////

// Пул потоков
//#include "runtime/runtime.h"

// Библиотека асинхронных процедур
//#include "asynch/asynch.h"

// Библиотека активных объектов
#include "acto/act_o.h"


#endif // __multiprog_h__