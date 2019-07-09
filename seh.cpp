////////////////////////////////////////////////////////////////////////
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
  
  Modifications by A. Reeves <aaron.reeves@naadsm.org>, for use
  with the Atriplex distributed computing system:
    - Renamed "seh.hpp" to "seh.h" (2008-03-17) 
    - Removed namespace definitions (2008-03-17) 
    
    
Structured Exception Handling and MinGW
.

Note: you might also be interested in LibSEH, which can be found here.

Structured Exception Handling is a feature implemented in most versions of Windows (at least since Windows 95) that allows one to trap hardware exceptions (and software exceptions raised by RaiseException) directly in one's software. Combined with the integrated support of SEH in Microsoft's compilers, using SEH (when appropriate) can make an application more robust. More can be learned about how Microsoft's compilers handle this feature here: http://msdn2.microsoft.com/en-us/library/swezty51(VS.80).aspx

A common issue that arises is using this feature from compilers that do not have built in support for SEH. The Microsoft C/C++ compiler supports some additional keywords that make this possible ( __try, __except), such as in this example:

      int main(int argc, char** argv)
      {
         int* mydata = NULL;
         __try {
           std::cout << "Attempting to write to memory address 0x00000000" << std::endl;
           *mydata = 10;
           std::cout << "The value pointed to by mydata is: " << *mydata << std::endl;
         }
         __except(AccessViolation(GetExceptionCode(), GetExceptionInformation()))
         {
           std::cerr << "Exception caught..." << std::endl;
         }

         return 0;
      }
    

AccessViolation is a filter function that is called when an exception is raised (not a C++ exception), which simply tests the exception code to determine if the exception that was raised is an Access Violation (in this case, it should be). It is not implemented here for sake of brevity.
SEH on MinGW
.

So, how can one use this functionality in MinGW? The MinGW headers include a set of pre-processor macros that allow one to gain some of the functionality gained by SEH. The headers provide the __try1(handler) and __except1 macros. These do not give true exception handling semantics in your program, however. The __try1 macro simply installs an exception handler (similar to a signal on Unix), and the __except1 macro simply removes it. When an exception is raised, your handler is called. Actually going back to a certain point of execution is not provided (such as resuming execution inside an exception handling block, and continuing execution of a function).

There's another issue with these macros to be aware of. That issue is that they do not work properly on newer versions of GCC. The problem is that these functions directly allocate space on the stack to store the location of the exception handler, and GCC is unaware of this manipulation, and ends up designating this space for any automatic variables (or new stack frames). Thus, one is highly likely to overwrite this registration information in the process.

The structure that is allocated on the stack, by the __try1 macro, by the way, is defined as:

      typedef struct _EXCEPTION_REGISTRATION
      {
        struct _EXCEPTION_REGISTRATION* prev;
        PEXCEPTION_HANDLER handler;
      } EXCEPTION_REGISTRATION, *PEXCEPTION_REGISTRATION;
    

As can be seen here, this is essentially a linked list of exception handlers, which are called in the reverse order of which they are added to the list (a stack).

