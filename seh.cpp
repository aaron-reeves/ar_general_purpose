////////////////////////////////////////////////////////////////////////
//                                                                    //
// FileVerifier: A tool for calculating hashes using                  //
//               on files using various different                     //
//               algorithms.                                          //
//                                                                    //
// (C) 2008 Tom Bramer                                                //
//                                                                    //
// Filename: seh.cpp                                                  //
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
  
  Modifications by A. Reeves <Aaron.Reeves@colostate.edu>, for use
  with the Atriplex distributed computing system:
    - Renamed "seh.hpp" to "seh.h" (2008-03-17) 
    - Removed namespace definitions (2008-03-17) 
*******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <fv/config.h>
#endif

#include "seh.h"

//NAMESPACE_BEGIN(PU)
//NAMESPACE_BEGIN(SEH)

EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionRouter(PEXCEPTION_RECORD pRecord, 
                                                     __SEH_EXCEPTION_REGISTRATION* pReg,
                                                     PCONTEXT pContext,
                                                     PEXCEPTION_RECORD pRecord2)
{
    return pReg->exthandler->ExceptionHandler(pRecord, pReg, pContext, pRecord2);
}

EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionHandler(PEXCEPTION_RECORD pRecord, 
                                                      __SEH_EXCEPTION_REGISTRATION* pReg,
                                                      PCONTEXT pContext,
                                                      PEXCEPTION_RECORD pRecord2)
{
    CopyMemory(&excContext, pContext, sizeof(_CONTEXT));
    CopyMemory(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));
    longjmp(context, 1);
}

//NAMESPACE_END(SEH)
//NAMESPACE_END(PU)
