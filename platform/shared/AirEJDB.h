/*
 Copyright (c) 2013 Justin Walsh, http://thejustinwalsh.com
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#ifndef air_ejdb_h
#define air_ejdb_h

#include "FlashRuntimeExtensions.h"
#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

// Expose our extension initializer and finalizer to C style name lookups
extern "C" {

/* ServerSocketExtInitializer()
 * The extension initializer is called the first time the ActionScript side of the extension
 * calls ExtensionContext.createExtensionContext() for any context.
 *
 * Please note: this should be same as the <initializer> specified in the extension.xml
 */
#ifdef WIN32
  __declspec(dllexport) 
#endif
void AirEJDBExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);

/* ServerSocketExtFinalizer()
 * The extension finalizer is called when the runtime unloads the extension. However, it may not always called.
 *
 * Please note: this should be same as the <finalizer> specified in the extension.xml
 */
#ifdef WIN32
  __declspec(dllexport) 
#endif
void AirEJDBExtFinalizer(void* extData);
    
}

/* ContextInitializer()
 * The context initializer is called when the runtime creates the extension context instance.
 */
void AirEJDBContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToTest, const FRENamedFunction** functionsToSet);

/* ContextFinalizer()
 * The context finalizer is called when the extension's ActionScript code
 * calls the ExtensionContext instance's dispose() method.
 * If the AIR runtime garbage collector disposes of the ExtensionContext instance, the runtime also calls ContextFinalizer().
 */
void AirEJDBContextFinalizer(FREContext ctx);

FREObject AirEJDBDebugLogging(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBOpen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBClose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBIsOpen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBEnsureCollection(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropCollection(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBSave(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBLoad(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBRemove(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBFind(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBSync(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropIndexes(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBOptimizeIndexes(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBEnsureStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBEnsureIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBEnsureNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBEnsureArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBRebuildStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBRebuildIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBRebuildNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBRebuildArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

FREObject AirEJDBDropArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
    
FREObject AirEJDBCursorClose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
    
FREObject AirEJDBCursorGetField(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
    
FREObject AirEJDBCursorGetObject(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);

#endif