This structure occupies 8 bytes on the stack on a 32-bit system. One way of working around this is to store this exception registration in an automatic variable explicitly, and then inform the system of where the registration is stored by modifying the thread information block (stored at the first DWORD, which just so happens to be pointed to by the FS selector on i486+ systems). as usual (see http://gcc.gnu.org/ml/java/2003-03/msg00203.html). As opposed to allocating more stack space by blindly subtracting from the SP register, doing it this way will not interfere with the compiler's accounting on the stack.

This is all good, but this doesn't give the exception handling semantics that are available in the Microsoft C/C++ compiler.
MinGW implementation
.

Note: The following describes my quick and dirty implementation of SEH on MinGW using pre-processor macros and a C++ class. It is not meant to be the most efficient, though it appears to work for my purposes, so I thought that I would share it here.

First off, I use the idea of storing the exception registration on the stack using an automatic variable. However, since I am wanting to use an exception handling object, as opposed to an exception handling function, I extend the EXCEPTION_REGISTRATION structure:

      typedef struct tag__SEH_EXCEPTION_REGISTRATION
      {
          tag__SEH_EXCEPTION_REGISTRATION* prev;
          PEXCEPTION_HANDLER handler;
          __SEH_HANDLER* exthandler;
      } __SEH_EXCEPTION_REGISTRATION;
    

Note that this is very similar to the _EXCEPTION_REGISTRATION structure above, except one should note that there is a third entry. __SEH_HANDLER is the class that handles the exceptions raised. Objects of this class will store the information about the context of where the exception handler was actually installed, so that exception handling semantics can be emulated.

The exception registration is stored on the stack explicitly, by declaring an automatic variable. I also declare an object of type __SEH_HANDLER, as I will need to store the information about the current context in such. Here is the part that actually registers the exception handler:

      // The exception registration structure, on the stack as an automatic variable
      __SEH_EXCEPTION_REGISTRATION _lseh_er;
      
      // The custom exception handling object, also on the stack.
      __SEH_HANDLER _lseh_handler;

      // Prepare the exception registration, by giving the exception handler and the
      // custom exception handling object.
      _lseh_er.handler = 
          reinterpret_cast<PEXCEPTION_HANDLER>(__SEH_HANDLER::ExceptionRouter);
      _lseh_er.exthandler = &_lseh_handler;                  

      // Point the appropriate field in the thread information block to the previously
      // declared exception registration object
      asm volatile ("movl %%fs:0, %0" : "=r" (_lseh_er.prev));
      asm volatile ("movl %0, %%fs:0" : : "r" (&_lseh_er));

      // Save the current context, so that the program can resume here from the exception
      // handler.
      int _lseh_setjmp_resQOperatingSystemVersion lhs  QOperatingSystemVersion QOperatingSystemVersion lhsSEQOperatingSystemVersion rhsatic class function that is called for each exception that is handled in this manner. Since the exception registration object is provided as an argument to this call, we can put a pointer to the actual __SEH_HANDLER instance in this structure also, so that one can call using the actual object within this context. As is shown, a pointer to the __SEH_HANDLER object is stored within the registration.

The two lines of in-line assembly actually store the pointer to the exception registration in the thread information block and repair the link in order to maintain the handler chain.

The last line uses the setjmp function to get information aboQOperatingSystemVersion rhsn pointer, other registers, etc) so that a non-local jump can be made back to this point. When an exception is raised, the exception handler will perform a non-local jump back to this point.

Whether or not code after tQOperatingSystemVersion lhs bQOperatingSystemVersion rhsed by the return code. See the respective manuals on setjmp and longjmp for more information on how these calls are used.

      while(true) {
        if(_lseh_setjmp_res != 0) { 
            break;
        }    

        // Your code here, which may potentially raise exceptions

        break; // Exit the while loop.
      }
    

This is the code that comes after the exception handler installation. The while loop is used because it is rather easy to jump beyond the loop by using a break statement.

      // These are provided by the __SEH_HANDLER object, as there is no other way to 
      // retrieve them outside the actual exception handler function.
      PEXCEPTION_RECORD rec = &_lseh_handler.excRecord;
      PCONTEXT ctx = &_lseh_handler.excContext;

      // Uninstall the exception handler that was installed previously
      asm volatile ("movl %0, %%fs:0" : : "r" (_lseh_er.prev));

      if(_lseh_setjmp_res != 0)
      {
          // Handle the exception here
      }
    

This actually handles an exception, if one actually occurs. The idea should be coming apparent...

