////////////////////////////////////////////////////////////////////////
//                                                                    //
// (C) 2008 Tom Bramer                                                //
//                                                                    //
// Filename: seh.hpp                                                  //
// Description: Structured exception handling support for MinGW.      //
// Author: Tom Bramer                                                 //
//                                                                    //
// This program is free software; you can redistribute it and/or      //
// modify it under the terms of the GNU General Public License        //
// as published by the Free Software Foundation; either version 2     //
// of the License, or (at your option) any later version.             //
//                                                                    //
// This program is distributed in the hope that it will be useful,    //
// but WITHOUT ANY WARRANTY; without even the implied warranty of     //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      //
// GNU General Public License for more details.                       //
//                                                                    //
// You should have received a copy of the GNU General Public License  //
// along with this program; if not, write to the Free Software        //
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA      //
// 02110-1301, USA.                                                   //
//                                                                    //
//////////////////////////////////////////////////////////////////////// 


/*******************************************************************************
  Original source downloaded from 
  http://www.programmingunlimited.net/siteexec/content.cgi?page=fv
  
  A complete description of the "exception" handling mechanism implemented
  here can be found at 
  http://www.programmingunlimited.net/siteexec/content.cgi?page=mingw-seh
  
  Modifications by A. Reeves <aaron.reeves@naadsm.org>, for use
  with the Atriplex distributed computing system:
    - Renamed "seh.hpp" to "seh.h" (2008-03-17) 
    - Removed namespace definitions (2008-03-17) 
    - Eliminated dependence on "defs.hpp" (2008-03-17)
    - Renamed macros to use all caps (2008-03-17) 
*******************************************************************************/

#ifndef FV_SEH_H
#define FV_SEH_H

#include <windows.h> // AR Commented out 7/20/11, due to conflict with Qt 4.1.4 "qatomic.h"
#include <excpt.h>
#include <setjmp.h>
//#include "defs.hpp"

#include <QtCore>

//NAMESPACE_BEGIN(PU)
//NAMESPACE_BEGIN(SEH)

class __SEH_HANDLER;

typedef struct tag__SEH_EXCEPTION_REGISTRATION
{
    tag__SEH_EXCEPTION_REGISTRATION* prev;
    PEXCEPTION_HANDLER handler;
    __SEH_HANDLER* exthandler;
} __SEH_EXCEPTION_REGISTRATION;


class __SEH_HANDLER
{
public:
    __SEH_HANDLER() { }
    virtual ~__SEH_HANDLER() { }
    static EXCEPTION_DISPOSITION ExceptionRouter(PEXCEPTION_RECORD pRecord, 
                                                 __SEH_EXCEPTION_REGISTRATION* pReg,
                                                 PCONTEXT pContext,
                                                 PEXCEPTION_RECORD pRecord2);

    virtual EXCEPTION_DISPOSITION ExceptionHandler(PEXCEPTION_RECORD pRecord, 
                                                   __SEH_EXCEPTION_REGISTRATION* pReg,
                                                   PCONTEXT pContext,
                                                   PEXCEPTION_RECORD pRecord2);
    
    jmp_buf context;
    EXCEPTION_RECORD excRecord;
    CONTEXT excContext;

  private:
    Q_DISABLE_COPY(__SEH_HANDLER)
};

#define SEH_TRY                                                        \
{                                                                      \
    __SEH_EXCEPTION_REGISTRATION _lseh_er;                             \
    __SEH_HANDLER _lseh_handler;                                       \
                                                                       \
    _lseh_er.handler = \
        reinterpret_cast<PEXCEPTION_HANDLER>(__SEH_HANDLER::ExceptionRouter);     \
    _lseh_er.exthandler = &_lseh_handler;                               \
    asm volatile ("movl %%fs:0, %0" : "=r" (_lseh_er.prev));           \
    asm volatile ("movl %0, %%fs:0" : : "r" (&_lseh_er));              \
    int _lseh_setjmp_res = setjmp(_lseh_handler.context);              \
    while(true) {                                                      \
        if(_lseh_setjmp_res != 0) {                                    \
            break;                                                     \
        }                                                              \

#define SEH_FINALLY                                                    \
        break;                                                         \
    }                                                                  \
    while(true) {
    

#define SEH_EXCEPT(rec, ctx)                                           \
        break;                                                         \
    }                                                                  \
    PEXCEPTION_RECORD rec = &_lseh_handler.excRecord;                  \
    PCONTEXT ctx = &_lseh_handler.excContext;                          \
                                                                       \
    asm volatile ("movl %0, %%fs:0" : : "r" (_lseh_er.prev));          \
    if(_lseh_setjmp_res != 0)
    
#define SEH_END }

//NAMESPACE_END(SEH)
//NAMESPACE_END(PU)

#endif