Here is the definition of the __SEH_HANDLER class:

      class __SEH_HANDLER
      {
        public:

          // This is the main exception handling function.  This is called
          // for each exception raised using this method.
          static EXCEPTION_DISPOSITION ExceptionRouter(PEXCEPTION_RECORD pRecord, 
                                                       __SEH_EXCEPTION_REGISTRATION* pReg,
                                                       PCONTEXT pContext,
                                                       PEXCEPTION_RECORD pRecord2);

          // This is the exception handler for this specific instance.  This is called by the
          // ExceptionRouter class function.
          virtual EXCEPTION_DISPOSITION ExceptionHandler(PEXCEPTION_RECORD pRecord, 
                                                         __SEH_EXCEPTION_REGISTRATION* pReg,
                                                         PCONTEXT pContext,
                                                         PEXCEPTION_RECORD pRecord2);
    
          // This is the context buffer used by setjmp.  This stores the context at a given point
          // in the program so that it can be resumed.
          jmp_buf context;

          // This is a copy of the EXCEPTION_RECORD structure passed to the exception handler.
          EXCEPTION_RECORD excRecord;
          // This is a copy of the CONTEXT structure passed to the exception handler.
          CONTEXT excContext;    
      };

      
      // The main exception handler.
      EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionRouter(PEXCEPTION_RECORD pRecord, 
                                                           __SEH_EXCEPTION_REGISTRATION* pReg,
                                                           PCONTEXT pContext,
                                                           PEXCEPTION_RECORD pRecord2)
      {
          // Retrieve the actual __SEH_HANDLER object from the registration, and call the 
          // specific exception handling function.  Everything could have been done from this
          // function alone, but I decided to use an instance method instead.
          return pReg->exthandler->ExceptionHandler(pRecord, pReg, pContext, pRecord2);
      }

      EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionHandler(PEXCEPTION_RECORD pRecord, 
                                                            __SEH_EXCEPTION_REGISTRATION* pReg,
                                                            PCONTEXT pContext,
                                                            PEXCEPTION_RECORD pRecord2)
      {
          // The objects pointed to by the pointers live on the stack, so a copy of them is required,
          // or they may get overwritten by the time we've hit the real exception handler code
          // back in the offending function. 
          CopyMemory(&excContext, pContext, sizeof(_CONTEXT));
          CopyMemory(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));

          // Jump back to the function where the exception actually occurred.  The 1 is the
          // return code that will be returned by set_jmp.
          longjmp(context, 1);
      }

    

This essentially jumps back to where the previous setjmp function was called, which is notified that an exception was raised by the return of setjmp.
Making it easy to use
.

One might be wondering, how can one use this in practice? There are some interesting aspects of how the above was coded, which it may seem that such could have been done in less code. While that may be true, the code above comes from a packaged set of macros I created, and is designed to work in the context of those macros. The exception installing and handling code is implemented as a set of macros, defined as:

      // Note the unmatched braces in these macros.  These are to allow one to use
      // the same variable name more than once (new scope).
      #define seh_try                                                               \
      {                                                                             \
          __SEH_EXCEPTION_REGISTRATION _lseh_er;                                    \
          __SEH_HANDLER _lseh_handler;                                              \
                                                                                    \
          _lseh_er.handler =                                                        \
              reinterpret_cast<PEXCEPTION_HANDLER>(__SEH_HANDLER::ExceptionRouter); \
          _lseh_er.exthandler = &_lseh_handler;                                     \
          asm volatile ("movl %%fs:0, %0" : "=r" (_lseh_er.prev));                  \
          asm volatile ("movl %0, %%fs:0" : : "r" (&_lseh_er));                     \
          int _lseh_setjmp_res = setjmp(_lseh_handler.context);                     \
          while(true) {                                                             \
              if(_lseh_setjmp_res != 0) {                                           \
                  break;                                                            \
              }                                                                     \


      #define seh_except(rec, ctx)                                                  \
              break;                                                                \
          }                                                                         \
          PEXCEPTION_RECORD rec = &_lseh_handler.excRecord;                         \
          PCONTEXT ctx = &_lseh_handler.excContext;                                 \
                                                                                    \
          asm volatile ("movl %0, %%fs:0" : : "r" (_lseh_er.prev));                 \
          if(_lseh_setjmp_res != 0)
          
      #define seh_end }

    

Using these macros, one can translate the Visual C++ example above to this in MinGW:

      int main(int argc, char** argv)
      {
         int* mydata = NULL;
         seh_try {
           std::cout << "Attempting to write to memory address 0x00000000" << std::endl;
           *mydata = 10;
           std::cout << "The value pointed to by mydata is: " << *mydata << std::endl;
         }
         seh_except(info, context)
         {
           if(info->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
              std::cerr << "Access Violation Exception caught..." << std::endl;
         }
         seh_end

         // The previous seh_end call is necessary, so don't forget it.
         return 0;
      }
        
    
    
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
    Q_UNUSED( pReg );
    Q_UNUSED( pRecord2 );
    CopyMemory(&excContext, pContext, sizeof(_CONTEXT));
    CopyMemory(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));
    longjmp(context, 1);
}

//NAMESPACE_END(SEH)
//NAMESPACE_END(PU)
